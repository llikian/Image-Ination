/***************************************************************************************************
 * @file  terrain.frag
 * @brief Fragment shader for rendering the terrain
 **************************************************************************************************/

#version 460 core

in vec3 position;
in vec3 normal;
in float maxHeight;
in flat uint biome1;
in flat uint biome2;
in float blendFactor;

out vec4 fragColor;

struct Light {
    vec3 position;
    vec3 direction;
    bool isGlobal;
};

uniform Light light;
uniform vec3 cameraPos;

float phongLighting();
void getBiomeColorRamp(out vec3 colors[4], out float weights[4]);
vec3 colorRamp4(in vec3 colors[4], in float weights[4], in float t);

void main() {
    vec3 colors1[4];
    float weights1[4];
    getBiomeColorRamp(colors1, weights1);

    vec3 colors2[4];
    float weights2[4];
    getBiomeColorRamp(colors2, weights2);

    vec3 colors[4] = {
        mix(colors1[0], colors2[0], blendFactor),
        mix(colors1[1], colors2[1], blendFactor),
        mix(colors1[2], colors2[2], blendFactor),
        mix(colors1[3], colors2[3], blendFactor)
    };

    float weights[4] = {
        0.0f,
        mix(weights1[1], weights2[1], blendFactor),
        mix(weights1[2], weights2[2], blendFactor),
        1.0f
    };

    fragColor.rgb = phongLighting() * colorRamp4(colors, weights, position.y / maxHeight);
    fragColor.a = 1.0f;
}

float phongLighting() {
    /* Ambient */
    float ambient = 0.1f;

    /* Diffuse */
    vec3 lightDirection = normalize(light.isGlobal ? light.direction : light.position - position);
    float diffuse = max(dot(normal, lightDirection), 0.0f);

    /* Specular */
    vec3 viewDirection = normalize(cameraPos - position);
    vec3 reflectionDir = reflect(-lightDirection, normal);
    float specular = 0.25f * pow(max(dot(viewDirection, reflectionDir), 0.0f), 16.0f);

    return ambient + diffuse + specular;
}

void getBiomeColorRamp(out vec3 colors[4], out float weights[4]) {
    weights[0] = 0.0f;
    weights[3] = 1.0f;
    switch (biome1) {
        case 0: // Mountains
            colors[0] = vec3(0.522f, 0.4f, 0.318f);
            colors[1] = vec3(0.71f);
            colors[2] = vec3(0.902f);
            colors[3] = vec3(1.0f);
            weights[1] = 0.33f;
            weights[2] = 0.66f;
            break;
        case 1: // Plains
            colors[0] = vec3(0.396f, 0.71f, 1.0f);
            colors[1] = vec3(0.788f, 0.643f, 0.345f);
            colors[2] = vec3(0.325f, 0.522f, 0.318f);
            colors[3] = vec3(0.345f, 0.788f, 0.353f);
            weights[1] = 0.4f;
            weights[2] = 0.66f;
            break;
        case 2: // Desert
            colors[0] = vec3(0.396f, 0.71f, 1.0f);
            colors[1] = vec3(0.396f, 0.71f, 1.0f);
            colors[2] = vec3(0.98f, 0.851f, 0.361f);
            colors[3] = vec3(1.0f, 0.929f, 0.396f);
            weights[1] = 0.2f;
            weights[2] = 0.3f;
            break;
    }
}

vec3 colorRamp4(in vec3 colors[4], in float weights[4], in float t) {
    vec3 color = vec3(0.0f);

    for (int i = 0; i < 3; ++i) {
        float w = clamp((t - weights[i]) / (weights[i+1] - weights[i]), 0.0f, 1.0f);
        color += (step(weights[i], t) - step(weights[i+1], t)) * mix(colors[i], colors[i+1], w);
    }

    return color;
}