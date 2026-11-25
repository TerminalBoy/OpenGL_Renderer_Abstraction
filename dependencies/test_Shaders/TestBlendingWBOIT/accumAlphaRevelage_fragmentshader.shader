#version 330 core 

layout (location = 1) out vec4 accumAlphaRevelage;

in vec2 TexCoords;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;

void main(){
  float alpha = texture(texture_diffuse1, TexCoords).a;
  accumAlphaRevelage = vec4(0.0, 0.0, 0.0, alpha);
}