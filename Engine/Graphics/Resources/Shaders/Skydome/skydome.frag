#version 450

layout(location = 0) in vec3 worldPos;
layout(location = 1) in vec3 direction;

layout(location = 0) out vec4 fragColor;

layout(set = 0, binding = 0) uniform SkyColors {
    vec4 zenith;
    vec4 sky;
    vec4 upperHorizon;
    vec4 horizon;
    vec4 groundHorizon;
    vec4 ground;
    float horizonOffset;
} skyData;

float rand(vec3 co) {
    return fract(sin(dot(co, vec3(12.9898, 78.233, 45.164))) * 43758.5453);
}

void main() {

    vec3 zenithColor        = skyData.zenith.rgb;
    vec3 skyColor           = skyData.sky.rgb;
    vec3 upperHorizonColor  = skyData.upperHorizon.rgb;
    vec3 horizonColor       = skyData.horizon.rgb;
    vec3 groundHorizonColor = skyData.groundHorizon.rgb;
    vec3 groundColor        = skyData.ground.rgb;

    float zenithY         = skyData.zenith.w;
    float skyY            = skyData.sky.w;
    float upperHorizonY   = skyData.upperHorizon.w;
    float horizonY        = skyData.horizon.w;
    float groundHorizonY  = skyData.groundHorizon.w;
    float groundY         = skyData.ground.w;

    vec3 dir = normalize(worldPos);
    float y = dir.y + skyData.horizonOffset;

    vec3 color;

    /*if (y > skyY) {
        float t = (y - skyY) / (zenithY - skyY);
        color = mix(skyColor, zenithColor, t);
    } else */

    if (y > upperHorizonY) {
        float t = (y - upperHorizonY) / (zenithY - upperHorizonY);
        color = mix(upperHorizonColor, zenithColor, t);
    } else if (y > horizonY) {
        float t = (y - horizonY) / (upperHorizonY - horizonY);
        color = mix(horizonColor, upperHorizonColor, t);
    } else if (y > groundHorizonY) {
        float t = (y - groundHorizonY) / (horizonY - groundHorizonY);
        color = mix(groundHorizonColor, horizonColor, t);
    } else {
        float t = (y - groundY) / (groundHorizonY - groundY);
        color = mix(groundColor, groundHorizonColor, t);
    }


    float noise = rand(direction * 1000);
    color += (noise - 0.5) / 256.0;

    fragColor = vec4(color, 1.0);
}