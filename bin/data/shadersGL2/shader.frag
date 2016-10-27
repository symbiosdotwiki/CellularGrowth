#version 120
varying float DEPTH ;

void main() {
    // far things appear white, near things black
    gl_FragColor=vec4(DEPTH, DEPTH, DEPTH, 1) ;
}
