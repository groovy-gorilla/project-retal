#version 450

layout(set = 0, binding = 0) uniform sampler2D atlas;

layout(location = 0) in vec2 fragUV;
layout(location = 1) in vec4 fragColor;

layout(location = 0) out vec4 outColor;

void main() {

    float alpha = texture(atlas, fragUV).r;

    outColor = vec4(alpha, alpha, alpha, alpha * fragColor.a) * vec4(fragColor.rgb, 1.0);

}