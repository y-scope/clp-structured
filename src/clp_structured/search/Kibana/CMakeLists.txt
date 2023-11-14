LIST(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

add_definitions(-DANTLR4CPP_STATIC)
set(ANTLR4_TAG 4.11.1)

set(ANTLR_EXECUTABLE ${PROJECT_SOURCE_DIR}/third_party/antlr/antlr-4.11.1-complete.jar)

include(ExternalAntlr4Cpp)

include_directories(${ANTLR4_INCLUDE_DIRS})

message(STATUS "Found antlr4cpp includes: ${ANTLR4_INCLUDE_DIRS} ")

find_package(ANTLR REQUIRED)

antlr_target(KibanaParser Kibana.g4 PARSER LEXER VISITOR
        PACKAGE kibana)

include_directories(${ANTLR_KibanaParser_OUTPUT_DIR})

add_library(Kibana
        Kibana.hpp
        Kibana.cpp
        ${ANTLR_KibanaParser_CXX_OUTPUTS}
)

target_link_libraries(Kibana PUBLIC clp-s-search PRIVATE antlr4_static)
target_include_directories(Kibana PUBLIC "${PROJECT_SOURCE_DIR}")
