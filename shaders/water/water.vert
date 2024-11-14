/***************************************************************************************************
 * @file  water.vert
 * @brief Vertex shader for rendering the water
 **************************************************************************************************/

#version 420 core

layout(location = 0) in vec3 aPos;

out vec3 position;

uniform vec3 cameraPos;
uniform mat4 vpMatrix;

void main() {
    position = aPos;
    gl_Position = vpMatrix * vec4(aPos + cameraPos, 1.0f);
}