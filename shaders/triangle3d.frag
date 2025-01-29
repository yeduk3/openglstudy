#version 150 core
in vec3 point_Color;
out vec4 out_Color;
void main() {
    out_Color = vec4(point_Color, 0.5);
}