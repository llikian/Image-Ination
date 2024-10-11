/***************************************************************************************************
 * @file  terrain.tesc
 * @brief Tesselation control shader for rendering the terrain
 **************************************************************************************************/

#version 460 core

layout (vertices = 4) out;

uniform vec2 chunk;
uniform vec2 cameraChunk;
uniform float chunkSize;

float nonZero(in float value) {
    return (value == 0.0f) ? 1.0f : value;
}

void main() {
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    if (gl_InvocationID == 0) {
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