cmake_minimum_required(VERSION 3.16.4)

project(LightingVisualiser
  LANGUAGES CXX
  )

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

# CMake ExternalProject dependencies
include(cmake/depends.cmake)

# Include headers from the library sub-project
# include_directories(libvisualiser)

# Build the sub-projects
add_subdirectory(libvisualiser)
add_subdirectory(visualiser)
