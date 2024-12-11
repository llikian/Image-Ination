/***************************************************************************************************
 * @file  water.vert
 * @brief Vertex shader for rendering the water
 **************************************************************************************************/

#version 420 core

layout(location = 0) in vec3 aPos;

uniform mat4 vpMatrix;
uniform float totalTerrainWidth;

void main() {
    gl_Position = vpMatrix * vec4(totalTerrainWidth * aPos, 1.0f);
}