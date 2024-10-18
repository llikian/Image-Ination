/***************************************************************************************************
 * @file  terrain.vert
 * @brief Tesselation evaluation shader for rendering the terrain
 **************************************************************************************************/

#version 460 core

layout (quads) in;

out vec3 position;
out vec3 normal;
out float minHeight;
out float maxHeight;

uniform mat4 vpMatrix;
uniform float chunkSize;

uniform int terrainSeed;
uniform float ampAnoise;
uniform float freqAnoise;
uniform uint octAnoise;
uniform int seedAnoise;

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

float perlinNoise(in vec2 pos, in int seed) {
    vec2 floorPos = floor(pos) + vec2(float(seed));
    vec2 fractPos = fract(pos);

    float g00 = rand2D(vec2(floorPos.x, floorPos.y));
    float g01 = rand2D(vec2(floorPos.x, floorPos.y + 1.0f));
    float g10 = rand2D(vec2(floorPos.x + 1.0f, floorPos.y));
    float g11 = rand2D(vec2(floorPos.x + 1.0f, floorPos.y + 1.0f));

    float nx = smoothLerp(g00, g10, fractPos.x);
    float ny = smoothLerp(g01, g11, fractPos.x);

    return smoothLerp(nx, ny, fractPos.y);
}

float noise(in vec2 pos, in float freq, in float amp, in uint oct, in int seed) {
    float total = 0.0f;

    for (uint i = 0u; i < oct; ++i) {
        total += (perlinNoise(pos * freq, seed) * 2.0f - 1.0f) * amp * 0.5f;

        maxHeight += amp * 0.5f;
        freq *= 2.0f;
        amp /= 2.0f;
    }

    return total;
}

float getHeight(in vec2 pos) {
    minHeight = 0.0f;
    maxHeight = 0.0f;

//    float ampNoise = noise(pos, freqAnoise, ampAnoise, octAnoise, seedAnoise);
//    maxHeight = 0.0f;

//    float result = noise(pos, 0.001f, ampAnoise, 8u, terrainSeed);

    float n1 = noise(pos, 0.01f, 25.0f, 8u, 0);
    float n2 = noise(pos, 0.005f, 200.0f, 8u, 0) + 30.0f;
    float result = max(n1, n2);

    minHeight = -25.0f;
    maxHeight = 230.0f;

    return result;
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