add_executable(test_example test_example.cpp)
target_link_libraries(test_example PRIVATE Catch2::Catch2)

add_test(test_example test_example)