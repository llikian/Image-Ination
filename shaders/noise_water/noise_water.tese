/***************************************************************************************************
 * @file  noise_water.tese
 * @brief Tesselation evaluation shader for rendering water with noise
 **************************************************************************************************/

#version 420 core

layout (quads) in;

out vec3 position;
out vec3 normal;
out vec2 texCoords;

out float minHeight;
out float maxHeight;

uniform mat4 vpMatrix;
uniform float chunkSize;

uniform float time;

struct Noise {
    vec2 position;
    float frequency;
    float amplitude;
};

float fade(in float x) {
    float x3 = x * x * x;
    return 6.0f * x3 * x * x - 15.0f * x3 * x + 10.0f * x3;
}

float smoothLerp(in float a, in float b, in float t) {
    return a + fade(t) * (b - a);
}

float rand2D(in vec2 co) {
    return fract(sin(dot(co, vec2(12.9898f, 78.233f))) * 43758.5453f);
}

float perlinNoise(in vec2 pos) {
    vec2 floorPos = floor(pos);
    vec2 fractPos = fract(pos);

    float g00 = rand2D(vec2(floorPos.x, floorPos.y));
    float g01 = rand2D(vec2(floorPos.x, floorPos.y + 1.0f));
    float g10 = rand2D(vec2(floorPos.x + 1.0f, floorPos.y));
    float g11 = rand2D(vec2(floorPos.x + 1.0f, floorPos.y + 1.0f));

    float nx = smoothLerp(g00, g10, fractPos.x);
    float ny = smoothLerp(g01, g11, fractPos.x);

    return smoothLerp(nx, ny, fractPos.y);
}

float getHeight(in vec2 pos) {
    float freq = 0.05f;
    float amp = 5.0f;
    uint octaves = 4u;

    float total = 10.0f;
    minHeight = 10.0f;
    maxHeight = 10.0f;

    for (uint i = 0u; i < octaves; ++i) {
        total += perlinNoise(pos * freq + 0.62f * time) * 0.75f * amp;
        total += perlinNoise(pos * freq - 0.12f * time) * 0.60f * amp;

        total += perlinNoise(vec2(pos.x * freq + 0.39f * time, pos.y * freq - 0.59f * time)) * 0.125f * amp;
        total += perlinNoise(vec2(pos.x * freq - 0.69f * time, pos.y * freq + 0.29f * time)) * 0.250f * amp;

        maxHeight += 0.75f * amp;
        maxHeight += 0.60f * amp;
        maxHeight += 0.125f * amp;
        maxHeight += 0.250f * amp;

        freq *= 2.0f;
        amp /= 2.0f;
    }

    return total;
}

vec3 getPosition(in vec2 uv) {
    vec3 pos;

    pos.xz = mix(mix(gl_in[0].gl_Position.xz, gl_in[1].gl_Position.xz, uv.x),
                 mix(gl_in[3].gl_Position.xz, gl_in[2].gl_Position.xz, uv.x),
                 uv.y);

    pos.y = getHeight(pos.xz);

    return pos;
}

void main() {
    vec2 delta = vec2(0.25f * chunkSize / gl_TessLevelInner[0], 0.0f);

    vec3 p1 = getPosition(gl_TessCoord.xy + delta.xy);
    vec3 p2 = getPosition(gl_TessCoord.xy + delta.yx);

    position = getPosition(gl_TessCoord.xy);
    normal = normalize(cross(p1 - position, p2 - position));

    gl_Position = vpMatrix * vec4(position, 1.0f);
}