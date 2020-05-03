//
// Created by dgf6 on 28/10/15.
//

#include "libvisualiser/Renderer.h"
#include "libvisualiser/ShaderProgram.h"
#include "libvisualiser/LightManager.h"
#include "libvisualiser/Stage.h"
#include "Constants.h"
#include "libvisualiser/Camera.h"
#include "libvisualiser/GeometryBuffer.h"
#include <GLFW/glfw3.h>
#include <boost/log/trivial.hpp>


visualiser::rendering::Renderer::Renderer(const unsigned int resX, const unsigned int resY, const bool fullscreen,
                                          std::map<unsigned short, CameraPreset> cameraPresets) :
        _shaderPrograms(std::make_unique<visualiser::rendering::ShaderProgram[]>(NUM_SHADER_PROGRAM_TYPES)),
        _camera(std::make_unique<visualiser::rendering::Camera>()),
        _gBuffer(std::make_unique<visualiser::rendering::GeometryBuffer>()),
        _resX(resX),
        _resY(resY),
        _fullScreen(fullscreen),
        _showGBuffer(false),
        _lightingMode(SIMPLE_LIGHTING) {
}

visualiser::rendering::Renderer::~Renderer() {
}

void visualiser::rendering::Renderer::setStage(std::shared_ptr<stage::Stage> stage) {
    _stage = stage;
}

void visualiser::rendering::Renderer::setLightingRig(std::shared_ptr<lighting::LightingRig> rig) {
    _rig = rig;
}

void visualiser::rendering::Renderer::setCameraPresets(const std::map<unsigned short, CameraPreset> &presets) {
    _cameraPresets = presets;
}

void visualiser::rendering::Renderer::start() {
    BOOST_LOG_TRIVIAL(info) << "Initialising renderer";

    if (!_stage) {
        BOOST_LOG_TRIVIAL(error) << "No stage attached to renderer";
        return;
    }

    if (!_rig) {
        BOOST_LOG_TRIVIAL(error) << "No lighting rig attached to renderer";
        return;
    }

    init();

    BOOST_LOG_TRIVIAL(info) << "Started render loop";
    while (!glfwWindowShouldClose(_window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        update();
        render();

        glfwSwapBuffers(_window);
        glfwPollEvents();
    }

    cleanup();
}

void visualiser::rendering::Renderer::shutdown() {
    glfwSetWindowShouldClose(_window, GL_TRUE);
}

visualiser::rendering::Renderer::LightingMode visualiser::rendering::Renderer::getLightingMode() const {
    return _lightingMode;
}

void visualiser::rendering::Renderer::init() {
    if (!glfwInit()) {
        BOOST_LOG_TRIVIAL(error) << "Failed to initialise GLFW";
        exit(EXIT_FAILURE);
    }

    BOOST_LOG_TRIVIAL(info) << "GLFW Version " << glfwGetVersionString();

    // Attempt to use modern OpenGL (version 3.2+)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);

    BOOST_LOG_TRIVIAL(debug) << "Using monitor " << glfwGetMonitorName(monitor);

    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
    glfwWindowHint(GLFW_SRGB_CAPABLE, GL_TRUE);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    if (_fullScreen) {
        _resX = (unsigned) mode->width;
        _resY = (unsigned) mode->height;

        _window = glfwCreateWindow(_resX, _resY, "Lighting Visualiser", monitor, NULL);
    } else {
        _window = glfwCreateWindow(_resX, _resY, "Lighting Visualiser", NULL, NULL);
    }

    if (!_window) {
        BOOST_LOG_TRIVIAL(error) << "Failed to create window";
        glfwDestroyWindow(_window);
        exit(EXIT_FAILURE);
    }

    // Set a custom pointer on the window to the location of this Renderer (used for callbacks)
    glfwSetWindowUserPointer(_window, this);
    glfwSetKeyCallback(_window, Renderer::keyCallback);
    glfwSetCursorPosCallback(_window, Renderer::cursorPosCallback);
    glfwSetScrollCallback(_window, Renderer::scrollCallback);

    glfwMakeContextCurrent(_window);
    glfwSwapInterval(1);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        BOOST_LOG_TRIVIAL(error) << "Failed to initialise GLEW";
        glfwDestroyWindow(_window);
        exit(EXIT_FAILURE);
    }
    BOOST_LOG_TRIVIAL(info) << "GLEW version " << glewGetString(GLEW_VERSION);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_FRAMEBUFFER_SRGB);

    // Cull back faces, use Assimp's winding order - front faces are wound counter-clockwise.
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    _quad = stage::Mesh::quad();

    initShaders();
    initScene();
    initLighting();

    _gBuffer->init(_resX, _resY);

    _camera->setViewportSize(_resX, _resY);
}

