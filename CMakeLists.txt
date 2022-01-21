# CMakeList.txt : CMake project for trove, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 20)

project("trove")

add_subdirectory("lib/spdlog")
find_package(spdlog)

include_directories("src")

# Add source to this project's executable.
add_executable(trove "src/trove.cpp" "src/trove.h" "src/lexer.h" "src/lexer.cpp" "src/parser.h" "src/parser.cpp" "src/token.h" "src/token.cpp" "src/types.h" "src/log.h" "src/log.cpp" "src/ast.h" "src/ast.cpp" "src/type.h" "src/borrowchecker.h" "src/borrowchecker.cpp" "src/cli.h" "src/clr.h" "src/analyse.h" "src/analyse.cpp" "src/cgenerator.cpp"  "src/type.cpp" "src/symtable.h" "src/error.h" "src/typecheck.h" "src/typecheck.cpp" "src/error.cpp" "src/unit.h" )

target_link_libraries(trove spdlog)

# TODO: Add tests and install targets if needed.
