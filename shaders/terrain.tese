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

float simple_interpolate(in float a, in float b, in float x) {
    return a + smoothstep(0.0f, 1.0f, x) * (b - a);
}

float rand2D(in vec2 co) {
    return fract(sin(dot(co, vec2(12.9898f, 78.233f))) * 43758.5453f);
}

float perlinNoise(in vec2 pos) {
    float integer_x = floor(pos.x);
    float fractional_x = fract(pos.x);

    float integer_z = floor(pos.y);
    float fractional_z = fract(pos.y);

    float v1 = rand2D(vec2(integer_x, integer_z));
    float v2 = rand2D(vec2(integer_x + 1.0f, integer_z));
    float v3 = rand2D(vec2(integer_x, integer_z + 1.0f));
    float v4 = rand2D(vec2(integer_x + 1.0f, integer_z + 1.0f));

    float i1 = simple_interpolate(v1, v3, fractional_z);
    float i2 = simple_interpolate(v2, v4, fractional_z);

    return simple_interpolate(i1, i2, fractional_x);
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