void visualiser::rendering::Renderer::initShaders() {
    shaderProgram(GENERATE_SHADOW_MAP).loadShaderFromFile(
            SHADERS_PATH + "generate_shadow_map.vert",
            ShaderProgram::VERTEX
    );
    shaderProgram(GENERATE_SHADOW_MAP).loadShaderFromFile(
            SHADERS_PATH + "generate_shadow_map.frag",
            ShaderProgram::FRAGMENT
    );
    shaderProgram(GENERATE_SHADOW_MAP).link();

    shaderProgram(SIMPLE_SCENE_OBJECT).loadShaderFromFile(
            SHADERS_PATH + "simple_scene_object.vert",
            ShaderProgram::VERTEX
    );
    shaderProgram(SIMPLE_SCENE_OBJECT).loadShaderFromFile(
            SHADERS_PATH + "simple_scene_object.frag",
            ShaderProgram::FRAGMENT
    );
    shaderProgram(SIMPLE_SCENE_OBJECT).link();

    shaderProgram(SIMPLE_SCENE_LIGHT).loadShaderFromFile(
            SHADERS_PATH + "simple_scene_light.vert",
            ShaderProgram::VERTEX
    );
    shaderProgram(SIMPLE_SCENE_LIGHT).loadShaderFromFile(
            SHADERS_PATH + "simple_scene_light.geom",
            ShaderProgram::GEOMETRY
    );
    shaderProgram(SIMPLE_SCENE_LIGHT).loadShaderFromFile(
            SHADERS_PATH + "simple_scene_light.frag",
            ShaderProgram::FRAGMENT
    );
    shaderProgram(SIMPLE_SCENE_LIGHT).link();

    shaderProgram(GEOMETRY_PASS).loadShaderFromFile(
            SHADERS_PATH + "geometry.vert",
            ShaderProgram::VERTEX);
    shaderProgram(GEOMETRY_PASS).loadShaderFromFile(
            SHADERS_PATH + "geometry.frag",
            ShaderProgram::FRAGMENT);
    shaderProgram(GEOMETRY_PASS).link();

    shaderProgram(LIGHTING_PASS_AMBIENT_LIGHT).loadShaderFromFile(
            SHADERS_PATH + "ambient_light.vert",
            ShaderProgram::VERTEX);
    shaderProgram(LIGHTING_PASS_AMBIENT_LIGHT).loadShaderFromFile(
            SHADERS_PATH + "ambient_light.frag",
            ShaderProgram::FRAGMENT);
    shaderProgram(LIGHTING_PASS_AMBIENT_LIGHT).link();

    shaderProgram(LIGHTING_PASS_POINT_LIGHT).loadShaderFromFile(
            SHADERS_PATH + "point_light.vert",
            ShaderProgram::VERTEX);
    shaderProgram(LIGHTING_PASS_POINT_LIGHT).loadShaderFromFile(
            SHADERS_PATH + "point_light.frag",
            ShaderProgram::FRAGMENT);
    shaderProgram(LIGHTING_PASS_POINT_LIGHT).link();

    shaderProgram(LIGHTING_PASS_SPOT_LIGHT).loadShaderFromFile(
            SHADERS_PATH + "spot_light.vert",
            ShaderProgram::VERTEX);
    shaderProgram(LIGHTING_PASS_SPOT_LIGHT).loadShaderFromFile(
            SHADERS_PATH + "spot_light.frag",
            ShaderProgram::FRAGMENT);
    shaderProgram(LIGHTING_PASS_SPOT_LIGHT).link();
}

