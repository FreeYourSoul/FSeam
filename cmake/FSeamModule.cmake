cmake_minimum_required(VERSION 3.5)

set(FSEAM_GENERATOR_DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

option(FSEAM_FORCE_GENERATION "Force the generation of the file " ON)
option(FSEAM_CLEANUP_DATA "Cleanup the data file  " OFF)

option(FSEAM_USE_CATCH2 "fseam catch2 usage" ON)
option(FSEAM_USE_GTEST "fseam catch2 usage" OFF)

if (FSEAM_USE_CATCH2)
    find_package(Catch2 REQUIRED)
    include(Catch)
elseif(FSEAM_USE_GTEST)
    find_package(GTest REQUIRED)
    include(GoogleTest)
endif ()

find_package(Python3 COMPONENTS Interpreter REQUIRED)

if (NOT FSEAM_GENERATOR_COMMMAND)
    find_file(FILE_FSEAMER_PY FSeamerFile.py)
    set(FSEAM_GENERATOR_COMMMAND ${Python3_EXECUTABLE} ${FILE_FSEAMER_PY})
endif ()


include(CTest)

## ============ NOT CLIENT FACING ====================
## Function used internally in order to :
## - generate C++ Seam mock classess
## - create a cmake variable FSEAM_TEST_SRC containing all source for the current test
##
function (setup_FSeam_test)

#    message(WARNING "BEFORE Source compiled ${FSEAM_TEST_SRC}")
    foreach (fileToMockPath ${ADDFSEAMTESTS_TO_MOCK})
        get_filename_component(FSEAM_GENERATED_BASENAME ${fileToMockPath} NAME_WE)
        # TODO sanitize filename or use glob matching
        list(FILTER FSEAM_TEST_SRC EXCLUDE REGEX .*${FSEAM_GENERATED_BASENAME}.cpp)
        string(REPLACE ";" " " FSEAM_GENERATOR_PRINT "${FSEAM_GENERATOR_COMMMAND}")
        message(STATUS "add custom command for ${ADDFSEAMTESTS_DESTINATION_TARGET} with fileToMock ${fileToMockPath}\n"
            "with command : ${FSEAM_GENERATOR_PRINT} ${fileToMockPath} ${FSEAM_GENERATOR_DESTINATION}")
        add_custom_command(
            COMMAND
                ${FSEAM_GENERATOR_COMMMAND}
                ARGS
                    ${fileToMockPath}
                    ${FSEAM_GENERATOR_DESTINATION}
            OUTPUT
                ${FSEAM_GENERATOR_DESTINATION}/${FSEAM_GENERATED_BASENAME}.fseam.cc
            DEPENDS
                ${fileToMockPath}
            USES_TERMINAL
            COMMENT "Generating FSEAM code for ${fileToMockPath}")

        add_custom_target(${ADDFSEAMTESTS_DESTINATION_TARGET}${FSEAM_GENERATED_BASENAME}Run ALL
                DEPENDS
                    ${FSEAM_GENERATOR_DESTINATION}/${FSEAM_GENERATED_BASENAME}.fseam.cc)

        set(FSEAM_TEST_SRC ${FSEAM_TEST_SRC}
                ${FSEAM_GENERATOR_DESTINATION}/${FSEAM_GENERATED_BASENAME}.fseam.cc)

    endforeach()
#    message(WARNING "AFTER Source compiled ${FSEAM_TEST_SRC}")
    set(FSEAM_TEST_SRC ${FSEAM_TEST_SRC} PARENT_SCOPE)
endfunction (setup_FSeam_test)

## ============ CLIENT FACING ====================
## Function to call in order to generate a test executable from the generated FSeam mock and the provided test source
##
## Using CMake Parse Argument (explicitly named in the function call)
## Mandatory
## arg DESTINATION_TARGET  : target name of the test executable generated via this method
## arg TST_SRC             : files containing the actual test to compile (Catch2, GTest test files for example)
## arg TO_MOCK             : files to mock for this specific given test
##
## either 
## arg TARGET_AS_SOURCE    : target of the library that contains the code to test
## arg FILES_AS_SOURCE       or source file containing the code to test
## arg FOLDER_INCLUDES       with includes folder
## The above either would be translated to : TARGET_AS_SOURCE || (FILES_AS_SOURCE && FOLDER_INCLUDES)
## 
## optional 
## arg MAIN_FILE           : file containing the main (if any), this file will be removed from the compilation of the test
##
function(addFSeamTests)

    set(oneValueArgs DESTINATION_TARGET TARGET_AS_SOURCE MAIN_FILE)
    set(multiValueArgs TO_MOCK TST_SRC FILES_AS_SOURCE FOLDER_INCLUDES)
    cmake_parse_arguments(ADDFSEAMTESTS "" "${oneValueArgs}" "${multiValueArgs}"  ${ARGN} )

    # Check arguments

    # Get input arguments and generate sources
    if (ADDFSEAMTESTS_TARGET_AS_SOURCE AND NOT ADDFSEAMTESTS_TARGET_AS_SOURCE STREQUAL "")
        get_target_property(FSEAM_TEST_SRC ${ADDFSEAMTESTS_TARGET_AS_SOURCE} SOURCES)
        get_target_property(FSEAM_TEST_INCLUDES ${ADDFSEAMTESTS_TARGET_AS_SOURCE} INCLUDE_DIRECTORIES)
    else ()
        set(FSEAM_TEST_SRC ${ADDFSEAMTESTS_FILES_AS_SOURCE})
        set(FSEAM_TEST_INCLUDES ${ADDFSEAMTESTS_FOLDER_INCLUDES})
    endif ()
    if (ADDFSEAMTESTS_MAIN_FILE AND NOT ADDFSEAMTESTS_MAIN_FILE STREQUAL "")
        list(FILTER FSEAM_TEST_SRC EXCLUDE REGEX .*${ADDFSEAMTESTS_MAIN_FILE})
    endif ()
    setup_FSeam_test()

    # Create testing target
    execute_process(COMMAND ${CMAKE_COMMAND} -E touch ${FSEAM_GENERATOR_DESTINATION}/FSeamMockData.hpp ${FSEAM_GENERATOR_DESTINATION}/FSeamSpecialization.cpp)
    add_executable(${ADDFSEAMTESTS_DESTINATION_TARGET} ${FSEAM_TEST_SRC} ${ADDFSEAMTESTS_TST_SRC}
            ${FSEAM_GENERATOR_DESTINATION}/FSeamMockData.hpp
            ${FSEAM_GENERATOR_DESTINATION}/FSeamSpecialization.cpp)
    set_target_properties(${ADDFSEAMTESTS_DESTINATION_TARGET} PROPERTIES CXX_STANDARD 17)
    target_include_directories(${ADDFSEAMTESTS_DESTINATION_TARGET}
            PUBLIC
                ${FSEAM_TEST_INCLUDES}
                ${FSEAM_GENERATOR_DESTINATION}
                ${CMAKE_CURRENT_SOURCE_DIR}/../FSeam)

    if (FSEAM_USE_CATCH2)
        target_compile_definitions(${ADDFSEAMTESTS_DESTINATION_TARGET} PRIVATE FSEAM_USE_CATCH2)
        target_link_libraries(${ADDFSEAMTESTS_DESTINATION_TARGET} FSeam Catch2::Catch2)
        catch_discover_tests(${ADDFSEAMTESTS_DESTINATION_TARGET})
    elseif(FSEAM_USE_GTEST)
#        target_compile_definitions(${ADDFSEAMTESTS_DESTINATION_TARGET} PRIVATE )
#        target_link_libraries(${ADDFSEAMTESTS_DESTINATION_TARGET} FSeam )
#        gtest_discover_tests(${ADDFSEAMTESTS_DESTINATION_TARGET})
    endif ()

endfunction(addFSeamTests)
