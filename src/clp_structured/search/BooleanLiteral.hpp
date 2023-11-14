#ifndef CLP_STRUCTURED_SEARCH_BOOLEAN_H
#define CLP_STRUCTURED_SEARCH_BOOLEAN_H

#include <memory>
#include <string>
#include <variant>

#include "Literal.hpp"

namespace clp_structured {
/**
 * Class representing a Boolean literal in the search AST
 */
class BooleanLiteral : public Literal {
public:
    // Deleted copy
    BooleanLiteral(BooleanLiteral const&) = delete;
    BooleanLiteral& operator=(BooleanLiteral const&) = delete;

    /**
     * Create a bool literal
     * @param v the value of the boolean
     * @return a Boolean literal
     */
    static std::shared_ptr<Literal> create_from_bool(bool v);

    /**
     * Attempt to create a bool literal from a string
     * @param v the string we are attempting to convert to bool
     * @return a Boolean literal, or nullptr if the string does not represent a bool
     */
    static std::shared_ptr<Literal> create_from_string(std::string const& v);

    /**
     * Strict checks for type matching against a given literal type.
     * @return true if the check succeeds
     */
    bool matches_type(LiteralType type) override { return type & LiteralType::BooleanT; }

    bool matches_any(LiteralTypeBitmask mask) override { return mask & LiteralType::BooleanT; }

    bool matches_exactly(LiteralTypeBitmask mask) override { return mask == LiteralType::BooleanT; }

    void print() override;

    /**
     * Functions to check type conversion and cast when possible under a given filter operation
     * @param ret the casted value
     * @param op the FilterOperation operating on the Literal
     * @return true if cast is successful
     */
    bool as_var_string(std::string& ret, FilterOperation op) override;
    bool as_bool(bool& ret, FilterOperation op) override;

private:
    bool m_v;

    BooleanLiteral() = default;
    explicit BooleanLiteral(bool v) : m_v(v){};
};
}  // namespace clp_structured

#endif  // CLP_STRUCTURED_SEARCH_BOOLEAN_H
