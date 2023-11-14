#ifndef CLP_STRUCTURED_SEARCH_DATELITERAL_H
#define CLP_STRUCTURED_SEARCH_DATELITERAL_H

#include <memory>

#include "clp_search/Defs.hpp"
#include "Integral.hpp"

namespace clp_structured {
constexpr LiteralTypeBitmask cDateLiteralTypes = EpochDateT | FloatDateT;

/**
 * Class for Date literal in the search AST. Represents time
 * in epoch time.
 */
class DateLiteral : public Integral {
public:
    // Deleted copy
    DateLiteral(DateLiteral const&) = delete;
    DateLiteral& operator=(DateLiteral const&) = delete;

    /**
     * Create a Date literal from an integral value
     * @param v the time as a double or epoch
     * @return a Date literal
     */
    static std::shared_ptr<Literal> create_from_float(double v);
    static std::shared_ptr<Literal> create_from_int(epochtime_t v);

    /**
     * Attempt to create a Date literal from string. Tries to parse the string using
     * TimestampPattern.
     * @return a Date Literal or nullptr if the string can not be parsed as date.
     */
    static std::shared_ptr<Literal> create_from_string(std::string const& v);

    /**
     * Strict checks for type matching against a given literal type.
     * @return true if the check succeeds
     */
    bool matches_type(LiteralType type) override { return type & cDateLiteralTypes; }

    bool matches_any(LiteralTypeBitmask mask) override { return mask & cDateLiteralTypes; }

    bool matches_exactly(LiteralTypeBitmask mask) override { return mask == cDateLiteralTypes; }

    void print() override;

    /**
     * Functions to check type conversion and cast when possible under a given filter operation
     * @param ret the casted value
     * @param op the FilterOperation operating on the Literal
     * @return true if cast is successful
     */
    bool as_epoch_date() override { return true; }

    bool as_float_date() override { return true; }

    bool as_clp_string(std::string& ret, FilterOperation op) override;

    bool as_var_string(std::string& ret, FilterOperation op) override;

private:
    std::string m_epoch_str;

    explicit DateLiteral(double v, std::string s);
    explicit DateLiteral(epochtime_t v, std::string s);
};
}  // namespace clp_structured

#endif
