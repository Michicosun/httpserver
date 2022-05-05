cmake_minimum_required(VERSION 3.14)

project(lib_worker)

message(STATUS "CONFIGURE WORKER BUILD")
message(STATUS "WORKER PATH = ${PROJECT_SOURCE_DIR}")

file(
    GLOB 
    WORKER_SRC_FILES 
    ${PROJECT_SOURCE_DIR}/*.cpp
)

message(STATUS "WORKER FILES = [${WORKER_SRC_FILES}]")

add_library(${PROJECT_NAME} ${WORKER_SRC_FILES})

message(STATUS "")


