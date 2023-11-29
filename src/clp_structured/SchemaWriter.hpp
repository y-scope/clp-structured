#ifndef CLP_STRUCTURED_SCHEMAWRITER_HPP
#define CLP_STRUCTURED_SCHEMAWRITER_HPP

#include <vector>

#include "ColumnWriter.hpp"
#include "FileWriter.hpp"
#include "ParsedMessage.hpp"
#include "ZstdCompressor.hpp"

namespace clp_structured {
class SchemaWriter {
public:
    // Constructor
    SchemaWriter() : m_num_messages(0) {}

    // Destructor
    ~SchemaWriter();

    /**
     * Opens the schema writer.
     * @param path
     * @param compression_level
     */
    void open(std::string path, int compression_level);

    /**
     * Appends a column to the schema writer.
     * @param column_writer
     */
    void append_column(BaseColumnWriter* column_writer);

    /**
     * Appends a message to the schema writer.
     * @param message
     * @return The size of the message in bytes.
     */
    size_t append_message(ParsedMessage& message);

    /**
     * Stores the schema to disk.
     */
    void store();

    /**
     * Closes the schema writer.
     */
    void close();

private:
    FileWriter m_file_writer;
    ZstdCompressor m_compressor;
    std::string m_path;
    int m_compression_level{};
    uint64_t m_num_messages;

    std::vector<BaseColumnWriter*> m_columns;
};
}  // namespace clp_structured

#endif  // CLP_STRUCTURED_SCHEMAWRITER_HPP
