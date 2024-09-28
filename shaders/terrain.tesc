/***************************************************************************************************
 * @file  terrain.tesc
 * @brief Tesselation control shader for rendering the terrain
 **************************************************************************************************/

#version 460 core

layout (vertices = 4) out;

uniform vec2 chunk;
uniform vec2 cameraChunk;

uniform float tesselationFactor;
uniform float totalTerrainWidth;

const float TESSELATION_LEVEL = 64.0f;
//const float coefficients[] = { 1.0f, 2.0f, 4.0f, 8.0f, 16.0f, 32.0f, 64.0f };

void main() {
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    float distance = distance(chunk, cameraChunk);
    distance = (distance <= 0.0f) ? 1.0f : distance;

   float level = floor(clamp(TESSELATION_LEVEL * tesselationFactor / distance, 1.0f, 64.0f));
//    float level = coefficients[int(clamp(totalTerrainWidth / (1.75 * distance), 0.0f, 6.0f))];

    gl_TessLevelInner[0] = level;
    gl_TessLevelInner[1] = level;
    gl_TessLevelOuter[0] = level;
    gl_TessLevelOuter[1] = level;
    gl_TessLevelOuter[2] = level;
    gl_TessLevelOuter[3] = level;
}