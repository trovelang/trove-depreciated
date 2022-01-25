add_executable(test_example test_example.cpp)
add_executable(test_types test_types.cpp)

target_link_libraries(test_example PRIVATE Catch2::Catch2)
target_link_libraries(test_types PRIVATE Catch2::Catch2)

add_test(test_example test_example)
add_test(test_types test_types)
