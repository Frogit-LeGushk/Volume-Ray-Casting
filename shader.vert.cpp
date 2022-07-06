#version 110

varying vec4 position;

void main(void)
{
    gl_Position = gl_ModelViewMatrix*gl_Vertex;
    position = gl_Vertex;
}
