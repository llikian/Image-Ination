/***************************************************************************************************
 * @file  water.vert
 * @brief Vertex shader for rendering the water
 **************************************************************************************************/

#version 420 core

layout(location = 0) in vec3 aPos;

uniform mat4 vpMatrix;
uniform float totalTerrainWidth;
uniform vec2 resolution;

void main() {
    vec3 pos = totalTerrainWidth * aPos;
//    pos.xz *= resolution;

    gl_Position = vpMatrix * vec4(pos, 1.0f);
}