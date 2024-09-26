/***************************************************************************************************
 * @file  water.vert
 * @brief Tesselation evaluation shader for rendering water
 **************************************************************************************************/

#version 460 core

layout(quads, fractional_even_spacing, ccw) in;

out vec3 position;
out vec3 normal;
out float maxHeight;

uniform mat4 vpMatrix;
uniform float deltaNormal;
uniform float time;

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

float perlinNoise2D(in vec2 pos) {
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

float rand3D(in vec3 co) {
    return fract(sin(dot(co, vec3(12.9898f, 78.233f, 144.7272f))) * 43758.5453f);
}

float perlinNoise3D(in vec3 pos) {
    vec3 floorPos = floor(pos);
    vec3 fractPos = fract(pos);
    vec2 delta = vec2(0.0f, 1.0f);

    float g000 = rand3D(floorPos);
    float g001 = rand3D(floorPos + delta.xxy);
    float g010 = rand3D(floorPos + delta.xyx);
    float g011 = rand3D(floorPos + delta.xyy);
    float g100 = rand3D(floorPos + delta.yxx);
    float g101 = rand3D(floorPos + delta.yxy);
    float g110 = rand3D(floorPos + delta.yyx);
    float g111 = rand3D(floorPos + delta.yyy);

    float i1 = smoothLerp(g000, g001, fractPos.z);
    float i2 = smoothLerp(g100, g101, fractPos.z);
    float i3 = smoothLerp(g010, g011, fractPos.z);
    float i4 = smoothLerp(g110, g111, fractPos.z);

    float ii1 = smoothLerp(i1, i2, fractPos.x);
    float ii2 = smoothLerp(i3, i4, fractPos.x);

    return smoothLerp(ii1, ii2, fractPos.y);
}

float noise(in vec2 pos, in float freq, in float amp, in uint oct) {
    float total = 0.0f;

    maxHeight = 0.0f;

    for(uint i = 0u ; i < oct ; ++i) {
    //total += perlinNoise3D(vec3(pos, sin(time)) * freq * time) * amp;
    total += perlinNoise2D(pos* freq + 0.62 * time ) * 0.5 * amp;
    total += perlinNoise2D(pos* freq - 0.29 * time ) * 0.5 * amp;
    total += perlinNoise2D(pos* freq + 0.5 * time ) * 0.2 * amp;
    total /= 3;

        maxHeight += amp;
        freq *= 2.0f;
        amp /= 2.0f;
    }
    maxHeight += 3;
    total += 3;

    return total;
}

float getHeight(in vec2 pos) {
    return noise(pos, 0.1f, 4.0f, 1u);
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