#version 120

const float GAMMA = 1.0 / 2.5;
const float BRIGHTNESS = 0.2;

uniform sampler2D colors;
uniform sampler2D shapes;
uniform sampler2D modulator;
uniform float timer;
uniform vec3 origin;

varying vec4 model_coordinate;

float rand(vec2 n)
{
    return 0.5 + 0.5 * fract(sin(dot(n.xy, vec2(12.9898, 78.233)))* 43758.5453);
}

vec3 pix_color(vec3 position)
{
    // Color palette sample
    vec3 c = texture2D(colors, vec2(0.6 + 0.5 * sin(timer * 0.04),
    0.3 + 0.5 * cos(timer * 0.1))).rgb;

    // Shape sample
    float x = timer * 0.06 + (origin.x + position.x) * 0.015;
    float s = texture2D(shapes, vec2( mod(x, 1.0), position.y * 0.25 + 0.5 )).r;

    float fluff = texture2D(modulator, position.xy * vec2(0.07, 0.10) + 0.5 + vec2(0.03, 0.01) * sin(timer * 0.1 + position.x * 0.1)).r;
    return c * s * s * fluff;
}


void main()
{
    vec3 light = pix_color(model_coordinate.xyz);
    gl_FragColor = vec4(pow(light, vec3(GAMMA)) * vec3(BRIGHTNESS), 1.0);
}
