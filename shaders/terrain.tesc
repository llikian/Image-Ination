/***************************************************************************************************
 * @file  terrain.tesc
 * @brief Tesselation control shader for rendering the terrain
 **************************************************************************************************/

#version 460 core

layout(vertices = 4) out;

uniform vec3 cameraPos;
uniform float chunkSize;
uniform int chunkX;
uniform int chunkZ;

const float TESSELATION_LEVEL = 64.0f;

void main() {
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    vec2 currentChunk = floor(cameraPos.xz / chunkSize);
    float distance = floor(distance(currentChunk, vec2(float(chunkX), float(chunkZ))));

    float level = clamp(16.0f * TESSELATION_LEVEL / distance, 1.0f, 64.0f);

    gl_TessLevelInner[0] = level;
    gl_TessLevelInner[1] = level;
    gl_TessLevelOuter[0] = level;
    gl_TessLevelOuter[1] = level;
    gl_TessLevelOuter[2] = level;
    gl_TessLevelOuter[3] = level;
}