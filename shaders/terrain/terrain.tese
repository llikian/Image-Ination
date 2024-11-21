/***************************************************************************************************
 * @file  terrain.vert
 * @brief Tesselation evaluation shader for rendering the terrain
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

struct Noise {
    float frequency;
    float amplitude;
    float height;
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

float getNoise(in vec2 pos, in Noise noise) {
    return (perlinNoise(pos * noise.frequency) * 2.0f - 1.0f) * noise.amplitude * 0.5f;
}

float getHeight(in vec2 pos) {
    Noise plains = Noise(0.01f, 25.0f, -37.0f);
    Noise plateaux = Noise(0.003f, 130.0, 0.0f);
    Noise mountains = Noise(0.004f, 250.0f, 25.0f);

    float heightPlain = plains.height;
    float heightPlateau = plateaux.height;
    float heightMountain = mountains.height;

    minHeight = plains.height;
    maxHeight = mountains.height;

    for(uint i = 0 ; i < 8u ; ++i) {
        heightPlain += getNoise(pos, plains);
        plains.frequency *= 2.0f;
        plains.amplitude /= 2.0f;
        minHeight -= plains.amplitude;

        heightPlateau += getNoise(pos, plateaux);
        plateaux.frequency *= 2.0f;
        plateaux.amplitude /= 2.0f;

        heightMountain += getNoise(pos, mountains);
        mountains.frequency *= 2.0f;
        mountains.amplitude /= 2.0f;
        maxHeight += mountains.amplitude;
    }

    return max(max(heightPlain, heightPlateau), heightMountain);
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
    texCoords = gl_TessCoord.xy;

    gl_Position = vpMatrix * vec4(position, 1.0f);
}