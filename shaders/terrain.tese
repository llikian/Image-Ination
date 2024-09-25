/***************************************************************************************************
 * @file  terrain.vert
 * @brief Tesselation evaluation shader for rendering the terrain
 **************************************************************************************************/

#version 460 core

layout(quads, fractional_even_spacing, ccw) in;

out vec3 position;
out vec3 normal;
out float maxHeight;

out uint biome1;
out uint biome2;
out float blendFactor;

uniform mat4 vpMatrix;

uniform float deltaNormal;

#define IN_POS(i) gl_in[i].gl_Position.xz

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

void getBiomeSettings(in uint biomeID, out float amp, out float freq, out uint oct) {
    switch(biomeID) {
        case 0: // Mountains
            freq = 0.1f;
            amp = 10.0f;
            oct = 8u;
            break;
        case 1: // Plains
            freq = 0.1f;
            amp = 2.0f;
            oct = 8u;
            break;
        case 2: // Desert
            freq = 0.075f;
            amp = 5.0f;
            oct = 4u;
            break;
    }
}

vec3 hash3(vec2 p) {
    vec3 q = vec3(dot(p, vec2(127.1f, 311.7f)),
    dot(p, vec2(269.5f, 183.3f)),
    dot(p, vec2(419.2f, 371.9f)));
    return fract(sin(q) * 43758.5453);
}

float voronoise(in vec2 p, float u, float v) {
    float k = 1.0f + 63.0f * pow(1.0f - v, 6.0f);

    vec2 i = floor(p);
    vec2 f = fract(p);

    vec3 uu1 = vec3(u, u, 1.0f);

    vec2 a = vec2(0.0f);
    for(int y = -2; y <= 2; y++) {
        for(int x = -2; x <= 2; x++) {
            vec2 g = vec2(x, y);
            vec3 o = hash3(i + g) * uu1;
            vec2 d = g - f + o.xy;
            float w = pow(1.0f - smoothstep(0.f, 1.414f, length(d)), k);
            a += vec2(o.z * w, w);
        }
    }

    return a.x / a.y;
}

float noise(in vec2 pos, float amp, float freq, uint oct) {
    float total = 0.0f;

    maxHeight = amp;

    for(uint i = 0u ; i < oct ; ++i) {
        total += perlinNoise(pos * freq) * amp;

        freq *= 2.0f;
        amp /= 2.0f;
        maxHeight += amp;
    }

    return total;
}

float getHeight(in vec2 pos) {
    float vnoise = voronoise(pos / 40.0f, 1.0f, 1.0f);
    vnoise = mod(vnoise * 3.0f, 3.0f);

    blendFactor = fract(vnoise);
    biome1 = uint(vnoise);
    biome2 = (biome1 + 1) % 3;

    float amplitude;
    float frequency;
    uint octave;
    getBiomeSettings(biome1, amplitude, frequency, octave);

    float amp;
    float freq;
    uint oct;
    getBiomeSettings(biome2, amp, freq, oct);

    amplitude = mix(amplitude, amp, blendFactor);
    frequency = mix(frequency, freq, blendFactor);
    octave = uint(mix(float(octave), float(oct), blendFactor));

    return noise(pos, amplitude, frequency, octave);
}

vec3 getPosition(in vec2 uv) {
    vec3 pos;

    pos.xz = mix(mix(IN_POS(0), IN_POS(1), uv.x), mix(IN_POS(3), IN_POS(2), uv.x), uv.y);
    pos.y = getHeight(mix(mix(IN_POS(0), IN_POS(1), uv.x), mix(IN_POS(3), IN_POS(2), uv.x), uv.y));

    return pos;
}

void getNormal() {
    vec2 delta = vec2(deltaNormal, 0.0f);

    vec3 p1 = getPosition(gl_TessCoord.xy + delta.xy);
    vec3 p2 = getPosition(gl_TessCoord.xy + delta.yx);
    position = getPosition(gl_TessCoord.xy);

    normal = normalize(cross(p1 - position, p2 - p1));
}

void main() {
    getNormal();
    gl_Position = vpMatrix * vec4(position, 1.0f);
}