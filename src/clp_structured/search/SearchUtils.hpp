#ifndef CLP_STRUCTURED_SEARCH_SEARCH_UTILS_H
#define CLP_STRUCTURED_SEARCH_SEARCH_UTILS_H

#include "Expression.hpp"
#include "Literal.hpp"
#include "src/clp_structured/SchemaTree.hpp"

namespace clp_structured { namespace search {

    /**
     * Splice a child expression into a parent expression at a given location
     * @param parent
     * @param child
     * @param location
     */
    void splice_into(
            std::shared_ptr<Expression> const& parent,
            std::shared_ptr<Expression> const& child,
            OpList::iterator location
    );

    /**
     * Converts a node type to a literal type
     * @param type
     * @return A literal type
     */
    LiteralType node_to_literal_type(NodeType type);

    /**
     * Casts a double to an int64_t, rounding up or down depending on the filter operation
     * @param in
     * @param op
     * @param out
     * @return false if under FilterOperation::EQ the cast double is not equal to int64_t out, true
     * otherwise
     */
    bool double_as_int(double in, FilterOperation op, int64_t& out);

    /**
     * Performs a wildcard match of a string against a pattern
     * @param s the string to match
     * @param p the pattern to match against
     * @return true if s matches p, false otherwise
     */
    bool wildcard_match(std::string_view s, std::string_view p);
}}  // namespace clp_structured::search

#endif  // CLP_STRUCTURED_SEARCH_SEARCH_UTILS_H
