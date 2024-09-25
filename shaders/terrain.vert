/***************************************************************************************************
 * @file  terrain.vert
 * @brief Vertex shader for rendering the terrain
 **************************************************************************************************/

#version 460 core

in vec3 aPos;

uniform float chunkSize;
uniform int chunkX;
uniform int chunkZ;

void main() {
    vec3 pos = vec3(aPos.x + float(chunkX), 0.0f, aPos.z + float(chunkZ)) * chunkSize;
    gl_Position = vec4(pos, 1.0f);
}