#version 450

vec2 vertices[3] = vec2[](
        vec2(-1.0, -1.0),
        vec2( 3.0, -1.0),
        vec2(-1.0,  3.0)
);

layout(push_constant) uniform PushConstants {
    float inAltitude;
    float inPitch;
    float inRoll;
    float inFov;
} pushData;

layout(location = 0) out vec2 uv;
layout(location = 1) out float altitude;
layout(location = 2) out float pitch;
layout(location = 3) out float roll;
layout(location = 4) out float fov;

void main() {

    vec2 pos = vertices[gl_VertexIndex];

    uv = pos * 0.5 + 0.5;

    gl_Position = vec4(pos, 0.0, 1.0);//pushData.mvp * vec4(inPosition, 1.0);

    altitude = pushData.inAltitude;
    pitch = pushData.inPitch;
    roll = pushData.inRoll;
    fov = pushData.inFov;

}