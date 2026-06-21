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

    vec3 GetForward() const;
    vec3 GetRight() const;
    vec3 GetUp() const;
    double GetFov() const { return m_fov; }
    void MoveForward(double distance);
    void MoveRight(double distance);
    void MoveUp(double distance);
    void AddRotation(double pitch, double yaw, double roll = 0.0);
    void AddPosition(const vec3& delta);

    void ClampPosition();

private:

    CameraProjection m_projectionType;

    vec3 m_position;
    vec3 m_rotation;

    mat4 m_projection;
    mat4 m_view;

    double m_fov;

};