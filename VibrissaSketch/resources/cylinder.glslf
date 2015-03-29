#version 120
#pragma optionNV (unroll all)
#define M_PI 3.1415926535897932384626433832795

uniform sampler2D colors;
uniform sampler2D shapes;
uniform float timer;
uniform float t;
uniform vec3 origin;

varying vec4 model_coordinate;

const int NUM_LEDS = 1024;
const vec3 ANISOTROPY = vec3(1.0f, 4.0f, 1.0f);
const float GAMMA = 1.0 / 2.5;
const float BRIGHTNESS = 0.2;
const float TURNS = 50.0;
const float HEIGHT = 1.8;


float led_angle(int index)
{
    return index * (M_PI * 2.0 * TURNS / NUM_LEDS);
}

vec3 led_position(int index)
{
    float angle = led_angle(index);
    float height = index * (HEIGHT / NUM_LEDS);
    return vec3(cos(angle), height, sin(angle));
}


vec3 led_color(int index, vec3 position)
{
    float x = timer * 0.01 + origin.x * 0.002;
    vec3 s = texture2D(shapes, vec2( mod(x, 1.0), position.y / HEIGHT )).rgb;

    float a = timer * 0.005;
    vec2 center = vec2(0.5) + vec2(sin(a), sin(a*0.5)) * s.r;

    float radius = 0.2;
    float gain = 0.3;

    vec3 c = texture2D(colors, center + position.xz * radius).rgb * gain;
    return c * c;
}


void main()
{
    vec3 light = vec3(0);
    for (int led = 0; led < NUM_LEDS; led++) {

        vec3 pos = led_position(led);

        vec3 v = (model_coordinate.xyz - pos) * ANISOTROPY;
        float dist2 = dot(v, v);
        dist2 *= dist2;
        dist2 *= dist2;
        dist2 *= dist2;

        light += led_color(led, pos) / dist2;
    }

    gl_FragColor = vec4(pow(light, vec3(GAMMA)) * vec3(BRIGHTNESS), 1.0);
}
