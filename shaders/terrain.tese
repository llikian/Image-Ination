/***************************************************************************************************
 * @file  terrain.vert
 * @brief Tesselation evaluation shader for rendering the terrain
 **************************************************************************************************/

#version 460 core

layout(quads, fractional_even_spacing, ccw) in;

out vec3 position;
out vec3 normal;
out float maxHeight;

uniform mat4 vpMatrix;
uniform float deltaNormal;

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

float noise(in vec2 pos, in float freq, in float amp, in uint oct) {
    float total = 0.0f;

    maxHeight = 0.0f;

    for(uint i = 0u ; i < oct ; ++i) {
        total += perlinNoise(pos * freq) * amp;

        maxHeight += amp;
        freq *= 2.0f;
        amp /= 2.0f;
    }

    return total;
}

float getHeight(in vec2 pos) {
    return noise(pos, 0.1f, 8.0f, 8u);
}

vec3 getPosition(in vec2 uv) {
    vec3 pos;

    #define IN_POS(i) gl_in[i].gl_Position.xz

    pos.xz = mix(mix(IN_POS(0), IN_POS(1), uv.x), mix(IN_POS(3), IN_POS(2), uv.x), uv.y);
    pos.y = getHeight(pos.xz);

    return pos;
}

vec3 getNormal() {
    vec2 delta = vec2(deltaNormal, 0.0f);

    vec3 p1 = getPosition(gl_TessCoord.xy + delta.xy);
    vec3 p2 = getPosition(gl_TessCoord.xy - delta.xy);
    vec3 p3 = getPosition(gl_TessCoord.xy + delta.yx);
    vec3 p4 = getPosition(gl_TessCoord.xy - delta.yx);

    return normalize(cross(p1 - p2, p3 - p4));
}

void main() {
    position = getPosition(gl_TessCoord.xy);
    normal = getNormal();

    gl_Position = vpMatrix * vec4(position, 1.0f);
}