cmake_minimum_required(VERSION 3.16.4)

add_definitions(-DGLM_FORCE_RADIANS)

add_library(libvisualiser STATIC
    src/AbstractFramebuffer.cpp
    src/BoostLogDestination.h
    src/BoostLogDestination.cpp
    src/Camera.cpp
    src/ColourManager.cpp
    src/DMXListener.cpp
    src/GeometryBuffer.cpp
    src/LightManager.cpp
    src/LightingRig.cpp
    src/Mesh.cpp
    src/Model.cpp
    src/PostProcessBuffer.cpp
    src/Renderer.cpp
    src/ShaderProgram.cpp
    src/ShadowBuffer.cpp
    src/ShowFileLoader.cpp
    src/Stage.cpp
    src/Texture.cpp
    src/VisualiserCore.cpp
    )

target_include_directories(libvisualiser
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE src
    )

set_target_properties(libvisualiser PROPERTIES
    CXX_STANDARD          17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS        NO
    )

# OpenGL
find_package(OpenGL COMPONENTS OpenGL REQUIRED)

# GLEW
find_package(GLEW REQUIRED STATIC)
include_directories(${GLEW_INCLUDE_DIRS})
target_link_libraries(libvisualiser PRIVATE ${GLEW_LIBRARIES})

# OLA
find_package(OLA REQUIRED)
include_directories(${OLA_INCLUDE_DIRS})
target_link_libraries(libvisualiser PRIVATE PRIVATE ${OLA_LIBRARIES})

# Boost
find_package(Boost 1.72.0 COMPONENTS log log_setup thread system filesystem REQUIRED)
link_directories(${Boost_LIBRARY_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(libvisualiser PRIVATE ${Boost_LIBRARIES})

# Final linkage
target_link_libraries(libvisualiser PRIVATE yaml-cpp glfw assimp protobuf glm OpenGL::OpenGL)
