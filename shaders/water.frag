/***************************************************************************************************
 * @file  water.frag
 * @brief Fragment shader for rendering water
 **************************************************************************************************/

#version 460 core

in vec3 position;
in vec3 normal;

in float maxHeight;

out vec4 fragColor;

uniform vec3 lightDirection;
uniform vec3 cameraPos;

float phongLighting();
vec3 colorRamp4(in vec3 colors[4], in float weights[4], in float t);

void main() {
    float weights[4] = {-1.0f, -0.5f, 0.5f, 1.f};
    vec3 colors[4] = {
        vec3(0, 0.234, 0.453),
        vec3(0, 0.463, 0.649),
        vec3(0.25, 0.482, 0.828),
        vec3(0.54, 0.625, 0.956)
    };

    fragColor.rgb = phongLighting() * colorRamp4(colors, weights, position.y / maxHeight);
    fragColor.a = 0.6f;
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