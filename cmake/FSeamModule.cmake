cmake_minimum_required(VERSION 3.5)

set(FSEAM_GENERATOR_DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/fseam)
set(FSEAM_GENERATOR_EXE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Generator/FSeamerFile.py>
                        $<INSTALL_INTERFACE:FSeamerFile.py>)

option(FSEAM_FORCE_GENERATION "Force the generation of the file " ON)
option(FSEAM_CLEANUP_DATA "Cleanup the data file  " OFF)

find_package(Catch2 REQUIRED)
include(CTest)
include(Catch)

##
## Function used by client in order to :
## - generate C++ Seam mock classess
## - create a cmake variable FSEAM_TEST_SRC containing all source for the current test
##
function (setup_FSeam_test testSrc listToMockPath)

    foreach (fileToMockPath ${listToMockPath})
        list(REMOVE_ITEM FSEAM_TEST_SRC fileToMockPath)
        get_filename_component(FSEAM_GENERATED_FILENAME ${fileToMockPath} NAME_WE)
        add_custom_command(
            COMMAND ${FSEAM_GENERATOR_EXE} fileToMockPath ${FSEAM_GENERATOR_DESTINATION} ${FSEAM_FORCE_GENERATION}
            OUTPUT  ${FSEAM_GENERATOR_DESTINATION}/MockData.hpp ${FSEAM_GENERATOR_DESTINATION}/${FSEAM_GENERATED_FILENAME}
            DEPENDS ${FSEAM_GENERATOR_EXE}
            COMMENT "Generating FSEAM code for ${fileToMockPath}")

        list(APPEND ${FSEAM_TEST_SRC} ${FSEAM_GENERATOR_DESTINATION}/MockData.hpp ${FSEAM_GENERATOR_DESTINATION}/${FSEAM_GENERATED_FILENAME})
    endforeach()

endfunction (setup_FSeam_test)

##
## Function to call in order to generate a test executable from the generated FSeam mock and the provided test source
##
## arg DESTINATION_TARGET  : target name of the test executable generated via this method
## arg SOURCE_TARGET          : target of the library that contains the code to test
## arg TST_SRC             : catch2 test files containing the actual test to compile
## arg TO_MOCK             : files to mock for this specific given test
##
function(addFSeamTests)

    set(oneValueArgs DESTINATION_TARGET SOURCE_TARGET)
    set(multiValueArgs TO_MOCK TST_SRC)
    cmake_parse_arguments(ADDFSEAMTESTS "" "${oneValueArgs}" "${multiValueArgs}"  ${ARGN} )

    get_target_property(FSEAM_TEST_SRC ${ADDFSEAMTESTS_SOURCE_TARGET} SOURCES)
    get_target_property(FSEAM_TEST_INCLUDES ${ADDFSEAMTESTS_SOURCE_TARGET} INCLUDE_DIRECTORIES)

    setup_FSeam_test(${FSEAM_TEST_SRC} ${ADDFSEAMTESTS_TO_MOCK})
    message(STATUS "Adding FSEAM testing executable with source ${ADDFSEAMTESTS_TST_SRC};${FSEAM_TEST_SRC}")
    add_executable(${ADDFSEAMTESTS_DESTINATION_TARGET} ${ADDFSEAMTESTS_TST_SRC} ${FSEAM_TEST_SRC})
    target_include_directories(${ADDFSEAMTESTS_DESTINATION_TARGET} PUBLIC ${FSEAM_TEST_INCLUDES})
    target_link_libraries(${ADDFSEAMTESTS_DESTINATION_TARGET} Catch2::Catch2)
    catch_discover_tests(${ADDFSEAMTESTS_DESTINATION_TARGET})

endfunction(addFSeamTests)
