#version 450

out vec4 color;

in vec3 color_out;
in vec2 UV;

uniform sampler2D texture_ ;

void main()
{
    //color = vec4(abs(color_out), 1);

	color = texture(texture_, UV);
	//color = vec4(vec3(texture(texture_, UV)),1);
	//color = vec4(UV,0.5f,1);
}