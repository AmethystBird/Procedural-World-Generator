#version 400 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

uniform mat4 mvp;
out vec3 ourColor;

void main()
{
	gl_Position = mvp * vec4(aPos, 1.0);
	ourColor = aColor;
}