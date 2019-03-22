set(FSEAM_GENERATOR_DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/fseam)

function (generateFSeamMock fileToMockName)

    set (FSeamMockName ${fileToMockName}.fseam.cpp)

endfunction ()

function (setupFSeamGeneration listFileNameToMock)

    if (NOT listToMockPath)
        set (listToMockPath newPath PARENT_SCOPE)
    else ()
        list (APPEND listToMockPath newPath)
    endif()

    foreach (fileToMockPath listToMockPath)
        get_filename_component(FSEAM_GENERATED_FILENAME ${fileToMockPath} NAME_WE)
        add_custom_command(
            COMMAND ${FSEAM_GENERATOR_EXE} fileToMockPath ${FSEAM_GENERATOR_DESTINATION} ${FSEAM_FORCE_GENERATION}
            OUTPUT  ${FSEAM_GENERATOR_DESTINATION}/MockData.hpp ${FSEAM_GENERATOR_DESTINATION}/${FSEAM_GENERATED_FILENAME}
            DEPENDS ${FSEAM_GENERATOR_EXE}
            COMMENT "Generating FSEAM code for ${fileToMockPath}")
    endforeach()

endfunction ()
