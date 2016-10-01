#version 120

varying float DEPTH ;

void main()
{
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    DEPTH = gl_Position.z / 100.0; // do not divide by w
}