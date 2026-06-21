#version 450

layout(location = 0) in vec2 uv;
layout(location = 1) in float altitude;
layout(location = 2) in float pitch;
layout(location = 3) in float roll;
layout(location = 4) in float fov;

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


    vec2 p = uv * 2.0 - 1.0;

    float c = cos(roll);
    float s = sin(roll);

    vec2 r;
    r.x = p.x * c - p.y * s;
    r.y = p.x * s + p.y * c;

    // Kąt pionowy to 90°, fov to 60°
    // 90° - fov = 30°
    // Więc diff w dół = -30°, góra = +30°
    float diff = radians(90.0) - fov - (fov / 2.0);
    float screenAngle = -r.y * diff;

    // Rzeczywisty kąt patrzenia
    float angle = screenAngle + pitch;

    // Normalizacja do zakresu około -1..1
    float y = angle / (0.5 * 3.14159265359);

    // Korekta horyzontu
    y += skyData.horizonOffset;

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