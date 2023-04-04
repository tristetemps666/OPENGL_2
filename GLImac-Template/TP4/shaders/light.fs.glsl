//  WE ARE IN VIEW COORDINATES

in vec3 vertexPos; // in view coordinates
in vec3 vertexNormal;
in vec2 texCoord;

uniform sampler2D uTexture;

out vec3 fFragColor;

uniform vec3  w_i; // position light
uniform vec3  L_i; // light color
uniform vec3  K_d; // reflection coefficient (a color) DIFFUSE
uniform vec3  K_s; // glossy factor
uniform float shininess;

vec3 w_o = normalize(-vertexPos);

vec3 light_dir = normalize(w_i - vertexPos); // actual w_i

vec3 halfVector = normalize((w_o + light_dir) / 2.);

vec3 Blinn_Phong()
{
    vec3 diffuse = K_d * dot(light_dir, vertexNormal);
    vec3 glossy  = K_s * (pow(dot(halfVector, vertexNormal), shininess));

    return L_i * (diffuse + glossy);
}

void main()
{
    // K_d = texture(uTexture, texCoord);

    fFragColor = Blinn_Phong() * texture(uTexture, texCoord).xyz;
    // fFragColor = vec3(light_dir.x);
}