#version 410 core
layout (location = 0) in vec3 aPos;

uniform mat4 u_projection;
uniform mat4 u_view;

void main() {
	gl_Position = vec4(aPos, 1.0); 
}