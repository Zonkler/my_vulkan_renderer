#version 450

layout(location = 0) in vec3 outColor;
layout(location = 0) out vec4 out_color;

void main() {
    out_color = vec4(outColor, 1.0);
}