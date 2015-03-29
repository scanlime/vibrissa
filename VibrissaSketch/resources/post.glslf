#version 110

uniform sampler2D scene;
uniform sampler2D blurred;

void main()
{
    vec3 scene_color = texture2D(scene, gl_TexCoord[0].st).rgb;
    vec3 blurred_color = texture2D(blurred, gl_TexCoord[0].st).rgb;

    gl_FragColor.rgb = pow(scene_color * 2.0 + blurred_color * 4.0, vec3(2.5));
    gl_FragColor.a = 1.0;
}