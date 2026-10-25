#version 330 core  

// struct Material {
//   //vec3 ambient;
// 	sampler2D specular;
// 	//vec3 diffuse;
// 	sampler2D diffuse;
// 	float specular_power;
// 	vec3 color;
// };

struct Camera{
  vec3 pos;
};

struct DirectionalLight{
	vec3 direction;
	vec3 color;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight {
	vec3 pos;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 color;

	float atten_constant;
	float atten_linear;
	float atten_quadratic;
	bool enabled;
};

struct Spotlight {
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
	bool enabled;
};


in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
out vec4 FragColor;  

// uniform Material material;
// uniform Light light;

uniform Camera camera;
uniform DirectionalLight sun;
uniform PointLight bulb;
uniform Spotlight flashlight;

// uniform Material material;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

uniform bool rendering_light_source;


vec3 Perform_DirectionalLighting(in DirectionalLight f_light, 
															in vec3 f_normal,
															in vec2 f_texcoords,
															in vec3 f_fragpos, 
															in Camera f_camera) {
	//ambient
	vec3 ambient = f_light.ambient * vec3(texture(texture_diffuse1, f_texcoords));

	//diffuse
	vec3 norm = normalize(f_normal);
	vec3 lightDir = normalize(f_light.direction);
	float diff= max(dot(norm, lightDir), 0.0);
	vec3 diffuse = f_light.diffuse * diff * vec3(texture(texture_diffuse1, f_texcoords));

	//specular
	vec3 viewDir = normalize(f_camera.pos - f_fragpos);
	vec3 reflectionDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectionDir), 0.0), 32.0);
	vec3 specular = f_light.specular * (spec * vec3(texture(texture_specular1, f_texcoords)));
	//vec3 specular = f_light.specular * (spec * vec3(1.0, 1.0, 1.0));
	
  
	return (ambient + diffuse + specular);

}

vec3 Perform_PointLighting(in PointLight f_light, 
															in vec3 f_normal, 
															in vec2 f_texcoords,
															in vec3 f_fragpos, 
															in Camera f_camera) {



	if (!f_light.enabled) return vec3(0.0);
	//ambient
	vec3 ambient = f_light.ambient * vec3(texture(texture_diffuse1, f_texcoords));

	//diffuse
	vec3 norm = normalize(f_normal);
	vec3 lightDir = normalize(f_light.pos - f_fragpos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = f_light.diffuse * diff * vec3(texture(texture_diffuse1, f_texcoords));

	//specular
	vec3 viewDir = normalize(f_camera.pos - f_fragpos);
	vec3 reflectionDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectionDir), 0.0), 32);
	vec3 specular = f_light.specular * (spec * vec3(texture(texture_specular1, f_texcoords)));
	vec3 result;

		//attenuation
	float distance = length(f_light.pos - f_fragpos);
	float attenuation = 
		1.0 / (f_light.atten_constant + (f_light.atten_linear * distance) + (f_light.atten_quadratic * distance * distance));
	
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
  
	return (ambient + diffuse + specular);
	
}

vec3 Perform_Spotlighting(in Spotlight f_light, 
															in vec3 f_normal, 
															in vec2 f_texcoords,
															in vec3 f_fragpos, 
															in Camera f_camera){
											
 
	if (!f_light.enabled) return vec3(0.0);
	//ambient
	vec3 ambient = f_light.ambient * vec3(texture(texture_diffuse1, f_texcoords));
	//vec3 ambient = vec3(0.0) * vec3(texture(texture_diffuse1, f_texcoords));

	//diffuse
	vec3 norm = normalize(f_normal);
	vec3 lightDir = normalize(f_light.pos - f_fragpos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = f_light.diffuse * diff * vec3(texture(texture_diffuse1, f_texcoords));

	//specular
	vec3 viewDir = normalize(f_camera.pos - f_fragpos);
	vec3 reflectionDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectionDir), 0.0), 32.0);
	vec3 specular = f_light.specular * (spec * vec3(texture(texture_specular1, f_texcoords)));
	vec3 result;

		//attenuation
	float distance = length(f_light.pos - f_fragpos);
	float attenuation = 
		1.0 / (f_light.atten_constant + (f_light.atten_linear * distance) + (f_light.atten_quadratic * distance * distance));
	
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
  




  // spotlight
	float theta = dot(lightDir, normalize(-f_light.direction));
	float epsilon = f_light.cutOff - f_light.outerCutOff;
	float intensity = clamp((theta - f_light.outerCutOff) / epsilon, 0.0, 1.0);

	//ambient *= intensity; // leave commented out for more natural ambient
	diffuse *= intensity;
	specular *= intensity;

	return (ambient + diffuse + specular);

}



void main() {  

	if (rendering_light_source){
		FragColor = vec4(bulb.color, 1.0);
		return;
	}

	vec3 result_directional = Perform_DirectionalLighting(sun, Normal, TexCoords, FragPos, camera);
	vec3 result_point =  Perform_PointLighting(bulb, Normal, TexCoords, FragPos, camera);
	vec3 result_spot = Perform_Spotlighting(flashlight, Normal, TexCoords, FragPos, camera);
	vec3 result = result_directional + result_point + result_spot;
	FragColor = vec4(result, 1.0);
  //FragColor = vec4(1.0);

	// if (rendering_light_source) {
	// 	FragColor = vec4(vec3(1.0, 1.0, 1.0), 1.0);
	// 	return;	
	// }


	// //ambient
	// vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

	// //diffuse
	// vec3 norm = normalize(Normal);
	// vec3 lightDir = normalize(light.pos - FragPos);
	// float diff= max(dot(norm, lightDir), 0.0);
	// vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

	// //specular
	// vec3 viewDir = normalize(camera.pos - FragPos);
	// vec3 reflectionDir = reflect(-lightDir, norm);
	// float spec = pow(max(dot(viewDir, reflectionDir), 0.0), material.specular_power);
	// vec3 specular = light.specular * (spec * vec3(texture(material.specular, TexCoords)));
	
	// //attenuation
	// float distance = length(light.pos - FragPos);
	// float attenuation = 
	// 	1.0 / (light.atten_constant + (light.atten_linear * distance) + (light.atten_quadratic * distance * distance));
	
	
	
	// vec3 result;

	// ambient *= attenuation;
	// diffuse *= attenuation;
	// specular *= attenuation;


  
	
	// float theta = dot(lightDir, normalize(-light.direction));
	// float epsilon = light.cutOff - light.outerCutOff;
	// float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

	// diffuse *= intensity;
	// specular *= intensity;

	// if (theta > light.outerCutOff) {
	// 	result = (ambient + diffuse + specular);

	// } else {
	// 	result = ambient;
	// }




	// vec3 result_directional = Perform_DirectionalLighting(sun, Normal, material, TexCoords, FragPos, camera);
	// vec3 result_point = Perform_PointLighting(bulb, Normal, material, TexCoords, FragPos, camera);
	// vec3 result_spot = Perform_Spotlighting(flashlight, Normal, material, TexCoords, FragPos, camera);

	// vec3 result_ =  result_directional + result_point + result_spot;

	// FragColor = vec4(result_, 1.0);  
}	