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
vec2 cloudrange = vec2(1000.0, -1000.0);
vec3 cloudHeight = vec3(0.0, -5000.0, 0.0);
mat3 m = mat3(0.00, 1.60, 1.20, -1.60, 0.72, -0.96, -1.20, -0.96, 1.28);

// Uniformes 
uniform vec2 resolution;
uniform vec3 cameraFront;
uniform vec3 cameraRight;
uniform vec3 cameraUp;
uniform float time;

struct CloudType {
    float densiteMin;
    float densiteMax;
    vec3 externColor;
    vec3 internColor;
};

CloudType cirrus = CloudType(0.5, 1., vec3(1.0, 1.0, 1.0), vec3(0.8, 0.8, 0.8));
CloudType cumulus = CloudType(0.6, 1., vec3(1.0, 1.0, 1.0), vec3(0.8, 0.8, 0.8));
CloudType stratus = CloudType(0.1, 0.8, vec3(0.9, 0.9, 0.9), vec3(0.5, 0.5, 0.5));

float hash(float n) {
    return fract(cos(n) * 114514.1919);
}

float Hash(in vec3 p)  
{
    p  = fract( p*0.3199+0.152 );
	p *= 17.0;
    return fract( p.x*p.y*p.z*(p.x+p.y+p.z) );
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

float Voronoi(vec3 x, float scale)
{
    vec3 p = floor(x / scale);
    vec3 f = fract(x / scale);

    float Distance = 100.0;
    for (int k = -1; k <= 1; k++)
    {
        for (int j = -1; j <= 1; j++)
        {
            for (int i = -1; i <= 1; i++)
            {
                vec3 b = vec3(float(i), float(j), float(k)) ;
                vec3 r = vec3(b) - f + Hash(p + b);
                float d = dot(r, r);

                if (d < Distance)
                {
                    Distance = d;
                }
            }
        }
    }

    return sqrt(Distance);
}

vec4 cloud(vec4 sum, float densiteMin, float densiteMax, vec3 externColor, vec3 internColor, vec3 direction) {
    vec3 position;
    float alpha, density;
    float stepSize = 100.0; 

    
    for (float depth = 0.0; depth < 100000.0; depth += stepSize) {
        position = cameraPos + direction * depth + cloudHeight;

        if (cloudrange.x > position.y && position.y > cloudrange.y) {

            density = mix(Voronoi(position, 10000.0), fbm(position * 0.0005), 0.5);
            alpha = smoothstep(densiteMin, densiteMax, density);

            vec3 localcolor = mix(externColor, internColor, alpha);

            alpha *= (1.0 - sum.a);
            sum += vec4(localcolor * alpha, alpha);

            if (sum.a > 0.95) break; 
        }
    }

    return sum;
}



CloudType mixCloudType(CloudType a, CloudType b, float t) {
    return CloudType(
        mix(a.densiteMin, b.densiteMin, t),
        mix(a.densiteMax, b.densiteMax, t),
        mix(a.externColor, b.externColor, t),
        mix(a.internColor, b.internColor, t)
    );
}

CloudType getActiveCloudType(float time) {
    float cycle = mod(time, 40.0);
    float t;

    if (cycle < 5.0) {
        return cirrus; 
    } else if (cycle < 10.0) {
        t = (cycle - 5.0) / 5.0; 
        return mixCloudType(cirrus, cumulus, t);
    } else if(cycle < 20.0){
        t = (cycle - 10.0) / 10.0; 
        return mixCloudType(cumulus, stratus, t);
    }
    else{
        t = (cycle - 20.0) / 20.0; 
        return mixCloudType(stratus, cirrus, t);
    }
}

void main() {
    vec2 uv = (2.0 * gl_FragCoord.xy - resolution) / resolution.y;
    float focalLength = 2.5f;
    vec3 direction = mat3(cameraRight, cameraUp, cameraFront) * normalize(vec3(uv, focalLength));

    vec4 sum = vec4(0.0);
    CloudType activeCloud = getActiveCloudType(time);

    sum = cloud(
        sum,
        activeCloud.densiteMin,
        activeCloud.densiteMax,
        activeCloud.externColor,
        activeCloud.internColor,
        direction 
    );

    

    float alpha = smoothstep(0.4, 1.0, sum.a);
    sum.rgb /= sum.a + 0.0001;
    float sundot = clamp(dot(direction, light), 0.0, 1.0);
    vec3 col = 0.8 * skytop;
    col += vec3(1.0, 1.0, 1.0) * pow(sundot, 350.0);
    col += 0.4 * vec3(1.0, 1.0, 1.0) * pow(sundot, 2.0);
    sum.rgb -= 0.6 * vec3(0.8, 0.75, 0.7) * pow(sundot, 13.0) * alpha;
    sum.rgb += 0.2 * vec3(1.3, 1.2, 1.0) * pow(sundot, 5.0) * (1.0 - alpha);

    

    col = mix(col, sum.rgb, sum.a);

    
    
    fragColor = vec4(col, 1.0);
}