void visualiser::rendering::Renderer::initLighting() {
    _rig->init();

    initShadowBuffers();
    generateShadowMaps();
}

void visualiser::rendering::Renderer::initScene() {
    for (auto const &model : _stage->getModels()) {
        model->init();
    }
}

void visualiser::rendering::Renderer::initShadowBuffers() {
    std::vector<lighting::SpotLight> *spotLights = _rig->getSpotLights();
    for (auto light = spotLights->begin(); light != spotLights->end(); light++) {
        light->shadowBuffer = new ShadowBuffer();
        light->shadowBuffer->init(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
    }
}

void visualiser::rendering::Renderer::cleanup() {
    BOOST_LOG_TRIVIAL(debug) << "Cleaning up renderer";
    glfwDestroyWindow(_window);
}

void visualiser::rendering::Renderer::generateShadowMaps() {
    BOOST_LOG_TRIVIAL(debug) << "Generating shadow maps";

    shaderProgram(GENERATE_SHADOW_MAP).use();

    // Create a shadow map for each individual spot light.
    for (auto spotLight : *_rig->getSpotLights()) {
        // Bind the light's shadow buffer for writing, and clear it.
        spotLight.shadowBuffer->bind(true);
        glClear(GL_DEPTH_BUFFER_BIT);

        glViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);

        shaderProgram(GENERATE_SHADOW_MAP).setUniform("matSpotLight", spotLight.matrix);

        for (auto const &m : _stage->getModels()) {
            m->render(shaderProgram(GENERATE_SHADOW_MAP));
        }

        BOOST_LOG_TRIVIAL(trace) << "Generated shadow map for spot light #" << spotLight.base.base.address;
    }

    // Bind the default framebuffer (GLFW window).
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Reset the viewport after generating shadow maps.
    glViewport(0, 0, _resX, _resY);

    BOOST_LOG_TRIVIAL(debug) << "Finished generating shadow maps";
}

void visualiser::rendering::Renderer::update() {
    _camera->update();
}

void visualiser::rendering::Renderer::render() {
    if (_lightingMode == REALISTIC_LIGHTING) {
        renderGeometryPass();

        if (_showGBuffer) {
            blitGBuffer();
        } else {
            renderLightingPass();
        }
    } else {
        renderSimpleScene();
    }
}

void visualiser::rendering::Renderer::renderSimpleScene() {
    // Draw to the default bufffer (GLFW window).
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Enable blending.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Render the objects.
    shaderProgram(SIMPLE_SCENE_OBJECT).use();
    shaderProgram(SIMPLE_SCENE_OBJECT).setUniform("camera", _camera->getMatrix());

    for (auto const &m : _stage->getModels()) {
        m->render(shaderProgram(SIMPLE_SCENE_OBJECT));
    }

    // Create a VAO for attribute-less rendering.
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Render lights.
    shaderProgram(SIMPLE_SCENE_LIGHT).use();
    shaderProgram(SIMPLE_SCENE_LIGHT).setUniform("camera", _camera->getMatrix());
    // Send spot lights to the shader, and draw.
    std::vector<lighting::SpotLight> *spotLights = _rig->getSpotLights();
    for (auto light = spotLights->begin(); light != spotLights->end(); light++) {
        std::string uniform = "spotLight.";
        shaderProgram(SIMPLE_SCENE_LIGHT).setUniform(std::string(uniform + "base.base.colour").c_str(),
                                                     light->base.base.colour);
        shaderProgram(SIMPLE_SCENE_LIGHT).setUniform(std::string(uniform + "base.base.intensity").c_str(),
                                                     light->base.base.intensity);
        shaderProgram(SIMPLE_SCENE_LIGHT).setUniform(std::string(uniform + "base.position").c_str(),
                                                     light->base.position);
        shaderProgram(SIMPLE_SCENE_LIGHT).setUniform(std::string(uniform + "direction").c_str(), light->direction);

        // Draw the light.
        glDrawArrays(GL_POINTS, 0, 1);
    }

    glDeleteVertexArrays(1, &vao);

    // Cleanup: disable blending.
    glDisable(GL_BLEND);
}

