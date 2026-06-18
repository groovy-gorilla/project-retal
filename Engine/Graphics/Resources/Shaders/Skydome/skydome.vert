#version 450

layout(push_constant) uniform PushConstants {
    mat4 mvp;
} pushData;

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec3 worldPos;
layout(location = 1) out vec3 direction;

void main() {

    worldPos = inPosition;

    gl_Position = pushData.mvp * vec4(inPosition, 1.0);
    direction = normalize(inPosition);
}