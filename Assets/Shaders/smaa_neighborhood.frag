#version 450

layout(set = 0, binding = 0) uniform sampler2D blendTexture;

layout(location = 0) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

void main()
{
    ivec2 texSize = textureSize(blendTexture, 0);

    vec2 texel = 1.0 / vec2(texSize);

    vec4 center = texture(blendTexture, fragUV);

    vec4 left  = texture(blendTexture, fragUV - vec2(texel.x, 0.0));
    vec4 right = texture(blendTexture, fragUV + vec2(texel.x, 0.0));

    vec4 up    = texture(blendTexture, fragUV - vec2(0.0, texel.y));
    vec4 down  = texture(blendTexture, fragUV + vec2(0.0, texel.y));

    outColor =
    (center + left + right + up + down) / 5.0;
}