void visualiser::rendering::Renderer::renderGeometryPass() {
    // Bind the framebuffer for writing, and clear it.
    _gBuffer->bind(true);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaderProgram(GEOMETRY_PASS).use();
    shaderProgram(GEOMETRY_PASS).setUniform("camera", _camera->getMatrix());

    for (auto const &m : _stage->getModels()) {
        m->render(shaderProgram(GEOMETRY_PASS));
    }
}

void visualiser::rendering::Renderer::renderLightingPass() {
    // Bind the OpenGL context for writing, and the geometry buffer for reading.
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    _gBuffer->bind();

    // Clear the main OpenGL context.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Enable additive blending.
    glEnable(GL_BLEND);
    glBlendEquation(GL_ADD);
    glBlendFunc(GL_ONE, GL_ONE);

    ///////////////////
    // AMBIENT LIGHT //
    ///////////////////

    shaderProgram(LIGHTING_PASS_AMBIENT_LIGHT).use();

    // Geometry buffer texture uniform.
    shaderProgram(LIGHTING_PASS_AMBIENT_LIGHT).setUniform("gDiffuse", SHADER_G_DIFFUSE_TEXTURE);
    shaderProgram(LIGHTING_PASS_AMBIENT_LIGHT).setUniform("ambientIntensity", _rig->getAmbientIntensity());

    // Render the full-screen quadrilateral.
    _quad->render(shaderProgram(LIGHTING_PASS_POINT_LIGHT));

    //////////////////
    // POINT LIGHTS //
    //////////////////

    shaderProgram(LIGHTING_PASS_POINT_LIGHT).use();

    // Geometry buffer texture uniforms.
    shaderProgram(LIGHTING_PASS_POINT_LIGHT).setUniform("gNormal", SHADER_G_NORMAL_TEXTURE);
    shaderProgram(LIGHTING_PASS_POINT_LIGHT).setUniform("gDiffuse", SHADER_G_DIFFUSE_TEXTURE);
    shaderProgram(LIGHTING_PASS_POINT_LIGHT).setUniform("gSpecular", SHADER_G_SPECULAR_TEXTURE);
    shaderProgram(LIGHTING_PASS_POINT_LIGHT).setUniform("gDepth", SHADER_G_DEPTH_TEXTURE);

    // Camera uniforms.
    shaderProgram(LIGHTING_PASS_POINT_LIGHT).setUniform("matView", _camera->getView());
    shaderProgram(LIGHTING_PASS_POINT_LIGHT).setUniform("matProjection", _camera->getProjection());

    // Draw each point light individually.
    for (auto pointLight : *_rig->getPointLights()) {
        // If the intensity is 0, skip the light.
        if (pointLight.base.intensity < 0.01f)
            continue;

        shaderProgram(LIGHTING_PASS_POINT_LIGHT).setUniform("pointLight.base.colour", pointLight.base.colour);
        shaderProgram(LIGHTING_PASS_POINT_LIGHT).setUniform("pointLight.base.intensity", pointLight.base.intensity);
        shaderProgram(LIGHTING_PASS_POINT_LIGHT).setUniform("pointLight.position", pointLight.position);

        // Render the full-screen quadrilateral.
        _quad->render(shaderProgram(LIGHTING_PASS_POINT_LIGHT));
    }

    /////////////////
    // SPOT LIGHTS //
    /////////////////

    shaderProgram(LIGHTING_PASS_SPOT_LIGHT).use();

    // Geometry buffer texture uniforms.
    shaderProgram(LIGHTING_PASS_SPOT_LIGHT).setUniform("gNormal", SHADER_G_NORMAL_TEXTURE);
    shaderProgram(LIGHTING_PASS_SPOT_LIGHT).setUniform("gDiffuse", SHADER_G_DIFFUSE_TEXTURE);
    shaderProgram(LIGHTING_PASS_SPOT_LIGHT).setUniform("gSpecular", SHADER_G_SPECULAR_TEXTURE);
    shaderProgram(LIGHTING_PASS_SPOT_LIGHT).setUniform("gDepth", SHADER_G_DEPTH_TEXTURE);

    // Camera uniforms.
    shaderProgram(LIGHTING_PASS_SPOT_LIGHT).setUniform("matView", _camera->getView());
    shaderProgram(LIGHTING_PASS_SPOT_LIGHT).setUniform("matProjection", _camera->getProjection());

    // Draw each spot light individually.
    for (auto spotLight : *_rig->getSpotLights()) {
        // If the intensity is 0, skip the light.
        if (spotLight.base.base.intensity < 0.01f)
            continue;

        shaderProgram(LIGHTING_PASS_SPOT_LIGHT).setUniform("spotLight.base.base.colour", spotLight.base.base.colour);
        shaderProgram(LIGHTING_PASS_SPOT_LIGHT).setUniform("spotLight.base.base.intensity", spotLight.base.base.intensity);
        shaderProgram(LIGHTING_PASS_SPOT_LIGHT).setUniform("spotLight.base.position", spotLight.base.position);
        shaderProgram(LIGHTING_PASS_SPOT_LIGHT).setUniform("spotLight.direction", spotLight.direction);
        shaderProgram(LIGHTING_PASS_SPOT_LIGHT).setUniform("spotLight.beamAngle", spotLight.beamAngle);
        shaderProgram(LIGHTING_PASS_SPOT_LIGHT).setUniform("spotLight.fieldAngle", spotLight.fieldAngle);
        shaderProgram(LIGHTING_PASS_SPOT_LIGHT).setUniform("spotLight.matrix", spotLight.matrix);

        // Bind the shadow map texture for reading, and set the shadow map uniform to that location.
        spotLight.shadowBuffer->bind();
        shaderProgram(LIGHTING_PASS_SPOT_LIGHT).setUniform("spotLight.texShadowMap", SHADER_SHADOW_TEXTURE);

        if (spotLight.hasGobo) {
            // Bind the gobo texture for reading, and set the gobo texture uniform to that location.
            spotLight.gobo->bind();
            shaderProgram(LIGHTING_PASS_SPOT_LIGHT).setUniform("spotLight.hasGobo", true);
            shaderProgram(LIGHTING_PASS_SPOT_LIGHT).setUniform("spotLight.goboMatrix", spotLight.goboMatrix);
            shaderProgram(LIGHTING_PASS_SPOT_LIGHT).setUniform("spotLight.texGobo", SHADER_GOBO_TEXTURE);
        } else {
            shaderProgram(LIGHTING_PASS_SPOT_LIGHT).setUniform("spotLight.hasGobo", false);
        }

        // Render the full-screen quadrilateral.
        _quad->render(shaderProgram(LIGHTING_PASS_SPOT_LIGHT));
    }

    // Cleanup: disable additive blending.
    glDisable(GL_BLEND);
}

