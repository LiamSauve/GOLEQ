#include "LifeRenderer.h"
#include "Constants.h"
#include "ShaderLoader.h"
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
  initialise_LoadShader();
  initialise_SetupQuadVertexData();
  initialise_ConfigureVertexAttributes();
}

void LifeRenderer::initialise_LoadShader()
{
  _shader = ShaderLoader::LoadShader(
    openGLContext,
    Constants::VertShaderLocation,
    Constants::FragShaderLocation);
}

void LifeRenderer::initialise_SetupQuadVertexData()
{
  float quadVertices[] = {
      -1.0f,  1.0f, 0.0f, 1.0f,
      -1.0f, -1.0f, 0.0f, 0.0f,
       1.0f, -1.0f, 1.0f, 0.0f,

      -1.0f,  1.0f, 0.0f, 1.0f,
       1.0f, -1.0f, 1.0f, 0.0f,
       1.0f,  1.0f, 1.0f, 1.0f
  };

  glGenVertexArrays(1, &_quadVertexArray);
  glBindVertexArray(_quadVertexArray);

  glGenBuffers(1, &_quadVertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, _quadVertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
}

void LifeRenderer::initialise_ConfigureVertexAttributes()
{
  // Attribute 0: position (vec2)
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  // Attribute 1: texture coordinates (vec2)
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // Unbind VAO to prevent accidental modification
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
  render_SetupViewportAndClear();
  if (!_shader) return;

  render_UpdateTextureIfReady();
  render_UseShaderAndBindTexture();
  render_DrawQuad();

}

void LifeRenderer::render_SetupViewportAndClear()
{
  const int w = getWidth();
  const int h = getHeight();

  glViewport(0, 0, w, h);
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);
}

void LifeRenderer::render_UpdateTextureIfReady()
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

void LifeRenderer::render_UseShaderAndBindTexture()
{
  _shader->use();

  if (_lifeTexture != 0)
  {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _lifeTexture);
    _shader->setUniform("lifeTexture", 0);
  }
}

void LifeRenderer::render_DrawQuad()
{
  glBindVertexArray(_quadVertexArray);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}

void LifeRenderer::SetLifeData(const std::vector<std::vector<Cell>>& data, int width, int height)
{
  std::lock_guard<std::mutex> lock(_dataMutex);

  _renderWidth  = width;
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
      dest.alive        = src.alive;
      dest.age          = src.age;
      _renderData[y][x] = dest;
    }
  }

  _isDataReady = true;
}

// render test 
// comment out the above setLifeData to check bounds and rendering direction
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

  // Preallocate memory to avoid reallocations during pixel push_back.
  // Each cell contributes 2 bytes: one for 'alive' status (R), one for 'age' (G).
  pixels.reserve(_renderWidth * _renderHeight * 2); // RG format

  // Iterate from bottom to top to vertically flip the image.
  // This is necessary because OpenGL's origin is bottom-left,
  // while many image formats expect top-left origin.
  for (int y = _renderHeight - 1; y >= 0; --y)
  {
    for (int x = 0; x < _renderWidth; ++x)
    {
      const CellRenderData& cell = _renderData[y][x];

      // Encode 'alive' status into the red channel.
      // We use 255 for alive and 0 for dead to maximize contrast.
      pixels.push_back(cell.alive ? 255 : 0);

      // Encode 'age' into the green channel.
      // This allows us to visualize aging cells with varying intensity.
      pixels.push_back(static_cast<uint8_t>(cell.age));
    }
  }

  return pixels;
}
