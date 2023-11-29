#include <iostream>
#include <string>
#include <vector>

#include "antlr4-runtime.h"
#include "KibanaBaseVisitor.h"
#include "KibanaLexer.h"
#include "KibanaParser.h"
// If redlining may want to add ${workspaceFolder}/build/**
// to include path for vscode C/C++ utils

#include "src/clp_structured/search/AndExpr.hpp"
#include "src/clp_structured/search/BooleanLiteral.hpp"
#include "src/clp_structured/search/ColumnDescriptor.hpp"
#include "src/clp_structured/search/DateLiteral.hpp"
#include "src/clp_structured/search/EmptyExpr.hpp"
#include "src/clp_structured/search/FilterExpr.hpp"
#include "src/clp_structured/search/Integral.hpp"
#include "src/clp_structured/search/NullLiteral.hpp"
#include "src/clp_structured/search/OrExpr.hpp"
#include "src/clp_structured/search/StringLiteral.hpp"
#include "src/clp_structured/Utils.hpp"

using namespace antlr4;
using namespace kibana;

namespace clp_structured { namespace search { namespace Kibana {
    class ErrorListener : public BaseErrorListener {
    private:
        bool m_error = false;

    public:
        void syntaxError(
                Recognizer* recognizer,
                Token* offending_symbol,
                size_t line,
                size_t char_position_in_line,
                std::string const& msg,
                std::exception_ptr e
        ) override {
            m_error = true;
        }

        bool error() const { return m_error; }
    };

    class ParseTreeVisitor : public KibanaBaseVisitor {
    private:
        static void prepend_column(
                std::shared_ptr<ColumnDescriptor> const& desc,
                DescriptorList const& prefix
        ) {
            desc->get_descriptor_list()
                    .insert(desc->descriptor_begin(), prefix.begin(), prefix.end());
        }

        void prepend_column(std::shared_ptr<Expression> const& expr, DescriptorList const& prefix) {
            for (auto const& op : expr->get_op_list()) {
                if (auto col = std::dynamic_pointer_cast<ColumnDescriptor>(op)) {
                    prepend_column(col, prefix);
                } else if (auto subexpr = std::dynamic_pointer_cast<Expression>(op)) {
                    prepend_column(subexpr, prefix);
                }
            }
        }

    public:
        static std::string unquote_string(std::string const& text) {
            if (text.at(0) == '"') {
                return text.substr(1, text.length() - 2);
            } else {
                return text;
            }
        }

        static std::string unquote_date_string(std::string const& text) {
            // date(...)
            // 012345
            return unquote_string(text.substr(5, text.size() - 6));
        }

        static std::shared_ptr<Literal> unquote_literal(std::string const& text) {
            std::string token = unquote_string(text);

            if (auto ret = Integral::create_from_string(token)) {
                return ret;
            } else if (auto ret = BooleanLiteral::create_from_string(token)) {
                return ret;
            } else if (auto ret = NullLiteral::create_from_string(token)) {
                return ret;
            } else {
                return StringLiteral::create(token);
            }
        }

        static std::shared_ptr<Literal> unquote_date_literal(std::string const& text) {
            std::string token = unquote_date_string(text);

            return DateLiteral::create_from_string(token);
        }

        std::any visitStart(KibanaParser::StartContext* ctx) override {
            // only go through first child (query) and avoid default
            // behaviour of returning result from last child (EOF in this case)
            return ctx->children[0]->accept(this);
        }

        std::any visitColumn(KibanaParser::ColumnContext* ctx) override {
            std::string column = unquote_string(ctx->LITERAL()->getText());

            std::vector<std::string> descriptor_tokens;
            StringUtils::tokenize_column_descriptor(column, descriptor_tokens);

            return ColumnDescriptor::create(descriptor_tokens);
        }

        std::any visitNestedQuery(KibanaParser::NestedQueryContext* ctx) override {
            auto descriptor
                    = std::any_cast<std::shared_ptr<ColumnDescriptor>>(ctx->col->accept(this));
            DescriptorList prefix = descriptor->get_descriptor_list();

            auto nested_expr = std::any_cast<std::shared_ptr<Expression>>(ctx->q->accept(this));
            prepend_column(nested_expr, prefix);

            return nested_expr;
        }

        std::any visitOrAndQuery(KibanaParser::OrAndQueryContext* ctx) override {
            auto lhs = std::any_cast<std::shared_ptr<Expression>>(ctx->lhs->accept(this));
            auto rhs = std::any_cast<std::shared_ptr<Expression>>(ctx->rhs->accept(this));
            if (ctx->op->getType() == KibanaParser::AND) {
                return AndExpr::create(lhs, rhs);
            } else {
                return OrExpr::create(lhs, rhs);
            }
        }

