#ifndef CLP_STRUCTURED_SEARCH_EMPTY_EXPRESSION_H
#define CLP_STRUCTURED_SEARCH_EMPTY_EXPRESSION_H

#include "Expression.hpp"

namespace clp_structured {
/**
 * Class representing the empty set/false. Useful
 * for constant propagation and eliminating expressions.
 */
class EmptyExpr : public Expression {
public:
    void print() override;

    /**
     * EmptyExpr never has any operands, so we arbitrarily
     * say that all operands are Expression
     */
    bool has_only_expression_operands() override { return true; }

    /**
     * Deep copy
     * @return a deep copy of this expression
     */
    std::shared_ptr<Expression> copy() const override;

    /**
     * Create an Empty expression which can optionally be attached to a parent
     * @param parent parent this expression is attached to
     * @return newly created Empty expression
     */
    static std::shared_ptr<Expression> create(Expression* parent = nullptr);

private:
    explicit EmptyExpr(Expression* parent = nullptr);

    EmptyExpr(EmptyExpr const&);
};
}  // namespace clp_structured

#endif  // CLP_STRUCTURED_SEARCH_EMPTY_EXPRESSION_H
