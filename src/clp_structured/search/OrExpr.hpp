#ifndef CLP_STRUCTURED_SEARCH_OR_EXPRESSION_H
#define CLP_STRUCTURED_SEARCH_OR_EXPRESSION_H

#include "Expression.hpp"

namespace clp_structured { namespace search {
    /**
     * Class representing a logical Or operation across all
     * children in its OpList. Can have arbitrarily many children.
     */
    class OrExpr : public Expression {
    public:
        /**
         * Create an empty Or expression which can optionally be inverted and attached to a parent
         * Children can be added via mutators inherited from Expression.
         * @param inverted expression is inverted when true
         * @param parent parent this expression is attached to
         * @return A newly created Or expression
         */
        static std::shared_ptr<Expression>
        create(bool inverted = false, Expression* parent = nullptr);

        /**
         * Create an Or expression with two children
         * @param op1 the first child operand
         * @param op2 the second child operand
         * @param inverted expression is inverted when true
         * @param parent parent this expression is attached to
         * @return A newly created Or expression
         */
        static std::shared_ptr<Expression> create(
                std::shared_ptr<Expression>& op1,
                std::shared_ptr<Expression>& op2,
                bool inverted = false,
                Expression* parent = nullptr
        );

        // Methods inherited from Value
        void print() override;

        // Methods inherited from Expression
        bool has_only_expression_operands() override { return true; }

        std::shared_ptr<Expression> copy() const override;

    private:
        // Constructor
        explicit OrExpr(bool inverted = false, Expression* parent = nullptr);

        OrExpr(OrExpr const&);
    };
}}  // namespace clp_structured::search

#endif  // CLP_STRUCTURED_SEARCH_OR_EXPRESSION_H
