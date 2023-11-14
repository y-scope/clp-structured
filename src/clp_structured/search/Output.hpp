#ifndef CLP_STRUCTURED_SEARCH_OUTPUT_H
#define CLP_STRUCTURED_SEARCH_OUTPUT_H

#include <map>
#include <set>
#include <string>
#include <unordered_set>
#include <utility>

#include "clp_search/Query.hpp"
#include "Expression.hpp"
#include "Integral.hpp"
#include "SchemaMatch.hpp"
#include "src/clp_structured/SchemaReader.hpp"
#include "src/clp_structured/Utils.hpp"
#include "StringLiteral.hpp"

#include "submodules/simdjson/include/simdjson.h"

using namespace simdjson;

namespace clp_structured {
class Output : public FilterClass {
public:
    Output(std::shared_ptr<SchemaTree> tree,
           std::shared_ptr<ReaderUtils::SchemaMap> schemas,
           SchemaMatch& match,
           std::shared_ptr<Expression> expr,
           std::string archive_dir,
           std::shared_ptr<TimestampDictionaryReader> timestamp_dict)
            : m_schema_tree(std::move(tree)),
              m_schemas(std::move(schemas)),
              m_match(match),
              m_expr(std::move(expr)),
              m_archive_dir(std::move(archive_dir)),
              m_timestamp_dict(std::move(timestamp_dict)) {}

    void filter();

private:
    SchemaMatch& m_match;
    std::shared_ptr<Expression> m_expr;
    std::string m_archive_dir;

    // variables for the current schema being filtered
    std::vector<BaseColumnReader*> m_searched_columns;
    std::vector<BaseColumnReader*> m_other_columns;
    int32_t m_schema;
    SchemaReader* m_reader;

    std::shared_ptr<SchemaTree> m_schema_tree;
    std::shared_ptr<VariableDictionaryReader> m_var_dict;
    std::shared_ptr<LogTypeDictionaryReader> m_log_dict;
    std::shared_ptr<LogTypeDictionaryReader> m_array_dict;
    std::shared_ptr<TimestampDictionaryReader> m_timestamp_dict;

    std::shared_ptr<ReaderUtils::SchemaMap> m_schemas;

    std::map<std::string, Query> m_string_query_map;
    std::map<std::string, std::unordered_set<int64_t>> m_string_var_match_map;
    std::unordered_map<Expression*, Query*> m_expr_clp_query;
    std::unordered_map<Expression*, std::unordered_set<int64_t>*> m_expr_var_match_map;
    std::map<int32_t, ClpStringColumnReader*> m_clp_string_readers;
    std::map<int32_t, VariableStringColumnReader*> m_var_string_readers;
    std::map<int32_t, DateStringColumnReader*> m_datestring_readers;
    std::map<int32_t, FloatDateStringColumnReader*> m_floatdatestring_readers;
    uint64_t m_cur_message;
    EvaluatedValue m_expression_value;

    std::map<ColumnDescriptor*, std::vector<int32_t>> m_wildcard_to_searched_clpstrings;
    std::map<ColumnDescriptor*, std::vector<int32_t>> m_wildcard_to_searched_varstrings;
    std::map<ColumnDescriptor*, std::vector<int32_t>> m_wildcard_to_searched_datestrings;
    std::map<ColumnDescriptor*, std::vector<int32_t>> m_wildcard_to_searched_floatdatestrings;
    std::map<ColumnDescriptor*, std::vector<int32_t>> m_wildcard_to_searched_columns;

    simdjson::ondemand::parser m_array_parser;
    std::string m_array_search_string;
    bool m_maybe_string, m_maybe_number;
    // init is called once for each schema after which filter is called once for every message
    // in the schema
    void
    init(SchemaReader* reader, int32_t schema_id, std::vector<BaseColumnReader*>& columns) override;

    bool filter(
            uint64_t cur_message,
            std::map<int32_t, std::variant<int64_t, double, std::string, uint8_t>>& extracted_values
    ) override;

    bool evaluate(
            Expression* expr,
            int32_t schema,
            std::map<int32_t, std::variant<int64_t, double, std::string, uint8_t>>& extracted_values
    );

    bool evaluate_filter(
            FilterExpr* expr,
            int32_t schema,
            std::map<int32_t, std::variant<int64_t, double, std::string, uint8_t>>& extracted_values
    );
    bool evaluate_wildcard_filter(
            FilterExpr* expr,
            int32_t schema,
            std::map<int32_t, std::variant<int64_t, double, std::string, uint8_t>>& extracted_values
    );

    static bool
    evaluate_int_filter(FilterOperation op, int64_t value, std::shared_ptr<Literal> const& operand);

    static bool evaluate_float_filter(
            FilterOperation op,
            double value,
            std::shared_ptr<Literal> const& operand
    );

    bool evaluate_clp_string_filter(
            FilterOperation op,
            Query* q,
            ClpStringColumnReader* reader,
            std::shared_ptr<Literal> const& operand
    ) const;

    bool evaluate_var_string_filter(
            FilterOperation op,
            VariableStringColumnReader* reader,
            std::unordered_set<int64_t>* matching_vars,
            std::shared_ptr<Literal> const& operand
    ) const;

    bool evaluate_epoch_date_filter(
            FilterOperation op,
            DateStringColumnReader* reader,
            std::shared_ptr<Literal>& operand
    );

    bool evaluate_float_date_filter(
            FilterOperation op,
            FloatDateStringColumnReader* reader,
            std::shared_ptr<Literal>& operand
    );

    bool evaluate_array_filter(
            FilterOperation op,
            DescriptorList const& unresolved_tokens,
            std::string const& value,
            std::shared_ptr<Literal> const& operand
    ) const;

    /**
     *
     * @param object
     * @param op
     * @param unresolved_tokens
     * @param cur_idx
     * @param value
     * @param operand
     * @param array_or_object if true, we are traversing an array
     * @return
     */
    bool evaluate_array_filter(
            json& object,
            FilterOperation op,
            DescriptorList const& unresolved_tokens,
            size_t cur_idx,
            std::shared_ptr<Literal> const& operand,
            bool array_or_object
    ) const;

    bool evaluate_wildcard_array_filter(
            FilterOperation op,
            std::string& value,
            std::shared_ptr<Literal> const& operand
    );

    bool evaluate_wildcard_array_filter(
            ondemand::array& array,
            FilterOperation op,
            std::shared_ptr<Literal> const& operand
    ) const;
    bool evaluate_wildcard_array_filter(
            ondemand::object& object,
            FilterOperation op,
            std::shared_ptr<Literal> const& operand
    ) const;

    static bool
    evaluate_bool_filter(FilterOperation op, bool value, std::shared_ptr<Literal> const& operand);

    void populate_string_queries(std::shared_ptr<Expression> const& expr);

    EvaluatedValue constant_propagate(std::shared_ptr<Expression> const& expr, int32_t schema_id);

    void populate_searched_wildcard_columns(std::shared_ptr<Expression> const& expr);

    void add_wildcard_columns_to_searched_columns();
};
}  // namespace clp_structured

#endif  // CLP_STRUCTURED_SEARCH_OUTPUT_H
