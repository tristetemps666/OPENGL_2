#version 330

in vec3 vColor;

out vec3 fFragColor;

void main()
{
    fFragColor = vec3(vColor.r, 1f, 0f);
    // fFragColor = gl_Position.xyz;
}