add_executable(test_skip_list)
target_sources(test_skip_list  PRIVATE test.cpp)
target_link_libraries(test_skip_list PRIVATE ${PROJECT_NAME})

add_test(NAME test_skip_list COMMAND $<TARGET_FILE:test_skip_list>)
