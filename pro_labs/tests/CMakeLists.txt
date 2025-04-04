# tests/CMakeListst.txt

cmake_minimum_required(VERSION 3.5)
#tests

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

function(build_test test_name)
    add_executable(${test_name}_test ${ARGN})
    target_link_libraries(${test_name}_test PRIVATE lib_dependencies)
    add_test(${test_name} ./${test_name}_test)
endfunction()

file(GLOB TEST_SOURCES "*.t.c" "*.t.cpp")

foreach(TEST_SOURCE IN LISTS TEST_SOURCES)
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    build_test(${TEST_NAME} ${TEST_SOURCE})
endforeach()
