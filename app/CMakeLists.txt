project(case1)

##############################################

add_subdirectory(case1_socket_tcp)
add_subdirectory(case2_socket_udp)
add_subdirectory(case3_select)
add_subdirectory(case4_poll)
add_subdirectory(case5_epoll)


##############################################

# add_executable(${PROJECT_NAME} ${SOURCES})

# ADD_LIB_MODULE(${PROJECT_NAME} Bar)

# find_package(benchmark REQUIRED)

# target_link_libraries(${PROJECT_NAME} PRIVATE benchmark::benchmark)

