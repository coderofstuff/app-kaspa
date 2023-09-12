cmake_minimum_required(VERSION 3.10)

if(${CMAKE_VERSION} VERSION_LESS 3.10)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

# project information
project(unit_tests
        VERSION 0.1
	      DESCRIPTION "Unit tests for Ledger Nano application"
        LANGUAGES C)


# guard against bad build-type strings
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

include(CTest)
ENABLE_TESTING()

# specify C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -pedantic -g -O0 --coverage")

set(GCC_COVERAGE_LINK_FLAGS "--coverage -lgcov")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")

# guard against in-source builds
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there. You may need to remove CMakeCache.txt. ")
endif()

add_compile_definitions(TEST HAVE_HASH HAVE_BLAKE2 HAVE_ECC USB_SEGMENT_SIZE=64 MAX_INPUT_COUNT=15 IO_SEPROXYHAL_BUFFER_SIZE_B=128)

include_directories(../src)
# include_directories(mock_includes)
include_directories(mock_includes)
# include_directories(/opt/nanos-secure-sdk/include/)
include_directories(/opt/ledger-secure-sdk/include/)
include_directories(/opt/ledger-secure-sdk/lib_cxng/include)
include_directories(/opt/ledger-secure-sdk/lib_standard_app/)


add_executable(test_address test_address.c)
add_executable(test_format test_format.c)
add_executable(test_sighash test_sighash.c)
add_executable(test_personal_message.h test_personal_message.c)
add_executable(test_apdu_parser test_apdu_parser.c)
add_executable(test_tx_parser test_tx_parser.c)
add_executable(test_tx_utils test_tx_utils.c)

add_library(address SHARED ../src/address.c)
add_library(blake2b SHARED ../src/import/blake2b.c)
add_library(cashaddr SHARED ../src/import/cashaddr.c)
add_library(bip32 SHARED /opt/ledger-secure-sdk/lib_standard_app/bip32.c)
add_library(buffer SHARED /opt/ledger-secure-sdk/lib_standard_app/buffer.c)
add_library(read SHARED /opt/ledger-secure-sdk/lib_standard_app/read.c)
add_library(sighash SHARED ../src/sighash.c)
add_library(personal_message SHARED ../src/personal_message.c)
add_library(write SHARED /opt/ledger-secure-sdk/lib_standard_app/write.c)
add_library(format SHARED ../src/common/format.c)
add_library(apdu_parser SHARED ../src/apdu/parser.c)
add_library(transaction_deserialize ../src/transaction/deserialize.c)
add_library(transaction_serialize ../src/transaction/serialize.c)
add_library(transaction_utils ../src/transaction/utils.c)
add_library(varint SHARED /opt/ledger-secure-sdk/lib_standard_app/varint.c)

target_link_libraries(test_address PUBLIC cmocka gcov address cashaddr)
target_link_libraries(test_format PUBLIC cmocka gcov format)
target_link_libraries(test_sighash PUBLIC cmocka gcov sighash blake2b write)
target_link_libraries(test_personal_message PUBLIC cmocka gcov personal_message blake2b write)
target_link_libraries(test_apdu_parser PUBLIC cmocka gcov apdu_parser)
target_link_libraries(test_tx_parser PUBLIC
                      transaction_deserialize
                      transaction_serialize
                      buffer
                      varint
                      bip32
                      cmocka
                      gcov
                      write
                      read
                      transaction_utils)
target_link_libraries(test_tx_utils PUBLIC
                      cmocka
                      gcov
                      transaction_utils
                      address
                      cashaddr)

add_test(test_address test_address)
add_test(test_format test_format)
add_test(test_sighash test_sighash)
add_test(test_personal_message test_personal_message)
add_test(test_apdu_parser test_apdu_parser)
add_test(test_tx_parser test_tx_parser)
add_test(test_tx_utils test_tx_utils)
