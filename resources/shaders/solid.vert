#version 330 core

// INPUT
layout (location = 0) in vec3 aPos;

// VARS
uniform mat4 view;
uniform mat4 projection;

// OUTPUT

// PROCESS
void main( void )
{
    gl_PointSize = 10.0; 
    gl_Position =  projection * view * vec4( aPos, 1.0 );     
}
