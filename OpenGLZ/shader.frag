#version 450

out vec4 color;

in vec3 color_out;

void main()
{
    color = vec4(abs(color_out), 1);
}