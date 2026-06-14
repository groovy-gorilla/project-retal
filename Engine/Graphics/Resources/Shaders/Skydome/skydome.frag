#version 450

layout(location = 0) in vec3 worldPos;

layout(location = 0) out vec4 fragColor;

layout(set = 0, binding = 0) uniform SkyColors {
    vec4 zenith;
    vec4 sky;
    vec4 horizon;
    vec4 groundHorizon;
    vec4 ground;
    float horizonOffset;
} skyData;

void main() {

    vec3 zenithColor        = skyData.zenith.rgb;
    vec3 skyColor           = skyData.sky.rgb;
    vec3 horizonColor       = skyData.horizon.rgb;
    vec3 groundHorizonColor = skyData.groundHorizon.rgb;
    vec3 groundColor        = skyData.ground.rgb;

    float zenithY         = skyData.zenith.w;
    float skyY            = skyData.sky.w;
    float horizonY        = skyData.horizon.w;
    float groundHorizonY  = skyData.groundHorizon.w;
    float groundY         = skyData.ground.w;

    vec3 dir = normalize(worldPos);

    float y = dir.y + skyData.horizonOffset;

    vec3 color;

    if (y > skyY){
        float t = (y - skyY) / (zenithY - skyY);
        color = mix(skyColor, zenithColor, t);
    } else if (y > horizonY){
        float t = (y - horizonY) / (skyY - horizonY);
        color = mix(horizonColor, skyColor, t);
    } else if (y > groundHorizonY){
        float t = (y - groundHorizonY) / (horizonY - groundHorizonY);
        color = mix(groundHorizonColor, horizonColor, t);
    } else {
        float t = (y - groundY) / (groundHorizonY - groundY);
        color = mix(groundColor, groundHorizonColor, t);
    }

    fragColor = vec4(color, 1.0);
}