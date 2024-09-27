/***************************************************************************************************
 * @file  terrain.vert
 * @brief Vertex shader for rendering the terrain
 **************************************************************************************************/

#version 460 core

in vec3 aPos;

uniform float chunkSize;
uniform ivec2 chunk;
uniform ivec2 cameraChunk;

void main() {
    gl_Position.x = chunkSize * (aPos.x + float(chunk.x + cameraChunk.x));
    gl_Position.y = 0.0f;
    gl_Position.z = chunkSize * (aPos.z + float(chunk.y + cameraChunk.y));
    gl_Position.w = 1.0f;
}