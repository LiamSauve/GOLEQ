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
  static constexpr auto CA_HighLife_Label = "HighLife";
  static constexpr auto CA_Seeds_Label = "Seeds";
  static constexpr auto CA_Maze_Label = "Maze";
  static constexpr auto CA_DataMosh_Label = "DataMosh";
  static constexpr auto CA_OrganicLife_Label = "OrganicLife";
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
}

enum CAVariant
{
  Conway = 1,
  HighLife,
  Seeds,
  Maze,
  DataMosh,
  OrganicLife,
  Testerino,
  NeonFlow
};

enum EffectType
{
  EQ = 1,
  Bitcrusher,
  Reverb
};
