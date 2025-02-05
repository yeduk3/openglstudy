#version 150 core

in vec3 worldPosition;
in vec3 normal;

uniform vec3 eyePosition;
uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 ambientColor;
uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform float shininess;

out vec4 out_Color;
void main() {

    // I = I_a * k_a + I_i(L . N)(k_d + k_s(R . V)^n)

    vec3 l = lightPosition - worldPosition;
    vec3 L = normalize(l);
    vec3 N = normalize(normal);
    vec3 R = 2 * dot(L, N) * N - L;
    vec3 V = normalize(eyePosition - worldPosition);
    vec3 I = lightColor / dot(l, l);

    vec3 ambient = diffuseColor * vec3(0.02);
    vec3 diffuse = I * diffuseColor * max(dot(L, N), 0);
    vec3 specular = I * specularColor * pow(max(dot(R, V), 0), shininess);

    vec3 phong = ambient + diffuse + specular;

    out_Color = vec4(phong, 1);
}