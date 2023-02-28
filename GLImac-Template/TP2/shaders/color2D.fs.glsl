#version 330

in vec3      vColor;
uniform vec2 iResolution;
float        aspecRatio = iResolution.y / iResolution.x;

out vec3 fFragColor;

vec2 get_normalized_uv()
{
    return (gl_FragCoord.xy / iResolution.xy - 0.5) / vec2(aspecRatio, 1.);
}

void main()
{
    // vec2 uv = gl_FragCoord.xy / iResolution.y;
    vec2 uv = get_normalized_uv();
    vec2 gv = fract(uv * 10);

    fFragColor = vec3(gv.x, gv.y, 0.);
    fFragColor = vec3(2*length(uv));
    // fFragColor = gl_Position.xyz;
}