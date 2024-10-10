/***************************************************************************************************
 * @file  sky.frag
 * @brief Fragment shader for rendering sky
 **************************************************************************************************/

#version 460 core

in vec2 texCoords;
out vec4 fragColor;

void main() {

    fragColor = vec4(0.1f, 0.2f, 0.8f, 1.0f);
}