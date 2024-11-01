/***************************************************************************************************
 * @file  noise_water.frag
 * @brief Fragment shader for rendering water
 **************************************************************************************************/

#version 420 core

in vec3 position;
in vec3 normal;

in float maxHeight;

out vec4 fragColor;

uniform vec3 lightDirection;
uniform vec3 cameraPos;

float phongLighting();
vec3 colorRamp4(in vec3 colors[4], in float weights[4], in float t);

void main() {
    float weights[4] = {-1.f, 0.3f, 0.5f, 1.f};
    vec3 colors[4] = {
        vec3(0.157, 0.127, 0.812),
        vec3(0.294, 0.273, 0.871),
        vec3(0.361, 0.575, 0.949),
        vec3(0.761, 0.727, 0.988)
    };

    fragColor.rgb = phongLighting() * colorRamp4(colors, weights, position.y / maxHeight);
    fragColor.a = 0.8f;
}

float phongLighting() {
    /* Ambient */
    float ambient = 0.1f;

    /* Diffuse */
    vec3 lightDir = normalize(lightDirection);
    float diffuse = max(dot(normal, lightDir), 0.0f);

    /* Specular */
    vec3 viewDirection = normalize(cameraPos - position);
    vec3 reflectionDir = reflect(-lightDir, normal);
    float specular = 0.25f * pow(max(dot(viewDirection, reflectionDir), 0.0f), 16.0f);

    return ambient + diffuse + specular;
}

vec3 colorRamp4(in vec3 colors[4], in float weights[4], in float t) {
    vec3 color = vec3(0.0f);

    for(int i = 0; i < 3; ++i) {
        float w = clamp((t - weights[i]) / (weights[i+1] - weights[i]), 0.0f, 1.0f);
        color += (step(weights[i], t) - step(weights[i+1], t)) * mix(colors[i], colors[i+1], w);
    }

    return color;
}