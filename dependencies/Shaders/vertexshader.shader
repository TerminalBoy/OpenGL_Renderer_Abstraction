#version 330 core 

layout (location = 0) in vec3 aPos; 
layout (location = 1) in vec2 texCoord; 
layout (location = 2) in vec3 aColor;

out vec3 vertexColor;
out vec2 v_TexCoord;

void main() { 

gl_Position = vec4(aPos, 1.0); 
vertexColor = aColor;
v_TexCoord = texCoord; 
}