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
uniform float frequency;
uniform float amplitude;
uniform int octave;
uniform float terrainSize;

float simple_interpolate(in float a, in float b, in float x) {
    return a + smoothstep(0.0f, 1.0f, x) * (b - a);
}

float rand2D(in vec2 co){
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

float noise(in vec2 pos) {
    float total = 0.0f;
    float amp = amplitude;
    pos *= frequency;

    maxHeight = amp;

    for(int i = 0 ; i < octave ; ++i) {
        total += perlinNoise(pos) * amp;

        pos *= 2.0f;
        amp /= 2.0f;
        maxHeight += amp;
    }

    return total;
}

float getHeight(in vec2 pos) {
    return noise(pos);
}

vec3 getPosition(in vec2 uv) {
    vec3 pos;

    vec3 p0 = gl_in[0].gl_Position.xyz;
    vec3 p1 = gl_in[1].gl_Position.xyz;
    vec3 p2 = gl_in[2].gl_Position.xyz;
    vec3 p3 = gl_in[3].gl_Position.xyz;

    pos = mix(mix(p0, p1, uv.x), mix(p3, p2, uv.x), uv.y);

    pos.y = getHeight(pos.xz);
    return pos;
}

vec3 getNormal(in vec3 position) {
    vec2 delta = vec2(deltaNormal, 0.0);

    vec3 p1 = getPosition(gl_TessCoord.xy + delta.xy);
    vec3 p2 = getPosition(gl_TessCoord.xy + delta.yx);

    return normalize(cross(p1 - position, p2 - p1));
}

void main() {
    position = getPosition(gl_TessCoord.xy);
    normal = getNormal(position);

    gl_Position = vpMatrix * vec4(position, 1.0f);
}