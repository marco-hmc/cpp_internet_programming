project(case3_server)

##############################################

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

add_executable(${PROJECT_NAME}_server ${SOURCES})
include_directories(${CMAKE_SOURCE_DIR}/utils)

ADD_LIB_MODULE(${PROJECT_NAME}_server Socket)
ADD_LIB_MODULE(${PROJECT_NAME}_server Connector)

##############################################

# add_executable(${PROJECT_NAME} ${SOURCES})

# ADD_LIB_MODULE(${PROJECT_NAME} Bar)

# find_package(benchmark REQUIRED)

# target_link_libraries(${PROJECT_NAME} PRIVATE benchmark::benchmark)

