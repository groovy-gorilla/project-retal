#version 450

layout(binding = 0)
uniform sampler2D imageTexture;

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 outColor;

void main()
{
    //outColor = texture(imageTexture, uv);
    outColor = texture(imageTexture, vec2(uv.x, 1.0 - uv.y));
}