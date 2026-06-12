#include "Camera.h"

Camera::Camera() {

    m_projectionType = CameraProjection::Perspective;
    m_position = vec3(0.0, 0.0, 0.0);
    m_rotation = vec3(0.0, 0.0, 0.0);
    m_projection = mat4(1.0);
    m_view = mat4(1.0);

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

    mat4 rotation = RotateXYZ(m_rotation.x, m_rotation.y, m_rotation.z);
    mat4 translation = Translate(vec3(-m_position.x, -m_position.y, -m_position.z));
    m_view = rotation * translation;

}
