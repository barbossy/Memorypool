cmake_minimum_required(VERSION 3.5)
project(mempool-tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

SET (UNIT_TESTS_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
SET (UNIT_TESTS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

set (TOP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../")

include(${TOP_DIR}/conan.cmake)

conan_cmake_configure(REQUIRES gtest/1.11.0
                      GENERATORS cmake)

conan_cmake_autodetect(settings)

conan_cmake_install(PATH_OR_REFERENCE .
                    BUILD missing
                    REMOTE conancenter
                    SETTINGS ${settings})

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()


set(UNIT_TESTS_SOURCES
    ${UNIT_TESTS_SRC_DIR}/mainunittest.cpp
    ${UNIT_TESTS_SRC_DIR}/mempoolunittest.cpp
    ${UNIT_TESTS_INCLUDE_DIR}/mempoolunittest.h
    )

set (SOURCES_TO_TEST
    ${TOP_DIR}/include/mempool.h
    ${TOP_DIR}/include/subpooldescriptor.h
    ${TOP_DIR}/src/mempool.cpp
    )

add_executable(mempool-tests ${UNIT_TESTS_SOURCES} ${SOURCES_TO_TEST})
target_link_libraries(mempool-tests ${CONAN_LIBS})

include_directories(${TOP_DIR}/include)
include_directories(include)
