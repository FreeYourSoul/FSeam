cmake_minimum_required(VERSION 3.10)

set(FSEAM_GENERATOR_DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

option(FSEAM_FORCE_GENERATION "Force the generation of the file " ON)
option(FSEAM_CLEANUP_DATA "Cleanup the data file  " OFF)

find_package(FSeam REQUIRED)
set(FSEAM_GENERATOR_COMMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/../Generator/FSeamerFile.py)

find_package(Catch2 REQUIRED)
include(CTest)
include(Catch)

##
## Function used by client in order to :
## - generate C++ Seam mock classess
## - create a cmake variable FSEAM_TEST_SRC containing all source for the current test
##
function (setup_FSeam_test)

#    message(WARNING "BEFORE Source compiled ${FSEAM_TEST_SRC}")
    foreach (fileToMockPath ${ADDFSEAMTESTS_TO_MOCK})
        get_filename_component(FSEAM_GENERATED_BASENAME ${fileToMockPath} NAME_WE)
        list(FILTER FSEAM_TEST_SRC EXCLUDE REGEX .*${FSEAM_GENERATED_BASENAME}.cpp)
        message(STATUS "add custom command for ${ADDFSEAMTESTS_DESTINATION_TARGET} with fileToMock ${fileToMockPath}
with command : ${PYTHON_EXECUTABLE} ${FSEAM_GENERATOR_COMMMAND} ${fileToMockPath} ${FSEAM_GENERATOR_DESTINATION}")
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

        add_custom_target(${FSEAM_GENERATED_BASENAME}Run ALL
                DEPENDS ${FSEAM_GENERATOR_DESTINATION}/${FSEAM_GENERATED_BASENAME}.fseam.cc)

        set(FSEAM_TEST_SRC ${FSEAM_TEST_SRC}
                ${FSEAM_GENERATOR_DESTINATION}/${FSEAM_GENERATED_BASENAME}.fseam.cc)

    endforeach()
#    message(WARNING "AFTER Source compiled ${FSEAM_TEST_SRC}")
    set(FSEAM_TEST_SRC ${FSEAM_TEST_SRC} PARENT_SCOPE)
endfunction (setup_FSeam_test)

##
## Function to call in order to generate a test executable from the generated FSeam mock and the provided test source
##
## Mandatory
## arg DESTINATION_TARGET  : target name of the test executable generated via this method
## arg TST_SRC             : catch2 test files containing the actual test to compile
## arg TO_MOCK             : files to mock for this specific given test
##
## either
## arg TARGET_AS_SOURCE    : target of the library that contains the code to test
## arg FILES_AS_SOURCE       or source file containing the code to test
##
function(addFSeamTests)

    set(oneValueArgs DESTINATION_TARGET TARGET_AS_SOURCE)
    set(multiValueArgs TO_MOCK TST_SRC)
    cmake_parse_arguments(ADDFSEAMTESTS "" "${oneValueArgs}" "${multiValueArgs}"  ${ARGN} )

    # Check arguments

    # Get input arguments and generate sources
    get_target_property(FSEAM_TEST_SRC ${ADDFSEAMTESTS_TARGET_AS_SOURCE} SOURCES)
    get_target_property(FSEAM_TEST_INCLUDES ${ADDFSEAMTESTS_TARGET_AS_SOURCE} INCLUDE_DIRECTORIES)
    setup_FSeam_test()

    # Create testing target
    add_executable(${ADDFSEAMTESTS_DESTINATION_TARGET} ${ADDFSEAMTESTS_TST_SRC} ${FSEAM_TEST_SRC}
            ${FSEAM_GENERATOR_DESTINATION}/MockData.hpp
            ${FSEAM_GENERATOR_DESTINATION}/FSeamSpecialization.cpp)
    target_include_directories(${ADDFSEAMTESTS_DESTINATION_TARGET}
            PUBLIC
                ${FSEAM_TEST_INCLUDES}
                ${FSEAM_GENERATOR_DESTINATION}
                ${CMAKE_CURRENT_SOURCE_DIR}/../FSeam)
    target_link_libraries(${ADDFSEAMTESTS_DESTINATION_TARGET} FSeam Catch2::Catch2)
    catch_discover_tests(${ADDFSEAMTESTS_DESTINATION_TARGET})

endfunction(addFSeamTests)
