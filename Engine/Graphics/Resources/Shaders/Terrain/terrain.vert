#version 450

layout(push_constant) uniform PushConstants {
    mat4 mvp;
} pushData;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;

layout(location = 0) out vec3 outColor;

void main()
{
    gl_Position = pushData.mvp * vec4(inPosition, 1.0);
    outColor = inColor * vec3(0.5, 0.5, 0.5);
}