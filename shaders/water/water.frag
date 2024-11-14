/***************************************************************************************************
 * @file  water.frag
 * @brief Fragment shader for rendering the water
 **************************************************************************************************/

#version 420 core

in vec3 position;

out vec4 fragColor;

#define color fragColor.rgb
#define alpha fragColor.a

uniform float time;
uniform vec2 resolution;

void main() {
    vec3 direction = (normalize(position) + 1.0f) * 0.5f;

    color = mix(vec3(0.969f, 0.753f, 0.584f), vec3(0.431f, 0.659f, 0.941f), direction.y * 0.7f - 0.3f);
    alpha = 1.0f;
}