cmake_minimum_required(VERSION 3.15.0)

project(InternetProgramming)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CTest)
enable_testing()

include(${CMAKE_SOURCE_DIR}/cmake/linkDll.cmake)

add_subdirectory(app)

find_package(spdlog REQUIRED)

file(GLOB SUBDIRS "lib/*")
foreach(subdir ${SUBDIRS})
    if(IS_DIRECTORY ${subdir})
        message(STATUS "building subdirectory: ${subdir}")
        add_subdirectory(${subdir})
    else()
        message(STATUS "Not a directory: ${subdir}")    
    endif()
endforeach()

file(GLOB SUBDIRS "dll/*")
foreach(subdir ${SUBDIRS})
    if(IS_DIRECTORY ${subdir})
        message(STATUS "building subdirectory: ${subdir}")
        add_subdirectory(${subdir})
    else()
        message(STATUS "Not a directory: ${subdir}")    
    endif()
endforeach()