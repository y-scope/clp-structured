#ifndef CLP_STRUCTURED_SEARCH_OUTPUT_H
#define CLP_STRUCTURED_SEARCH_OUTPUT_H

#include <map>
#include <set>
#include <string>
#include <unordered_set>
#include <utility>

#include "clp_search/Query.hpp"
#include "Expression.hpp"
#include "Integral.hpp"
#include "SchemaMatch.hpp"
#include "src/clp_structured/SchemaReader.hpp"
#include "src/clp_structured/Utils.hpp"
#include "StringLiteral.hpp"
#include "submodules/simdjson/include/simdjson.h"

using namespace simdjson;
using namespace clp_structured::search::clp_search;

namespace clp_structured { namespace search {
    class Output : public FilterClass {
    public:
        Output(std::shared_ptr<SchemaTree> tree,
               std::shared_ptr<ReaderUtils::SchemaMap> schemas,
               SchemaMatch& match,
               std::shared_ptr<Expression> expr,
               std::string archive_dir,
               std::shared_ptr<TimestampDictionaryReader> timestamp_dict)
                : m_schema_tree(std::move(tree)),
                  m_schemas(std::move(schemas)),
                  m_match(match),
                  m_expr(std::move(expr)),
                  m_archive_dir(std::move(archive_dir)),
                  m_timestamp_dict(std::move(timestamp_dict)) {}

        /**
         * Filters messages from all archives
         */
        void filter();

    private:
        SchemaMatch& m_match;
        std::shared_ptr<Expression> m_expr;
        std::string m_archive_dir;

        // variables for the current schema being filtered
        std::vector<BaseColumnReader*> m_searched_columns;
        std::vector<BaseColumnReader*> m_other_columns;
        std::set<int32_t> m_cached_string_columns;

        int32_t m_schema;
        SchemaReader* m_reader;

        std::shared_ptr<SchemaTree> m_schema_tree;
        std::shared_ptr<VariableDictionaryReader> m_var_dict;
        std::shared_ptr<LogTypeDictionaryReader> m_log_dict;
        std::shared_ptr<LogTypeDictionaryReader> m_array_dict;
        std::shared_ptr<TimestampDictionaryReader> m_timestamp_dict;

        std::shared_ptr<ReaderUtils::SchemaMap> m_schemas;

        std::map<std::string, Query> m_string_query_map;
        std::map<std::string, std::unordered_set<int64_t>> m_string_var_match_map;
        std::unordered_map<Expression*, Query*> m_expr_clp_query;
        std::unordered_map<Expression*, std::unordered_set<int64_t>*> m_expr_var_match_map;
        std::unordered_map<int32_t, ClpStringColumnReader*> m_clp_string_readers;
        std::unordered_map<int32_t, VariableStringColumnReader*> m_var_string_readers;
        std::unordered_map<int32_t, DateStringColumnReader*> m_datestring_readers;
        std::unordered_map<int32_t, FloatDateStringColumnReader*> m_floatdatestring_readers;
        uint64_t m_cur_message;
        EvaluatedValue m_expression_value;

        std::map<ColumnDescriptor*, std::vector<int32_t>> m_wildcard_to_searched_clpstrings;
        std::map<ColumnDescriptor*, std::vector<int32_t>> m_wildcard_to_searched_varstrings;
        std::map<ColumnDescriptor*, std::vector<int32_t>> m_wildcard_to_searched_datestrings;
        std::map<ColumnDescriptor*, std::vector<int32_t>> m_wildcard_to_searched_floatdatestrings;
        std::map<ColumnDescriptor*, std::vector<int32_t>> m_wildcard_to_searched_columns;

        simdjson::ondemand::parser m_array_parser;
        std::string m_array_search_string;
        bool m_maybe_string, m_maybe_number;

        /**
         * Initializes the variables. It is init is called once for each schema after which filter
         * is called once for every message in the schema
         * @param reader
         * @param schema_id
         * @param columns
         */
        void init(SchemaReader* reader, int32_t schema_id, std::vector<BaseColumnReader*>& columns)
                override;

        /**
         * Evaluates an expression
         * @param expr
         * @param schema
         * @param extracted_values
         * @return true if the expression evaluates to true, false otherwise
         */
        bool evaluate(
                Expression* expr,
                int32_t schema,
                std::map<int32_t, std::variant<int64_t, double, std::string, uint8_t>>&
                        extracted_values
        );

        /**
         * Evaluates a filter expression
         * @param expr
         * @param schema
         * @param extracted_values
         * @return true if the expression evaluates to true, false otherwise
         */
        bool evaluate_filter(
                FilterExpr* expr,
                int32_t schema,
                std::map<int32_t, std::variant<int64_t, double, std::string, uint8_t>>&
                        extracted_values
        );

        /**
         * Evaluates a wildcard filter expression
         * @param expr
         * @param schema
         * @param extracted_values
         * @return true if the expression evaluates to true, false otherwise
         */
        bool evaluate_wildcard_filter(
                FilterExpr* expr,
                int32_t schema,
                std::map<int32_t, std::variant<int64_t, double, std::string, uint8_t>>&
                        extracted_values
        );

