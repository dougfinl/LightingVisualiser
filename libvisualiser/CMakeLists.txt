cmake_minimum_required(VERSION 3.16.4)

add_definitions(-DGLM_FORCE_RADIANS)

set(VISUALISER_SOURCE_FILES
        visualiser/rendering/Renderer.cpp
        visualiser/rendering/Renderer.h
        visualiser/stage/Stage.cpp
        visualiser/stage/Stage.h
        visualiser/rendering/ShaderProgram.cpp
        visualiser/rendering/ShaderProgram.h
        visualiser/rendering/Camera.cpp
        visualiser/rendering/Camera.h
        visualiser/stage/Mesh.cpp
        visualiser/stage/Mesh.h
        visualiser/stage/Model.cpp
        visualiser/stage/Model.h
        visualiser/lighting/LightManager.cpp
        visualiser/lighting/LightManager.h
        visualiser/lighting/DMXListener.cpp
        visualiser/lighting/DMXListener.h
        visualiser/lighting/LightingRig.cpp
        visualiser/lighting/LightingRig.h
        visualiser/ShowFileLoader.cpp
        visualiser/ShowFileLoader.h
        visualiser/rendering/IUpdatable.h
        visualiser/rendering/IRenderable.h
        visualiser/Visualiser.cpp
        visualiser/Visualiser.h
        visualiser/Constants.h
        visualiser/ColourManager.cpp
        visualiser/ColourManager.h
        visualiser/stage/Texture.cpp
        visualiser/stage/Texture.h
        visualiser/rendering/GeometryBuffer.h
        visualiser/rendering/GeometryBuffer.cpp
        visualiser/rendering/PostProcessBuffer.cpp
        visualiser/rendering/PostProcessBuffer.h
        visualiser/rendering/AbstractFramebuffer.h
        visualiser/rendering/AbstractFramebuffer.cpp
        visualiser/rendering/ShadowBuffer.cpp
        visualiser/rendering/ShadowBuffer.h
        ola/BoostLogDestination.cpp
        ola/BoostLogDestination.h)
add_library(libvisualiser STATIC ${VISUALISER_SOURCE_FILES})

set_target_properties(libvisualiser PROPERTIES
  CXX_STANDARD          17
  CXX_STANDARD_REQUIRED YES
  CXX_EXTENSIONS        NO
  )

# OpenGL
find_package(OpenGL REQUIRED
  COMPONENTS OpenGL
  )

# GLEW
find_package(GLEW REQUIRED STATIC)
include_directories(${GLEW_INCLUDE_DIRS})
target_link_libraries(libvisualiser ${GLEW_LIBRARIES})

# OLA
find_package(OLA REQUIRED)
include_directories(${OLA_INCLUDE_DIRS})
target_link_libraries(libvisualiser ${OLA_LIBRARIES})

# Boost
find_package(Boost 1.72.0 COMPONENTS log log_setup thread system filesystem REQUIRED)
link_directories(${Boost_LIBRARY_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(libvisualiser ${Boost_LIBRARIES})

# Final linkage
target_link_libraries(libvisualiser yaml-cpp glfw assimp protobuf glm OpenGL::OpenGL)
