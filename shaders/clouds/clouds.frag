/***************************************************************************************************
 * @file  clouds.frag
 * @brief Fragment shader for rendering the clouds
 **************************************************************************************************/

#version 420 core

in vec3 cameraPos;

out vec4 fragColor;

// Paramètres de couleur et de lumière
vec3 skytop = vec3(0.1, 0.5, 0.9);
vec3 light = normalize(vec3(0.1, 0.2, 0.9));
vec2 cloudrange = vec2(100., -1000.);
vec3 cloudHeight = vec3(0., -50000., 0.);
mat3 m = mat3(0.00, 1.60, 1.20, -1.60, 0.72, -0.96, -1.20, -0.96, 1.28);

// Uniformes 
uniform vec2 resolution;
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

vec4 cloud(vec4 sum, float densiteMin, float densiteMax, vec3 externColor, vec3 internColor, vec3 direction) {
    vec3 position;

    for (float depth = 0.0; depth < 100000.0; depth += 100.0) {
        position = cameraPos + direction * depth + cloudHeight;

        if (cloudrange.x > position.y && position.y > cloudrange.y) {
            float alpha = smoothstep(densiteMin, densiteMax, fbm(position * 0.000050)); //modifier densité nuage
            vec3 localcolor = mix(externColor, internColor, alpha);
            alpha = (1.0 - sum.a) * alpha;
            sum += vec4(localcolor * alpha, alpha);
        }
    }

    return sum;
}

mat3 setCamera( in vec3 ro, in vec3 ta, float cr ) {
    vec3 cw = normalize(ta-ro);
    vec3 cp = vec3(sin(cr), cos(cr),0.0);
    vec3 cu = normalize( cross(cw,cp) );
    vec3 cv =          ( cross(cu,cw) );
    return mat3( cu, cv, cw );
}

void main() {
    // Calcul des coordonnées UV
    vec2 uv = (2.0f * gl_FragCoord.xy - resolution) / resolution.y;
//    vec3 direction = normalize(cameraFront + uv.x * cameraRight + uv.y * cameraUp);

    mat3 camera = mat3(cameraRight, cameraUp, cameraFront);
    vec3 direction = camera * normalize(vec3(uv, 2.5f));

    vec4 sum = vec4(0.0, 0.0, 0.0, 0.0);
    sum = cloud(sum, 0.5, 0.9, vec3(1., 1., 1.), vec3(0.8, 0.8, 0.8), direction);

    float alpha = smoothstep(0.4, 1.0, sum.a);
    sum.rgb /= sum.a + 0.0001;
    float sundot = clamp(dot(direction, light), 0.0, 1.0);
    vec3 col = 0.8 * (skytop);
    col += vec3(1.0, 1.0, 1.0) * pow(sundot, 350.0);
    col += 0.4 * vec3(1.0, 1.0, 1.0) * pow(sundot, 2.0);
    sum.rgb -= 0.6 * vec3(0.8, 0.75, 0.7) * pow(sundot, 13.0) * alpha;
    sum.rgb += 0.2 * vec3(1.3, 1.2, 1.0) * pow(sundot, 5.0) * (1.0 - alpha);

    col = mix(col, sum.rgb, sum.a);

    fragColor = vec4(col, 1.0);
}