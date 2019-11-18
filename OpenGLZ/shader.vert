#version 450

in vec3 position;
in vec3 normal;
in vec2 uv;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

out vec3 color_out;
out vec2 UV;

void main()
{
    gl_Position = projection * view * transform * vec4(position, 1.0) ;

    //color_out = (transform * vec4(normal,1) ).xyz ;
	//color_out = vec3(1,0,0) * normal ;
	color_out = vec3(uv,1);

	UV = uv;

}