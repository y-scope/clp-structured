#ifndef CLP_STRUCTURED_SEARCH_FILTER_EXPRESSION_H
#define CLP_STRUCTURED_SEARCH_FILTER_EXPRESSION_H

#include <string>

#include "ColumnDescriptor.hpp"
#include "Expression.hpp"
#include "FilterOperation.hpp"
#include "Literal.hpp"

namespace clp_structured {
/**
 * Class for simple filter conditions in the AST. Consists of a column,
 * a filtering operation, and usually a literal.
 *
 * Conventionally the OpList contains a ColumnExpr followed by some Literal. I.e. a FilterExpr
 * always has a ColumnExpr, but may not have a Literal.
 */
class FilterExpr : public Expression {
public:
    void print() override;

    /**
     * Filter expressions never have expressions as children by construction.
     */
    bool has_only_expression_operands() override { return false; }

    /**
     * Get filtering operation
     * @return FilterOperation this Filter performs
     */
    FilterOperation get_operation() { return m_op; }

    /**
     * Get the Column this Filter acts on.
     * @return this Filter's column
     */
    std::shared_ptr<ColumnDescriptor> get_column() {
        return std::static_pointer_cast<ColumnDescriptor>(*op_begin());
    }

    /**
     * Get the Literal this Filter filters based on
     * @return this Filter's Literal or nullptr if there is no Literal
     */
    std::shared_ptr<Literal> get_operand();

    /**
     * Deep copy
     * @return a deep copy of this expression
     */
    std::shared_ptr<Expression> copy() const override;

    /**
     * Create a Filter expression with a Column and FilterOperation but no Literal
     * Literal can be added later using mutators provided by the Expression parent class
     * @param column the Column this Filter acts on
     * @param op the Operation this Filter uses to Filter the Column
     * @param inverted expression is inverted when true
     * @param parent parent this expression is attached to
     * @return newly created Or expression
     */
    static std::shared_ptr<Expression> create(
            std::shared_ptr<ColumnDescriptor>& column,
            FilterOperation op,
            bool inverted = false,
            Expression* parent = nullptr
    );

    /**
     * Create a Filter expression with a Column, FilterOperation and Literal
     * @param column the Column this Filter acts on
     * @param op the Operation this Filter uses to Filter the Column
     * @param inverted expression is inverted when true
     * @param parent parent this expression is attached to
     * @return newly created Or expression
     */
    static std::shared_ptr<Expression> create(
            std::shared_ptr<ColumnDescriptor>& column,
            FilterOperation op,
            std::shared_ptr<Literal>& operand,
            bool inverted = false,
            Expression* parent = nullptr
    );

    /**
     * Helper function to turn FilterOperation into string for printing
     * @param op the operation we want to convert to string
     * @return a string representing the operation
     */
    static std::string op_type_str(FilterOperation op);

private:
    FilterOperation m_op;

    FilterExpr(
            std::shared_ptr<ColumnDescriptor> const& column,
            FilterOperation op,
            bool inverted = false,
            Expression* parent = nullptr
    );
    FilterExpr(FilterExpr const&);
};
}  // namespace clp_structured

#endif  // CLP_STRUCTURED_SEARCH_FILTER_EXPRESSION_H
