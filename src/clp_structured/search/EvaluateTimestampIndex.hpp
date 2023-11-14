#ifndef CLP_STRUCTURED_SEARCH_EVALUATETIMESTAMPINDEX_H
#define CLP_STRUCTURED_SEARCH_EVALUATETIMESTAMPINDEX_H

#include "Expression.hpp"
#include "src/clp_structured/TimestampDictionaryReader.hpp"
#include "src/clp_structured/Utils.hpp"

namespace clp_structured {
class EvaluateTimestampIndex {
public:
    EvaluateTimestampIndex(std::shared_ptr<TimestampDictionaryReader> const& timestamp_dict)
            : m_timestamp_dict(timestamp_dict) {}

    /**
     * Takes an expression and attempts to prove its output (true/false/unknown) based on
     * a timestamp index. Currently doesn't do any constant propagation.
     *
     * Should only be run after type narrowing.
     *
     * @param expr the expression to evaluate against the timestamp index
     * @return The evaluated value of the expression given the index (True, False, Unknown)
     */
    EvaluatedValue run(std::shared_ptr<Expression> const& expr);

private:
    std::shared_ptr<TimestampDictionaryReader> m_timestamp_dict;
};
}  // namespace clp_structured

#endif  // CLP_STRUCTURED_SEARCH_EVALUATETIMESTAMPINDEX_H
