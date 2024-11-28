/***************************************************************************************************
 * @file  water.frag
 * @brief Fragment shader for rendering the water
 **************************************************************************************************/

#version 420 core

out vec4 fragColor;

#define alpha fraagColor.a

uniform vec3 cameraPos;
uniform float time;
uniform vec3 cameraFront;
uniform vec3 cameraRight;
uniform vec3 cameraUp;
uniform vec2 resolution;

const uint MAX_STEPS = 64;
const float MIN_DISTANCE = 0.001f;
const float MAX_DISTANCE = 1000.0f;
const float WATER_DEPTH = 5.0f;
const float DRAG = 0.38f;

struct Ray {
    vec3 origin;
    vec3 direction;
};

vec2 getUV() {
    return (2.0f * gl_FragCoord.xy - resolution) / resolution.y;
}

vec2 wavedx(vec2 pos, vec2 dir, float freq, float timeShift) {
    float x = dot(dir, pos) * freq + timeShift;
    float wave = exp(sin(x) - 1.0f);
    float dx = wave * cos(x);
    return vec2(wave, -dx);
}

float getWaves(vec2 position) {
    float waveShift = length(position);
    float iter = 0.0f;
    float freq = 1.0f;
    float timeMult = 2.0f;
    float weight = 1.0f;
    float sumValues = 0.0f;
    float sumWaights = 0.0f;

    for (int i = 0; i < 12; i++) {
        vec2 p = vec2(sin(iter), cos(iter));
        vec2 wave = wavedx(position, p, freq, time * timeMult + waveShift);

        position += p * wave.y * weight * DRAG;

        sumValues += wave.x * weight;
        sumWaights += weight;

        weight *= 0.8f;
        freq *= 1.018f;
        timeMult *= 1.07f;
        iter += 123.399963;
    }

    return sumValues / sumWaights;
}

float map(vec3 position) {
    float waveHeight = getWaves(position.xz) * WATER_DEPTH;
    float toWater = position.y - waveHeight;
    vec3 color = vec3(0.3f, 0.6f, 0.8f);

    return toWater;
}

vec3 calculateNormal(vec2 position, float epsilon) {
    float height = getWaves(position) * WATER_DEPTH;
    vec3 p = vec3(position.x, height, position.y);

    vec3 dx = p - vec3(position.x - epsilon, getWaves(position - vec2(epsilon, 0.0f)) * WATER_DEPTH, position.y);
    vec3 dy = p - vec3(position.x, getWaves(position + vec2(epsilon, 0.0f)) * WATER_DEPTH, position.y + epsilon);

    return normalize(cross(dx, dy));
}

float raymarch(in Ray ray) {
    float distance;
    float distanceFromOrigin = 0.0f;

    for (uint i = 0u; i < MAX_STEPS; ++i) {
        distance = map(ray.origin + ray.direction * distanceFromOrigin);
        distanceFromOrigin += distance;

        if (abs(distance) < MIN_DISTANCE || distanceFromOrigin >= MAX_DISTANCE) {
            break;
        }
    }

    return distanceFromOrigin;
}

void main() {
    vec2 uv = getUV();
    float focalLength = 2.5f;
    Ray ray = Ray(cameraPos, mat3(cameraRight, cameraUp, cameraFront) * normalize(vec3(uv, focalLength)));

    float distance = raymarch(ray);
    vec3 color = vec3(0.0f);

    if (distance < MAX_DISTANCE) {
        vec3 hitPos = ray.origin + ray.direction * distance;
        vec3 normal = calculateNormal(hitPos.xz, 0.01f);
        vec3 viewDir = normalize(cameraPos - hitPos);

        // Fresnel effect
        float fresnel = pow(1.0f - dot(viewDir, normal), 3.0f) * 0.65f;
        fresnel = clamp(fresnel, 0.0f, 0.5f);

        // Reflection (simple sky color for now)
        vec3 reflection = vec3(0.0f); // Sky color
        color = mix(vec3(0.3f, 0.6f, 0.8f), reflection, fresnel);
    }

    fragColor = vec4(color, 1.0f - abs(distance) / MAX_DISTANCE);
}