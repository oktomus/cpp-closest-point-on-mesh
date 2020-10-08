#version 330 core
precision mediump float;

// INPUT
varying vec4 frag_color;

// OUTPUT
out vec4 FragColor;

void main()
{
    vec2 cxy = 2.0 * gl_PointCoord - 1.0;
    float r = dot(cxy, cxy);
    
    if (r > 1.0)
        discard;

    FragColor = frag_color;
}