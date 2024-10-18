/***************************************************************************************************
 * @file  clouds.frag
 * @brief Fragment shader for rendering the clouds
 **************************************************************************************************/

#version 460 core

out vec4 fragColor;

#define color fragColor.rgb
#define alpha fragColor.a

uniform vec2 resolution;

void main() {
    vec2 uv = (2.0f * gl_FragCoord.xy - resolution) / resolution.y;

    color = vec3(uv.x * 0.5f + 0.5f, 0.0f, uv.y * 0.5f + 0.5f);
    alpha = 1.0f;
}