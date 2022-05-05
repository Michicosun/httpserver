cmake_minimum_required(VERSION 3.14)

project(lib_thread_pool)

message(STATUS "CONFIGURE THREAD_POOL BUILD")
message(STATUS "THREAD_POOL PATH = ${PROJECT_SOURCE_DIR}")

file(
    GLOB 
    THREAD_POOL_SRC_FILES 
    ${PROJECT_SOURCE_DIR}/*.cpp
)

message(STATUS "THREAD_POOL FILES = [${THREAD_POOL_SRC_FILES}]")

add_library(${PROJECT_NAME} ${THREAD_POOL_SRC_FILES})

message(STATUS "")


