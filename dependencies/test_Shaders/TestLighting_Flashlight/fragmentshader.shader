#version 330 core  

struct Material {
  //vec3 ambient;
	sampler2D specular;
	//vec3 diffuse;
	sampler2D diffuse;
	float specular_power;
	vec3 color;
};

struct Light{
	vec3 pos;
	vec3 color;
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float atten_constant;
	float atten_linear;
	float atten_quadratic;
	float cutOff;
	float outerCutOff;
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
	vec3 specular = light.specular * (spec * vec3(texture(material.specular, TexCoords)));
	
	//attenuation
	float distance = length(light.pos - FragPos);
	float attenuation = 
		1.0 / (light.atten_constant + (light.atten_linear * distance) + (light.atten_quadratic * distance * distance));
	
	
	
	vec3 result;

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;


  
	
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

	diffuse *= intensity;
	specular *= intensity;

	if (theta > light.outerCutOff) {
		result = (ambient + diffuse + specular);

	} else {
		result = ambient;
	}


	FragColor = vec4(result, 1.0);  
}	