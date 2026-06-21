#version 450

layout(push_constant) uniform PushConstants {
    mat4 projection;
    vec4 position;
    vec4 color;
} push;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inUV;

layout(location = 0) out vec2 fragUV;
layout(location = 1) out vec4 fragColor;

void main()
{
    vec2 worldPos = inPosition + push.position.xy;

    gl_Position = push.projection * vec4(worldPos, 0.0, 1.0);

    fragUV = inUV;
    fragColor = push.color;
}