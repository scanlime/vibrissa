#version 120
#pragma optionNV (unroll all)
#define M_PI 3.1415926535897932384626433832795

uniform sampler2D colors;
uniform float timer;

varying vec4 model_coordinate;
varying vec4 world_coordinate;

const int NUM_LEDS = 512;
const vec3 ANISOTROPY = vec3(1.0f, 4.0f, 1.0f);
const float GAMMA = 1.0 / 2.5;
const float BRIGHTNESS = 0.1;
const float TURNS = 64.0;
const float HEIGHT = 1.3;


vec3 led_position(int index)
{
    float angle = index * (M_PI * 2.0 * TURNS / NUM_LEDS);
    float height = index * (HEIGHT / NUM_LEDS);

    return vec3(cos(angle), height, sin(angle));
}


vec3 led_color(int index, vec3 position)
{
    vec3 c = texture2D(colors, vec2(mod(position.x + timer * 0.1, 1.0), position.y)).rgb;
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
