#ifndef CLP_STRUCTURED_SEARCH_SCHEMA_MATCH_H
#define CLP_STRUCTURED_SEARCH_SCHEMA_MATCH_H

#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include "ColumnDescriptor.hpp"
#include "Expression.hpp"
#include "FilterExpr.hpp"
#include "Literal.hpp"
#include "src/clp_structured/ReaderUtils.hpp"
#include "Transformation.hpp"

namespace clp_structured {
class SchemaMatch : public Transformation {
public:
    SchemaMatch(std::shared_ptr<SchemaTree> tree, std::shared_ptr<ReaderUtils::SchemaMap> schemas);

    std::shared_ptr<Expression> run(std::shared_ptr<Expression>& expr) override;

    std::shared_ptr<Expression> get_query_for_schema(int32_t schema);

    bool schema_matched(int32_t schema_id);

    bool schema_searches_against_column(int32_t schema, int32_t column_id);
    // only used for pure wildcard
    void add_searched_column_to_schema(int32_t schema, int32_t column);

    bool has_array(int32_t schema_id);

    bool has_array_search(int32_t schema_id);

private:
    std::unordered_map<uint32_t, std::set<ColumnDescriptor*>> m_column_to_descriptor;
    // TODO: The value in the map can be a set of k:v pairs with a hash & comparison
    // that only considers the key since each column descriptor only has one matching
    // column id per schema
    std::unordered_map<ColumnDescriptor*, std::map<int32_t, int32_t>> m_descriptor_to_schema;
    std::map<ColumnDescriptor*, std::set<int32_t>> m_unresolved_descriptor_to_descriptor;
    std::unordered_map<Expression*, std::unordered_set<int32_t>> m_expression_to_schemas;
    std::unordered_set<int32_t> m_matched_schema_ids;
    std::unordered_set<int32_t> m_array_schema_ids;
    std::unordered_set<int32_t> m_array_search_schema_ids;
    std::map<int32_t, std::shared_ptr<Expression>> m_schema_to_query;

    std::unordered_map<int32_t, std::set<int32_t>> m_schema_to_searched_columns;
    std::shared_ptr<SchemaTree> m_tree;
    std::shared_ptr<ReaderUtils::SchemaMap> m_schemas;

    bool populate_column_mapping(
            ColumnDescriptor* column,
            DescriptorList::iterator it,
            int32_t node_id,
            bool wildcard_special_flag = false
    );

    bool populate_column_mapping(ColumnDescriptor* column);

    std::shared_ptr<Expression> populate_column_mapping(std::shared_ptr<Expression> cur);

    void populate_schema_mapping();

    std::shared_ptr<Expression> intersect_schemas(std::shared_ptr<Expression> cur);

    bool intersect_and_sub_expr(
            std::shared_ptr<Expression> const& cur,
            std::set<int32_t>& common_schema,
            std::set<ColumnDescriptor*>& columns,
            bool first
    );

    void split_expression_by_schema(
            std::shared_ptr<Expression> const& expr,
            std::map<int32_t, std::shared_ptr<Expression>>& queries,
            std::unordered_set<int32_t> const& relevant_schemas
    );

    int32_t get_column_id_for_descriptor(ColumnDescriptor* column, int32_t schema);

    LiteralType get_literal_type_for_column(ColumnDescriptor* column, int32_t schema);
};
}  // namespace clp_structured

#endif  // CLP_STRUCTURED_SEARCH_SCHEMA_MATCH_H
