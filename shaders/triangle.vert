#version 410 core
layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Color;
out vec3 point_Color;
void main() {
    gl_Position = vec4(in_Position, 1);
    point_Color = in_Color;
}