cmake_minimum_required(VERSION 3.13)
project(tests NONE)

find_package(GTest REQUIRED)
find_package(GSL REQUIRED)

add_compile_definitions(DEBUG)
include_directories(${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS})
add_executable(tests main.cc bb84.cc qubit.cc q_reg.cc)
target_link_libraries(tests ${GTEST_LIBRARIES} GSL::gsl GSL::gslcblas gmock gmock_main pthread)

target_include_directories(tests
    PUBLIC)
