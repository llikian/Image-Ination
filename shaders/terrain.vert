/***************************************************************************************************
 * @file  terrain.vert
 * @brief Vertex shader for rendering the terrain
 **************************************************************************************************/

#version 460 core

in vec3 aPos;

uniform float terrainSize;

void main() {
    gl_Position = vec4(aPos * terrainSize, 1.0f);
}