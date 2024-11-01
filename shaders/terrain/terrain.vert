/***************************************************************************************************
 * @file  terrain.vert
 * @brief Vertex shader for rendering the terrain
 **************************************************************************************************/

#version 420 core

layout(location = 0) in vec3 aPos;

uniform vec2 cameraChunk;
uniform float chunkSize;

void main() {
    gl_Position.xzyw = vec4(aPos.xz + chunkSize * cameraChunk, 0.0f, 1.0f);
}