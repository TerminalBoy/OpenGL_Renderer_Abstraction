#version 330 core 

layout (location = 0) out vec4 accumColorContributionAndAlpha;

in vec2 TexCoords;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;


void main(){
  vec4 color = texture(texture_diffuse1, TexCoords);
  vec3 ContributionColor = vec3(color.r * color.a, color.g * color.a, color.b * color.a);
  accumColorContributionAndAlpha = vec4(ContributionColor, color.a); 
  //accumColorContributionAndAlpha = vec4(1.0, 1.0, 0.0, 1.0); 

}