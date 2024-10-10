/***************************************************************************************************
 * @file  sky.frag
 * @brief Fragment shader for rendering sky
 **************************************************************************************************/

#version 460 core

in vec3 position;
out vec4 fragColor;

uniform vec2 resolution;

#define color fragColor.rgb
#define alpha fragColor.a

void main() {
    color = vec3(0.0f, 0.0f, normalize(position).y * 0.7f + 0.3f);
    alpha = 1.0f;
}