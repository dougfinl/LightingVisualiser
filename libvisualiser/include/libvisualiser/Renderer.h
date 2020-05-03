//
// Created by dgf6 on 28/10/15.
//

#ifndef VISUALISER_RENDERER_H
#define VISUALISER_RENDERER_H

#include "IUpdatable.h"
#include "LightingRig.h"
#include "Camera.h"
#include <GL/glew.h>
#include <map>
#include <memory>
#include <vector>


struct GLFWwindow;

namespace visualiser {
namespace stage {

class Stage;
class Mesh;

} // namespace stage

namespace lighting {

class LightingRig;
class LightManager;
struct Light;

} // namespace lighting

namespace rendering {

class Camera;
class GeometryBuffer;
class ShaderProgram;

//
// An interactive OpenGL/GLFW renderer for a stage and lighting rig.
//
class Renderer: public IUpdatable {
    public:
        // Represents each lighting mode supported by the renderer.
        enum LightingMode {
            SIMPLE_LIGHTING,
            REALISTIC_LIGHTING
        };

        // Constructor.
        explicit Renderer(const unsigned int resX = 800, const unsigned int resY = 600, const bool fullscreen = false,
                          std::map<unsigned short, CameraPreset> cameraPresets = {});
        // Destructor.
        virtual ~Renderer();
        // Attaches a stage to the renderer. If a stage is already attached, it will be replaced.
        void setStage(std::shared_ptr<stage::Stage> stage);
        // Attaches a lighting rig to the renderer. If a lighting rig is already attached, it will be replaced.
        void setLightingRig(std::shared_ptr<lighting::LightingRig> rig);
        // Sets the possible camera presets for the renderer.
        void setCameraPresets(const std::map<unsigned short, CameraPreset> &presets);
        // Creates the interactive render window and starts rendering the scene.
        void start();
        // Stops rendering the scene and destroys the render window.
        void shutdown();
        // Changes the lighting mode of the renderer.
        void setLightingMode(const LightingMode mode);
        // Returns the current lighting mode of the renderer.
        LightingMode getLightingMode() const;

    private:
        // Represents each shader program used in the renderer.
        enum ShaderProgramType {
            GENERATE_SHADOW_MAP,
            SIMPLE_SCENE_OBJECT,
            SIMPLE_SCENE_LIGHT,
            GEOMETRY_PASS,
            LIGHTING_PASS_AMBIENT_LIGHT,
            LIGHTING_PASS_POINT_LIGHT,
            LIGHTING_PASS_SPOT_LIGHT,
            NUM_SHADER_PROGRAM_TYPES
        };

        void init();
        void initShaders();
        void initLighting();
        void initScene();
        void initShadowBuffers();
        void cleanup();
        void generateShadowMaps();
        void update();
        void render();
        void renderSimpleScene();
        void renderGeometryPass();
        void renderLightingPass();
        void blitGBuffer();
        void keyPressed(int key, int scancode, int action, int mods);
        void cursorMoved(double xpos, double ypos);
        void scrollWheel(double xoffset, double yoffset);
        ShaderProgram &shaderProgram(const ShaderProgramType &type);
        void loadCameraPreset(const unsigned short key);
        static void keyCallback(GLFWwindow *w, int keycode, int scan, int action, int mods);
        static void cursorPosCallback(GLFWwindow *w, double xpos, double ypos);
        static void scrollCallback(GLFWwindow *w, double xoffset, double yoffset);

        GLFWwindow *_window;
        std::unique_ptr<ShaderProgram[]> _shaderPrograms;
        std::unique_ptr<Camera> _camera;
        std::unique_ptr<GeometryBuffer> _gBuffer;
        std::shared_ptr<stage::Mesh> _quad;
        std::shared_ptr<stage::Stage> _stage;
        std::shared_ptr<lighting::LightingRig> _rig;
        double _oldCursorX;
        double _oldCursorY;
        unsigned int _resX;
        unsigned int _resY;
        bool _fullScreen;
        bool _showGBuffer;
        LightingMode _lightingMode;
        std::map<unsigned short, CameraPreset> _cameraPresets;
};

} // namespace rendering
} // namespace visualiser

#endif //VISUALISER_RENDERER_H
