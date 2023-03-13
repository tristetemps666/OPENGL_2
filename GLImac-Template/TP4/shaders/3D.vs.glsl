#version 330

// get attributes (pos, normal, texture uv)
layout(location = 0) in vec3 aVertexPosition;
layout(location = 1) in vec3 aVertexNormal;
layout(location = 2) in vec2 aVertexUv;


// transform matrix
uniform mat4 uMVPMatrix;
uniform mat4 uMVMatrix;
uniform mat4 uNormalMatrix;


out vec3 vertexPos;  // in view coordinates
out vec3 vertexNormal; // in view coordinates
out vec2 texCoord;

void main(){

    // homogène
    vec4 HvertexPos = vec4(aVertexPosition,1.);
    vec4 HvertexNormal = vec4(aVertexNormal,0.);

    // position de sortie
    texCoord = aVertexUv;
    vertexPos = vec3(uMVMatrix*HvertexPos);
    vertexNormal = vec3(uNormalMatrix*HvertexNormal);
    
    gl_Position = uMVPMatrix * HvertexPos;
}
