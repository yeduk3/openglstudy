#version 410 core
in vec3 in_Position;
uniform mat4 testMat;
void main() {
    vec4 pos = vec4(in_Position, 1);
    gl_Position = pos * testMat;
}