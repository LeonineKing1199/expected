cmake_minimum_required(VERSION 3.9.3)

project(expected)

# Find Boost
find_package(Boost 1.65 REQUIRED)

# Find Catch
find_path(CATCH_INCLUDE_DIR catch.hpp)
add_library(Catch INTERFACE)
target_include_directories(
    Catch

    INTERFACE
    ${CATCH_INCLUDE_DIR}
)

# Find Fit
find_path(
    FIT_INCLUDE_DIR fit.hpp
    HINTS ${CMAKE_CURRENT_SOURCE_DIR}/Fit/include
)
add_library(Fit INTERFACE)
target_include_directories(
    Fit

    INTERFACE
    ${FIT_INCLUDE_DIR}
)

# Create our core library
add_library(expected_core INTERFACE)

target_compile_features(
    expected_core

    INTERFACE
    cxx_std_14
)

target_include_directories(
    expected_core

    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${Boost_INCLUDE_DIRS}
)

target_link_libraries(
    expected_core

    INTERFACE
    Fit
)

# Build tests
add_executable(
    expected_tests

    ${CMAKE_CURRENT_SOURCE_DIR}/debug/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/expected_test.cpp
)

target_link_libraries(
    expected_tests

    Catch
    expected_core
)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake_modules/")
enable_testing()
include(ParseAndAddCatchTests)
ParseAndAddCatchTests(expected_tests)