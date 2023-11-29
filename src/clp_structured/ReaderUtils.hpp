#ifndef CLP_STRUCTURED__READERUTILS_HPP
#define CLP_STRUCTURED__READERUTILS_HPP

#include "DictionaryReader.hpp"
#include "SchemaReader.hpp"
#include "SchemaTree.hpp"
#include "TimestampDictionaryReader.hpp"
#include "TraceableException.hpp"

namespace clp_structured {
class ReaderUtils {
public:
    class OperationFailed : public TraceableException {
    public:
        // Constructors
        OperationFailed(ErrorCode error_code, char const* const filename, int line_number)
                : TraceableException(error_code, filename, line_number) {}
    };

    typedef std::map<int32_t, std::set<int32_t>> SchemaMap;
    static constexpr size_t cDecompressorFileReadBufferCapacity = 64 * 1024;  // 64 KB

    /**
     * Reads the schema tree from the given archive directory
     * @param archive_dir
     * @return The schema tree
     */
    static std::shared_ptr<SchemaTree> read_schema_tree(std::string const& archive_dir);

    /**
     * Opens and gets the variable dictionary reader for the given archive path
     * @param archive_path
     * @return the variable dictionary reader
     */
    static std::shared_ptr<VariableDictionaryReader> get_variable_dictionary_reader(
            std::string const& archive_path
    );

    /**
     * Opens and gets the log type dictionary reader for the given archive path
     * @param archive_path
     * @return the log type dictionary reader
     */
    static std::shared_ptr<LogTypeDictionaryReader> get_log_type_dictionary_reader(
            std::string const& archive_path
    );

    /**
     * Opens and gets the array dictionary reader for the given archive path
     * @param archive_path
     * @return the array dictionary reader
     */
    static std::shared_ptr<LogTypeDictionaryReader> get_array_dictionary_reader(
            std::string const& archive_path
    );

    /**
     * Reads the schema map from the given archive directory
     * @param archive_path
     * @return the schema map
     */
    static std::shared_ptr<SchemaMap> read_schemas(std::string const& archive_dir);

    /**
     * Opens and gets the timestamp dictionary reader for the given archive path
     * @param archive_path
     * @return the timestamp dictionary reader
     */
    static std::shared_ptr<TimestampDictionaryReader> read_timestamp_dictionary(
            std::string const& archive_dir
    );

    /**
     * Opens and gets the local timestamp dictionary reader for the given archive path
     * @param archive_path
     * @return the timestamp dictionary reader
     */
    static std::shared_ptr<TimestampDictionaryReader> read_local_timestamp_dictionary(
            std::string const& archive_path
    );

    /**
     * Gets the list of archives in the given archive directory
     * @param archive_dir
     * @return the list of archives
     */
    static std::vector<std::string> get_archives(std::string const& archive_dir);

    /**
     * Gets the list of schemas in the given archive
     * @param archive_path
     * @return the list of schemas
     */
    static std::vector<int32_t> get_schemas(std::string const& archive_path);

    /**
     * Append a set of columns to the given schema reader
     * @param reader
     * @param columns
     * @param schema_tree
     * @param var_dict
     * @param log_dict
     * @param array_dict
     * @param timestamp_dict
     */
    static void append_reader_columns(
            SchemaReader* reader,
            std::set<int32_t>& columns,
            std::shared_ptr<SchemaTree> const& schema_tree,
            std::shared_ptr<VariableDictionaryReader> const& var_dict,
            std::shared_ptr<LogTypeDictionaryReader> const& log_dict,
            std::shared_ptr<LogTypeDictionaryReader> const& array_dict,
            std::shared_ptr<TimestampDictionaryReader> const& timestamp_dict
    );
};
}  // namespace clp_structured

#endif  // CLP_STRUCTURED__READERUTILS_HPP
