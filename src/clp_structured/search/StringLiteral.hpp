#ifndef CLP_STRUCTURED_SEARCH_STRING_H
#define CLP_STRUCTURED_SEARCH_STRING_H

#include <memory>
#include <string>

#include "Literal.hpp"

namespace clp_structured {
/**
 * Class for String literals in the search AST
 *
 * StringLiteral will automatically classify itself as possibly matching
 * a clp style (containing spaces) and/or variable style (not containing spaces)
 * string at creation time.
 */
class StringLiteral : public Literal {
public:
    // Deleted copy
    StringLiteral(StringLiteral const&) = delete;
    StringLiteral& operator=(StringLiteral const&) = delete;

    // Explicit create from string
    static std::shared_ptr<Literal> create(std::string const& v);

    /**
     * Strict checks for type matching against a given literal type.
     * @return true if the check succeeds
     */
    bool matches_type(LiteralType type) override { return type & m_string_type; }

    bool matches_any(LiteralTypeBitmask mask) override { return mask & m_string_type; }

    bool matches_exactly(LiteralTypeBitmask mask) override { return mask == m_string_type; }

    void print() override;

    /**
     * Get a reference to the underlying string
     * @return reference to underlying string
     */
    std::string& get();

    /**
     * Functions to check type conversion and cast when possible under a given filter operation
     * @param ret the casted value
     * @param op the FilterOperation operating on the Literal
     * @return true if cast is successful
     */
    bool as_clp_string(std::string& ret, FilterOperation op) override;
    bool as_var_string(std::string& ret, FilterOperation op) override;
    bool as_float(double& ret, FilterOperation op) override;
    bool as_int(int64_t& ret, FilterOperation op) override;
    bool as_bool(bool& ret, FilterOperation op) override;
    bool as_null(FilterOperation op) override;
    bool as_any(FilterOperation op) override;

private:
    std::string m_v;
    LiteralTypeBitmask m_string_type;

    explicit StringLiteral(std::string v) : m_v(std::move(v)), m_string_type(0) {
        if (m_v.find(' ') != std::string::npos) {
            m_string_type = LiteralType::ClpStringT;
        } else {
            m_string_type = LiteralType::VarStringT;
        }

        if (m_v.find('*') != std::string::npos) {
            m_string_type |= LiteralType::ClpStringT;
        }
    }
};
}  // namespace clp_structured

#endif  // CLP_STRUCTURED_SEARCH_STRING_H
