#ifndef CLP_STRUCTURED_SEARCH_NARROW_TYPES_H
#define CLP_STRUCTURED_SEARCH_NARROW_TYPES_H

#include "Transformation.hpp"

namespace clp_structured { namespace search {
    class NarrowTypes : public Transformation {
    public:
        // Methods inherited from Transformation
        std::shared_ptr<Expression> run(std::shared_ptr<Expression>& expr) override;

    private:
        /**
         * Narrow the type of an expression
         * @param cur the expression to narrow
         * @return the narrowed expression
         */
        static std::shared_ptr<Expression> narrow(std::shared_ptr<Expression> cur);
    };
}}  // namespace clp_structured::search

#endif  // CLP_STRUCTURED_SEARCH_NARROW_TYPES_H
