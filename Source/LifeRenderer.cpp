#include "LifeRenderer.h"
#include "Constants.h"
#include <fstream>
#include <sstream>
//#include <juce_opengl/juce_opengl.h>

using namespace juce::gl;

LifeRenderer::LifeRenderer()
{
  setSize(Constants::WindowWidth, Constants::SimulationHeight);
}

LifeRenderer::~LifeRenderer()
{
  shutdownOpenGL();
}

void LifeRenderer::paint(juce::Graphics& g)
{
  //g.fillAll(juce::Colours::blue);
}

void LifeRenderer::initialise()
{
  const char* vertexShader = R"(
    #version 330 core
    layout(location = 0) in vec2 position;
    layout(location = 1) in vec2 texCoord;

    out vec2 vTexCoord;

    void main()
    {
      gl_Position = vec4(position, 0.0, 1.0);
      vTexCoord = texCoord;
    }
  )";

  const char* fragmentShader = R"(
    #version 330 core
    in vec2 vTexCoord;
    out vec4 fragColor;

    uniform sampler2D lifeTexture;

    void main()
    {
      vec2 texCoord = vTexCoord;
      vec4 data = texture(lifeTexture, texCoord);

      float alive = data.r; // 0.0 or 1.0
      float age = data.g;   // 0.0 -> 1.0

      if (alive < 0.5)
        discard;

      fragColor = mix(vec4(0.0, 0.2, 0.8, 1.0), vec4(1.0, 1.0, 1.0, 1.0), age);
}
  )";

  _shader = std::make_unique<juce::OpenGLShaderProgram>(openGLContext);
  if (!_shader->addVertexShader(juce::CharPointer_UTF8(vertexShader)) ||
      !_shader->addFragmentShader(juce::CharPointer_UTF8(fragmentShader)) ||
      !_shader->link())
  {
    DBG("Shader error: " + _shader->getLastError());
    jassertfalse;
  }

  float quadVertices[] = {
    // positions   // uvs
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
  };;

  // Setup full-screen quad
  glGenVertexArrays(1, &_quadVertexArray);
  glBindVertexArray(_quadVertexArray);

  glGenBuffers(1, &_quadVertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, _quadVertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

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
  auto w = getWidth();
  auto h = getHeight();
  glViewport(0, 0, w, h);
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);

  if (!_shader)
    return;

  {
    std::lock_guard<std::mutex> lock(_dataMutex);
    if (_isDataReady)
    {
      std::vector<uint8_t> pixelData(_pendingWidth * _pendingHeight * 2);

      for (size_t i = 0; i < _pendingData.size(); ++i)
      {
        pixelData[i * 2 + 0] = _pendingData[i].alive ? 255 : 0;
        pixelData[i * 2 + 1] = _pendingData[i].age;
      }

      if (_lifeTexture == 0)
        glGenTextures(1, &_lifeTexture);

      glBindTexture(GL_TEXTURE_2D, _lifeTexture);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RG8, _pendingWidth, _pendingHeight, 0, GL_RG, GL_UNSIGNED_BYTE, pixelData.data());

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

      _isDataReady = false;
    }
  }

  _shader->use();

  if (_lifeTexture != 0)
  {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _lifeTexture);

    _shader->setUniform("lifeTexture", 0);
  }

  glBindVertexArray(_quadVertexArray);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}


void LifeRenderer::SetLifeData(const std::vector<CellRenderData>& data, int width, int height)
{
  std::lock_guard<std::mutex> lock(_dataMutex);
  _pendingData = data;
  _pendingWidth = width;
  _pendingHeight = height;
  _isDataReady = true;
}
