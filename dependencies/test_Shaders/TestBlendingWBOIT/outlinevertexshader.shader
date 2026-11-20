#version 330 core 

layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aNormal;
//layout (location = 2) in vec2 aTexCoord;
 
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 AABBcenter;
uniform float scaleFactor;
//out vec3 Normal;
//out vec3 FragPos;
//out vec2 TexCoords;

void main() { 

  vec3 originVertex = aPos - AABBcenter;
  vec3 scaledVertex = originVertex * scaleFactor;
  gl_Position = projection * view * model * vec4((scaledVertex + AABBcenter), 1.0); 
  
  //FragPos = vec3(model * vec4(aPos, 1.0));
  //Normal = vec3(model * vec4(aNormal, 0.0)); // apply only the rotations and uniform scaling to the normal vector, then normalize it
  //TexCoords = aTexCoord;
}