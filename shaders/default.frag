/***************************************************************************************************
 * @file  default.frag
 * @brief Default fragment shader
 **************************************************************************************************/

#version 460 core

in vec3 position;
in vec3 normal;

out vec4 fragColor;

uniform vec3 cameraPos;

const vec3 LIGHT_COLOR = vec3(1.0f);
const vec3 LIGHT_POSITION = vec3(5.0f);

vec3 phongLighting();

void main() {
    fragColor = vec4(phongLighting(), 1.0f);
}

vec3 phongLighting() {
    /* Ambient */
    float ambient = 0.25f;

    /* Diffuse */
    vec3 lightDirection = normalize(LIGHT_POSITION - position);
    float diffuse = max(dot(normal, lightDirection), 0.0f);

    /* Specular */
    vec3 viewDirection = normalize(cameraPos - position);
    vec3 reflectionDir = reflect(-lightDirection, normal);
    float specular = 0.25f * pow(max(dot(viewDirection, reflectionDir), 0.0f), 32.0f);

    return LIGHT_COLOR * (ambient + diffuse + specular);
}