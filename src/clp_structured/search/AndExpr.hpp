#ifndef CLP_STRUCTURED_SEARCH_AND_EXPRESSION_H
#define CLP_STRUCTURED_SEARCH_AND_EXPRESSION_H

#include "Expression.hpp"

namespace clp_structured { namespace search {
    /**
     * Class representing a logical And operation across all
     * children in its OpList. Can have arbitrarily many children.
     */
    class AndExpr : public Expression {
    public:
        void print() override;

        /**
         * And expressions only have other expressions as children by construction
         */
        bool has_only_expression_operands() override { return true; }

        /**
         * Deep copy
         * @return A deep copy of this expression
         */
        std::shared_ptr<Expression> copy() const override;

        /**
         * Create an empty And expression which can optionally be inverted and attached to a parent.
         * Children can be added via mutators inherited from Expression.
         * @param inverted expression is inverted when true
         * @param parent parent this expression is attached to
         * @return Newly created Or expression
         */
        static std::shared_ptr<Expression>
        create(bool inverted = false, Expression* parent = nullptr);

        /**
         * Create an And expression with two children
         * @param op1 the first child operand
         * @param op2 the second child operand
         * @param inverted expression is inverted when true
         * @param parent parent this expression is attached to
         * @return Newly created Or expression
         */
        static std::shared_ptr<Expression> create(
                std::shared_ptr<Expression>& op1,
                std::shared_ptr<Expression>& op2,
                bool inverted = false,
                Expression* parent = nullptr
        );

    private:
        // Constructor
        explicit AndExpr(bool inverted = false, Expression* parent = nullptr);

        AndExpr(AndExpr const&);
    };
}}  // namespace clp_structured::search

#endif  // CLP_STRUCTURED_SEARCH_AND_EXPRESSION_H
