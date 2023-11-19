#ifndef CLP_STRUCTURED_SEARCH_FILTER_OPERATION_H
#define CLP_STRUCTURED_SEARCH_FILTER_OPERATION_H

namespace clp_structured { namespace search {
    /**
     * Enum describing all supported filtering operations in the search AST
     */
    enum FilterOperation {
        EXISTS,
        NEXISTS,
        EQ,
        NEQ,
        LT,
        GT,
        LTE,
        GTE
    };
}}  // namespace clp_structured::search

#endif  // CLP_STRUCTURED_SEARCH_FILTER_OPERATION_H
