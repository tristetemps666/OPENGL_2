#version 330

layout(location = 0) in vec2 aVertexPosition;
layout(location = 1) in vec2 aVertexUv;


out vec2     vUV;
uniform vec2 iResolution;
uniform mat3x3 TransformMatrix;
uniform float aspectRatio;


void main()
{
    vUV      = aVertexUv;
    vec3 pos = vec3(aVertexPosition,1);
    pos = TransformMatrix*vec3(aVertexPosition,1);
    pos.x/=aspectRatio;
    gl_Position = vec4(pos.xy, 0, 1);
}