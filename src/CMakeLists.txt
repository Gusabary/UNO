aux_source_directory(. APP_DIR)

add_executable(uno ${APP_DIR})
target_link_libraries(uno PRIVATE asio)
target_link_libraries(uno PRIVATE cxxopts)
