#version 330
//  WE ARE IN VIEW COORDINATES

in vec3 vertexPos; // in view coordinates
in vec3 vertexNormal;
in vec2 texCoord;

uniform sampler2D uTexture;

out vec3 fFragColor;

// LIGHTS DATA
uniform vec3  w_i[256]; // position light
uniform vec3  L_i[256]; // light color
uniform float intensity[256];
uniform int   nb_light;

// MATERIAL DATA
uniform vec3  K_d; // reflection coefficient (a color) DIFFUSE
uniform vec3  K_s; // glossy factor
uniform float shininess;

vec3 w_o = normalize(-vertexPos);

// Bling phong for i light
vec3 Blinn_Phong(int i)
{
    vec3 light_dir = normalize(w_i[i] - vertexPos); // actual w_i

    vec3 halfVector = normalize((w_o + light_dir) / 2.);

    vec3 diffuse = K_d * dot(light_dir, vertexNormal);
    vec3 glossy  = K_s * (pow(dot(halfVector, vertexNormal), shininess));

    float light_distance_i = distance(w_i[i], vertexPos);

    vec3 light_factor = intensity[i] * L_i[i] / (light_distance_i * light_distance_i);

    return light_factor * (diffuse + glossy);
}

void main()
{
    vec3 light = vec3(0.);
    for (int i = 0; i < nb_light; i++) {
        light += Blinn_Phong(i);
    }
    fFragColor = light * texture(uTexture, texCoord).xyz;
}