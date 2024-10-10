/***************************************************************************************************
 * @file  Camera.cpp
 * @brief Implementation of the Camera class
 **************************************************************************************************/

#include "Camera.hpp"

#include <cmath>
#include <glm/trigonometric.hpp>

Camera::Camera(const vec3& position)
    : movementSpeed(20.0f),
      position(position),
      worldUp(0.0f, 1.0f, 0.0f),
      view(1.0f) {

    pitch = 0.0f;
    yaw = M_PIf;

    look(vec2());
}

mat4 Camera::getVPmatrix(const mat4& projection) const {
    return projection * view;
}

const mat4& Camera::getViewMatrix() const {
    return view;
}

vec3 Camera::getPosition() const {
    return position;
}

const vec3& Camera::getPositionReference() const {
    return position;
}

vec3 Camera::getDirection() const {
    return front;
}

vec3 Camera::getRight() const {
    return right;
}

vec3 Camera::getUp() const {
    return up;
}

void Camera::move(CameraControls direction, float deltaTime) {
    const float speed = movementSpeed * deltaTime;

    switch(direction) {
        case CameraControls::forward:
            position += front * speed;
            break;
        case CameraControls::backward:
            position -= front * speed;
            break;
        case CameraControls::left:
            position -= normalize(cross(front, worldUp)) * speed;
            break;
        case CameraControls::right:
            position += normalize(cross(front, worldUp)) * speed;
            break;
        case CameraControls::upward:
            position.y += speed;
            break;
        case CameraControls::downward:
            position.y -= speed;
            break;
    }

    view[3][0] = -dot(right, position);
    view[3][1] = -dot(up, position);
    view[3][2] = dot(front, position);
}

void Camera::look(vec2 mouseOffset) {
    constexpr float sensitivity = 0.1f;
    constexpr float epsilon = 0.00001f;
    mouseOffset *= sensitivity;

    yaw += radians(mouseOffset.x);
    if(yaw > 2.0f * M_PIf) {
        yaw -= 2.0f * M_PIf;
    } else if(yaw < -2.0f * M_PIf) {
        yaw += 2.0f * M_PIf;
    }

    pitch -= radians(mouseOffset.y);
    if(pitch > M_PI_2f - epsilon) {
        pitch = M_PI_2f - epsilon;
    } else if(pitch < -M_PI_2f + epsilon) {
        pitch = -M_PI_2f + epsilon;
    }

    front.x = cosf(pitch) * cosf(yaw);
    front.y = sinf(pitch);
    front.z = cosf(pitch) * sinf(yaw);

    right = normalize(cross(front, worldUp));
    up = normalize(cross(right, front));

    view[0][0] = right.x;
    view[1][0] = right.y;
    view[2][0] = right.z;
    view[3][0] = -dot(right, position);

    view[0][1] = up.x;
    view[1][1] = up.y;
    view[2][1] = up.z;
    view[3][1] = -dot(up, position);

    view[0][2] = -front.x;
    view[1][2] = -front.y;
    view[2][2] = -front.z;
    view[3][2] = dot(front, position);
}
