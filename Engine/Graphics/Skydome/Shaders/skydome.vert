#version 450

layout(push_constant) uniform PushConstants {
    mat4 mvp;
    float inAltitude;
} pushData;

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec3 worldPos;
layout(location = 1) out vec3 altitude;

void main() {

    worldPos = inPosition;

    gl_Position = pushData.mvp * vec4(inPosition, 1.0);

    altitude = pushData.inAltitude;
}