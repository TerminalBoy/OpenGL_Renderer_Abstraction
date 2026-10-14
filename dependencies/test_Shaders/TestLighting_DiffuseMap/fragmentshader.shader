#version 330 core  

struct Material {
  //vec3 ambient;
	vec3 specular;
	//vec3 diffuse;
	sampler2D diffuse;
	float specular_power;
	vec3 color;
};

struct Light{
	vec3 pos;
	vec3 color;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct Camera{
  vec3 pos;
};

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
out vec4 FragColor;  

uniform Material material;
uniform Light light;
uniform Camera camera;

uniform bool rendering_light_source;
uniform bool rendering_specular;

void main() {  

	if (rendering_light_source) {
		FragColor = vec4(light.color, 1.0);
		return;	
	}


	//ambient
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

	//diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.pos - FragPos);
	float diff= max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

	//specular
	vec3 viewDir = normalize(camera.pos - FragPos);
	vec3 reflectionDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectionDir), 0.0), material.specular_power);
	vec3 specular = light.specular * (spec * material.specular);
	vec3 result;
  if (rendering_specular) {
		result = (ambient + diffuse + specular);
	} else {
		result = (ambient + diffuse);
	}

	FragColor = vec4(result, 1.0);  
}	