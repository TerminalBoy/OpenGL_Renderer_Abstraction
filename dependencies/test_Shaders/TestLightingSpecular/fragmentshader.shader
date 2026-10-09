#version 330 core  

in vec3 Normal;
in vec3 FragPos;
out vec4 FragColor;  

uniform vec3 object_color;
uniform vec3 light_color;
uniform bool rendering_light_source;
uniform vec3 lightPos;
uniform vec3 viewPos;


void main() {  

	if (rendering_light_source) {
		FragColor = vec4(light_color, 1.0);
		return;	
	}

	float ambientstrength = 0.1;
	vec3 ambient = ambientstrength * light_color;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff= max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * light_color;

	float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectionDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectionDir), 0.0), 32);
	vec3 specular = specularStrength * spec * light_color;

	vec3 result = (ambient + diffuse + specular) * object_color;
	
	FragColor = vec4(result, 1.0);  
}