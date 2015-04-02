#version 120

varying vec4 model_coordinate;

void main()
{
    model_coordinate = gl_Vertex;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
