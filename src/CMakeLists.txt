aux_source_directory(. APP_DIR)
aux_source_directory(common APP_DIR)
aux_source_directory(game APP_DIR)
aux_source_directory(network APP_DIR)
aux_source_directory(ui APP_DIR)

add_executable(uno ${APP_DIR})
target_link_libraries(uno PRIVATE asio)
target_link_libraries(uno PRIVATE cxxopts)
target_link_libraries(uno PRIVATE yaml-cpp)

if(ENABLE_LOG)
    target_link_libraries(uno PRIVATE spdlog::spdlog)
endif()