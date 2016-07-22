//
// Created by dougfinl on 02/11/15.
//

#ifndef VISUALISER_CAMERA_H
#define VISUALISER_CAMERA_H

#include "IUpdatable.h"
#include <glm/glm.hpp>
#include <map>


namespace visualiser {
namespace rendering {

struct CameraPreset {
    glm::vec3 position;
    glm::vec3 target;
};

//
// A simple orbital camera.
//
class Camera: public IUpdatable {
    public:
        static const float CLIP_DIST_NEAR;
        static const float CLIP_DIST_FAR;
        static const float FOV;
        static const float ZOOM_SPEED;
        static const float MIN_RADIUS;
        static const float MAX_RADIUS;
        static const float X_ROT_SPEED;
        static const float Y_ROT_SPEED;
        static const float MIN_PITCH;
        static const float MAX_PITCH;

        // Creates an orbital camera.
        Camera();
        // Destructor
        virtual ~Camera();
        // Updates the camera's transformation matrix.
        virtual void update();
        // Updates the viewport size of the camera, important for maintaining the correct aspect ratio.
        void setViewportSize(float width, float height);
        // Zooms the camera based on the defined ZOOM_SPEED.
        void scrollWheel(double amount);
        // Orbits the camera.
        void cursorMoved(double deltax, double deltay);
        // Calculates the camera's transformation matrix.
        glm::mat4 getMatrix() const;
        // Gets the camera's view matrix.
        glm::mat4 getView() const;
        // Gets the camera's projection matrix.
        glm::mat4 getProjection() const;
        // Loads the camera's parameters from a given preset.
        void loadPreset(const CameraPreset &preset);

    private:
        void reset();

        float _viewportWidth;
        float _viewportHeight;
        glm::vec3 _target;
        glm::vec3 _up;
        float _radius;
        float _heading;
        float _pitch;
        glm::mat4 _projection;
        glm::mat4 _view;
};

} // namespace rendering
} // namespace visualiser

#endif //VISUALISER_CAMERA_H
