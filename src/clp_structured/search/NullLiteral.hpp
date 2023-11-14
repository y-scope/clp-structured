#ifndef CLP_STRUCTURED_SEARCH_NULL_H
#define CLP_STRUCTURED_SEARCH_NULL_H

#include <memory>
#include <string>
#include <variant>

#include "Literal.hpp"

namespace clp_structured {
/**
 * Class for Null literals in the search AST
 */
class NullLiteral : public Literal {
public:
    // Deleted copy
    NullLiteral(NullLiteral const&) = delete;
    NullLiteral& operator=(NullLiteral const&) = delete;

    /**
     * Explicit create
     * @return a newly created null literal
     */
    static std::shared_ptr<Literal> create();

    /**
     * Try to create a null literal from a string
     * @param v the string we are attempting to convert to Null
     * @return a null literal, or nullptr if the string does not represent "null"
     */
    static std::shared_ptr<Literal> create_from_string(std::string const& v);

    /**
     * Strict checks for type matching against a given literal type.
     * @return true if the check succeeds
     */
    bool matches_type(LiteralType type) override { return type & LiteralType::NullT; }

    bool matches_any(LiteralTypeBitmask mask) override { return mask & LiteralType::NullT; }

    bool matches_exactly(LiteralTypeBitmask mask) override { return mask == LiteralType::NullT; }

    void print() override;

    /**
     * Functions to check type conversion and cast when possible under a given filter operation
     * @param ret the casted value
     * @param op the FilterOperation operating on the Literal
     * @return true if cast is successful
     */
    bool as_var_string(std::string& ret, FilterOperation op) override;
    bool as_null(FilterOperation op) override;

private:
    NullLiteral() = default;
};
}  // namespace clp_structured

#endif  // CLP_STRUCTURED_SEARCH_NULL_H
