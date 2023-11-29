#ifndef CLP_STRUCTURED_SEARCH_CONVERT_TO_EXISTS_H
#define CLP_STRUCTURED_SEARCH_CONVERT_TO_EXISTS_H

#include "Transformation.hpp"

namespace clp_structured { namespace search {
    // Must run after NarrowTypes pass
    class ConvertToExists : public Transformation {
    public:
        // Constructors
        ConvertToExists() : m_needs_constant_prop(false), m_needs_standard_form(false) {}

        // Methods inherited from Transformation
        std::shared_ptr<Expression> run(std::shared_ptr<Expression>& expr) override;

    private:
        bool m_needs_constant_prop;
        bool m_needs_standard_form;

        /**
         * Convert an expression to exists form
         * @param cur the expression to convert
         * @return A new expression in exists form
         */
        std::shared_ptr<Expression> convert(std::shared_ptr<Expression> cur);
    };
}}  // namespace clp_structured::search

#endif  // CLP_STRUCTURED_SEARCH_CONVERT_TO_EXISTS_H
