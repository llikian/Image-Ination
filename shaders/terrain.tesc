/***************************************************************************************************
 * @file  terrain.tesc
 * @brief Tesselation control shader for rendering the terrain
 **************************************************************************************************/

#version 460 core

layout(vertices = 4) out;

uniform float tesselationLevel;

uniform vec3 cameraPos;
uniform float terrainSize;
uniform int chunkX;
uniform int chunkZ;

void main() {
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    vec2 currentChunk = floor(cameraPos.xz / terrainSize);
    float dist = floor(distance(currentChunk, vec2(float(chunkX), float(chunkZ))));

    float level = clamp(8.0f * tesselationLevel / dist, 1.0f, 64.0f);

    gl_TessLevelInner[0] = level;
    gl_TessLevelInner[1] = level;
    gl_TessLevelOuter[0] = level;
    gl_TessLevelOuter[1] = level;
    gl_TessLevelOuter[2] = level;
    gl_TessLevelOuter[3] = level;
}