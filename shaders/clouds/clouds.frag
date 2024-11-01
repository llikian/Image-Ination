/***************************************************************************************************
 * @file  clouds.frag
 * @brief Fragment shader for rendering the clouds
 **************************************************************************************************/

#version 420 core

out vec4 fragColor;

struct Ray {
  vec3 position;
  vec3 direction;
};

// Paramètres de couleur et de lumière
vec3 skytop = vec3(0.0, 0.3, 0.5);
vec3 light = normalize(vec3(0.1, 0.2, 0.9));
vec2 cloudrange = vec2(100., -10000.);
mat3 m = mat3(0.00, 1.60, 1.20, -1.60, 0.72, -0.96, -1.20, -0.96, 1.28);

// Uniformes 
uniform vec2 resolution;
uniform vec3 cameraPos;
uniform vec3 cameraFront;
uniform vec3 cameraRight;
uniform vec3 cameraUp;

float hash(float n) {
    return fract(cos(n) * 114514.1919);
}

float noise(in vec3 x) {
    vec3 p = floor(x);
    vec3 f = smoothstep(0.0, 1.0, fract(x));
    float n = p.x + p.y * 10.0 + p.z * 100.0;
    return mix(
        mix(mix(hash(n + 0.0), hash(n + 1.0), f.x),
            mix(hash(n + 10.0), hash(n + 11.0), f.x), f.y),
        mix(mix(hash(n + 100.0), hash(n + 101.0), f.x),
            mix(hash(n + 110.0), hash(n + 111.0), f.x), f.y), f.z);
}

float fbm(vec3 p) {
    float f = 0.5000 * noise(p);
    p = m * p;
    f += 0.2500 * noise(p);
    p = m * p;
    f += 0.1666 * noise(p);
    p = m * p;
    f += 0.0834 * noise(p);
    return f;
}

void main() {
    // Calcul des coordonnées UV
    vec2 uv = (2.0f * gl_FragCoord.xy - resolution) / resolution.y;
    Ray ray = Ray(cameraPos, normalize(cameraFront + uv.x * cameraRight + uv.y * cameraUp));

    vec4 sum = vec4(0.0, 0.0, 0.0, 0.0);
    for (float depth = 0.0; depth < 100000.0; depth += 200.0) {
        ray.position = cameraPos + ray.direction * depth;
        if (cloudrange.x > ray.position.y && ray.position.y > cloudrange.y) {
            float alpha = smoothstep(0.5, 0.9, fbm(ray.position * 0.000050)); //modifier densité nuage
            vec3 localcolor = mix(vec3(1.0, 1.0, 1.0), vec3(0.6, 0.6, 0.6), alpha);
            alpha = (1.0 - sum.a) * alpha;
            sum += vec4(localcolor * alpha, alpha);
        }
    }

    float alpha = smoothstep(0.4, 1.0, sum.a);
    sum.rgb /= sum.a + 0.0001;
    float sundot = clamp(dot(ray.direction, light), 0.0, 1.0);
    vec3 col = 0.8 * (skytop);
    col += vec3(1.0, 1.0, 1.0) * pow(sundot, 350.0);
    col += 0.4 * vec3(1.0, 1.0, 1.0) * pow(sundot, 2.0);
    sum.rgb -= 0.6 * vec3(0.8, 0.75, 0.7) * pow(sundot, 13.0) * alpha;
    sum.rgb += 0.2 * vec3(1.3, 1.2, 1.0) * pow(sundot, 5.0) * (1.0 - alpha);

    col = mix(col, sum.rgb, sum.a);

    fragColor = vec4(col, 1.0);
}