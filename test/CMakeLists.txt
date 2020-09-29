aux_source_directory(. TEST_DIR)

add_executable(uno_test ${TEST_DIR})
target_link_libraries(uno_test PRIVATE asio)
target_link_libraries(uno_test PRIVATE cxxopts)
target_link_libraries(uno_test PRIVATE gtest)
target_link_libraries(uno_test PRIVATE gmock)