/***************************************************************************************************
 * @file  terrain.tesc
 * @brief Tesselation control shader for rendering the terrain
 **************************************************************************************************/

#version 460 core

layout (vertices = 4) out;

uniform vec2 cameraChunk;
uniform float chunkSize;

// TODO: Find a way to create faces that avoid gaps when two different tesselation levels are next to each other

vec2 getChunk(int id) {
    vec2 chunk = gl_in[id].gl_Position.xz + gl_in[id + 1].gl_Position.xz;
    chunk += gl_in[id + 2].gl_Position.xz + gl_in[id + 3].gl_Position.xz;
    chunk /= 4.0f;

    chunk.x = floor(0.5f + chunk.x / chunkSize);
    chunk.y = floor(0.5f + chunk.y / chunkSize);

    return chunk;
}

float nonZero(in float value) {
    return (value == 0.0f) ? 1.0f : value;
}

void main() {
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    if (gl_InvocationID % 4 == 0) {
        float dist = distance(getChunk(gl_InvocationID), cameraChunk);
        float level = clamp((chunkSize * chunkSize * 0.5f) / nonZero(dist), 1.0f, 64.0f);

        gl_TessLevelInner[0] = level;
        gl_TessLevelInner[1] = level;

        gl_TessLevelOuter[0] = level;
        gl_TessLevelOuter[1] = level;
        gl_TessLevelOuter[2] = level;
        gl_TessLevelOuter[3] = level;
    }
}