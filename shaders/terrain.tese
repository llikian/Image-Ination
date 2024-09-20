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
    return a + smoothstep(0.0,1.0,x) * (b-a);
}

float rand2D(in vec2 co) {
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float rand3D(in vec3 co) {
    return fract(sin(dot(co.xyz ,vec3(12.9898,78.233,144.7272))) * 43758.5453);
}

float interpolatedNoise3D(in vec3 pos) {
    float integer_x = pos.x - fract(pos.x);
    float fractional_x = pos.x - integer_x;

    float integer_y = pos.y - fract(pos.y);
    float fractional_y = pos.y - integer_y;

    float integer_z = pos.z - fract(pos.z);
    float fractional_z = pos.z - integer_z;

    float v1 = rand3D(vec3(integer_x, integer_y, integer_z));
    float v2 = rand3D(vec3(integer_x + 1.0f, integer_y, integer_z));
    float v3 = rand3D(vec3(integer_x, integer_y + 1.0f, integer_z));
    float v4 = rand3D(vec3(integer_x + 1.0f, integer_y + 1.0f, integer_z));

    float v5 = rand3D(vec3(integer_x, integer_y, integer_z + 1.0f));
    float v6 = rand3D(vec3(integer_x + 1.0f, integer_y, integer_z + 1.0f));
    float v7 = rand3D(vec3(integer_x, integer_y + 1.0f, integer_z + 1.0f));
    float v8 = rand3D(vec3(integer_x + 1.0f, integer_y + 1.0f, integer_z + 1.0f));

    float i1 = simple_interpolate(v1, v5, fractional_z);
    float i2 = simple_interpolate(v2, v6, fractional_z);
    float i3 = simple_interpolate(v3, v7, fractional_z);
    float i4 = simple_interpolate(v4, v8, fractional_z);

    float ii1 = simple_interpolate(i1, i2, fractional_x);
    float ii2 = simple_interpolate(i3, i4, fractional_x);

    return simple_interpolate(ii1, ii2, fractional_y);
}

float noise(in vec2 pos) {
    const vec3 POS = vec3(pos.x, 0.0f, pos.y);
    float total = 0.0f;
    float amp = amplitude;
    float freq = frequency;

    maxHeight = 0.0f;

    for(int i = 0 ; i < octave ; ++i) {
        maxHeight += amp;

        total += interpolatedNoise3D(POS * freq) * amp;
        freq *= 2.0f;
        amp /= 2.0f;

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

void main() {
    position = getPosition(gl_TessCoord.xy);

    vec2 delta = vec2(deltaNormal, 0.0);
    vec3 p1 = getPosition(gl_TessCoord.xy + delta.xy);
    vec3 p2 = getPosition(gl_TessCoord.xy + delta.yx);
    normal = normalize(cross(p1 - position, p2 - p1));

    gl_Position = vpMatrix * vec4(position, 1.0f);
}