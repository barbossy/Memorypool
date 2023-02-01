cmake_minimum_required(VERSION 3.5)

project(mempool LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(SRC_DIR src)
SET(INCLUDE_DIR include)

SET(SOURCES
    ${INCLUDE_DIR}/mempool.h
    ${INCLUDE_DIR}/subpooldescriptor.h
    ${INCLUDE_DIR}/statistic.h
    ${SRC_DIR}/mempool.cpp
    )

add_executable(mempool src/main.cpp ${SOURCES})
include_directories(include)

