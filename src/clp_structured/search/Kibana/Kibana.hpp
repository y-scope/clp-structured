#ifndef CLP_STRUCTURED_PARSE_KIBANA_H
#define CLP_STRUCTURED_PARSE_KIBANA_H

#include <istream>

#include "src/clp_structured/search/Expression.hpp"

namespace clp_structured { namespace search { namespace Kibana {
    /**
     * Generate a search AST from a Kibana expression in an input stream
     * @param in input stream containing a Kibana expression followed by EOF
     * @return a search AST
     */
    std::shared_ptr<Expression> parse_kibana_expression(std::istream& in);
}}}  // namespace clp_structured::search::Kibana

#endif  // CLP_STRUCTURED_PARSE_KIBANA_H
