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
#include "spdlog/sinks/stdout_sinks.h"
#include "TimestampPattern.hpp"
#include "Utils.hpp"

namespace po = boost::program_options;

using namespace clp_structured::search;

enum class Command : char {
    Compress = 'c',
    Extract = 'x',
    Search = 's'
};

void print_basic_usage() {
    std::cerr << "Usage: "
              << "clp-s [OPTIONS] COMMAND [COMMAND ARGUMENTS]\n";
}

void print_compression_basic_usage() {
    std::cerr << "Usage: "
              << "clp-s [OPTIONS] c OUTPUT_DIR [FILE/DIR ...]\n";
}

void print_extraction_basic_usage() {
    std::cerr << "Usage: "
              << "clp-s [OPTIONS] x ARCHIVES_DIR OUTPUT_DIR\n";
}

void print_search_basic_usage() {
    std::cerr << "Usage: "
              << "clp-s [OPTIONS] s ARCHIVES_DIR QUERY\n";
}

int main(int argc, char const* argv[]) {
    try {
        auto stderr_logger = spdlog::stderr_logger_st("stderr");
        spdlog::set_default_logger(stderr_logger);
        spdlog::set_pattern("%Y-%m-%dT%H:%M:%S.%e%z [%l] %v");
    } catch (std::exception& e) {
        // NOTE: We can't log an exception if the logger couldn't be constructed
        return -1;
    }

    po::options_description general_options("General options");
    general_options.add_options()("help,h", "Print help");

    char command_input;
    po::options_description general_positional_options("General positional options");
    general_positional_options.add_options()("command", po::value<char>(&command_input))(
            "command-args",
            po::value<std::vector<std::string>>()
    );

    po::positional_options_description general_positional_options_description;
    general_positional_options_description.add("command", 1);
    general_positional_options_description.add("command-args", -1);

    po::options_description all_descriptions;
    all_descriptions.add(general_options);
    all_descriptions.add(general_positional_options);

    try {
        po::variables_map parsed_command_line_options;
        po::parsed_options parsed = po::command_line_parser(argc, argv)
                                            .options(all_descriptions)
                                            .positional(general_positional_options_description)
                                            .allow_unregistered()
                                            .run();
        po::store(parsed, parsed_command_line_options);
        po::notify(parsed_command_line_options);

        if (parsed_command_line_options.count("command") == 0
            && parsed_command_line_options.count("help") != 0)
        {
            print_basic_usage();

            std::cerr << "COMMAND is one of:\n";
            std::cerr << "  c - compress\n";
            std::cerr << "  x - decompress\n";
            std::cerr << "  s - search\n\n";
            std::cerr << "Try "
                      << " c --help OR "
                      << " x --help OR "
                      << "q --help for command-specific details.\n\n"
                      << "Options can be specified on the command line.\n";
            po::options_description visible_options;
            visible_options.add(general_options);
            std::cerr << visible_options << '\n';
            return 0;
        }

        if ((char)Command::Compress == command_input) {
            std::vector<std::string> file_paths;
            std::string timestamp_key;
            std::string output_dir;
            int compression_level;
            size_t max_encoding_size;
            po::options_description compression_positional_options;
            compression_positional_options
                    .add_options()("output-dir", po::value<std::string>(&output_dir)->value_name("DIR"), "output directory")(
                            "input-paths",
                            po::value<std::vector<std::string>>(&file_paths)->value_name("PATHS"),
                            "input paths"
                    );

            po::options_description compression_options("Compressions options");
            compression_options.add_options()
            ("help", "Print help")
            ("compression-level", po::value<int>(&compression_level)->value_name("LEVEL")->
                    default_value(3), "set compression level")
            ("max-encoding-size", po::value<size_t>(&max_encoding_size)->
                    value_name("MAX_DICT_SIZE")->default_value(8UL * 1024 * 1024 * 1024), /* 8GB*/
                    "maximum encoding size")
            ("timestamp-key", po::value<std::string>(&timestamp_key)->value_name("TIMESTAMPCOL")->
                    default_value(""), "timestamp column");

            po::positional_options_description positional_options;
            positional_options.add("output-dir", 1);
            positional_options.add("input-paths", -1);

            po::options_description all_compression_options;
            all_compression_options.add(compression_options);
            all_compression_options.add(compression_positional_options);

            std::vector<std::string> unrecognized_options
                    = po::collect_unrecognized(parsed.options, po::include_positional);
            unrecognized_options.erase(unrecognized_options.begin());
            po::store(
                    po::command_line_parser(unrecognized_options)
                            .options(all_compression_options)
                            .positional(positional_options)
                            .run(),
                    parsed_command_line_options
            );
            po::notify(parsed_command_line_options);

            if (parsed_command_line_options.count("help")) {
                print_compression_basic_usage();
                std::cerr << "Examples:\n";
                std::cerr << "  Compress file1.txt and dir1 into the output dir\n";
                std::cerr << "   c output-dir file1.txt dir1\n\n";

                po::options_description visible_options;
                visible_options.add(general_options);
                visible_options.add(compression_options);
                std::cerr << visible_options << '\n';
                return 0;
            }
            if (file_paths.empty() || output_dir.empty()) {
                std::cerr << "Path unspecified\n";
                print_compression_basic_usage();
                return 1;
            }

            clp_structured::TimestampPattern::init();

            clp_structured::JsonParserOption option;
            option.file_paths = std::move(file_paths);
            option.output_dir = std::move(output_dir);
            option.max_encoding_size = max_encoding_size;
            option.compression_level = compression_level;
            if (false == timestamp_key.empty()) {
                clp_structured::StringUtils::tokenize_column_descriptor(
                        timestamp_key,
                        option.timestamp_column
                );
            }

            clp_structured::JsonParser parser(option);
            parser.parse();
            parser.store();
            parser.close();
        } else if ((char)Command::Extract == command_input) {
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

            if (parsed_command_line_options.count("help")) {
                print_extraction_basic_usage();

                std::cerr << "Examples:\n";
                std::cerr << "  Decompress all files from archives-dir into output-dir\n";
                std::cerr << "   x archives-dir output-dir\n\n";

                po::options_description visible_options;
                visible_options.add(general_options);
                std::cerr << visible_options << '\n';
                return 0;
            }
            if (archive_dir.empty() || output_dir.empty()) {
                std::cerr << "Path unspecified\n";
                print_extraction_basic_usage();
                return 1;
            }

            clp_structured::JsonConstructorOption option;
            option.archive_dir = std::move(archive_dir);
            option.output_dir = std::move(output_dir);

            clp_structured::JsonConstructor constructor(option);
            constructor.construct();
            constructor.store();
            constructor.close();
        } else if ((char)Command::Search == command_input) {
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

            if (parsed_command_line_options.count("help")) {
                print_search_basic_usage();

                std::cerr << "Examples:\n";
                std::cerr << "  Perform a query on archives-dir\n";
                std::cerr << "   s archives-dir query\n\n";

                po::options_description visible_options;
                visible_options.add(general_options);
                std::cerr << visible_options << '\n';
                return 0;
            }
            if (archive_dir.empty() || query.empty()) {
                std::cerr << "Path or query unspecified\n";
                print_search_basic_usage();
                return 1;
            }

            // Kibana parsing may end up parsing timestamps. Initialize timestamp
            // parsing module first here.
            clp_structured::TimestampPattern::init();

            auto query_stream = std::istringstream(query);
            auto expr = Kibana::parse_kibana_expression(query_stream);

            if (std::dynamic_pointer_cast<EmptyExpr>(expr)) {
                return 1;
            }

            OrOfAndForm standardize_pass;
            expr = standardize_pass.run(expr);

            if (std::dynamic_pointer_cast<EmptyExpr>(expr)) {
                return 1;
            }

            NarrowTypes narrow_pass;
            expr = narrow_pass.run(expr);

            if (std::dynamic_pointer_cast<EmptyExpr>(expr)) {
                return 1;
            }

            ConvertToExists convert_pass;
            expr = convert_pass.run(expr);

            if (std::dynamic_pointer_cast<EmptyExpr>(expr)) {
                return 1;
            }

            // skip decompressing the archive if we won't match based on
            // the timestamp index
            auto timestamp_dict
                    = clp_structured::ReaderUtils::read_timestamp_dictionary(archive_dir);
            EvaluateTimestampIndex timestamp_index(timestamp_dict);
            if (clp_structured::EvaluatedValue::False == timestamp_index.run(expr)) {
                return 1;
            }

            auto schema_tree = clp_structured::ReaderUtils::read_schema_tree(archive_dir);
            auto schemas = clp_structured::ReaderUtils::read_schemas(archive_dir);

            // Narrow against schemas
            SchemaMatch match_pass(schema_tree, schemas);
            expr = match_pass.run(expr);

            if (std::dynamic_pointer_cast<EmptyExpr>(expr)) {
                return 1;
            }

            // output result
            Output output(schema_tree, schemas, match_pass, expr, archive_dir, timestamp_dict);
            output.filter();

            return 0;
        } else {
            throw std::invalid_argument(std::string("Unknown action '") + command_input + "'");
        }

    } catch (std::exception& e) {
        spdlog::error("{}", e.what());
        return -1;
    }

    return 0;
}
