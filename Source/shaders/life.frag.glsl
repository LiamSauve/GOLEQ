#version 330 core
in vec2 texCoord;
out vec4 fragColor;
uniform sampler2D lifeTexture;

void main()
{
  float cell = texture(lifeTexture, texCoord).r;
  fragColor = vec4(vec3(cell), 1.0);
}
