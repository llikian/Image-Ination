/***************************************************************************************************
 * @file  terrain.vert
 * @brief Tesselation evaluation shader for rendering the terrain
 **************************************************************************************************/

#version 460 core

layout(quads, equal_spacing, ccw) in;  // Ensure counter-clockwise order

uniform mat4 vpMatrix;

void main() {
    vec3 p0 = gl_in[0].gl_Position.xyz;
    vec3 p1 = gl_in[1].gl_Position.xyz;
    vec3 p2 = gl_in[2].gl_Position.xyz;
    vec3 p3 = gl_in[3].gl_Position.xyz;

    vec3 position = mix(mix(p0, p1, gl_TessCoord.x), mix(p3, p2, gl_TessCoord.x), gl_TessCoord.y);
    gl_Position = vpMatrix * vec4(position, 1.0);
}