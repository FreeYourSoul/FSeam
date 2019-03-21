function (generateFSeamMock fileToMockName) 

    set (FSeamMockName ${fileToMockName}.fseam.cpp)

endfunction ()

function (setupFSeamGeneration listFileToMock )

    if (NOT listToMockPath)
        set (listToMockPath newPath PARENT_SCOPE)
    else ()
        list (APPEND listToMockPath newPath)
    endif()

endfunction ()
