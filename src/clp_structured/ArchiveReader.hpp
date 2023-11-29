#ifndef CLP_STRUCTURED_ARCHIVEREADER_HPP
#define CLP_STRUCTURED_ARCHIVEREADER_HPP

#include <map>
#include <set>
#include <utility>

#include <boost/filesystem.hpp>

#include "DictionaryReader.hpp"
#include "SchemaReader.hpp"
#include "TimestampDictionaryReader.hpp"

namespace clp_structured {
struct ArchiveReaderOption {
    std::string archive_path;
    std::map<int32_t, std::set<int32_t>> id_to_schema;
};

class ArchiveReader {
public:
    class OperationFailed : public TraceableException {
    public:
        // Constructors
        OperationFailed(ErrorCode error_code, char const* const filename, int line_number)
                : TraceableException(error_code, filename, line_number) {}
    };

    // Constructor
    ArchiveReader(
            std::shared_ptr<SchemaTree> schema_tree,
            std::map<int32_t, std::set<int32_t>> id_to_schema,
            std::shared_ptr<TimestampDictionaryReader> timestamp_dict
    )
            : m_schema_tree(std::move(schema_tree)),
              m_id_to_schema(std::move(id_to_schema)),
              m_timestamp_dict(std::move(timestamp_dict)) {}

    /**
     * Opens an archive for reading.
     * @param option
     */
    void open(ArchiveReaderOption& option);

    /**
     * Writes decoded messages to a file.
     * @param writer
     */
    void store(FileWriter& writer);

    /**
     * Closes the archive.
     */
    void close();

private:
    std::string m_archive_path;

    std::shared_ptr<VariableDictionaryReader> m_var_dict;
    std::shared_ptr<LogTypeDictionaryReader> m_log_dict;
    std::shared_ptr<LogTypeDictionaryReader> m_array_dict;

    std::shared_ptr<SchemaTree> m_schema_tree;
    std::map<int32_t, std::set<int32_t>> m_id_to_schema;
    std::map<int32_t, SchemaReader*> m_schema_id_to_reader;

    std::shared_ptr<TimestampDictionaryReader> m_timestamp_dict;
};
}  // namespace clp_structured

#endif  // CLP_STRUCTURED_ARCHIVEREADER_HPP
