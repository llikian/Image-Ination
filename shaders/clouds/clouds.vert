/***************************************************************************************************
 * @file  clouds.vert
 * @brief Vertex shader for rendering the clouds
 **************************************************************************************************/

#version 420 core

out vec3 cameraPos;

uniform vec3 cameraPosition;

layout(location = 0) in vec3 aPos;

void main() {
    cameraPos = cameraPosition * 100.0f;
    gl_Position = vec4(aPos, 1.0f);
}