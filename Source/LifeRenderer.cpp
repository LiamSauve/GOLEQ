#include "LifeRenderer.h"
#include "Constants.h"

// #include <gl/GL.h>

LifeRenderer::LifeRenderer()
{
  setSize(Constants::WindowWidth, Constants::WindowHeight);
}

LifeRenderer::~LifeRenderer()
{
  shutdownOpenGL();
}

void LifeRenderer::initialise()
{
  const char* vertexShader = R"(
    #version 330 core
    layout(location = 0) in vec2 position;
    out vec2 texCoord;
    
    void main()
    {
      texCoord = (position + 1.0) * 0.5;
      gl_Position = vec4(position, 0.0, 1.0);
    }
  )";

  const char* fragmentShader = R"(
    #version 330 core
    in vec2 texCoord;
    out vec4FragColor;
    uniform sampler2D lifeTexture;
    
    void main()
    {
      float cell = texture(lifeTexture, texCoord).r;
      fragColor = vec4(vec3(cell), 1.0); // wh
    }
  )";

  _shader = std::make_unique<juce::OpenGLShaderProgram>(openGLContext);
  if (!_shader->addVertexShader(vertexShader) ||
      !_shader->addFragmentShader(fragmentShader) ||
      !_shader->link())
  {
    jassertfalse;
  }

  float quadVertices[] = {
    -1.0f, -1.0f,
     1.0f, -1.0f,
    -1.0f,  1.0f,
     1.0f,  1.0f,
  };

  //glGenVertexArrays(1, &vao);
  //glGenBuffers(1, &vbo);
  //
  //glBindVertexArray(vao);
  //glBindBuffer(GL_ARRAY_BUFFER, vbo);
  //glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
  //glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  //glEnableVertexAttribArray(0);
}

void LifeRenderer::shutdown()
{
  _shader.reset();
  if (_lifeTexture != 0)
  {
    //glDeleteTextures(1, &_lifeTexture);
  }

  if (_vbo != 0)
  {
    //glDeleteBuffers(1, &_vbo);
  }

  if (_vao != 0)
  {
    //glDeleteVertexArrays(1, &_vao);
  }
}

void LifeRenderer::render()
{

}

void LifeRenderer::setLifeData(const std::vector<uint8_t>& data, int width, int height)
{
  jassert(juce::OpenGLHelpers::isContextActive());
  // glClear(GL_COLOR_BUFFER_BIT);

  if (_shader == nullptr || _lifeTexture == 0)
  {
    return;
  }
  _shader->use();

  //glActiveTexture(GL_TEXTURE0);
  //glBindTexture(GL_TEXTURE_2D, lifeTexture);
  _shader->setUniform("_lifeTexture", 0);

  //glBindVertexArray(_vao);
  //glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

}