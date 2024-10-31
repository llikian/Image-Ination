/***************************************************************************************************
 * @file  terrain.frag
 * @brief Fragment shader for rendering the terrain
 **************************************************************************************************/

#version 460 core

in vec3 position;
in vec3 normal;
in vec2 texCoords;

in float minHeight;
in float maxHeight;

out vec4 fragColor;

uniform vec3 cameraPos;
uniform vec3 lightDirection;

uniform float totalTerrainWidth;
uniform bool isFogActive;

uniform sampler2D texRock;
uniform sampler2D texRockSmooth;
uniform sampler2D texGrass;
uniform sampler2D texGrassDark;
uniform sampler2D texSnow;

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

vec3 getTextureColor() {
    vec3 color = vec3(0.0f);

    float height = (position.y - minHeight) / (maxHeight - minHeight);

    vec3 textures[4] = {
        texture(texGrass, texCoords).xyz,
        texture(texGrassDark, texCoords).xyz,
        texture(texRock, texCoords).xyz,
        texture(texSnow, texCoords).xyz
    };

    /* x: Low Height ; y: Optimal Height ; z: High Height */
    vec3 heights[4] = {
        vec3(0.000f, 0.100f, 0.200f),
        vec3(0.110f, 0.220f, 0.450f),
        vec3(0.240f, 0.475f, 0.700f),
        vec3(0.500f, 0.720f, 1.000f)
    };

    for (int i = 0; i < 4; i++) {
        if (height < heights[i].y) {
            color += textures[i] * clamp((height - heights[i].x) / (heights[i].y - heights[i].x), 0.0f, 1.0f);
        } else {
            color += textures[i] * clamp((heights[i].z - height) / (heights[i].z - heights[i].y), 0.0f, 1.0f);
        }
    }

    return color;
}

float fogFactor(float minDistance, float maxDistance) {
    float dist = distance(position.xz, cameraPos.xz);
    float fogFactor = (maxDistance - dist) / (maxDistance - minDistance);
    return clamp(exp(fogFactor), 0.0f, 1.0f);
}

void main() {
    fragColor.rgb = phongLighting() * getTextureColor();
    fragColor.a = isFogActive ? fogFactor(totalTerrainWidth * 0.8f, totalTerrainWidth * 0.9f) : 1.0f;
}
