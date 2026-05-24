#pragma once

#include "Math/Lina64.h"

using namespace lina;

enum class CameraProjection {
    Perspective,
    Orthographic
};

class Camera {

public:

    Camera();

    void SetPosition(const vec3& position);
    void SetRotation(const vec3& rotation);

    void SetPerspective(
        double fov,
        double aspect,
        double nearPlane,
        double farPlane
    );

    void SetOrthographic(
        double left,
        double right,
        double bottom,
        double top,
        double nearPlane,
        double farPlane
    );

    void UpdateView();

    const mat4& GetProjection() const { return m_projection; }
    const mat4& GetView() const { return m_view; }

    const vec3& GetPosition() const { return m_position; }
    const vec3& GetRotation() const { return m_rotation; }

    CameraProjection GetProjectionType() const { return m_projectionType; }

private:

    CameraProjection m_projectionType;

    vec3 m_position;
    vec3 m_rotation;

    mat4 m_projection;
    mat4 m_view;

};