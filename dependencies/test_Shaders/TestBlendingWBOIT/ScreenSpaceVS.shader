#version 330 core

layout (location = 0) in vec2 aScreenCoords;
layout (location = 1) in vec2 aSamplerCoords; // texture

out vec2 SamplerCoords;

void main(){
  gl_Position = vec4(aScreenCoords, 0.0, 1.0);
  SamplerCoords = aSamplerCoords;
}