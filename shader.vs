#version 330

layout(location = 0) in vec4 pos;
layout(location = 1) in vec4 color;

uniform mat4 projMatr;
uniform mat4 rotMatr;

smooth out vec4 _color;

void main() {
    gl_Position = projMatr * (rotMatr*pos + vec4(0.0f, -1.5f, -4.0f, 0.0f));
    _color = color;
}
