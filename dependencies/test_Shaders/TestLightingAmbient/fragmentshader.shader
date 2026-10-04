#version 330 core  

out vec4 FragColor;  

uniform vec3 object_color;
uniform vec3 light_color;
uniform bool rendering_light_source;

void main() {  

	if (rendering_light_source) {
		FragColor = vec4(light_color, 1.0);
		return;	
	}

	float ambientstrength = 0.1;
	vec3 ambient = ambientstrength * light_color;
	vec3 result = ambient * object_color;

	FragColor = vec4(result, 1.0);  
}