cmake_minimum_required(VERSION 3.25)
set(CMAKE_CXX_STANDARD 20)
project(PROLABS VERSION 1.0 LANGUAGES C CXX)

find_package(Threads REQUIRED)

set(COMPILATION_FALGS "")

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

option(ENABLE_MPI "Enable MPI parallelization" ON)

add_library(lib_dependencies INTERFACE)
# Adds tests
ENABLE_TESTING()
add_subdirectory("tests")

# General purpose include dir
target_include_directories(lib_dependencies INTERFACE ${CMAKE_SOURCE_DIR}/include)

if(ENABLE_MPI)
    find_package(MPI REQUIRED)
    target_include_directories(lib_dependencies INTERFACE ${MPI_INCLUDE_PATH})
    target_link_libraries(lib_dependencies INTERFACE ${MPI_LIBRARIES})
    set(CMAKE_C_FLAGS "${CMAKE_FLAGS} ${MPI_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MPI_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${MPI_EXE_LINKER_FLAGS}")
endif(ENABLE_MPI)

if(WIN32)
    target_link_libraries(lib_dependencies INTERFACE ws2_32)
    target_compile_definitions(lib_dependencies INTERFACE _WIN32_WINNT=0x0601)
endif()

# Checks does str_name have platrofm prefix
# and if it matches currnet platform
# then sets IS_PLATFORM_MATCH to ture
# if it matches other platform, then return false
function(is_platform_matches str_name)
    string(TOLOWER "${str_name}" DIR_LOWER)
    string(REGEX REPLACE "_.*" "" DIR_LOWER "${DIR_LOWER}")

    set(PLATFORMS "win" "linux" "mac")
    if(WIN32)
        set(PLATFORM_PREFIX "win")
    elseif(APPLE)
        set(PLATFORM_PREFIX "mac")
    elseif(UNIX)
        set(PLATFORM_PREFIX "linux")
    else()
        set(PLATFORM_PREFIX "unknown")
    endif()
    list(REMOVE_ITEM PLATFORMS "${PLATFORM_PREFIX}")

    set(IS_PLATFORM_MATCH TRUE PARENT_SCOPE)
    list(FIND PLATFORMS "${DIR_LOWER}" MATCH_INDEX)
    if(MATCH_INDEX GREATER_EQUAL 0)
        set(IS_PLATFORM_MATCH FALSE PARENT_SCOPE)
    endif()
endfunction()



file(GLOB SRC_DIRS RELATIVE ${CMAKE_SOURCE_DIR}/src CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*)

foreach(DIR ${SRC_DIRS})
    file(GLOB_RECURSE PROJ_SOURCE_FILES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/${DIR}/*)
    file(GLOB PROJ_INCLUDE_DIR CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/${DIR}/include)
    is_platform_matches("${DIR}")
    if (NOT IS_PLATFORM_MATCH)
        message("${DIR} skipped, it doesnt run on this platform or OS")
        continue()
    endif()
    if(PROJ_SOURCE_FILES)
        # Use directory name as target name
        set(TARGET_NAME ${DIR})

        add_executable(${TARGET_NAME} ${PROJ_SOURCE_FILES})

        if(PROJ_INCLUDE_DIR)
            target_include_directories(${TARGET_NAME} PRIVATE ${PROJ_INCLUDE_DIR})
        endif()

        target_link_libraries(${TARGET_NAME} PRIVATE lib_dependencies)

    endif()
endforeach()
