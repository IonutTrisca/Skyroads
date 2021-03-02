#version 330

uniform sampler2D u_texture_0;
uniform int type;

layout(location = 0) out vec4 out_color;

void main()
{
	out_color = vec4(0, 0, 0, 1);

	if(type == 0)
		out_color = vec4(0.501, 0, 0.501, 1);
	if(type == 1)
		out_color = vec4(1, 0, 0, 1);
	if(type == 2)
		out_color = vec4(0, 0, 1, 1);
	if(type == 3)
		out_color = vec4(1, 1, 0, 1);
	if(type == 4)
		out_color = vec4(1, 0.647, 0, 1);
	if(type == 5)
		out_color = vec4(0, 0.5, 0, 1);
	if(type == 6)
		out_color = vec4(1);
	if(type == 7)
		out_color = vec4(1, 0, 1, 1);
}