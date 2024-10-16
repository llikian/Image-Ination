/***************************************************************************************************
 * @file  sky.vert
 * @brief Vertex shader for rendering the sky
 **************************************************************************************************/

#version 460 core

layout(location = 0) in vec3 aPos;

out vec3 position;

uniform mat4 vpMatrix;
uniform vec3 cameraPos;

void main() {
    position = aPos;
    gl_Position = vpMatrix * vec4(aPos + cameraPos, 1.0f);
}