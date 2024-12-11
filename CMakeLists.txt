
cmake_minimum_required(VERSION 3.28)
project(aoc2024 CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
    flux
    GIT_REPOSITORY https://github.com/tcbrindle/flux.git
    GIT_TAG pr/iterable
)

FetchContent_Declare(
    ctre
    GIT_REPOSITORY https://github.com/hanickadot/compile-time-regular-expressions.git
    GIT_TAG main
)

FetchContent_Declare(
    unordered_dense
    GIT_REPOSITORY https://github.com/martinus/unordered_dense.git
    GIT_TAG v4.5.0
)

FetchContent_MakeAvailable(flux ctre unordered_dense)

add_library(aoc INTERFACE)
target_sources(
    aoc INTERFACE
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES aoc.hpp)
target_link_libraries(aoc INTERFACE ctre::ctre flux::flux unordered_dense::unordered_dense)
target_precompile_headers(aoc INTERFACE aoc.hpp)

function(ADD_DAY DATE)
    add_executable(${DATE} ${DATE}/main.cpp)
    target_link_libraries(${DATE} PRIVATE aoc)
endfunction()

add_day(dec01)
add_day(dec02)
add_day(dec03)
add_day(dec04)
add_day(dec05)
add_day(dec06)
add_day(dec07)
add_day(dec08)
add_day(dec09)
add_day(dec10)
add_day(dec11)