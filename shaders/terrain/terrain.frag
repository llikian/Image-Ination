/***************************************************************************************************
 * @file  terrain.frag
 * @brief Fragment shader for rendering the terrain
 **************************************************************************************************/

#version 420 core

in vec3 position;
in vec3 normal;
in vec2 texCoords;

in float minHeight;
in float maxHeight;

out vec4 fragColor;

uniform vec3 cameraPos;
uniform vec3 lightDirection;

uniform float totalTerrainWidth;

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

    return ambient + diffuse;
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

    float t;
    bool under;
    for (int i = 0; i < 4; i++) {
        under = height < heights[i].y;

        t = float(under) * (height - heights[i].x) / (heights[i].y - heights[i].x);
        t += float(!under) * (heights[i].z - height) / (heights[i].z - heights[i].y);

        color += textures[i] * clamp(t, 0.0f, 1.0f);
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
    fragColor.a = fogFactor(totalTerrainWidth * 0.6f, totalTerrainWidth * 0.7f);
}
