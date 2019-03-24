cmake_minimum_required(VERSION 3.5)
project(testing CXX)

set(CMAKE_VERBOSE_MAKEFILE ON)
find_package(Catch2 REQUIRED)

include(../cmake/FSeamModule.cmake)

add_library(testLib
        src/DependencyNonGettable.cpp
        src/DependencyNonGettable.hh
        src/DependencyGettable.hh
        src/DependencyGettable.cpp
        src/TestingClass.cpp
        src/TestingClass.hh
        )
target_include_directories(testLib
        PUBLIC src)
target_link_libraries(testLib Catch2::Catch2)

enable_testing()
addFSeamTests(DESTINATION_TARGET testFSeam
        SOURCE_TARGET testLib
        TST_SRC testMain.cpp FSeamBasicTestCase.cpp
        TO_MOCK src/DependencyNonGettable.cpp src/DependencyGettable.cpp)