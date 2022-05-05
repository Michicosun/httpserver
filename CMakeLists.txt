cmake_minimum_required(VERSION 3.14)

project(httpserver)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "-std=c++20 -pthread -g -Wthread-safety -Wall -Werror")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

list(APPEND LIBS 
    server 
    worker
    utils
)

foreach(comp IN LISTS LIBS)
    add_subdirectory(${PROJECT_SOURCE_DIR}/${comp})
    target_include_directories(lib_${comp} PUBLIC ${PROJECT_SOURCE_DIR})
endforeach()

message(STATUS "CONFIGURE HTTPSERVER BUILD")
message(STATUS "HTTPSERVER PATH = ${PROJECT_SOURCE_DIR}")

file(
    GLOB 
    HTTPSERVER_SRC_FILES 
    ${PROJECT_SOURCE_DIR}/*.cpp
)

message(STATUS "HTTPSERVER FILES = [${HTTPSERVER_SRC_FILES}]")

add_executable(${PROJECT_NAME} ${HTTPSERVER_SRC_FILES})

foreach(comp IN LISTS LIBS)
    target_link_libraries(${PROJECT_NAME} lib_${comp})
endforeach()

message(STATUS "")

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})

