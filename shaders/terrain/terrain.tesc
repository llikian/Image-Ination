/***************************************************************************************************
 * @file  terrain.tesc
 * @brief Tesselation control shader for rendering the terrain
 **************************************************************************************************/

#version 420 core

layout (vertices = 4) out;

uniform vec3 cameraPos;
uniform float totalTerrainWidth;

const int MAX_TESS_LEVEL = 48;

float getLevel(int id) {
    return clamp(distance(gl_in[id].gl_Position.xz, cameraPos.xz) / totalTerrainWidth, 0.0f, 1.0f);
}

void main() {
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    if (gl_InvocationID % 4 == 0) {
        float dist0 = getLevel(gl_InvocationID + 0);
        float dist1 = getLevel(gl_InvocationID + 1);
        float dist2 = getLevel(gl_InvocationID + 2);
        float dist3 = getLevel(gl_InvocationID + 3);

        gl_TessLevelOuter[0] = mix(MAX_TESS_LEVEL, 2, min(dist3, dist0));
        gl_TessLevelOuter[1] = mix(MAX_TESS_LEVEL, 2, min(dist0, dist1));
        gl_TessLevelOuter[2] = mix(MAX_TESS_LEVEL, 2, min(dist1, dist2));
        gl_TessLevelOuter[3] = mix(MAX_TESS_LEVEL, 2, min(dist2, dist3));

        gl_TessLevelInner[0] = max(gl_TessLevelOuter[1], gl_TessLevelOuter[3]);
        gl_TessLevelInner[1] = max(gl_TessLevelOuter[0], gl_TessLevelOuter[2]);
    }
}