/***************************************************************************************************
 * @file  water.vert
 * @brief Vertex shader for rendering the water
 **************************************************************************************************/

#version 420 core

layout(location = 0) in vec3 aPos;

void main() {
    gl_Position = vec4(aPos, 1.0f);
}