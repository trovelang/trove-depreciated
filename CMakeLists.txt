# CMakeList.txt : CMake project for trove, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_FLAGS "-std=c++20")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

# disable warnings
add_definitions(-w)

project("trove")

option(CODEGEN_CLANG "Specify C compiler for codegen" OFF)
if (CODEGEN_CLANG)
  add_definitions(-DCODEGEN_CLANG)
endif()
option(CODEGEN_GCC "Specify C compiler for codegen" ON)
if (CODEGEN_GCC)
  add_definitions(-DCODEGEN_GCC)
endif()

add_subdirectory("lib/Catch2")
add_subdirectory("lib/spdlog")
# find_package(spdlog REQUIRED)


include_directories("src")
include_directories("lib/spdlog/include/")

set(SAUCE "src/lexer.cpp" "src/parser.cpp" "src/token.cpp" "src/log.cpp" "src/ast.cpp" "src/borrowchecker.cpp" "src/analyse.cpp" "src/cgenerator.cpp"  "src/type.cpp" "src/pass1.cpp" "src/error.cpp" )
set(HEADERS "src/trove.h" "src/lexer.h"  "src/parser.h" "src/token.h" "src/types.h" "src/log.h" "src/ast.h" "src/type.h" "src/borrowchecker.h" "src/cli.h" "src/clr.h" "src/analyse.h" "src/symtable.h" "src/error.h" "src/pass1.h" "src/unit.h" )

add_library(my_cpp_lib STATIC ${SAUCE})

# Add source to this project's executable.
add_executable(trove "src/trove.cpp" )
set_property(TARGET trove PROPERTY CXX_STANDARD 20)
target_link_libraries(trove my_cpp_lib spdlog::spdlog)
target_include_directories(trove PUBLIC "src")

enable_testing()
find_package(Catch2 3 REQUIRED)
list(APPEND CMAKE_MODULE_PATH"lib/Catch2/extras")
include(CTest)
include(Dart)

# add_subdirectory("tests")

# tests go here
add_executable(test_example "tests/test_example.cpp")
add_test(test_example test_example)
target_link_libraries(test_example PUBLIC my_cpp_lib PUBLIC spdlog::spdlog PRIVATE Catch2::Catch2WithMain)

add_executable(test_types "tests/test_type.cpp")
add_test(test_types test_types)
target_link_libraries(test_types PUBLIC my_cpp_lib PUBLIC spdlog::spdlog PRIVATE Catch2::Catch2WithMain)

add_executable(test_token "tests/test_token.cpp")
add_test(test_token test_token)
target_link_libraries(test_token PUBLIC my_cpp_lib PUBLIC spdlog::spdlog PRIVATE Catch2::Catch2WithMain)

add_executable(test_lexer "tests/test_lexer.cpp")
add_test(test_lexer test_lexer)
target_link_libraries(test_lexer PUBLIC my_cpp_lib PUBLIC spdlog::spdlog PRIVATE Catch2::Catch2WithMain)

add_executable(test_parser "tests/test_parser.cpp")
add_test(test_parser test_parser)
target_link_libraries(test_parser PUBLIC my_cpp_lib PUBLIC spdlog::spdlog PRIVATE Catch2::Catch2WithMain)

# TODO for some reason #include <symtable.h> breaks...
#add_executable(test_sym_table "tests/test_sym_table.cpp")
#add_test(test_sym_table test_sym_table)
#target_link_libraries(test_sym_table PUBLIC my_cpp_lib PUBLIC spdlog::spdlog PRIVATE Catch2::Catch2WithMain)