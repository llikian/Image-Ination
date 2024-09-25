/***************************************************************************************************
 * @file  terrain.frag
 * @brief Fragment shader for rendering the terrain
 **************************************************************************************************/

#version 460 core

in vec3 position;
in vec3 normal;
in float maxHeight;

out vec4 fragColor;

struct Light {
    vec3 position;
    vec3 direction;
    bool isGlobal;
};

uniform Light light;
uniform vec3 cameraPos;

float phongLighting();
vec3 colorRamp4(in vec3 colors[4], in float weights[4], in float t);

void main() {
    float weights[4] = {0.0f, 0.2f, 0.5f, 1.0f};
    vec3 colors[4] = {
        vec3(0.184f, 0.694f, 0.831f),
        vec3(0.357f, 0.6f, 0.369f),
        vec3(0.58f, 0.49f, 0.388f),
        vec3(0.969f, 1.0f, 0.996f)
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

vec3 colorRamp4(in vec3 colors[4], in float weights[4], in float t) {
    vec3 color = vec3(0.0f);

    for (int i = 0; i < 3; ++i) {
        float w = clamp((t - weights[i]) / (weights[i+1] - weights[i]), 0.0f, 1.0f);
        color += (step(weights[i], t) - step(weights[i+1], t)) * mix(colors[i], colors[i+1], w);
    }

    return color;
}