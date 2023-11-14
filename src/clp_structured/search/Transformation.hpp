#ifndef CLP_STRUCTURED_SEARCH_TRANSFORMATION_H
#define CLP_STRUCTURED_SEARCH_TRANSFORMATION_H

#include "Expression.hpp"

namespace clp_structured {
/**
 * Generic class representing a transformation on some expression.
 */
class Transformation {
public:
    /**
     * Run the pass. The expression passed as input may be mutated by the pass.
     * @param expr the expression that the pass will run on
     * @return a new expression; may be the same as the input expression or different
     */
    virtual std::shared_ptr<Expression> run(std::shared_ptr<Expression>& expr) = 0;
};
}  // namespace clp_structured

#endif
