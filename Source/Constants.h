#pragma once
#include <JuceHeader.h>

namespace Constants
{
  // Window Dimensions
  static constexpr int WindowWidth = 1280;
  static constexpr int WindowHeight = 1080;
  static constexpr int SimulationHeight = 720;

  static constexpr int MinAge = 0;
  static constexpr int MaxAge = 20;
  static constexpr auto LifeVertShaderPath = "shaders/life.vert.glsl";
  static constexpr auto LifeFragShaderPath = "shaders/life.frag.glsl";

  // Control Panel: Button & Toggle Labels
  static constexpr auto PlayPauseLabel = "Play / Pause";
  static constexpr auto RandomizeLabel = "Randomize";
  static constexpr auto RandomizeMeaningfullyLabel = "~(*)~";
  static constexpr auto TempoToggleLabel = "Tempo / Time";
  static constexpr auto NextGenerationLabel = "Next Generation";

  // Control Panel: Cellular Automaton Variants
  static constexpr auto CA_Conway_Label = "Conway's Life";
  static constexpr auto CA_Seeds_Label = "Seeds";
  static constexpr auto CA_Maze_Label = "Maze";
  static constexpr auto CA_Testerino = "Testerino";
  static constexpr auto CA_NeonFlow = "Neon Flow";

  // Control Panel: Audio Effect Labels
  static constexpr auto Effect_EQ_Label = "EQ Sweep";
  static constexpr auto Effect_Bitcrusher_Label = "Bitcrusher";
  static constexpr auto Effect_Reverb_Label = "Reverb Warp";

  // Control Panel: Time Slider Configuration
  static constexpr double TimeSliderMin = 0.016;
  static constexpr double TimeSliderMax = 2.0;
  static constexpr double TimeSliderInterval = 0.001;

  // Control Panel: Simulation Size Sliders
  static constexpr int SimWidthMin            = 32;
  static constexpr int SimWidthMax            = 640;
  static constexpr int SimHeightMin           = 18;
  static constexpr int SimHeightMax           = 360;
  static constexpr int SimSizeSliderInterval  = 1;

  // Control Panel: Layout Dimensions & Styling
  static constexpr int CellPaintSize            = 10;
  static constexpr int ComponentPadding         = 10;
  static constexpr int ControlHeight            = 30;
  static constexpr int ControlMinWidth          = 100;
  static constexpr int ButtonWidth              = 120;
  static constexpr int DropdownWidth            = 140;
  static constexpr int RotarySliderExtraHeight  = 20;
  static constexpr int TextBoxWidth             = 60;
  static constexpr int TextBoxHeight            = 20;
  static const juce::Colour ControlPanelBackground{ 35, 35, 45 };

  static const juce::String ShaderPathPrefix          = "../../Resource/shaders/";
  static const juce::String DefaultFragShaderLocation = ShaderPathPrefix + "life.frag.glsl";
  static const juce::String DefaultVertShaderLocation = ShaderPathPrefix + "life.vert.glsl";
  static const juce::String FunkyFragShaderLocation   = ShaderPathPrefix + "funkylife.frag.glsl";
  static const juce::String FunkyVertShaderLocation   = ShaderPathPrefix + "funkylife.vert.glsl";
  static const juce::String NeonFragShaderLocation    = ShaderPathPrefix + "neonlife.frag.glsl";
  static const juce::String NeonVertShaderLocation    = ShaderPathPrefix + "neonlife.vert.glsl";

  namespace Bands
  {
    inline constexpr float LowStartHz = 20.0f;
    inline constexpr float LowEndHz = 140.0f;
    inline constexpr float MidEndHz = 2000.0f;
    inline constexpr float HighEndHz = 8000.0f;
  }

  // FFT config (shared across Processor and Editor)
  inline constexpr int FFTOrder                    = 11;           // 2048-point FFT
  inline constexpr int FFTSize                     = 1 << FFTOrder;
  inline constexpr int AnalysisFifoCapacitySamples = FFTSize * 8; // 8 windows

  namespace Smoothing
  {
    inline constexpr float Attack = 0.35f;
    inline constexpr float Release = 0.08f;

    inline float apply(float x, float y)
    {
      const float a = (x > y) ? Attack : Release;
      return y + (x - y) * a;
    }
  }

  namespace GLDebug
  {
    // NVIDIA-only, notification-level message IDs that tend to be noisy.
    // Meanings can vary by driver version; treat these as "mute this chatter".
    namespace NV
    {
      using Id = std::uint32_t; // matches GLuint width without pulling in GL headers

      inline constexpr Id BUFFER_DETAILED_INFO = 131185; // e.g. "Buffer detailed info… will use VIDEO memory"
      inline constexpr Id OTHER_INFO_131204 = 131204; // generic info/other spam
      inline constexpr Id SHADER_PROGRAM_INFO = 131218; // shader/program state/info noise
      inline constexpr Id OTHER_INFO_131222 = 131222; // generic info/other spam
    }

    inline constexpr std::array<std::uint32_t, 4> NV_NOISY_NOTIFICATION_IDS = {
        NV::BUFFER_DETAILED_INFO,
        NV::OTHER_INFO_131204,
        NV::SHADER_PROGRAM_INFO,
        NV::OTHER_INFO_131222
    };
  }
}

enum CAVariant
{
  Conway = 1,
  Seeds,
  Maze,
  Testerino,
  NeonFlow
};

enum EffectType
{
  EQ = 1,
  Bitcrusher,
  Reverb
};
