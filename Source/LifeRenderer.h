#pragma once

#include <JuceHeader.h>

class LifeRenderer : public juce::OpenGLAppComponent
{
public:
  LifeRenderer();
  ~LifeRenderer() override;

  // JUCE OpenGL lifecycle
  void initialise() override;
  void shutdown() override;
  void render() override;
  //void update() override {}

  // Upload life data to GPU
  void setLifeData(const std::vector<uint8_t>& data, int width, int height);

private:
  std::unique_ptr<juce::OpenGLShaderProgram> _shader;

  GLuint _lifeTexture = 0;
  GLuint _vao = 0;
  GLuint _vbo = 0;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LifeRenderer)
};