        std::any visitNotQuery(KibanaParser::NotQueryContext* ctx) override {
            auto q = std::any_cast<std::shared_ptr<Expression>>(ctx->q->accept(this));
            q->invert();
            return q;
        }

        std::any visitSubQuery(KibanaParser::SubQueryContext* ctx) override {
            return ctx->q->accept(this);
        }

        std::any visitColumn_value_expression(KibanaParser::Column_value_expressionContext* ctx
        ) override {
            auto descriptor
                    = std::any_cast<std::shared_ptr<ColumnDescriptor>>(ctx->col->accept(this));

            if (ctx->lit) {
                auto lit = unquote_literal(ctx->lit->getText());
                return FilterExpr::create(descriptor, FilterOperation::EQ, lit);
            } else if (ctx->date_lit) {
                auto lit = unquote_date_literal(ctx->date_lit->getText());
                return FilterExpr::create(descriptor, FilterOperation::EQ, lit);
            } else /*if (ctx->list) */ {
                auto list_expr
                        = std::any_cast<std::shared_ptr<Expression>>(ctx->list->accept(this));
                DescriptorList prefix = descriptor->get_descriptor_list();
                prepend_column(list_expr, prefix);
                return list_expr;
            }
        }

        std::any visitColumn_range_expression(KibanaParser::Column_range_expressionContext* ctx
        ) override {
            auto descriptor
                    = std::any_cast<std::shared_ptr<ColumnDescriptor>>(ctx->col->accept(this));
            std::shared_ptr<Literal> lit;
            if (ctx->lit) {
                lit = unquote_literal(ctx->lit->getText());
            } else /*if (ctx->date_lit)*/ {
                lit = unquote_date_literal(ctx->date_lit->getText());
            }
            std::string range_op = ctx->RANGE_OPERATOR()->getText();

            FilterOperation op = FilterOperation::EQ;
            if (range_op == "<=") {
                op = FilterOperation::LTE;
            } else if (range_op == ">=") {
                op = FilterOperation::GTE;
            } else if (range_op == "<") {
                op = FilterOperation::LT;
            } else if (range_op == ">") {
                op = FilterOperation::GT;
            }

            return FilterExpr::create(descriptor, op, lit);
        }

        std::any visitValue_expression(KibanaParser::Value_expressionContext* ctx) override {
            auto lit = unquote_literal(ctx->LITERAL()->getText());
            auto descriptor = ColumnDescriptor::create("*");
            return FilterExpr::create(descriptor, FilterOperation::EQ, lit);
        }

        std::any visitList_of_values(KibanaParser::List_of_valuesContext* ctx) override {
            std::shared_ptr<Expression> base(nullptr);
            bool invert_each_filter = false;
            if (ctx->condition) {
                if (ctx->AND()) {
                    base = AndExpr::create();
                } else if (ctx->OR()) {
                    base = OrExpr::create();
                } else if (ctx->NOT()) {
                    invert_each_filter = true;
                    base = AndExpr::create();
                }
            } else {
                base = OrExpr::create();
            }

            auto empty_descriptor = ColumnDescriptor::create(DescriptorList());
            for (auto token : ctx->literals) {
                auto literal = unquote_literal(token->getText());
                auto expr = FilterExpr::create(
                        empty_descriptor,
                        FilterOperation::EQ,
                        literal,
                        invert_each_filter
                );
                base->add_operand(expr);
            }
            return base;
        }
    };

    std::shared_ptr<Expression> parse_kibana_expression(std::istream& in) {
        std::shared_ptr<Expression> expr = EmptyExpr::create();
        ErrorListener lexer_error_listener;
        ErrorListener parser_error_listener;

        ANTLRInputStream input(in);
        KibanaLexer lexer(&input);
        lexer.addErrorListener(&lexer_error_listener);
        CommonTokenStream tokens(&lexer);
        KibanaParser parser(&tokens);
        parser.addErrorListener(&parser_error_listener);
        KibanaParser::StartContext* tree = parser.start();

        if (lexer_error_listener.error()) {
            std::cout << "Lexer Error" << std::endl;
            return expr;
        } else if (parser_error_listener.error()) {
            std::cout << "Parser Error" << std::endl;
            return expr;
        }

        ParseTreeVisitor visitor;
        expr = std::any_cast<std::shared_ptr<Expression>>(visitor.visitStart(tree));
        return expr;
    }
}}}  // namespace clp_structured::search::Kibana
