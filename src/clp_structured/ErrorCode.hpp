// Code from CLP

#ifndef CLP_STRUCTURED_ERRORCODE_HPP
#define CLP_STRUCTURED_ERRORCODE_HPP

namespace clp_structured {
typedef enum {
    ErrorCodeSuccess = 0,
    ErrorCodeBadParam,
    ErrorCodeBadParamDbUri,
    ErrorCodeCorrupt,
    ErrorCodeErrno,
    ErrorCodeEndOfFile,
    ErrorCodeFileExists,
    ErrorCodeFileNotFound,
    ErrorCodeNoMem,
    ErrorCodeNotInit,
    ErrorCodeNotReady,
    ErrorCodeOutOfBounds,
    ErrorCodeTooLong,
    ErrorCodeTruncated,
    ErrorCodeUnsupported,
    ErrorCodeNoAccess,
    ErrorCodeFailure,
    ErrorCodeFailureMetadataCorrupted,
    ErrorCodeMetadataCorrupted,
    ErrorCodeFailureDbBulkWrite
} ErrorCode;
}  // namespace clp_structured

#endif  // CLP_STRUCTURED_ERRORCODE_HPP
