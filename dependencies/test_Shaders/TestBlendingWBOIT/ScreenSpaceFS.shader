#version 330 core

in vec2 SamplerCoords;
layout (location = 0) out vec4 FragColor;

uniform sampler2D sourcetexture;

void main(){
  FragColor = texture(sourcetexture, SamplerCoords);
}