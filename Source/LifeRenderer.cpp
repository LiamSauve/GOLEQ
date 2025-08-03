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

    void main()
    {
      gl_Position = vec4(position, 0.0, 1.0);
    }
  )";

  const char* fragmentShader = R"(
    #version 330 core
    out vec4 fragColor;

    void main()
    {
      fragColor = vec4(1.0, 0.0, 0.0, 1.0); // Red
    }
  )";

  _shader = std::make_unique<juce::OpenGLShaderProgram>(openGLContext);
  if (!_shader->addVertexShader(vertexShader) ||
    !_shader->addFragmentShader(fragmentShader) ||
    !_shader->link())
  {
    DBG("Shader compile/link failed: " + _shader->getLastError());
    jassertfalse;
  }

  float triangleVertices[] = {
    // position
     0.0f,  0.5f,   // top
    -0.5f, -0.5f,   // bottom left
     0.5f, -0.5f    // bottom right
  };

  glGenVertexArrays(1, &_quadVertexArray);
  glBindVertexArray(_quadVertexArray);

  glGenBuffers(1, &_quadVertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, _quadVertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
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

  auto w = getWidth();
  auto h = getHeight();
  glViewport(0, 0, w, h);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  if (_shader == nullptr)
    return;

  _shader->use();

  glBindVertexArray(_quadVertexArray);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  glBindVertexArray(0);
}

void LifeRenderer::SetLifeData(const std::vector<CellRenderData>& data, int width, int height)
{
  std::lock_guard<std::mutex> lock(_dataMutex);

  _pendingWidth = width;
  _pendingHeight = height;

  _pendingData.resize(width * height);

  for (int y = 0; y < height; ++y)
  {
    for (int x = 0; x < width; ++x)
    {
      bool isAlive = ((x / 8) + (y / 8)) % 2 == 0; // checkerboard
      uint8_t age = isAlive ? 255 : 0;

      _pendingData[y * width + x] = { isAlive, age };
    }
  }

  _isDataReady = true;
}
