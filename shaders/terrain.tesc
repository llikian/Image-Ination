/***************************************************************************************************
 * @file  terrain.tesc
 * @brief Tesselation control shader for rendering the terrain
 **************************************************************************************************/

#version 460 core

#define id gl_InvocationID

layout (vertices = 4) out;

uniform vec2 cameraChunk;
uniform float chunkSize;

float nonZero(in float value) {
    return (value == 0.0f) ? 1.0f : value;
}

void main() {
    gl_out[id].gl_Position = gl_in[id].gl_Position;

    if (gl_InvocationID % 4 == 0) {
        vec2 chunk = gl_in[id].gl_Position.xz + gl_in[id + 1].gl_Position.xz;
        chunk += gl_in[id + 2].gl_Position.xz + gl_in[id + 3].gl_Position.xz;
        chunk /= 4.0f;

        chunk.x = floor(chunk.x / chunkSize + 0.5f);
        chunk.y = floor(chunk.y / chunkSize + 0.5f);

        float level = (chunkSize * chunkSize * 0.5f) / nonZero(distance(chunk, cameraChunk));
        level = clamp(level, 1.0f, 64.0f);

        gl_TessLevelInner[0] = level;
        gl_TessLevelInner[1] = level;
        gl_TessLevelOuter[0] = level;
        gl_TessLevelOuter[1] = level;
        gl_TessLevelOuter[2] = level;
        gl_TessLevelOuter[3] = level;
    }
}