        /**
         * Evaluates a int filter expression
         * @param op
         * @param value
         * @param operand
         * @return true if the expression evaluates to true, false otherwise
         */
        static bool evaluate_int_filter(
                FilterOperation op,
                int64_t value,
                std::shared_ptr<Literal> const& operand
        );

        /**
         * Evaluates a float filter expression
         * @param op
         * @param value
         * @param operand
         * @return true if the expression evaluates to true, false otherwise
         */
        static bool evaluate_float_filter(
                FilterOperation op,
                double value,
                std::shared_ptr<Literal> const& operand
        );

        /**
         * Evaluates a clp string filter expression
         * @param op
         * @param q
         * @param column_id
         * @param operand
         * @param extracted_values
         * @return true if the expression evaluates to true, false otherwise
         */
        bool evaluate_clp_string_filter(
                FilterOperation op,
                Query* q,
                int32_t column_id,
                std::shared_ptr<Literal> const& operand,
                std::map<int32_t, std::variant<int64_t, double, std::string, uint8_t>>&
                        extracted_values
        );

        /**
         * Evaluates a var string filter expression
         * @param op
         * @param reader
         * @param matching_vars
         * @param operand
         * @return true if the expression evaluates to true, false otherwise
         */
        bool evaluate_var_string_filter(
                FilterOperation op,
                VariableStringColumnReader* reader,
                std::unordered_set<int64_t>* matching_vars,
                std::shared_ptr<Literal> const& operand
        ) const;

        /**
         * Evaluates a epoch date string filter expression
         * @param op
         * @param reader
         * @param operand
         * @return true if the expression evaluates to true, false otherwise
         */
        bool evaluate_epoch_date_filter(
                FilterOperation op,
                DateStringColumnReader* reader,
                std::shared_ptr<Literal>& operand
        );

        /**
         * Evaluates a float date string filter expression
         * @param op
         * @param reader
         * @param operand
         * @return true if the expression evaluates to true, false otherwise
         */
        bool evaluate_float_date_filter(
                FilterOperation op,
                FloatDateStringColumnReader* reader,
                std::shared_ptr<Literal>& operand
        );

        /**
         * Evaluates an array filter expression
         * @param op
         * @param unresolved_tokens
         * @param value
         * @param operand
         * @return true if the expression evaluates to true, false otherwise
         */
        bool evaluate_array_filter(
                FilterOperation op,
                DescriptorList const& unresolved_tokens,
                std::string const& value,
                std::shared_ptr<Literal> const& operand
        ) const;

        /**
         * The implementation of evaluate_array_filter
         * @param object
         * @param op
         * @param unresolved_tokens
         * @param cur_idx
         * @param value
         * @param operand
         * @param array_or_object if true, we are traversing an array
         * @return true if the expression evaluates to true, false otherwise
         */
        bool evaluate_array_filter(
                json& object,
                FilterOperation op,
                DescriptorList const& unresolved_tokens,
                size_t cur_idx,
                std::shared_ptr<Literal> const& operand,
                bool array_or_object
        ) const;

        /**
         * Evaluates a wildcard array filter expression
         * @param op
         * @param value
         * @param operand
         * @return true if the expression evaluates to true, false otherwise
         */
        bool evaluate_wildcard_array_filter(
                FilterOperation op,
                std::string& value,
                std::shared_ptr<Literal> const& operand
        );

        /**
         * The implementation of evaluate_wildcard_array_filter
         * @param array
         * @param op
         * @param operand
         * @return true if the expression evaluates to true, false otherwise
         */
        bool evaluate_wildcard_array_filter(
                ondemand::array& array,
                FilterOperation op,
                std::shared_ptr<Literal> const& operand
        ) const;

        /**
         * The implementation of evaluate_wildcard_array_filter
         * @param object
         * @param op
         * @param operand
         * @return true if the expression evaluates to true, false otherwise
         */
        bool evaluate_wildcard_array_filter(
                ondemand::object& object,
                FilterOperation op,
                std::shared_ptr<Literal> const& operand
        ) const;

        /**
         * Evaluates a bool filter expression
         * @param op
         * @param value
         * @param operand
         * @return true if the expression evaluates to true, false otherwise
         */
        static bool evaluate_bool_filter(
                FilterOperation op,
                bool value,
                std::shared_ptr<Literal> const& operand
        );

        /**
         * Populates the string queries
         * @param expr
         */
        void populate_string_queries(std::shared_ptr<Expression> const& expr);

        /**
         * Constant propagates an expression
         * @param expr
         * @param schema_id
         * @return EvaluatedValue::True if the expression evaluates to true, EvaluatedValue::False
         * if the expression evaluates to false, EvaluatedValue::Unknown otherwise
         */
        EvaluatedValue
        constant_propagate(std::shared_ptr<Expression> const& expr, int32_t schema_id);

        /**
         * Populates searched wildcard columns
         * @param expr
         */
        void populate_searched_wildcard_columns(std::shared_ptr<Expression> const& expr);

        /**
         * Adds wildcard columns to searched columns
         */
        void add_wildcard_columns_to_searched_columns();

        // Methods inherited from FilterClass
        bool filter(
                uint64_t cur_message,
                std::map<int32_t, std::variant<int64_t, double, std::string, uint8_t>>&
                        extracted_values
        ) override;
    };
}}  // namespace clp_structured::search

#endif  // CLP_STRUCTURED_SEARCH_OUTPUT_H
