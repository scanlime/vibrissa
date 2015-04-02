#version 120
#define M_PI 3.1415926535897932385
#define M_E  2.7182818284590452354

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

mat4 rotationMatrix(vec3 axis, float angle)
{
axis = normalize(axis);
float s = sin(angle);
float c = cos(angle);
float oc = 1.0 - c;

return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
0.0,                                0.0,                                0.0,                                1.0);
}



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
    // Color palette sample
    vec3 c = texture2D(colors, vec2(0.6 + 0.5 * sin(timer * 0.04),
                                    0.3 + 0.5 * cos(timer * 0.1))).rgb;

    // Shape sample
    float x = timer * 0.03 + -origin.x * 0.001 + -position.x * 0.00015;
    float s = texture2D(shapes, vec2( mod(x, 1.0), position.y / HEIGHT )).r;

    return c * s * s;
}


void main()
{
    vec3 light = vec3(0);
    for (int led = 0; led < NUM_LEDS; led++) {

        vec3 pos = led_position(led);

        vec3 v = (model_coordinate.xyz - pos) * ANISOTROPY;
        float dist2 = dot(v, v);

        // focus / diffusion
        for (int sq = 0; sq < 4; sq++) {
            dist2 *= dist2;
        }

        light += led_color(led, pos) / dist2;
    }

    gl_FragColor = vec4(pow(light, vec3(GAMMA)) * vec3(BRIGHTNESS), 1.0);
}
