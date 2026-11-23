#version 330 core  

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D screentexture;

// post processing flags

uniform int PostProcessMode;

      // None = 0, 
      // Inversion = 1, 
      // Grayscale = 2, 
      // Kernel = 3, 
      // Blur = 4, 
      // EdgeDetection = 5



void main(){
  switch (PostProcessMode){
    case 0: // None
    
    FragColor = texture(screentexture, TexCoords);

    break;


    case 1: // Inversion

    FragColor = vec4(vec3(1.0 - texture(screentexture, TexCoords)), 1.0);

    break;


    case 2:
    
    FragColor = texture(screentexture, TexCoords);
    float average = (FragColor.r + FragColor.g + FragColor.b) / 3.0;
    FragColor = vec4(average, average, average, 1.0);
    
    break;


    case 3:
    {
    const float offset = 1.0 / 300.0;

    vec2 offsets[9] = vec2[](
      
      vec2(-offset, offset), // top-left
      vec2( 0.0f, offset), // top-center
      vec2( offset, offset), // top-right
      vec2(-offset, 0.0f), // center-left
      vec2( 0.0f, 0.0f), // center-center
      vec2( offset, 0.0f), // center-right
      vec2(-offset, -offset), // bottom-left
      vec2( 0.0f, -offset), // bottom-center
      vec2( offset, -offset) // bottom-right
    );


    float kernel[9] = float[](
      -1, -1, -1,
      -1, 9, -1,
      -1, -1, -1
    );
    
    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
      sampleTex[i] = vec3(texture(screentexture, TexCoords.st +
      offsets[i]));
    }
    vec3 col = vec3(0.0);
    
    for(int i = 0; i < 9; i++)
      col += sampleTex[i] * kernel[i];
    
    FragColor = vec4(col, 1.0);

    }
    break;


    case 4:

    {
    const float offset = 1.0 / 300.0;

    vec2 offsets[9] = vec2[](
      
      vec2(-offset, offset), // top-left
      vec2( 0.0f, offset), // top-center
      vec2( offset, offset), // top-right
      vec2(-offset, 0.0f), // center-left
      vec2( 0.0f, 0.0f), // center-center
      vec2( offset, 0.0f), // center-right
      vec2(-offset, -offset), // bottom-left
      vec2( 0.0f, -offset), // bottom-center
      vec2( offset, -offset) // bottom-right
    );


    float kernel[9] = float[](
      1.0 / 16, 2.0 / 16, 1.0 / 16,
      2.0 / 16, 4.0 / 16, 2.0 / 16,
      1.0 / 16, 2.0 / 16, 1.0 / 16
    );
    
    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
      sampleTex[i] = vec3(texture(screentexture, TexCoords.st +
      offsets[i]));
    }
    vec3 col = vec3(0.0);
    
    for(int i = 0; i < 9; i++)
      col += sampleTex[i] * kernel[i];
    
    FragColor = vec4(col, 1.0);

    }


    break;


    case 5:


    {
    const float offset = 1.0 / 300.0;

    vec2 offsets[9] = vec2[](
      
      vec2(-offset, offset), // top-left
      vec2( 0.0f, offset), // top-center
      vec2( offset, offset), // top-right
      vec2(-offset, 0.0f), // center-left
      vec2( 0.0f, 0.0f), // center-center
      vec2( offset, 0.0f), // center-right
      vec2(-offset, -offset), // bottom-left
      vec2( 0.0f, -offset), // bottom-center
      vec2( offset, -offset) // bottom-right
    );


    float kernel[9] = float[](
      1, 1, 1,
      1, -8, 1,
      1, 1, 1
    );
    
    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
      sampleTex[i] = vec3(texture(screentexture, TexCoords.st +
      offsets[i]));
    }
    vec3 col = vec3(0.0);
    
    for(int i = 0; i < 9; i++)
      col += sampleTex[i] * kernel[i];
    
    FragColor = vec4(col, 1.0);

    }

    break;

    default:

    FragColor = texture(screentexture, TexCoords);
    FragColor = vec4(FragColor.r * 0.2, FragColor.g * 0.2, FragColor.b * 0.2, 1.0);
    
    break;
  }



  //FragColor = texture(screentexture, TexCoords);
}