void visualiser::rendering::Renderer::blitGBuffer() {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    _gBuffer->bind();

    // Print the multiple render targets to the screen.
    // Normal buffer.
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glBlitFramebuffer(0, 0, _resX, _resY, 0, _resY / 2.0, _resX / 2.0, _resY,
                      GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    // Diffuse buffer.
    glReadBuffer(GL_COLOR_ATTACHMENT1);
    glBlitFramebuffer(0, 0, _resX, _resY, _resX / 2.0, _resY / 2.0, _resX, _resY,
                      GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    // Specular buffer.
    glReadBuffer(GL_COLOR_ATTACHMENT2);
    glBlitFramebuffer(0, 0, _resX, _resY, 0, 0, _resX / 2.0, _resY / 2.0,
                      GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    // Depth buffer.
    glReadBuffer(GL_DEPTH_ATTACHMENT);
    glBlitFramebuffer(0, 0, _resX, _resY, _resX / 2.0, 0, _resX, _resY / 2.0,
                      GL_DEPTH_BUFFER_BIT, GL_NEAREST);
}

void visualiser::rendering::Renderer::keyPressed(int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                shutdown();
                break;
            case GLFW_KEY_L:
                // Toggle lighting mode.
                setLightingMode((getLightingMode() == REALISTIC_LIGHTING ? SIMPLE_LIGHTING : REALISTIC_LIGHTING));
                _showGBuffer = false;
                break;
            case GLFW_KEY_G:
                // Toggle viewing of the g-buffer.
                _showGBuffer = !_showGBuffer;
                break;
            case GLFW_KEY_0:
                loadCameraPreset(0);
                break;
            case GLFW_KEY_1:
                loadCameraPreset(1);
                break;
            case GLFW_KEY_2:
                loadCameraPreset(2);
                break;
            case GLFW_KEY_3:
                loadCameraPreset(3);
                break;
            case GLFW_KEY_4:
                loadCameraPreset(4);
                break;
            case GLFW_KEY_5:
                loadCameraPreset(5);
                break;
            case GLFW_KEY_6:
                loadCameraPreset(6);
                break;
            case GLFW_KEY_7:
                loadCameraPreset(7);
                break;
            case GLFW_KEY_8:
                loadCameraPreset(8);
                break;
            case GLFW_KEY_9:
                loadCameraPreset(9);
                break;
            default:
                break;
        }
    }
}

void visualiser::rendering::Renderer::cursorMoved(double xpos, double ypos) {
    if (glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_LEFT))
        _camera->cursorMoved(xpos - _oldCursorX, ypos - _oldCursorY);

    _oldCursorX = xpos;
    _oldCursorY = ypos;
}

