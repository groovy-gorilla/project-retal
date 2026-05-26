#version 450

layout(push_constant) uniform PushConstants
{
    mat4 projection;
    vec2 position;
    vec2 size;

} push;

layout(location = 0) out vec2 fragUV;

void main() {

    vec2 positions[4] = {
        vec2(0.0, 0.0),
        vec2(1.0, 0.0),
        vec2(0.0, 1.0),
        vec2(1.0, 1.0)
    };

    vec2 uvs[4] = {
        vec2(0.0, 0.0),
        vec2(1.0, 0.0),
        vec2(0.0, 1.0),
        vec2(1.0, 1.0)
    };

    vec2 localPos = positions[gl_VertexIndex];

    vec2 worldPos = push.position + localPos * push.size;

    gl_Position = push.projection * vec4(worldPos, 0.0, 1.0);

    fragUV = uvs[gl_VertexIndex];
}