#version 120
varying float DEPTH ;

void main() {
    float c = 1.0 - exp( - DEPTH * 0.2 );
    
   // gl_FragColor =  mix( vec4(0,0,0,1),vec4(0.5,0.6,0.7,1), c);
    
    // far things appear white, near things black
    gl_FragColor=vec4(DEPTH,DEPTH,DEPTH,1) ;
    
}
