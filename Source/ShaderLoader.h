#pragma once

#include <juce_opengl/juce_opengl.h>

class ShaderLoader
{
public:
  static std::unique_ptr<juce::OpenGLShaderProgram> LoadShader(
    juce::OpenGLContext& context,
    const juce::String& vertPath,
    const juce::String& fragPath);

private:
  static const char* GetDefaultVertexShader();
  static const char* GetDefaultFragmentShader();
};
