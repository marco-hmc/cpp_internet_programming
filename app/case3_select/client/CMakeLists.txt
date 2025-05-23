project(case3_client)

##############################################

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

add_executable(${PROJECT_NAME}_client ${SOURCES})
include_directories(${CMAKE_SOURCE_DIR}/utils)

ADD_LIB_MODULE(${PROJECT_NAME}_client Socket)
##############################################

# add_executable(${PROJECT_NAME} ${SOURCES})


# find_package(benchmark REQUIRED)

# target_link_libraries(${PROJECT_NAME} PRIVATE benchmark::benchmark)

