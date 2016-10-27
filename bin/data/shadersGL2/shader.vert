#version 120

varying float DEPTH ;
uniform vec2 clipping;

void main()
{
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    
    DEPTH = (clamp(gl_Position.z, clipping.x, clipping.y) - clipping.x)/(clipping.y - clipping.x);
}