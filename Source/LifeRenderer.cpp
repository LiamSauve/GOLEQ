#include "LifeRenderer.h"
#include "Constants.h"
//#include <juce_opengl/juce_opengl.h>

using namespace juce::gl;

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
    out vec4 fragColor;
    uniform sampler2D lifeTexture;
    
    void main()
    {
      float cell = texture(lifeTexture, texCoord).r;
      fragColor = vec4(vec3(cell), 1.0);
    }
  )";

  _shader = std::make_unique<juce::OpenGLShaderProgram>(openGLContext);
  if (!_shader->addVertexShader(vertexShader) ||
      !_shader->addFragmentShader(fragmentShader) ||
      !_shader->link())
  {
    jassertfalse;
  }
  GLenum err = glGetError();
  jassert(err == GL_NO_ERROR);

  float quadVertices[] = {
    -1.0f, -1.0f,
     1.0f, -1.0f,
    -1.0f,  1.0f,
     1.0f,  1.0f,
  };
  glGenVertexArrays(1, &_quadVertexArray);
  glBindVertexArray(_quadVertexArray);
  glGenBuffers(1, &_quadVertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, _quadVertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);
}

void LifeRenderer::shutdown()
{
  _shader.reset();
  if (_lifeTexture != 0)
  {
    glDeleteTextures(1, &_lifeTexture);
  }

  if (_quadVertexBuffer != 0)
  {
    glDeleteBuffers(1, &_quadVertexBuffer);
  }

  if (_quadVertexArray != 0)
  {
    glDeleteVertexArrays(1, &_quadVertexArray);
  }
}

void LifeRenderer::render()
{
  jassert(juce::OpenGLHelpers::isContextActive());
  
  glClear(GL_COLOR_BUFFER_BIT);

  if (_shader == nullptr || _lifeTexture == 0)
  {
    return;
  }

  _shader->use();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _lifeTexture);
  _shader->setUniform("lifeTexture", 0);

  glBindVertexArray(_quadVertexArray);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
}

void LifeRenderer::SetLifeData(const std::vector<uint8_t>& data, int width, int height)
{
  jassert(juce::OpenGLHelpers::isContextActive());

  if (_lifeTexture == 0)
  {
    glGenTextures(1, &_lifeTexture);
    glBindTexture(GL_TEXTURE_2D, _lifeTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  }
  glBindTexture(GL_TEXTURE_2D, _lifeTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data.data());
}