
cmake_minimum_required(VERSION 3.28)
project(aoc2024 CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
    flux
    GIT_REPOSITORY https://github.com/tcbrindle/flux.git
    GIT_TAG main
)

FetchContent_Declare(
    ctre
    GIT_REPOSITORY https://github.com/hanickadot/compile-time-regular-expressions.git
    GIT_TAG v3.9.0
)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 11.0.2
)

FetchContent_Declare(
    unordered_dense
    GIT_REPOSITORY https://github.com/martinus/unordered_dense.git
    GIT_TAG v4.4.0
)

FetchContent_MakeAvailable(flux ctre fmt unordered_dense)

add_library(aoc INTERFACE)
target_sources(
    aoc INTERFACE
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES aoc.hpp)
target_link_libraries(aoc INTERFACE ctre::ctre fmt::fmt flux::flux unordered_dense::unordered_dense)
target_precompile_headers(aoc INTERFACE aoc.hpp)

function(ADD_DAY DATE)
    add_executable(${DATE} ${DATE}/main.cpp)
    target_link_libraries(${DATE} PRIVATE aoc)
endfunction()

add_day(dec01)
add_day(dec02)