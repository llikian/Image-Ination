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

out uint biome1;
out uint biome2;

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

vec2 random2(vec2 p) {
    return fract(sin(vec2(dot(p, vec2(127.1, 311.7)),
    dot(p, vec2(269.5, 183.3)))) * 43758.5453);
}

void findTwoMinIndices(float values[4], out int min1_idx, out int min2_idx) {
    // Step 1: Initialize indices for the first pair
    int ab_min_idx = (values[0] < values[1]) ? 0 : 1;
    int ab_max_idx = (values[0] < values[1]) ? 1 : 0;

    // Step 2: Initialize indices for the second pair
    int cd_min_idx = (values[2] < values[3]) ? 2 : 3;
    int cd_max_idx = (values[2] < values[3]) ? 3 : 2;

    // Step 3: Find the overall minimum index
    min1_idx = (values[ab_min_idx] < values[cd_min_idx]) ? ab_min_idx : cd_min_idx;

    // Step 4: Find the second smallest index
    // Choose the smallest of the remaining values, but make sure it's not the same as min1
    int second_min_ab = (min1_idx == ab_min_idx) ? ab_max_idx : ab_min_idx;
    int second_min_cd = (min1_idx == cd_min_idx) ? cd_max_idx : cd_min_idx;
    min2_idx = (values[second_min_ab] < values[second_min_cd]) ? second_min_ab : second_min_cd;
}

vec2 voronoiNoise2D(in float x, in float y, in float xrand, in float yrand) {
    float integer_x = x - fract(x);
    float fractional_x = x - integer_x;

    float integer_y = y - fract(y);
    float fractional_y = y - integer_y;

    float val[4];

    val[0] = rand2D(vec2(integer_x, integer_y));
    val[1] = rand2D(vec2(integer_x+1.0, integer_y));
    val[2] = rand2D(vec2(integer_x, integer_y+1.0));
    val[3] = rand2D(vec2(integer_x+1.0, integer_y+1.0));

    float xshift[4];

    xshift[0] = xrand * (rand2D(vec2(integer_x+0.5, integer_y)) - 0.5);
    xshift[1] = xrand * (rand2D(vec2(integer_x+1.5, integer_y)) -0.5);
    xshift[2] = xrand * (rand2D(vec2(integer_x+0.5, integer_y+1.0))-0.5);
    xshift[3] = xrand * (rand2D(vec2(integer_x+1.5, integer_y+1.0))-0.5);

    float yshift[4];

    yshift[0] = yrand * (rand2D(vec2(integer_x, integer_y +0.5)) - 0.5);
    yshift[1] = yrand * (rand2D(vec2(integer_x+1.0, integer_y+0.5)) -0.5);
    yshift[2] = yrand * (rand2D(vec2(integer_x, integer_y+1.5))-0.5);
    yshift[3] = yrand * (rand2D(vec2(integer_x+1.5, integer_y+1.5))-0.5);

    float dist[4];

    dist[0] = sqrt((fractional_x + xshift[0]) * (fractional_x + xshift[0]) + (fractional_y + yshift[0]) * (fractional_y + yshift[0]));
    dist[1] = sqrt((1.0 -fractional_x + xshift[1]) * (1.0-fractional_x+xshift[1]) + (fractional_y +yshift[1]) * (fractional_y+yshift[1]));
    dist[2] = sqrt((fractional_x + xshift[2]) * (fractional_x + xshift[2]) + (1.0-fractional_y +yshift[2]) * (1.0-fractional_y + yshift[2]));
    dist[3] = sqrt((1.0-fractional_x + xshift[3]) * (1.0-fractional_x + xshift[3]) + (1.0-fractional_y +yshift[3]) * (1.0-fractional_y + yshift[3]));

    int i1, i2;
    findTwoMinIndices(dist, i1, i2);

    return vec2(val[i1], val[i2]);

}

vec2 VoronoiNoise2D(in vec2 coord, in float wavelength, in float xrand, in float yrand) {
    return voronoiNoise2D(coord.x/wavelength, coord.y/wavelength, xrand, yrand);
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

float blendFactor(float dist1, float dist2) {
    return smoothstep(0.0, 1.0, dist1 / (dist1 + dist2));
}

float getHeight(in vec2 pos) {
    vec2 vNoise = VoronoiNoise2D(pos, 40.0f, 1.0f, 1.0f);

    float t = blendFactor(vNoise.x, vNoise.y);
    biome1 = uint(mod(vNoise.x * 3.0f, 3.0f));
    biome2 = uint(mod(vNoise.y * 3.0f, 3.0f));

    float amp1, amp2;
    float freq1, freq2;
    uint oct1, oct2;
    getBiomeSettings(biome1, amp1, freq1, oct1);
    getBiomeSettings(biome2, amp2, freq2, oct2);

    float amplitude = mix(amp1, amp2, t);
    float frequency = mix(freq1, freq2, t);
    uint octave = uint(mix(float(oct1), float(oct2), t));

    return noise(pos, amplitude, frequency, octave);
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