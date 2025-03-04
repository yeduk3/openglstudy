#version 410 core
layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Color;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projMat;

out vec3 point_Color;
void main() {
    vec4 worldPos = modelMat * vec4(in_Position, 1);
    gl_Position = projMat * viewMat * worldPos;
    point_Color = in_Color;
}