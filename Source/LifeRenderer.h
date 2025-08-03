#pragma once

#include <JuceHeader.h>
#include "Cell.h"
#include <mutex>

class LifeRenderer : public juce::OpenGLAppComponent
{
public:
  LifeRenderer();
  ~LifeRenderer() override;

  void initialise() override;
  void shutdown() override;
  void render() override;

  void SetLifeData(const std::vector<CellRenderData>& data, int width, int height);

private:
  std::unique_ptr<juce::OpenGLShaderProgram> _shader;
  GLuint _lifeTexture = 0;
  GLuint _quadVertexArray = 0;
  GLuint _quadVertexBuffer = 0;

  std::vector<CellRenderData> _pendingData;
  int _pendingWidth = 0;
  int _pendingHeight = 0;
  bool _isDataReady = false;
  std::mutex _dataMutex;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LifeRenderer)
};