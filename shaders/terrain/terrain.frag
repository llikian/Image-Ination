/***************************************************************************************************
 * @file  terrain.frag
 * @brief Fragment shader for rendering the terrain
 **************************************************************************************************/

#version 460 core

in vec3 position;
in vec3 normal;

in float maxHeight;

out vec4 fragColor;

uniform vec3 cameraPos;
uniform vec3 lightDirection;

uniform float totalTerrainWidth;
uniform bool isFogActive;

uniform vec3 u_colors[4];
uniform float u_weights[4];

float phongLighting() {
    /* Ambient */
    float ambient = 0.1f;

    /* Diffuse */
    vec3 lightDir = normalize(lightDirection);
    float diffuse = max(dot(normal, lightDir), 0.0f);

    /* Specular */
//    vec3 viewDirection = normalize(cameraPos - position);
//    vec3 reflectionDir = reflect(-lightDir, normal);
//    float specular = 0.25f * pow(max(dot(viewDirection, reflectionDir), 0.0f), 16.0f);
    float specular = 0.0f;

    return ambient + diffuse + specular;
}

float fogFactor(float minDistance, float maxDistance) {
    float fogFactor = (maxDistance - distance(position, cameraPos)) / (maxDistance - minDistance);
    return clamp(exp(fogFactor), 0.0f, 1.0f);
}

vec3 colorRamp4(in vec3 colors[4], in float weights[4], in float t) {
    vec3 color = vec3(0.0f);

    for (int i = 0; i < 3; ++i) {
        float w = clamp((t - weights[i]) / (weights[i + 1] - weights[i]), 0.0f, 1.0f);
        color += (step(weights[i], t) - step(weights[i + 1], t)) * mix(colors[i], colors[i + 1], w);
    }

    return color;
}

void main() {
    fragColor.rgb = phongLighting() * colorRamp4(u_colors, u_weights, position.y / maxHeight);
    fragColor.a = isFogActive ? fogFactor(totalTerrainWidth * 0.5f, totalTerrainWidth * 0.7f) : 1.0f;
}
