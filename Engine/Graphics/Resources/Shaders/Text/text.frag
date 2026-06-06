#version 450

layout(set = 0, binding = 0) uniform sampler2D atlas;

layout(location = 0) in vec2 fragUV;
layout(location = 1) in vec4 fragColor;

layout(location = 0) out vec4 outColor;

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

void main() {

    vec4 texel = texture(atlas, fragUV);

    float msdf = median(texel.r, texel.g, texel.b);

    float sdf = texel.a;

    float dist = max(msdf, sdf);

    float w = fwidth(dist);

    float alpha = smoothstep(0.5 - w * 0.5, 0.5 + w * 0.5, dist);

    outColor = vec4(fragColor.rgb, alpha * fragColor.a);

}