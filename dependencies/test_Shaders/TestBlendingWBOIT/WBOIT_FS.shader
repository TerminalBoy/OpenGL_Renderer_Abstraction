#version 330 core 

out vec4 PixelColor;

in vec2 SamplerCoords;

uniform sampler2D accumColorContributionAndAlpha;
uniform sampler2D accumAlphaRevelage;

const float EPSILON = 1e-5;

void main(){
  

  vec3 accumColor = texture(accumColorContributionAndAlpha, SamplerCoords).rgb;
  float accumAlpha = texture(accumColorContributionAndAlpha, SamplerCoords).a;
  float finalRevelage = texture(accumAlphaRevelage, SamplerCoords).a;

  vec3 NormalizedColor = accumColor / max(accumAlpha, EPSILON);
  vec4 finalPixelColor = vec4(NormalizedColor, 1 - finalRevelage);

  PixelColor = finalPixelColor;
  //PixelColor = texture(accumColorContributionAndAlpha, SamplerCoords);
  // PixelColor = vec4(1.0, 0.0, 0.0, 1.0);
}