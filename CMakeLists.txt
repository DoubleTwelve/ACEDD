cmake_minimum_required (VERSION 2.8)

project(ACEDD)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    set(TARGET_SYSTEM "Linux")
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(TARGET_SYSTEM "Windows")
else()
    set(TARGET_SYSTEM "Unknown")
endif()

message("Target system is: ${TARGET_SYSTEM}")
set(bitness 64)

set(LIBRARY_TYPE STATIC)

if (${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
    message(SEND_ERROR "In-source builds are not allowed.")
endif()

set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"))
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-missing-braces -Wno-enum-compare -Wno-deprecated -Wno-deprecated-declarations")
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-register")
endif()

add_definitions(-D__STDC_LIMIT_MACROS)


###############################################################################
#                                                                             #
#                   Dependencies                                              #
#                                                                             #
###############################################################################
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/Modules/")

if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/opencv_path.cmake")
    include("${CMAKE_CURRENT_LIST_DIR}/opencv_path.cmake")
endif()

set(CMAKE_INCLUDE_PATH $ENV{CMAKE_INCLUDE_PATH} CACHE PATH "")
set(CMAKE_LIBRARY_PATH $ENV{CMAKE_LIBRARY_PATH} CACHE PATH "")

find_package(OpenCV REQUIRED)
if (${OpenCV_VERSION} VERSION_LESS 2.4.8)
    message(FATAL_ERROR "OpenCV version is not compatible : ${OpenCV_VERSION}")
endif()

message(STATUS "OpenCV_VERSION: ${OpenCV_VERSION}")
message(STATUS "OpenCV_INCLUDE_DIRS: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV_LIB_DIR: ${OpenCV_LIB_DIR}")

###############################################################################
#                                                                             #
#                   Build paths                                               #
#                                                                             #
###############################################################################
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

link_directories(
    ${OpenCV_LIB_DIR}
    ${LIBRARY_OUTPUT_PATH}
)

set(ACEDD_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
set(ACEDD_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(ACEDD_LIBNAME ACEDD)
set(ACEDD_APP ACEDD_APP)


include_directories(
    ${OpenCV_INCLUDE_DIRS}
    ${ACEDD_INCLUDE_DIR}
    ${CMAKE_BINARY_DIR}/bin/src
)


message(STATUS "Acedd_INCLUDE_DIR: ${ACEDD_INCLUDE_DIR}")

set(ACEDD_SOURCES
    ${ACEDD_SOURCE_DIR}/acedd.cpp
    ${ACEDD_SOURCE_DIR}/acedd_quant.cpp
    ${ACEDD_SOURCE_DIR}/fuzzy_10bin.cpp
    ${ACEDD_SOURCE_DIR}/fuzzy_24bin.cpp
)

set(ACEDD_HEADERS
    ${ACEDD_INCLUDE_DIR}/acedd.h
    ${ACEDD_INCLUDE_DIR}/acedd_quant.h
    ${ACEDD_INCLUDE_DIR}/fuzzy_10bin.h
    ${ACEDD_INCLUDE_DIR}/fuzzy_24bin.h
)

set(ACEDD_APP_SOURCES
    ${PROJECT_SOURCE_DIR}/apps/acedd_app.cpp
)

add_library(${ACEDD_LIBNAME} ${LIBRARY_TYPE} ${ACEDD_HEADERS} ${ACEDD_SOURCES})
target_link_libraries(${ACEDD_LIBNAME} ${OpenCV_LIBS})
add_executable(${ACEDD_APP} ${ACEDD_APP_SOURCES})
target_link_libraries(${ACEDD_APP} ACEDD ${OpenCV_LIBS})
