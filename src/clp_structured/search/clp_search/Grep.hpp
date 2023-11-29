// Code from CLP

#ifndef CLP_STRUCTURED_SEARCH_CLP_SEARCH_GREP_HPP
#define CLP_STRUCTURED_SEARCH_CLP_SEARCH_GREP_HPP

#include <string>

#include "Query.hpp"
#include "src/clp_structured/Defs.hpp"
// FIXME: refactor includes here?
#include "src/clp_structured/DictionaryReader.hpp"

namespace clp_structured { namespace search { namespace clp_search {
    class Grep {
    public:
        // Methods
        /**
         * Processes a raw user query into a Query
         * @param archive
         * @param search_string
         * @param search_begin_ts
         * @param search_end_ts
         * @param ignore_case
         * @param query
         * @return true if query may match messages, false otherwise
         */
        static bool process_raw_query(
                std::shared_ptr<LogTypeDictionaryReader> log_dict,
                std::shared_ptr<VariableDictionaryReader> var_dict,
                std::string const& search_string,
                bool ignore_case,
                Query& query,
                bool add_wildcards = true,
                bool use_heuristic = true
        );

        /**
         * Returns bounds of next potential variable (either a definite variable or a token with
         * wildcards)
         * @param value String containing token
         * @param begin_pos Begin position of last token, changes to begin position of next token
         * @param end_pos End position of last token, changes to end position of next token
         * @param is_var Whether the token is definitely a variable
         * @return true if another potential variable was found, false otherwise
         */
        static bool get_bounds_of_next_potential_var(
                std::string const& value,
                size_t& begin_pos,
                size_t& end_pos,
                bool& is_var
        );
    };
}}}  // namespace clp_structured::search::clp_search

#endif  // CLP_STRUCTURED_SEARCH_CLP_SEARCH_GREP_HPP
