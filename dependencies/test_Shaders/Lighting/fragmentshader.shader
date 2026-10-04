#version 330 core  

out vec4 FragColor;  

uniform vec3 u_Color;
uniform vec3 multiplier_light_color;

void main() {  
	FragColor = vec4(multiplier_light_color, 1.0) * vec4(u_Color, 1.0);  
}