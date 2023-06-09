cmake_minimum_required(VERSION 3.10)

if(${CMAKE_VERSION} VERSION_LESS 3.10)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

# project information
project(FuzzTxParser
        VERSION 1.0
	      DESCRIPTION "Fuzzing of transaction parser"
        LANGUAGES CXX)

# guard against bad build-type strings
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

# guard against in-source builds
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there. You may need to remove CMakeCache.txt. ")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_definitions(MAX_INPUT_COUNT=15 USB_SEGMENT_SIZE=64)

include(extra/TxParser.cmake)

add_executable(fuzz_tx_parser fuzz_tx_parser.cc)
add_executable(fuzz_txout_parser fuzz_txout_parser.cc)
add_executable(fuzz_txin_parser fuzz_txin_parser.cc)

target_compile_options(fuzz_tx_parser
    PRIVATE $<$<C_COMPILER_ID:Clang>:-g -O2 -fsanitize=fuzzer,address,undefined>
)
target_compile_options(fuzz_txout_parser
    PRIVATE $<$<C_COMPILER_ID:Clang>:-g -O2 -fsanitize=fuzzer,address,undefined>
)
target_compile_options(fuzz_txin_parser
    PRIVATE $<$<C_COMPILER_ID:Clang>:-g -O2 -fsanitize=fuzzer,address,undefined>
)

target_link_libraries(fuzz_tx_parser
    PRIVATE $<$<C_COMPILER_ID:Clang>:-fsanitize=fuzzer,address,undefined>
    PUBLIC txparser
)
target_link_libraries(fuzz_txout_parser
    PRIVATE $<$<C_COMPILER_ID:Clang>:-fsanitize=fuzzer,address,undefined>
    PUBLIC txparser
)
target_link_libraries(fuzz_txin_parser
    PRIVATE $<$<C_COMPILER_ID:Clang>:-fsanitize=fuzzer,address,undefined>
    PUBLIC txparser
)
