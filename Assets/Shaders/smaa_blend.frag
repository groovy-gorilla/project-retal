#version 450

layout(set = 0, binding = 0) uniform sampler2D edgeTexture;

layout(location = 0) in vec2 fragUV;

layout(location = 0) out vec4 outBlend;

void main()
{
    vec2 edges = texture(edgeTexture, fragUV).rg;

    float weight = max(edges.r, edges.g);

    outBlend = vec4(weight, weight, weight, 1.0);
}