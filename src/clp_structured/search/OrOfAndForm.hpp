#ifndef CLP_STRUCTURED_SEARCH_OR_OF_AND_FORM_H
#define CLP_STRUCTURED_SEARCH_OR_OF_AND_FORM_H

#include <vector>

#include "AndExpr.hpp"
#include "OrExpr.hpp"
#include "Transformation.hpp"

namespace clp_structured {
typedef std::vector<std::shared_ptr<Expression>> ExpressionVector;
typedef std::list<std::shared_ptr<Expression>> ExpressionList;

// TODO: handle degenerate forms like empty or/and expressions

class OrOfAndForm : public Transformation {
public:
    std::shared_ptr<Expression> run(std::shared_ptr<Expression>& expr) override;

private:
    static void de_morgan(std::shared_ptr<Expression>& expr);

    static std::shared_ptr<Expression> simplify(std::shared_ptr<Expression> const& expr);

    static std::shared_ptr<Expression> simplify_or(std::shared_ptr<Expression> const& expr);

    static std::shared_ptr<Expression> simplify_and(std::shared_ptr<Expression> const& expr);

    static void insert_all_combinations(
            std::shared_ptr<Expression> const& new_or_expr,
            std::shared_ptr<Expression> const& base_and_expr,
            ExpressionVector::iterator cur,
            ExpressionVector::iterator end,
            ExpressionList& prefix
    );
};
}  // namespace clp_structured

#endif  // CLP_STRUCTURED_SEARCH_OR_OF_AND_FORM_H
