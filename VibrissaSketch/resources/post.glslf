#version 110

uniform sampler2D scene;
uniform sampler2D blurred;

void main()
{
    vec3 scene_color = texture2D(scene, gl_TexCoord[0].st).rgb;
    vec3 blurred_color = texture2D(blurred, gl_TexCoord[0].st).rgb;

gl_FragColor.rgb = scene_color + blurred_color * blurred_color;
    gl_FragColor.a = 1.0;
}