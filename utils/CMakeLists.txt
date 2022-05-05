cmake_minimum_required(VERSION 3.14)

project(lib_utils)

message(STATUS "CONFIGURE UTILS BUILD")
message(STATUS "UTILS PATH = ${PROJECT_SOURCE_DIR}")

file(
    GLOB 
    UTILS_SRC_FILES 
    ${PROJECT_SOURCE_DIR}/*.cpp
)

message(STATUS "UTILS FILES = [${UTILS_SRC_FILES}]")

add_library(${PROJECT_NAME} ${UTILS_SRC_FILES})

message(STATUS "")


