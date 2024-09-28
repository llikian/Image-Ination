/***************************************************************************************************
 * @file  terrain.vert
 * @brief Vertex shader for rendering the terrain
 **************************************************************************************************/

#version 460 core

layout(location = 0) in vec3 aPos;

uniform float chunkSize;
uniform vec2 chunk;

void main() {
    gl_Position.xzyw = vec4(chunkSize * (aPos.xz + chunk), 0.0f, 1.0f);
}