void visualiser::rendering::Renderer::scrollWheel(double xoffset, double yoffset) {
    (void) xoffset;
    _camera->scrollWheel(-yoffset);
}

visualiser::rendering::ShaderProgram &visualiser::rendering::Renderer::shaderProgram(
        const Renderer::ShaderProgramType &type) {
    return _shaderPrograms[type];
}

void visualiser::rendering::Renderer::setLightingMode(const LightingMode mode) {
    _lightingMode = mode;
}

void visualiser::rendering::Renderer::loadCameraPreset(const unsigned short key) {
    if (_cameraPresets.count(key) > 0) {
        BOOST_LOG_TRIVIAL(trace) << "Loading camera preset " << key;
        _camera->loadPreset(_cameraPresets[key]);
    } else {
        BOOST_LOG_TRIVIAL(warning) << "Camera preset " << key << " does not exist";
    }
}

/******************
 * STATIC METHODS *
 ******************/
void visualiser::rendering::Renderer::keyCallback(GLFWwindow *w, int keycode, int scan, int action, int mods) {
    static_cast<Renderer *>(glfwGetWindowUserPointer(w))->keyPressed(keycode, scan, action, mods);
}

void visualiser::rendering::Renderer::cursorPosCallback(GLFWwindow *w, double xpos, double ypos) {
    static_cast<Renderer *>(glfwGetWindowUserPointer(w))->cursorMoved(xpos, ypos);
}

void visualiser::rendering::Renderer::scrollCallback(GLFWwindow *w, double xoffset, double yoffset) {
    static_cast<Renderer *>(glfwGetWindowUserPointer(w))->scrollWheel(xoffset, yoffset);
}
