#version 120

varying vec4 model_coordinate;
varying vec4 world_coordinate;

void main()
{
    model_coordinate = gl_Vertex;
    world_coordinate = gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
