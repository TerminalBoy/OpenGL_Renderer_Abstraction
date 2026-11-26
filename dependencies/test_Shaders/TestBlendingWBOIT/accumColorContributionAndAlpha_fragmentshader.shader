#version 330 core 

layout (location = 0) out vec4 accumColorContributionAndAlpha;

in vec2 TexCoords;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;

float weight(float z, float a){
  float result = 0;
  result = a * clamp(
    10 / (
        0.00001, + 
        pow(abs(z) / 5.0, 2.0) +
        pow(abs(z) / 200.0, 6.0)
      ),

    0.01,

    3000.0
      
  );

  return result;
}

void main(){
  
  vec4 color = texture(texture_diffuse1, TexCoords);
  float w = weight(FragPos.z, color.a);

  vec3 ContributionColor = vec3(
    color.r * color.a * w, 
    color.g * color.a * w, 
    color.b * color.a * w);
  
  accumColorContributionAndAlpha = vec4(ContributionColor, color.a * w); 
  //accumColorContributionAndAlpha = vec4(1.0, 1.0, 0.0, 1.0); 

}