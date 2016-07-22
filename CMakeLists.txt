cmake_minimum_required(VERSION 2.8.4)
project(CO3015LightingVisualiser)

# add_definitions(-DCMAKE_BUILD_TYPE=Debug)

# CMake module search path.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

# C++ compiler flags.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Werror -pedantic")

set(Boost_USE_STATIC_LIBS ON)

# Include headers from the library sub-project.
include_directories(libvisualiser)

# CMake sub-projects.
add_subdirectory(libvisualiser)

# Exectuable and linked library.
add_executable(visualiser main.cpp)
target_link_libraries(visualiser libvisualiser)

# Boost Program Options.
find_package(Boost COMPONENTS program_options REQUIRED)
link_directories(${Boost_LIBRARY_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(visualiser ${Boost_LIBRARIES})

