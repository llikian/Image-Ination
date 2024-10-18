/***************************************************************************************************
 * @file  terrain.vert
 * @brief Vertex shader for rendering the terrain
 **************************************************************************************************/

#version 460 core

layout(location = 0) in vec3 aPos;

void main() {
    gl_Position = vec4(aPos.x, 0.0f, aPos.z, 1.0f);
}