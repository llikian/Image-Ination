#version 420 core

out vec4 fragColor;

// Uniformes 
uniform vec2 resolution;
uniform float time;


float text(vec2 fragCoord)
{
    vec2 uv = mod(fragCoord.xy, 16.) * 0.0625;
    vec2 block = fragCoord * 0.0625 - uv;
    uv = uv * 0.8 + 0.1;
    uv += floor(max(1.0, sin(block.y / 16.0 * 0.2 + time)) * 8.0);
    uv *= 0.0625;
    float border = 0.1;
    float cube = step(border, uv.x) * step(uv.x, 1.0 - border) *
                 step(border, uv.y) * step(uv.y, 1.0 - border);

    return cube;
}


vec3 rain(vec2 fragCoord)
{
	fragCoord.x -= mod(fragCoord.x, 1.);
    //fragCoord.y -= mod(fragCoord.y, 16.);
    
    float speed=cos(fragCoord.x*3.)*.3+.7;
   
    float y = fract(fragCoord.y/resolution.y + time*speed );
    return vec3(1.0, 1.0, 1.0) / (y*20.);
}

void main()
{
    fragColor = vec4(text(gl_FragCoord.xy)*rain(gl_FragCoord.xy),1.);
}