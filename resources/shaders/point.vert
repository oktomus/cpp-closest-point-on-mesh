// Many thanks to https://www.desultoryquest.com/blog/drawing-anti-aliased-circular-points-using-opengl-slash-webgl/
#version 330 core
#extension GL_EXT_separate_shader_objects : enable

// INPUT
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;

// VARS
uniform mat4 view;
uniform mat4 projection;
uniform float size;

varying vec4 frag_color;

void main()
{
    gl_Position = projection * view * vec4(aPos, 1.0);
    gl_PointSize = size;
    frag_color = aColor;
}