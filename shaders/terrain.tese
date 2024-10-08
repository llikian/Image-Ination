/***************************************************************************************************
 * @file  terrain.vert
 * @brief Tesselation evaluation shader for rendering the terrain
 **************************************************************************************************/

#version 460 core

layout (quads, fractional_even_spacing, ccw) in;

out vec3 position;
out vec3 normal;
out float maxHeight;

uniform mat4 vpMatrix;
uniform float chunkSize;

uniform float ampFnoise, ampAnoise, freqFnoise, freqAnoise;
uniform uint octFnoise, octAnoise;
uniform int seedFnoise, seedAnoise;

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
        total += perlinNoise(pos * freq, seed) * amp;

        maxHeight += amp;
        freq *= 2.0f;
        amp /= 2.0f;
    }

    return total;
}

float getHeight(in vec2 pos) {
    maxHeight = 0.0f;

    float freqNoise = noise(pos, freqFnoise, ampFnoise, octFnoise, seedFnoise);
    maxHeight = 0.0f;
    float ampNoise = noise(pos, freqAnoise, ampAnoise, octAnoise, seedAnoise);

    return noise(pos, freqNoise, ampNoise, 8u, 0);
}

vec3 getPosition(in vec2 uv) {
    vec3 pos;

    #define IN_POS(i) gl_in[i].gl_Position.xz

    pos.xz = mix(mix(IN_POS(0), IN_POS(1), uv.x), mix(IN_POS(3), IN_POS(2), uv.x), uv.y);
    pos.y = getHeight(pos.xz);

    return pos;
}

vec3 getNormal() {
    vec2 delta = vec2(0.5f * chunkSize / gl_TessLevelInner[0], 0.0f);

    vec3 p1 = getPosition(gl_TessCoord.xy + delta.xy);
    vec3 p2 = getPosition(gl_TessCoord.xy + delta.yx);
    position = getPosition(gl_TessCoord.xy);

    return normalize(cross(p1 - position, p2 - position));
}

void main() {
    normal = getNormal();

    gl_Position = vpMatrix * vec4(position, 1.0f);
}