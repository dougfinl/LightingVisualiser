//
// Created by dougfinl on 02/11/15.
//

#include <glm/gtc/matrix_transform.hpp>
#include "libvisualiser/Camera.h"
#include <boost/log/trivial.hpp>


const float visualiser::rendering::Camera::CLIP_DIST_NEAR = 0.1f;
const float visualiser::rendering::Camera::CLIP_DIST_FAR  = 150.0f;
const float visualiser::rendering::Camera::FOV            = 50.0f;
const float visualiser::rendering::Camera::ZOOM_SPEED     = 0.5f;
const float visualiser::rendering::Camera::MIN_RADIUS     = 5.0f;
const float visualiser::rendering::Camera::MAX_RADIUS     = 100.0f;
const float visualiser::rendering::Camera::X_ROT_SPEED    = 0.5f;
const float visualiser::rendering::Camera::Y_ROT_SPEED    = 0.5f;
const float visualiser::rendering::Camera::MIN_PITCH      = -89.5f;
const float visualiser::rendering::Camera::MAX_PITCH      = 89.5f;

visualiser::rendering::Camera::Camera() :
  _viewportWidth(1),
  _viewportHeight(1) {
    reset();
}

visualiser::rendering::Camera::~Camera() {
}

void visualiser::rendering::Camera::update() {
    glm::mat4 transformation(1.0f); // Identity matrix
    transformation = glm::rotate(transformation, glm::radians(_heading), glm::vec3(0.0f, 1.0f, 0.0f));
    transformation = glm::rotate(transformation, glm::radians(-_pitch), glm::vec3(1.0f, 0.0f, 0.0f));
    transformation = glm::translate(transformation, glm::vec3(0.0f, 0.0f, _radius));

    glm::vec3 position = (glm::vec3) (transformation * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    _projection = (glm::mat4) glm::perspective(glm::radians(FOV), _viewportWidth / _viewportHeight,
                                               CLIP_DIST_NEAR, CLIP_DIST_FAR);
    _view = glm::lookAt(position, _target, _up);
}

void visualiser::rendering::Camera::setViewportSize(float width, float height) {
    _viewportWidth = width;
    _viewportHeight = height;
}

void visualiser::rendering::Camera::scrollWheel(double amount) {
    float newRadius = _radius + ((float) amount * ZOOM_SPEED);
    _radius = glm::clamp(newRadius, MIN_RADIUS, MAX_RADIUS);
}

void visualiser::rendering::Camera::cursorMoved(double deltax, double deltay) {
    _heading -= (float) deltax * X_ROT_SPEED;
    if (_heading < 0.0f)
        _heading = 360.0f - _heading;
    else if (_heading >= 360.0f)
        _heading = _heading - 360.0f;

    _pitch += (float) deltay * Y_ROT_SPEED;
    _pitch = glm::clamp(_pitch, MIN_PITCH, MAX_PITCH);
}

glm::mat4 visualiser::rendering::Camera::getMatrix() const {
    return _projection * _view;
}

glm::mat4 visualiser::rendering::Camera::getView() const {
    return _view;
}

glm::mat4 visualiser::rendering::Camera::getProjection() const {
    return _projection;
}

void visualiser::rendering::Camera::loadPreset(const CameraPreset &preset) {
    // Ensure the camera's position isn't the same as its target.
    if (preset.position == preset.target) {
        BOOST_LOG_TRIVIAL(error) << "Camera position cannot be the same as target";
        return;
    }

    glm::vec3 targetToCamera = preset.position - preset.target;

    _target = preset.target;

    // Calculate the radius for the camera, and clamp the magnitude of targetToCamera to fit the radius constraints.
    float radius = glm::length(targetToCamera);
    if (radius < MIN_RADIUS) {
        BOOST_LOG_TRIVIAL(warning) << "Camera radius too small, clamping to " << MIN_RADIUS;
        targetToCamera = glm::normalize(targetToCamera) * MIN_RADIUS;
    } else if (radius > MAX_RADIUS) {
        BOOST_LOG_TRIVIAL(warning) << "Camera radius too large, clamping to " << MAX_RADIUS;
        targetToCamera = glm::normalize(targetToCamera) * MAX_RADIUS;
    }
    _radius = radius;

    float pitch = glm::degrees(glm::asin(targetToCamera.y / radius));
    if (pitch < MIN_PITCH) {
        BOOST_LOG_TRIVIAL(warning) << "Camera pitch is too low, clamping to " << MIN_PITCH << " degrees";
        pitch = MIN_PITCH;
    } else if (pitch > MAX_PITCH) {
        BOOST_LOG_TRIVIAL(warning) << "Camera pitch is too high, clamping to " << MAX_PITCH << " degrees";
        pitch = MAX_PITCH;
    }
    _pitch = pitch;

    float heading = glm::degrees(glm::atan(targetToCamera.x, targetToCamera.z));
    _heading = heading;
}

void visualiser::rendering::Camera::reset() {
    _radius = 5.0f;
    _heading = 0.0f;
    _pitch = 0.0f;
    _target = glm::vec3(0.0f, 0.0f, 0.0f);
    _up = glm::vec3(0.0f, 1.0f, 0.0f);

    update();
}
