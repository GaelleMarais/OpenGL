#version 450

in vec3 position;
in vec3 color;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

out vec3 color_out;

void main()
{
    gl_Position = projection * view * transform * vec4(position, 1.0) ;

    color_out = color;
}