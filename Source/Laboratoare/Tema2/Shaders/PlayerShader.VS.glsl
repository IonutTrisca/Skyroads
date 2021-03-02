#version 330

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;
layout(location = 3) in vec3 v_color;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform int is_powered_up;

out vec3 frag_normal;
out vec3 frag_color;
out vec2 tex_coord;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
	frag_normal = v_normal;
	frag_color = v_color;
	tex_coord = v_texture_coord;
	vec3 new_pos = v_position;

	if (is_powered_up != 0)
		new_pos = v_position + rand(v_position.xy + is_powered_up) * v_normal;

	gl_Position = Projection * View * Model * vec4(new_pos, 1.0);
}
