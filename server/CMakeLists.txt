cmake_minimum_required(VERSION 3.14)

project(lib_server)

message(STATUS "CONFIGURE SERVER BUILD")
message(STATUS "SERVER PATH = ${PROJECT_SOURCE_DIR}")

file(
    GLOB 
    SERVER_SRC_FILES 
    ${PROJECT_SOURCE_DIR}/*.cpp
)

message(STATUS "SERVER FILES = [${SERVER_SRC_FILES}]")

add_library(${PROJECT_NAME} ${SERVER_SRC_FILES})

message(STATUS "")


