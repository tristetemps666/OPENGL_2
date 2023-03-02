#version 330

in vec2      vUV;

uniform vec2 iResolution;
uniform sampler2D uTexture;



float        aspecRatio = iResolution.y / iResolution.x;

out vec3 fFragColor;

vec2 get_normalized_uv()
{
    return (gl_FragCoord.xy / iResolution.xy - 0.5) / vec2(aspecRatio, 1.);
}

void main()
{
    vec4 coolTex = texture(uTexture,vUV);
    fFragColor = coolTex.xyz;
    // fFragColor = vec3(vUV.x, vUV.y, 0.);
}