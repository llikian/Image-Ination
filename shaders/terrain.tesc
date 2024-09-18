/***************************************************************************************************
 * @file  terrain.tesc
 * @brief Tesselation control shader for rendering the terrain
 **************************************************************************************************/

#version 460 core

layout(vertices = 4) out;  // 4 control points for the

const float tessLevelInner = 2.0;
const float tessLevelOuter = 2.0;

void main() {
    // Set tessellation levels (outer and inner)
    gl_TessLevelInner[0] = tessLevelInner;  // Tessellation inside the patch
    gl_TessLevelInner[1] = tessLevelInner;
    gl_TessLevelOuter[0] = tessLevelOuter;  // Level of tessellation on the edges
    gl_TessLevelOuter[1] = tessLevelOuter;
    gl_TessLevelOuter[2] = tessLevelOuter;
    gl_TessLevelOuter[3] = tessLevelOuter;

    // Pass control points to the tessellation evaluation shader
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}