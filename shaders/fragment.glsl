#version 330 core

out vec4 color;
in vec3 vert;

void main()
{
  color = vec4(abs(vert.x), abs(vert.y), abs(vert.z), 1.0);
}