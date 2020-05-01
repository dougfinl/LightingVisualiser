cmake_minimum_required(VERSION 3.16.4)
project(LightingVisualiser
  LANGUAGES CXX
  )

include(FetchContent)

# CMake module search path.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

set(CMAKE_BUILD_TYPE Debug)

# Include headers from the library sub-project.
include_directories(libvisualiser)

# CMake sub-projects.
add_subdirectory(libvisualiser)

# Exectuable and linked library.
add_executable(visualiser main.cpp)
target_link_libraries(visualiser libvisualiser)

# Boost Program Options.
find_package(Boost 1.72.0 COMPONENTS program_options REQUIRED)
link_directories(${Boost_LIBRARY_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(visualiser ${Boost_LIBRARIES})

# ASSIMP
FetchContent_Declare(
  assimp
  GIT_REPOSITORY https://github.com/assimp/assimp
  GIT_TAG        v5.0.1
  )
FetchContent_MakeAvailable(assimp)

# GLEW
 FetchContent_Declare(
   glew
   GIT_REPOSITORY https://github.com/nigels-com/glew
   GIT_TAG        glew-2.1.0
   )
 FetchContent_MakeAvailable(glew)

# GLFW3
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw
  GIT_TAG        3.3.2
  )
FetchContent_MakeAvailable(glfw)

# GLM
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm
  GIT_TAG        0.9.9.7
  )
FetchContent_MakeAvailable(glm)

# Protobuf
FetchContent_Declare(
  protobuf
  GIT_REPOSITORY https://github.com/protocolbuffers/protobuf
  GIT_TAG        v3.11.4
  )
FetchContent_MakeAvailable(protobuf)

# YAML-CPP
FetchContent_Declare(
  yaml-cpp
  GIT_REPOSITORY https://github.com/jbeder/yaml-cpp
  GIT_TAG        yaml-cpp-0.6.3
  )
FetchContent_MakeAvailable(yaml-cpp)
