#version 330

layout(location = 0) in vec2 aVertexPosition;
layout(location = 1) in vec3 aVertexColor;
layout(location = 2) in vec2 aMousePos;

out vec3     vColor;
uniform vec2 iResolution;

void main()
{
    vColor      = aVertexColor;
    gl_Position = vec4(aVertexPosition, 0, 1);
}