#ifndef CLP_STRUCTURED_TIMESTAMPENTRY_HPP
#define CLP_STRUCTURED_TIMESTAMPENTRY_HPP

#include <string>
#include <variant>

#include "Defs.hpp"
#include "ErrorCode.hpp"
#include "search/FilterOperation.hpp"
#include "Utils.hpp"
#include "ZstdCompressor.hpp"
#include "ZstdDecompressor.hpp"

using clp_structured::search::FilterOperation;

namespace clp_structured {
class TimestampEntry {
public:
    // Types
    class OperationFailed : public TraceableException {
    public:
        // Constructors
        OperationFailed(ErrorCode error_code, char const* const filename, int line_number)
                : TraceableException(error_code, filename, line_number) {}

        // Methods
        char const* what() const noexcept override { return "TimestampEntry operation failed"; }
    };

    // Constants
    enum TimestampEncoding : uint64_t {
        UnkownTimestampEncoding,
        Epoch,
        DoubleEpoch
    };

    // Constructors
    TimestampEntry()
            : m_encoding(UnkownTimestampEncoding),
              m_epoch_start_double(cDoubleEpochTimeMax),
              m_epoch_end_double(cDoubleEpochTimeMin),
              m_epoch_start(cEpochTimeMax),
              m_epoch_end(cEpochTimeMin) {}

    /**
     * Ingest a timestamp potentially adjusting the start and end bounds for this
     * TimestampEntry.
     *
     * @param timestamp the timestamp to be ingested
     * @return the epoch time corresponding to the string timestamp
     */
    void ingest_timestamp(epochtime_t timestamp);
    void ingest_timestamp(double timestamp);

    /**
     * Merge a timestamp range potentially adjusting the start and end bounds for this
     *
     * @param timestamp the timestamp to be ingested
     * @return the epoch time corresponding to the string timestamp
     */
    void merge_range(TimestampEntry const& entry);

    /**
     * Write the timestamp entry to a file
     * @param compressor
     * @param column
     */
    void write_to_file(ZstdCompressor& compressor, std::string const& column) const;

    /**
     * Try to read the timestamp entry from a file
     * @param decompressor
     * @param column
     * @return ErrorCode
     */
    ErrorCode try_read_from_file(ZstdDecompressor& decompressor, std::string& column);

    /**
     * Read the timestamp entry from a file
     * @param decompressor
     * @param column
     */
    void read_from_file(ZstdDecompressor& decompressor, std::string& column);

    /**
     * Check if a timestamp is in the range of this TimestampEntry
     * @param op
     * @param timestamp
     * @return
     */
    EvaluatedValue evaluate_filter(FilterOperation op, double timestamp);
    EvaluatedValue evaluate_filter(FilterOperation op, epochtime_t timestamp);

private:
    TimestampEncoding m_encoding;
    double m_epoch_start_double, m_epoch_end_double;
    epochtime_t m_epoch_start, m_epoch_end;
};
}  // namespace clp_structured

#endif  // CLP_STRUCTURED_TIMESTAMPENTRY_HPP
