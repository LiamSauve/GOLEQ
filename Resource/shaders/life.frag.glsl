//#version 330 core
//in vec2 texCoord;
//out vec4 fragColor;
//uniform sampler2D lifeTexture;
//
//void main()
//{
//  float cell = texture(lifeTexture, texCoord).r;
//  fragColor = vec4(vec3(cell), 1.0);
//}

#version 330 core
  in vec2 texCoord;
  out vec4 fragColor;

  uniform sampler2D lifeTexture;

  void main()
  {
    vec4 data = texture(lifeTexture, texCoord);
    float alive = data.r;
    float age = data.g;

    // Offset the UVs based on age
    vec2 offset = vec2(sin(age * 20.0), cos(age * 20.0)) * 0.01;
    vec4 warped = texture(lifeTexture, texCoord + offset);

    fragColor = vec4(age, warped.r, warped.g, 1.0);
  }