#version 330
in vec3 frag_normal;

uniform sampler2D u_texture_0;
uniform int type;

layout(location = 0) out vec4 out_color;

void main()
{
	if (type == 0)
		out_color = vec4(1);

	if (type == 1)
		out_color = vec4(1, 0, 0, 1);

	if (type == 2)
		out_color = vec4(1, 1, 0, 1);

	if (type == 3)
		out_color = vec4(0, 0, 0, 1);
}