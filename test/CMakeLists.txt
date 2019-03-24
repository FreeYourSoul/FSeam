cmake_minimum_required(VERSION 3.5)
project(testing CXX)

set (CMAKE_CXX_STANDARD 17)

#set(CMAKE_VERBOSE_MAKEFILE ON)
find_package(Catch2 REQUIRED)

include(../cmake/FSeamModule.cmake)

add_library(testLib
        ${CMAKE_CURRENT_SOURCE_DIR}/src/DependencyNonGettable.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/DependencyNonGettable.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/src/DependencyGettable.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/src/DependencyGettable.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/TestingClass.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/TestingClass.hh
        )
target_include_directories(testLib
        PUBLIC src)
target_link_libraries(testLib Catch2::Catch2)

enable_testing()
addFSeamTests(DESTINATION_TARGET testFSeam
        TARGET_AS_SOURCE testLib
        TST_SRC ${CMAKE_CURRENT_SOURCE_DIR}/testMain.cpp ${CMAKE_CURRENT_SOURCE_DIR}/FSeamBasicTestCase.cpp
        TO_MOCK ${CMAKE_CURRENT_SOURCE_DIR}/src/DependencyNonGettable.hh ${CMAKE_CURRENT_SOURCE_DIR}/src/DependencyGettable.hh)
