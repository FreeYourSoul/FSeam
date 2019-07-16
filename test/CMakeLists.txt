cmake_minimum_required(VERSION 3.5)
project(testing CXX)

set (CMAKE_CXX_STANDARD 17)

#set(CMAKE_VERBOSE_MAKEFILE ON)

# Travis required compile flag...
option(TRAVIS_BUILD "Travis only option for cmake" OFF)
if (TRAVIS_BUILD)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++17")
endif ()

include(../cmake/FSeamModule.cmake)

add_library(testLib
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ClassWithConstructor.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ClassWithConstructor.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/src/DependencyNonGettable.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/DependencyNonGettable.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/src/DependencyGettable.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/src/DependencyGettable.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ArgsStruct.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/src/TestingClass.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/TestingClass.hh)

target_include_directories(testLib
        PUBLIC src)
set_target_properties(testLib PROPERTIES CXX_STANDARD 17)

enable_testing()
set(FSEAM_GENERATOR_COMMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/../Generator/FSeamerFile.py)

addFSeamTests(
        DESTINATION_TARGET testFSeam
        TARGET_AS_SOURCE testLib
        TST_SRC
            ${CMAKE_CURRENT_SOURCE_DIR}/testMain.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/FSeamBasicMockTestCase.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/FSeamClassWithConstructorTestCase.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/FSeamDefaultMockTestCase.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/FSeamSingletonTestCase.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/FSeamGeneratedHelperUsageTestCase.cpp
        TO_MOCK
            ${CMAKE_CURRENT_SOURCE_DIR}/src/ClassWithConstructor.hh
            ${CMAKE_CURRENT_SOURCE_DIR}/src/DependencyNonGettable.hh
            ${CMAKE_CURRENT_SOURCE_DIR}/src/DependencyGettable.hh)

addFSeamTests(
        DESTINATION_TARGET testFSeamFreeFunction
        TARGET_AS_SOURCE testLib
        TST_SRC
            ${CMAKE_CURRENT_SOURCE_DIR}/testMain.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/FSeamFreeFunctionTestCase.cpp
        TO_MOCK
            ${CMAKE_CURRENT_SOURCE_DIR}/src/FreeFunctionClass.hh)
