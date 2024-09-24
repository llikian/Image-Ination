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

struct RampColor {
    vec3 color;
    float weight;
};

uniform Light light;
uniform vec3 cameraPos;

vec3 phongLighting();
vec3 colorRamp4(in vec3 colors[4], in float weights[2], in float t);

void main() {
    float weights[2] = {0.2f, 0.5f};
    vec3 colors[4] = {
        vec3(0.184f, 0.694f, 0.831f),
        vec3(0.357f, 0.6f, 0.369f),
        vec3(0.631f, 0.545f, 0.467f),
        vec3(0.969f, 1.0f, 0.996f)
    };

    fragColor.rgb = phongLighting() * colorRamp4(colors, weights, position.y / maxHeight);
    fragColor.a = 1.0f;
}

vec3 phongLighting() {
    /* Ambient */
    float ambient = 0.1f;

    /* Diffuse */
    vec3 lightDirection = normalize(light.isGlobal ? light.direction : light.position - position);
    float diffuse = max(dot(normal, lightDirection), 0.0f);

    /* Specular */
    vec3 viewDirection = normalize(cameraPos - position);
    vec3 reflectionDir = reflect(-lightDirection, normal);
    float specular = 0.25f * pow(max(dot(viewDirection, reflectionDir), 0.0f), 16.0f);

    return vec3(ambient + diffuse + specular);
}

vec3 colorRamp4(in vec3 colors[4], in float weights[2], in float t) {
    if(t >= weights[1]) {
        return mix(colors[2], colors[3], (t - weights[1]) / (1.0f - weights[1]));
    } else if(t >= weights[0]) {
        return mix(colors[1], colors[2], (t - weights[0]) / (weights[1] - weights[0]));
    } else {
        return mix(colors[0], colors[1], t / weights[0]);
    }
}