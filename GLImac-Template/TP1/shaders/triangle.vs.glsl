#version 330

layout(location = 3) in vec2 aVertexPosition;
layout(location = 8) in vec3 aVertexColor;
// layout(location = 1) in ivec2 aResolution;

out vec3     vColor;
uniform vec2 iResolution;

void main()
{
    vColor      = aVertexColor;
    gl_Position = vec4(aVertexPosition, 0, 1);
}