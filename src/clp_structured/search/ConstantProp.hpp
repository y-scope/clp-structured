#ifndef CLP_STRUCTURED_SEARCH_CONSTANT_PROP_H
#define CLP_STRUCTURED_SEARCH_CONSTANT_PROP_H

#include "Transformation.hpp"

namespace clp_structured { namespace search {
    // Constant propagate empty expressions keeping all remaining data IN PLACE
    class ConstantProp : public Transformation {
    public:
        // Methods inherited from Transformation
        std::shared_ptr<Expression> run(std::shared_ptr<Expression>& expr) override;

    private:
        /**
         * Propagate empty expressions through the expression tree
         * @param cur
         * @return A new expression with empty expressions propagated
         */
        static std::shared_ptr<Expression> propagate_empty(std::shared_ptr<Expression> cur);
    };
}}  // namespace clp_structured::search

#endif  // CLP_STRUCTURED_SEARCH_CONSTANT_PROP_H
