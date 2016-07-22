cmake_minimum_required(VERSION 2.8.4)

# add_definitions(-DBOOST_LOG_DYN_LINK)
add_definitions(-DGLM_FORCE_RADIANS)

set(Boost_USE_STATIC_LIBS ON)

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

# OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_libraries(libvisualiser ${OpenGL_LIBRARIES})

# GLEW
find_package(GLEW REQUIRED STATIC)
include_directories(${GLEW_INCLUDE_DIRS})
target_link_libraries(libvisualiser ${GLEW_LIBRARIES})

# GLFW3
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
include_directories(${GLFW_INCLUDE_DIRS})
target_link_libraries(libvisualiser ${GLFW_STATIC_LIBRARIES})

# Protobuf
pkg_search_module(PROTOBUF REQUIRED protobuf)
include_directories(${PROTOBUF_INCLUDE_DIRS})
target_link_libraries(libvisualiser ${PROTOBUF_STATIC_LIBRARIES})

# OLA
find_package(OLA REQUIRED)
include_directories(${OLA_INCLUDE_DIRS})
target_link_libraries(libvisualiser ${OLA_LIBRARIES})

# YAML-CPP
find_package(YamlCpp REQUIRED)
include_directories(${YAMLCPP_INCLUDE_DIR})
target_link_libraries(libvisualiser ${YAMLCPP_LIBRARY})

# ASSIMP
pkg_search_module(ASSIMP REQUIRED assimp)
include_directories(${ASSIMP_INCLUDE_DIRS})
target_link_libraries(libvisualiser ${ASSIMP_LIBRARIES})

# Boost
find_package(Boost COMPONENTS log log_setup thread system filesystem REQUIRED)
link_directories(${Boost_LIBRARY_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(libvisualiser ${Boost_LIBRARIES})
