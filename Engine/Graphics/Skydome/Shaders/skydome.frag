#version 450

layout(location = 0) in vec3 worldPos;
layout(location = 1) in float altitude;

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


    float y = normalize(worldPos).y + skyData.horizonOffset;

    vec3 color;

    // ZENITH
    if (y > skyY) {
        color = zenithColor;
    }

    // SKY -> ZENITH
    else if (y > upperHorizonY) {
        float t = (y - upperHorizonY) / (skyY - upperHorizonY);
        t = 1.0 - pow(1.0 - t, 2.5);
        color = mix(upperHorizonColor, skyColor, t);
    }

    // HORIZON -> SKY
    else if (y > horizonY) {
        float t = (y - horizonY) / (upperHorizonY - horizonY);
        color = mix(horizonColor, upperHorizonColor, t);
    }

    // GROUND HORIZON -> HORIZON
    else if (y > groundHorizonY) {
        float t = (y - groundHorizonY) / (horizonY - groundHorizonY);
        color = mix(groundHorizonColor, horizonColor, t);
    }

    // GROUND -> GROUND HORIZON
    else if (y > groundY){
        float t = (y - groundY) / (groundHorizonY - groundY);

        float h = clamp(altitude / 20000.0f, 0.0f, 1.0f);
        float exponent = 8.0f - 6.0f * sqrt(h);

        t = pow(t, exponent);
        color = mix(groundColor, groundHorizonColor, t);
    }

    // NADIR
    else {
        color = groundColor;
    }

    fragColor = vec4(color, 1.0);
}