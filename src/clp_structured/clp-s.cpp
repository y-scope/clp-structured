#include <boost/program_options.hpp>
#include <boost/serialization/export.hpp>

#include "JsonConstructor.hpp"
#include "JsonParser.hpp"
#include "ReaderUtils.hpp"
#include "search/ConvertToExists.hpp"
#include "search/EmptyExpr.hpp"
#include "search/EvaluateTimestampIndex.hpp"
#include "search/Kibana/Kibana.hpp"
#include "search/NarrowTypes.hpp"
#include "search/OrOfAndForm.hpp"
#include "search/Output.hpp"
#include "search/SchemaMatch.hpp"
#include "TimestampPattern.hpp"
#include "Utils.hpp"

namespace po = boost::program_options;

enum class Command : char {
    Compress = 'c',
    Extract = 'x',
    Search = 'q'
};

int main(int argc, char const* argv[]) {
    po::options_description general_options("General options");
    char command_input;
    general_options.add_options()("command", po::value<char>(&command_input))(
            "command-args",
            po::value<std::vector<std::string>>()
    );

    po::positional_options_description general_positional_options_description;
    general_positional_options_description.add("command", 1);
    general_positional_options_description.add("command-args", -1);

    po::variables_map parsed_command_line_options;
    po::parsed_options parsed = po::command_line_parser(argc, argv)
                                        .options(general_options)
                                        .positional(general_positional_options_description)
                                        .allow_unregistered()
                                        .run();
    po::store(parsed, parsed_command_line_options);
    po::notify(parsed_command_line_options);

    if (command_input == (char)Command::Compress) {
        std::vector<std::string> file_paths;
        std::string timestamp_key;
        std::string output_dir;
        int compression_level;
        size_t max_encoding_size;

        po::options_description compression_options("Compressions options");
        compression_options.add_options()
            ("help", "Print help")
            ("compression-level", po::value<int>(&compression_level)->value_name("LEVEL")->default_value(3), "set compression level")
            ("max-encoding-size", po::value<size_t>(&max_encoding_size)->value_name("MAX_DICT_SIZE")->default_value(8UL * 1024 * 1024 * 1024), // 8GB
             "maximum encoding size")
            ("output-dir", po::value<std::string>(&output_dir)->value_name("FILES"), "output directory")
            ("input-paths", po::value<std::vector<std::string> >(&file_paths)->value_name("PATHS"), "input paths")
            ("timestamp-key", po::value<std::string>(&timestamp_key)->value_name("TIMESTAMPCOL")->default_value(""), "timestamp column");

        po::positional_options_description positional_options;
        positional_options.add("output-dir", 1);
        positional_options.add("input-paths", -1);

        std::vector<std::string> unrecognized_options
                = po::collect_unrecognized(parsed.options, po::include_positional);
        unrecognized_options.erase(unrecognized_options.begin());
        po::store(
                po::command_line_parser(unrecognized_options)
                        .options(compression_options)
                        .positional(positional_options)
                        .run(),
                parsed_command_line_options
        );
        po::notify(parsed_command_line_options);

        if (file_paths.empty()) {
            return -1;
        }

        clp_structured::TimestampPattern::init();

        clp_structured::JsonParserOption option;
        option.file_paths = std::move(file_paths);
        option.output_dir = std::move(output_dir);
        option.max_encoding_size = max_encoding_size;
        option.compression_level = compression_level;
        if (!timestamp_key.empty()) {
            clp_structured::StringUtils::tokenize_column_descriptor(
                    timestamp_key,
                    option.timestamp_column
            );
        }

        clp_structured::JsonParser parser(option);
        parser.parse();
        parser.store();
        parser.close();
    } else if (command_input == (char)Command::Extract) {
        std::string archive_dir;
        std::string output_dir;

        po::options_description extraction_options;
        extraction_options.add_options()("archive-dir", po::value<std::string>(&archive_dir))(
                "output-dir",
                po::value<std::string>(&output_dir)
        );

        po::positional_options_description positional_options;
        positional_options.add("archive-dir", 1);
        positional_options.add("output-dir", 1);

        std::vector<std::string> unrecognized_options
                = po::collect_unrecognized(parsed.options, po::include_positional);
        unrecognized_options.erase(unrecognized_options.begin());
        po::store(
                po::command_line_parser(unrecognized_options)
                        .options(extraction_options)
                        .positional(positional_options)
                        .run(),
                parsed_command_line_options
        );

        po::notify(parsed_command_line_options);

        clp_structured::JsonConstructorOption option;
        option.archive_dir = std::move(archive_dir);
        option.output_dir = std::move(output_dir);

        clp_structured::JsonConstructor constructor(option);
        constructor.construct();
        constructor.store();
        constructor.close();
    } else if (command_input == (char)Command::Search) {
        std::string archive_dir;
        std::string query;

        po::options_description search_options;
        search_options.add_options()("archive-dir", po::value<std::string>(&archive_dir))(
                "query,q",
                po::value<std::string>(&query)
        );

        po::positional_options_description positional_options;
        positional_options.add("archive-dir", 1);
        positional_options.add("query", 1);

        std::vector<std::string> unrecognized_options
                = po::collect_unrecognized(parsed.options, po::include_positional);
        unrecognized_options.erase(unrecognized_options.begin());
        po::store(
                po::command_line_parser(unrecognized_options)
                        .options(search_options)
                        .positional(positional_options)
                        .run(),
                parsed_command_line_options
        );

        po::notify(parsed_command_line_options);

        // Kibana parsing may end up parsing timestamps. Initialize timestamp
        // parsing module first here.
        clp_structured::TimestampPattern::init();

        auto query_stream = std::istringstream(query);
        auto expr = clp_structured::parse_kibana_expression(query_stream);

        if (std::dynamic_pointer_cast<clp_structured::EmptyExpr>(expr)) {
            return 1;
        }

        clp_structured::OrOfAndForm standardize_pass;
        expr = standardize_pass.run(expr);

        if (std::dynamic_pointer_cast<clp_structured::EmptyExpr>(expr)) {
            return 1;
        }

        clp_structured::NarrowTypes narrow_pass;
        expr = narrow_pass.run(expr);

        if (std::dynamic_pointer_cast<clp_structured::EmptyExpr>(expr)) {
            return 1;
        }

        clp_structured::ConvertToExists convert_pass;
        expr = convert_pass.run(expr);

        if (std::dynamic_pointer_cast<clp_structured::EmptyExpr>(expr)) {
            return 1;
        }

        // skip decompressing the archive if we won't match based on
        // the timestamp index
        auto timestamp_dict = clp_structured::ReaderUtils::read_timestamp_dictionary(archive_dir);
        clp_structured::EvaluateTimestampIndex timestamp_index(timestamp_dict);
        if (timestamp_index.run(expr) == clp_structured::EvaluatedValue::False) {
            return 1;
        }

        auto schema_tree = clp_structured::ReaderUtils::read_schema_tree(archive_dir);
        auto schemas = clp_structured::ReaderUtils::read_schemas(archive_dir);

        // Narrow against schemas
        clp_structured::SchemaMatch match_pass(schema_tree, schemas);
        expr = match_pass.run(expr);

        if (std::dynamic_pointer_cast<clp_structured::EmptyExpr>(expr)) {
            return 1;
        }

        // output result
        clp_structured::Output
                output(schema_tree, schemas, match_pass, expr, archive_dir, timestamp_dict);
        output.filter();

        return 0;
    } else {
        throw std::invalid_argument(std::string("Unknown action '") + command_input + "'");
    }

    return 0;
}
