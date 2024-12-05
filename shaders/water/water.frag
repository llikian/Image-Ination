/***************************************************************************************************
 * @file  water.frag
 * @brief Fragment shader for rendering the water
 **************************************************************************************************/

#version 420 core

out vec4 fragColor;

#define alpha fragColor.a

uniform vec3 cameraPos;
uniform float time;
uniform vec3 cameraFront;
uniform vec3 cameraRight;
uniform vec3 cameraUp;
uniform vec2 resolution;

const uint MAX_STEPS = 128;
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
    float waveShift = length(position) * 0.1;
    float iter = 0.0f;
    float freq = 1.0f;
    float timeMult = 2.0f;
    float weight = 1.0f;
    float sumValues = 0.0f;
    float sumWeights = 0.0f;

    for (int i = 0; i < 12; i++) {
        vec2 wavedir = vec2(sin(iter), cos(iter));
        vec2 wave = wavedx(position, wavedir, freq, time * timeMult + waveShift);

        position += wavedir * wave.y * weight * DRAG;

        sumValues += wave.x * weight;
        sumWeights += weight;

        weight = mix(weight, 0.0f, 0.2f);
        freq *= 1.018f;
        timeMult *= 1.07f;
        iter += 1282.26113f;
    }

    return sumValues / sumWeights;
}

float map(vec3 position) {
    float waveHeight = getWaves(position.xz) * WATER_DEPTH;
    float toWater = position.y - waveHeight;

    return toWater;
}

vec3 calculateNormal(vec2 position, float epsilon) {
    float height = getWaves(position) * WATER_DEPTH;
    vec3 p = vec3(position.x, height, position.y);

    vec3 dx = p - vec3(position.x - epsilon, getWaves(position - vec2(epsilon, 0.0f)) * WATER_DEPTH, position.y);
    vec3 dy = p - vec3(position.x, getWaves(position + vec2(0.0f, epsilon)) * WATER_DEPTH, position.y + epsilon);

    return normalize(cross(dx, dy));
}

float raymarch(in Ray ray) {
    float dist;
    float distanceFromOrigin = 0.0f;

    for (uint i = 0u; i < MAX_STEPS; ++i) {
        dist = map(ray.origin + ray.direction * distanceFromOrigin);
        distanceFromOrigin += dist;

        if (abs(dist) < MIN_DISTANCE || distanceFromOrigin >= MAX_DISTANCE) {
            break;
        }
    }

    return distanceFromOrigin;
}

float raymarchwater(in Ray ray){
    vec3 position = ray.origin;
    vec3 direction = normalize(ray.direction);

    for (uint i = 0u; i < MAX_STEPS; i++) {
        float waveHeight = getWaves(position.xz);
        if(position.y <= waveHeight + MIN_DISTANCE){
            return length(position-ray.origin);
        }

        position += direction * (position.y - waveHeight);

        if (length(position - ray.origin) >= MAX_DISTANCE) {
            break;
        }
    }
}

void main() {
    vec2 uv = getUV();
    float focalLength = 2.5f;
    Ray ray = Ray(cameraPos, mat3(cameraRight, cameraUp, cameraFront) * normalize(vec3(uv, focalLength)));

    if(ray.direction.y >= -0.056){
        fragColor = vec4(0.0f);
        fragColor.a = clamp(ray.direction.y * 0.8f, 0.0f, 1.0f);
        return;
    }


    float distance = raymarchwater(ray);
    vec3 color = vec3(0.3f, 0.6f, 0.8f);

    if (distance < MAX_DISTANCE) {
        vec3 hitPos = ray.origin + ray.direction * distance;
        vec3 normal = calculateNormal(hitPos.xz, 0.01f);
        vec3 viewDir = normalize(cameraPos - hitPos);

        // Fresnel effect
        float fresnel = pow(1.0f - dot(viewDir, normal), 3.0f) * 0.65f;
        fresnel = clamp(fresnel, 0.0f, 0.5f);

        // Reflection (simple sky color for now)
        vec3 reflection = vec3(0.0f); // Sky color
        color = mix(color, 0.5*reflection, fresnel);
    }

    fragColor = vec4(color, 0.8 + 0.2 * (1.0f - abs(distance) / MAX_DISTANCE));
}