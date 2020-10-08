#version 330 core

// INPUT
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

// VARS
uniform mat4 view;
uniform mat4 projection;

varying vec3 frag_pos;
varying vec3 frag_norm;

// PROCESS
void main( void )
{
    vec4 pos = vec4(aPos, 1.0);

    vec4 frag_pos4 = view * pos;
    frag_pos = frag_pos4.xyz;

    mat3 normal_matrix = mat3(transpose(inverse(view)));
    frag_norm = normal_matrix * aNormal;

    gl_Position =  projection * frag_pos4;
    gl_PointSize = 20.0;
}