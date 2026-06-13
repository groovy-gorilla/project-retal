#include "Camera.h"

Camera::Camera() {

    m_projectionType = CameraProjection::Perspective;
    m_position = vec3(0.0, 0.0, 0.0);
    m_rotation = vec3(0.0, 0.0, 0.0);
    m_projection = mat4(1.0);
    m_view = mat4(1.0);

    UpdateView();

}

void Camera::SetPosition(const vec3& position) {

    m_position = position;
    UpdateView();

}

void Camera::SetRotation(const vec3& rotation) {

    m_rotation = rotation;
    UpdateView();

}

void Camera::SetPerspective(double fov, double aspect, double nearPlane, double farPlane) {

    m_projectionType = CameraProjection::Perspective;
    m_projection = Perspective(fov, aspect, nearPlane, farPlane);

}

void Camera::SetOrthographic(double left, double right, double bottom, double top, double nearPlane, double farPlane) {

    m_projectionType = CameraProjection::Orthographic;
    m_projection = Orthographic(left, right, bottom, top, nearPlane, farPlane);

}

void Camera::UpdateView() {

    m_view = LookAt(
        m_position,
        m_position + GetForward(),
        vec3(0.0, 1.0, 0.0)
    );

}

vec3 Camera::GetForward() const {

    double pitch = m_rotation.x;
    double yaw = m_rotation.y;

    vec3 forward;

    forward.x = std::cos(pitch) * std::sin(yaw);
    forward.y = std::sin(pitch);
    forward.z = -std::cos(pitch) * std::cos(yaw);

    return Normalize(forward);
}

vec3 Camera::GetRight() const {

    return Normalize(
        Cross(
            GetForward(),
            vec3(0.0, 1.0, 0.0)
        )
    );
}

vec3 Camera::GetUp() const {

    return Normalize(
        Cross(
            GetRight(),
            GetForward()
        )
    );
}

void Camera::MoveForward(double distance) {

    m_position += GetForward() * distance;

    UpdateView();
}

void Camera::MoveRight(double distance) {

    m_position += GetRight() * distance;

    UpdateView();
}

void Camera::MoveUp(double distance) {

    //m_position += GetUp() * distance;
    m_position.y += distance;

    UpdateView();
}

void Camera::AddRotation(double pitch, double yaw, double roll) {
    m_rotation.x += pitch;
    m_rotation.y += yaw;
    m_rotation.z += roll;

    m_rotation.x = std::clamp(
        m_rotation.x,
        ToRadians(-89.0),
        ToRadians(89.0)
    );

    UpdateView();
}

void Camera::AddPosition(const vec3& delta) {

    m_position += delta;

    UpdateView();
}




















