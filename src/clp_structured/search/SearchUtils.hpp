#ifndef CLP_STRUCTURED_SEARCH_SEARCH_UTILS_H
#define CLP_STRUCTURED_SEARCH_SEARCH_UTILS_H

#include "Expression.hpp"
#include "Literal.hpp"
#include "src/clp_structured/SchemaTree.hpp"

namespace clp_structured {
void splice_into(
        std::shared_ptr<Expression> const& parent,
        std::shared_ptr<Expression> const& child,
        OpList::iterator location
);

LiteralType node_to_literal_type(NodeType type);

bool double_as_int(double in, FilterOperation op, int64_t& out);

bool wildcard_match(std::string_view s, std::string_view p);

// Iterators
// Ones that dynamically cast to a given type, and only show elements of that type within an oplist
// Ones that do the above but recursively walking entire expression tree from some point instead of
// just a single oplist
}  // namespace clp_structured

#endif  // CLP_STRUCTURED_SEARCH_SEARCH_UTILS_H
