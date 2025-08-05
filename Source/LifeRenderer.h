#pragma once

#include <JuceHeader.h>
#include "Cell.h"
#include <mutex>

struct CellRenderData
{
  bool alive;
  uint8_t age;
};

class LifeRenderer : public juce::OpenGLAppComponent
{
public:
  LifeRenderer();
  ~LifeRenderer() override;

  void paint(juce::Graphics& g);

  void initialise() override;
  void initialise_LoadShader();
  void initialise_SetupQuadVertexData();
  void initialise_ConfigureVertexAttributes();

  void shutdown() override;

  void render() override;
  void render_SetupViewportAndClear();
  void render_UpdateTextureIfReady();
  void render_UseShaderAndBindTexture();
  void render_DrawQuad();

  void SetLifeData(const std::vector<std::vector<Cell>>& data, int width, int height);
  std::vector<uint8_t> FlattenRenderDataToPixels();

private:
  std::unique_ptr<juce::OpenGLShaderProgram> _shader;
  GLuint _lifeTexture = 0;
  GLuint _quadVertexArray = 0;
  GLuint _quadVertexBuffer = 0;

  std::mutex _dataMutex;
  std::vector < std::vector<CellRenderData>> _renderData;
  int _renderWidth;
  int _renderHeight;
  bool _isDataReady;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LifeRenderer)
};