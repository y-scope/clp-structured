cmake_minimum_required(VERSION 3.16...3.19)
project(clp-structured)

set(CMAKE_CXX_STANDARD 17)

string(LENGTH "${CMAKE_SOURCE_DIR}/" SOURCE_PATH_SIZE)
add_definitions("-DSOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}")

add_definitions(-DFLUSH_TO_DISK_ENABLED=1)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# Setting NDEBUG turns off some runtime checks for simdjson and gives
# faster json parsing
add_definitions(-DNDEBUG)

include(cmake/Modules/FindLibraryDependencies.cmake)

find_package(Boost REQUIRED serialization program_options iostreams filesystem)
if (Boost_FOUND)
    message(STATUS "Found Boost ${Boost_VERSION}")
else ()
    message(FATAL_ERROR "Could not find libraries for Boost")
endif ()


find_package(ZStd REQUIRED)
if (ZStd_FOUND)
    message(STATUS "Found ZStd ${ZStd_VERSION}")
else ()
    message(FATAL_ERROR "Could not find libraries for ZStd")
endif ()

# SIMDJSON builds a binary for several architectures by default. This can be a problem
# if the assembler doesn't support one of these ISAs. The following should be set on
# machines with toolchains predating avx512 if linker errors start appearing.
# https://github.com/simdjson/simdjson/blob/8c89ae0cd9f50a018f57d7dda2f2138a97a05ccb/doc/implementation-selection.md
#SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -DSIMDJSON_IMPLEMENTATION_ICELAKE=0")

# Add Abseil
set(ABSL_PROPAGATE_CXX_STD ON)
add_subdirectory(submodules/abseil-cpp EXCLUDE_FROM_ALL)
add_subdirectory(submodules/nlohmann_json EXCLUDE_FROM_ALL)
add_subdirectory(submodules/simdjson EXCLUDE_FROM_ALL)
add_subdirectory(submodules/spdlog EXCLUDE_FROM_ALL)

add_subdirectory(src/clp_structured/search)

add_library(clp-s-archive
        src/clp_structured/ArchiveReader.cpp
        src/clp_structured/ArchiveReader.hpp
        src/clp_structured/ArchiveWriter.cpp
        src/clp_structured/ArchiveWriter.hpp
        src/clp_structured/ColumnReader.cpp
        src/clp_structured/ColumnReader.hpp
        src/clp_structured/ColumnWriter.cpp
        src/clp_structured/ColumnWriter.hpp
        src/clp_structured/Compressor.cpp
        src/clp_structured/Compressor.hpp
        src/clp_structured/Decompressor.cpp
        src/clp_structured/Decompressor.hpp
        src/clp_structured/Defs.hpp
        src/clp_structured/DictionaryEntry.cpp
        src/clp_structured/DictionaryEntry.hpp
        src/clp_structured/DictionaryReader.cpp
        src/clp_structured/DictionaryReader.hpp
        src/clp_structured/DictionaryWriter.cpp
        src/clp_structured/DictionaryWriter.hpp
        src/clp_structured/ErrorCode.hpp
        src/clp_structured/FileReader.cpp
        src/clp_structured/FileReader.hpp
        src/clp_structured/FileWriter.cpp
        src/clp_structured/FileWriter.hpp
        src/clp_structured/JsonConstructor.cpp
        src/clp_structured/JsonConstructor.hpp
        src/clp_structured/JsonFileIterator.cpp
        src/clp_structured/JsonFileIterator.hpp
        src/clp_structured/JsonParser.cpp
        src/clp_structured/JsonParser.hpp
        src/clp_structured/ParsedMessage.cpp
        src/clp_structured/ParsedMessage.hpp
        src/clp_structured/ReaderUtils.cpp
        src/clp_structured/ReaderUtils.hpp
        src/clp_structured/SchemaTree.cpp
        src/clp_structured/SchemaTree.hpp
        src/clp_structured/SchemaReader.cpp
        src/clp_structured/SchemaReader.hpp
        src/clp_structured/SchemaWriter.cpp
        src/clp_structured/SchemaWriter.hpp
        src/clp_structured/TimestampDictionaryReader.cpp
        src/clp_structured/TimestampDictionaryReader.hpp
        src/clp_structured/TimestampDictionaryWriter.cpp
        src/clp_structured/TimestampDictionaryWriter.hpp
        src/clp_structured/TimestampEntry.cpp
        src/clp_structured/TimestampEntry.hpp
        src/clp_structured/TimestampPattern.cpp
        src/clp_structured/TimestampPattern.hpp
        src/clp_structured/TraceableException.cpp
        src/clp_structured/TraceableException.hpp
        src/clp_structured/Utils.cpp
        src/clp_structured/Utils.hpp
        src/clp_structured/VariableDecoder.cpp
        src/clp_structured/VariableDecoder.hpp
        src/clp_structured/VariableEncoder.cpp
        src/clp_structured/VariableEncoder.hpp
        src/clp_structured/ZstdCompressor.cpp
        src/clp_structured/ZstdCompressor.hpp
        src/clp_structured/ZstdDecompressor.cpp
        src/clp_structured/ZstdDecompressor.hpp
        submodules/date/include/date/date.h
)

target_link_libraries(clp-s-archive PUBLIC simdjson spdlog nlohmann_json ZStd::ZStd
        Boost::serialization Boost::filesystem Boost::iostreams absl::flat_hash_map)

add_executable(clp-s src/clp_structured/clp-s.cpp)
target_link_libraries(clp-s PRIVATE clp-s-archive Boost::program_options Kibana)
