#version 450

in vec3 position;
in vec3 normal;
in vec2 uv;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

out vec2 UV;
out vec3 normal_Out;

void main()
{
    gl_Position = projection * view * transform * vec4(position, 1.0) ;

	UV = uv;
	normal_Out = normal;

}