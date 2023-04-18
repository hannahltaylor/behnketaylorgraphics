#version 410
layout (location = 0) in vec3 VertexPosition;
out vec3 Vec;
//uniform mat4 MVP;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main() {
    Vec = VertexPosition;
    gl_Position = projection * view * model * vec4(VertexPosition,1.0);
}