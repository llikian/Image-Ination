/***************************************************************************************************
 * @file  default.frag
 * @brief Default fragment shader
 **************************************************************************************************/

#version 460 core

in vec3 position;
in vec3 normal;
in float maxHeight;

out vec4 fragColor;

uniform bool isLight;
uniform vec3 cameraPos;
uniform vec3 lightPos;

uniform bool isTerrain;

const vec3 LIGHT_COLOR = vec3(1.0f);

vec3 phongLighting();

void main() {
    if(isLight) {
        fragColor = vec4(1.0f);
        return;
    } else {
        fragColor = vec4(phongLighting(), 1.0f);
    }

    if(isTerrain) {
        const int n = 4;
        float height = position.y / maxHeight;

        vec3 colors[n] = {
                vec3(0.184f, 0.694f, 0.831f),
                vec3(0.357f, 0.6f, 0.369f),
                vec3(0.631f, 0.545f, 0.467f),
                vec3(0.969f, 1f, 0.996f)
        };

        float weights[n] = {0.0f, 0.2f, 0.5f, 1.0f};

        for(int i = 0 ; i < n - 1 ; ++i) {
            if(height >= weights[i] && height <= weights[i + 1]) {
                fragColor.rgb *= mix(colors[i], colors[i + 1],
                (height - weights[i]) / (weights[i + 1] - weights[i]));
            }
        }
    }
}

vec3 phongLighting() {
    /* Ambient */
    float ambient = 0.1f;

    /* Diffuse */
    vec3 lightDirection = normalize(lightPos - position);
    float diffuse = max(dot(normal, lightDirection), 0.0f);

    /* Specular */
    vec3 viewDirection = normalize(cameraPos - position);
    vec3 reflectionDir = reflect(-lightDirection, normal);
    float specular = 0.25f * pow(max(dot(viewDirection, reflectionDir), 0.0f), 16.0f);

    return LIGHT_COLOR * (ambient + diffuse + specular);
}