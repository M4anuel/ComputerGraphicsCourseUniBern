cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
cmake_policy(VERSION 3.12.0)
project(RayTracing
        VERSION 19
        LANGUAGES CXX
)

# https://blog.kitware.com/cmake-and-the-default-build-type/
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo"  CACHE STRING "Choose the type of build." FORCE)
    # options for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
            "Debug" "Release" "RelWithDebInfo")
endif()

foreach(suffix "" "_Debug" "_Release" "_RelWithDebInfo")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY${suffix} "${CMAKE_CURRENT_BINARY_DIR}")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY${suffix} "${CMAKE_CURRENT_BINARY_DIR}")
endforeach()


add_subdirectory(src)

# documentation
find_package(Doxygen)
if(DOXYGEN_FOUND)
    # prepare doxygen configuration file
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    # add doc as target
    add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
endif(DOXYGEN_FOUND)
