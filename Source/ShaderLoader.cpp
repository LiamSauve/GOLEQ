#include "ShaderLoader.h"
std::unique_ptr<juce::OpenGLShaderProgram> ShaderLoader::LoadShader(
  juce::OpenGLContext& context,
  const juce::String& vertPath,
  const juce::String& fragPath)
{
  juce::String vertexCode;
  juce::String fragmentCode;

  juce::File vertexFile = juce::File::getCurrentWorkingDirectory().getChildFile(vertPath);
  juce::File fragmentFile = juce::File::getCurrentWorkingDirectory().getChildFile(fragPath);

  if (vertexFile.existsAsFile())
  {
    vertexCode = vertexFile.loadFileAsString();
  }
  else
  {
    DBG("Vertex shader not found: " + vertexFile.getFullPathName());
    vertexCode = juce::String(juce::CharPointer_UTF8(GetDefaultVertexShader()));
  }

  if (fragmentFile.existsAsFile())
  {
    fragmentCode = fragmentFile.loadFileAsString();
  }
  else
  {
    DBG("Fragment shader not found: " + fragmentFile.getFullPathName());
    fragmentCode = juce::String(juce::CharPointer_UTF8(GetDefaultFragmentShader()));
  }

  auto shader = std::make_unique<juce::OpenGLShaderProgram>(context);

  if (!shader->addVertexShader(vertexCode) ||
      !shader->addFragmentShader(fragmentCode) ||
      !shader->link())
  {
    DBG("Shader error: " + shader->getLastError());
    return std::make_unique<juce::OpenGLShaderProgram>(context); // empty fallback
  }

  return shader;
}

const char* ShaderLoader::GetDefaultVertexShader()
{
  return R"(
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
}


const char* ShaderLoader::GetDefaultFragmentShader()
{
  return R"(
    #version 330 core
    in vec2 vTexCoord;
    out vec4 fragColor;
  
    uniform sampler2D lifeTexture;
  
    void main()
    {
      // Use vTexCoord directly — no vertical flip
      vec4 data = texture(lifeTexture, vTexCoord);
  
      float alive = data.r; // 0.0 or 1.0
      float age = data.g;   // 0.0 -> 1.0 (assuming normalized)
  
      if (alive < 0.5)
      {
        discard;
      }
  
      fragColor = mix(vec4(0.0, 0.2, 0.8, 1.0), vec4(1.0, 1.0, 1.0, 1.0), age);
    }

  )";
}
