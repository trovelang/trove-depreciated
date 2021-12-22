# CMakeList.txt : CMake project for trove, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 20)

project("trove")

include_directories("src")

# Add source to this project's executable.
add_executable(trove "src/trove.cpp" "src/trove.h" "src/lexer.h" "src/lexer.cpp" "src/parser.h" "src/parser.cpp" "src/token.h" "src/token.cpp" "src/types.h" "src/log.h" "src/log.cpp")

# TODO: Add tests and install targets if needed.
