#version 330

in vec3      vColor;
uniform vec2 iResolution;
float        aspecRatio = iResolution.y / iResolution.x;

out vec3 fFragColor;

vec2 get_normalized_uv()
{
    return (gl_FragCoord.xy / iResolution.xy - 0.5f) / vec2(aspecRatio, 1f);
}

void main()
{
    // vec2 uv = gl_FragCoord.xy / iResolution.y;
    vec2 uv = get_normalized_uv();
    vec2 gv = fract(uv * 10);

    fFragColor = vec3(gv.x, gv.y, 0f);
    // fFragColor = gl_Position.xyz;
}