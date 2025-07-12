#version 450

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vColor;

layout (location = 0) out vec3 outColor;


layout(push_constant) uniform Push{
	mat4 transform;
	vec3 color;
} push;

void main() {
	gl_Position = push.transform * vec4(vPosition,1.0);
	outColor = vColor;
}