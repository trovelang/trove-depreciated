# CMakeList.txt : CMake project for trove, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("trove")

include_directories ("src")

# Add source to this project's executable.
add_executable (trove "src/trove.cpp" "src/trove.h")

# TODO: Add tests and install targets if needed.
