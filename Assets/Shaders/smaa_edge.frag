#version 450

layout(set = 0, binding = 0) uniform sampler2D sceneTexture;

layout(location = 0) in vec2 fragUV;

layout(location = 0) out vec2 outEdges;

void main()
{
    ivec2 texSize = textureSize(sceneTexture, 0);

    vec2 texel = 1.0 / vec2(texSize);

    vec3 C  = texture(sceneTexture, fragUV).rgb;
    vec3 CX = texture(sceneTexture, fragUV + vec2(texel.x, 0.0)).rgb;
    vec3 CY = texture(sceneTexture, fragUV + vec2(0.0, texel.y)).rgb;

    float deltaX = length(C - CX);
    float deltaY = length(C - CY);

    float threshold = 0.1;

    float edgeHorizontal = step(threshold, deltaX);
    float edgeVertical   = step(threshold, deltaY);

    outEdges = vec2(edgeHorizontal, edgeVertical);
}