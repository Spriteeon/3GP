#version 330

out vec4 fragment_colour;

in vec3 varying_normal;
in vec3 varying_position;
in vec2 varying_coord;

uniform sampler2D sampler_tex;

void main(void)
{

	vec3 light_position = vec3(0, 5000, 0);
	vec3 tex_colour = texture(sampler_tex, varying_coord).rgb;

	vec3 P = varying_position;

	vec3 light_direction = P - light_position;

	vec3 L = normalize(-light_direction);
	vec3 N = normalize(varying_normal);
	
	float diffuse_intensity = max(0, dot( L, N )) * 0.8;

	vec3 final_colour = tex_colour * diffuse_intensity;

	vec4 fragment_colour_one = vec4(final_colour, 1.0); //Point Light
	vec4 fragment_colour_two = vec4(tex_colour, 1.0) * 0.7; //Ambient Lighting

	fragment_colour = fragment_colour_one + fragment_colour_two;

}