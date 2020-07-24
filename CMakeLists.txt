cmake_minimum_required(VERSION 3.5)

# The version number.
set (FSEAM_VERSION_MAJOR 1)
set (FSEAM_VERSION_MINOR 0)
set (FSEAM_VERSION_PATCH 1)
set (FSEAM_VERSION ${FSEAM_VERSION_MAJOR}.${FSEAM_VERSION_MINOR}.${FSEAM_VERSION_PATCH})

project(FSeam VERSION ${FSEAM_VERSION})

set(CMAKE_VERBOSE_MAKEFILE ON)

list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
        "${CMAKE_CURRENT_SOURCE_DIR}/FSeam/Versioner.hh.in"
        "${CMAKE_CURRENT_SOURCE_DIR}/FSeam/Versioner.hh")

message(STATUS "FSeam v${FSEAM_VERSION}")

set(FSEAM_HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/FSeam/FSeam.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/FSeam/Versioner.hh)

set(FSEAM_GENERATOR_PYTH
        ${CMAKE_CURRENT_SOURCE_DIR}/Generator/FSeamerFile.py
        ${CMAKE_CURRENT_SOURCE_DIR}/Generator/CppHeaderParser.py)
        

foreach (target FSeam FSeam-static)
  add_library(${target} INTERFACE)
  target_include_directories(${target} INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/FSeam/>
                                                 $<INSTALL_INTERFACE:include>)
endforeach()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(TARGETS FSeam FSeam-static
        EXPORT ${PROJECT_NAME}-targets)

install(FILES ${FSEAM_HEADERS} DESTINATION include/FSeam)
install(PROGRAMS ${FSEAM_GENERATOR_PYTH} DESTINATION share/bin)
install(PROGRAMS ${FSEAM_GENERATOR_PYTH} DESTINATION bin)

# GNUInstallDirs "DATADIR" wrong here; CMake search path wants "share".
set(FSEAM_CMAKECONFIG_INSTALL_DIR "share/cmake/${PROJECT_NAME}" CACHE STRING "install path for FSeamConfig.cmake")

export(EXPORT ${PROJECT_NAME}-targets
     FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

configure_package_config_file(${PROJECT_NAME}Config.cmake.in
                        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
                        INSTALL_DESTINATION ${FSEAM_CMAKECONFIG_INSTALL_DIR})

write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
                        VERSION ${FSEAM_VERSION}                        
                        COMPATIBILITY AnyNewerVersion)

install(EXPORT ${PROJECT_NAME}-targets
		NAMESPACE FSeam::
        FILE ${PROJECT_NAME}Targets.cmake
        DESTINATION ${FSEAM_CMAKECONFIG_INSTALL_DIR})

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        DESTINATION ${FSEAM_CMAKECONFIG_INSTALL_DIR})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/cmake/FSeamModule.cmake
        DESTINATION ${FSEAM_CMAKECONFIG_INSTALL_DIR})

 option(FSEAM_BUILD_TESTS "Whether or not to build the tests" ON)
 if (FSEAM_BUILD_TESTS)
     enable_testing()
     add_subdirectory(test)
 endif()

