#include "LifeRenderer.h"
#include "Constants.h"
#include <fstream>
#include <sstream>
//#include <juce_opengl/juce_opengl.h>

using namespace juce::gl;

LifeRenderer::LifeRenderer() :
  _renderWidth(0),
  _renderHeight(0),
  _isDataReady(false)
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
  uniform vec2 texelSize; // = 1.0 / texture resolution passed from C++

  void main()
  {
    float glow = 0.0;
    float radius = 3.0; // in texels

    // Sample a 7x7 neighborhood for smoother glow
    for (int x = -3; x <= 3; ++x)
    {
      for (int y = -3; y <= 3; ++y)
      {
        vec2 offset = vec2(float(x), float(y)) * texelSize;
        vec2 sampleUV = clamp(vTexCoord + offset, vec2(0.0), vec2(1.0));
        float neighborAlive = texture(lifeTexture, sampleUV).r;

        float dist = length(offset);
        float weight = neighborAlive / max(dist, 0.001);
        glow += weight;
      }
    }

    float threshold = 1.5;

    if (glow < threshold)
      discard;

    fragColor = vec4(mix(vec3(0.0, 0.2, 0.8), vec3(1.0), clamp(glow / (threshold * 3.0), 0.0, 1.0)), 1.0);
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
  // posX,  posY, u,    v
    -1.0f,  1.0f, 0.0f, 1.0f, // top-left
    -1.0f, -1.0f, 0.0f, 0.0f, // bottom-left
     1.0f, -1.0f, 1.0f, 0.0f, // bottom-right

    -1.0f,  1.0f, 0.0f, 1.0f, // top-left
     1.0f, -1.0f, 1.0f, 0.0f, // bottom-right
     1.0f,  1.0f, 1.0f, 1.0f  // top-right
  };

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
  const int w = getWidth();
  const int h = getHeight();

  glViewport(0, 0, w, h);
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);

  if (!_shader)
    return;

  {
    std::lock_guard<std::mutex> lock(_dataMutex);

    if (_isDataReady)
    {
      std::vector<uint8_t> pixelData = FlattenRenderDataToPixels();

      if (_lifeTexture == 0)
        glGenTextures(1, &_lifeTexture);

      glBindTexture(GL_TEXTURE_2D, _lifeTexture);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RG8,
        _renderWidth, _renderHeight,
        0, GL_RG, GL_UNSIGNED_BYTE,
        pixelData.data());

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

void LifeRenderer::SetLifeData(const std::vector<std::vector<Cell>>& data, int width, int height)
{
  std::lock_guard<std::mutex> lock(_dataMutex);

  _renderWidth = width;
  _renderHeight = height;

  _renderData.clear();
  _renderData.resize(height, std::vector<CellRenderData>(width));

  for (int y = 0; y < height; ++y)
  {
    for (int x = 0; x < width; ++x)
    {
      // Note the swapped index order here
      const Cell& src = data[x][y];

      CellRenderData dest;
      dest.alive = src.alive;
      dest.age = src.age;

      _renderData[y][x] = dest;
    }
  }

  _isDataReady = true;
}

//void LifeRenderer::SetLifeData(const std::vector<std::vector<Cell>>& data, int width, int height)
//{
//  std::lock_guard<std::mutex> lock(_dataMutex);
//
//  _renderWidth = width;
//  _renderHeight = height;
//
//  // Clear and prepare render buffer
//  _renderData.clear();
//  _renderData.resize(height, std::vector<CellRenderData>(width));
//
//  // Validate and transform each row
//  for (int y = 0; y < height; ++y)
//  {
//    if (y >= static_cast<int>(data.size()))
//    {
//      DBG("SetLifeData: WARNING - input has fewer rows than expected height (" +
//        juce::String(height) + ")");
//      break;
//    }
//
//    const auto& simRow = data[y];
//
//    if (static_cast<int>(simRow.size()) != width)
//    {
//      DBG("SetLifeData: WARNING - row " + juce::String(y) + " has " +
//        juce::String(simRow.size()) + " columns, expected " + juce::String(width));
//    }
//
//    for (int x = 0; x < width; ++x)
//    {
//      if (x >= static_cast<int>(simRow.size()))
//        continue;
//
//      const Cell& src = simRow[x];
//      CellRenderData dest;
//      dest.alive = src.alive;
//      dest.age = src.age;
//
//      _renderData[y][x] = dest;
//    }
//  }
//
//  _isDataReady = true;
//}

// testo
//void LifeRenderer::SetLifeData(const std::vector<std::vector<Cell>>& data, int width, int height)
//{
//  std::lock_guard<std::mutex> lock(_dataMutex);
//
//  _renderWidth = width;
//  _renderHeight = height;
//
//  _renderData.clear();
//  _renderData.resize(height, std::vector<CellRenderData>(width));
//
//  for (int y = 0; y < height; ++y)
//  {
//    for (int x = 0; x < width; ++x)
//    {
//      CellRenderData cell{};
//      cell.alive = false;
//      cell.age = 0;
//
//      // Diagonal line from top-left to bottom-right
//      if (x == y)
//      {
//        cell.alive = true;
//        cell.age = 255;
//      }
//
//      // Vertical center line
//      if (x == width / 2)
//      {
//        cell.alive = true;
//        cell.age = 150;
//      }
//
//      // Horizontal center line
//      if (y == height / 2)
//      {
//        cell.alive = true;
//        cell.age = 200;
//      }
//
//      // Outer border
//      if (x == 0 || x == width - 1 || y == 0 || y == height - 1)
//      {
//        cell.alive = true;
//        cell.age = 50;
//      }
//
//      _renderData[y][x] = cell;
//    }
//  }
//
//  _isDataReady = true;
//}

std::vector<uint8_t> LifeRenderer::FlattenRenderDataToPixels()
{
  std::vector<uint8_t> pixels;
  pixels.reserve(_renderWidth * _renderHeight * 2); // RG format

  for (int y = _renderHeight - 1; y >= 0; --y)  // FLIP VERTICALLY HERE
  {
    for (int x = 0; x < _renderWidth; ++x)
    {
      const CellRenderData& cell = _renderData[y][x];
      pixels.push_back(cell.alive ? 255 : 0); // R channel
      pixels.push_back(static_cast<uint8_t>(cell.age)); // G channel
    }
  }

  return pixels;
}
