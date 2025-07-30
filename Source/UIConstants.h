#pragma once

namespace UIConstants
{
  // Window Dimensions
  static constexpr int WindowWidth = 1280;
  static constexpr int WindowHeight = 1080;
  static constexpr int SimHeight = 720;

  // Control Panel: Button & Toggle Labels
  static constexpr auto PlayPauseLabel = "Play / Pause";
  static constexpr auto RandomizeLabel = "Randomize";
  static constexpr auto TempoToggleLabel = "Tempo / Time";
  static constexpr auto NextGenerationLabel = "Next Generation";

  // Cellular Automaton Variants
  static constexpr auto CA_Conway = "Conway's Life";
  static constexpr auto CA_HighLife = "HighLife";
  static constexpr auto CA_Seeds = "Seeds";

  // Audio Effect Labels
  static constexpr auto Effect_EQ = "EQ Sweep";
  static constexpr auto Effect_Bitcrusher = "Bitcrusher";
  static constexpr auto Effect_Reverb = "Reverb Warp";

  // Time Slider Configuration
  static constexpr double TimeSliderMin = 0.016;
  static constexpr double TimeSliderMax = 2.0;
  static constexpr double TimeSliderInterval = 0.001;

  // Simulation Size Sliders
  static constexpr int SimWidthMin = 128;
  static constexpr int SimWidthMax = 1280;
  static constexpr int SimHeightMin = 72;
  static constexpr int SimHeightMax = 720;
  static constexpr int SimSizeSliderInterval = 1;

  // Layout Dimensions & Styling
  static constexpr int ComponentPadding = 10;
  static constexpr int ControlHeight = 30;
  static constexpr int ControlMinWidth = 100;
  static constexpr int ButtonWidth = 120;
  static constexpr int DropdownWidth = 140;
  static constexpr int RotarySliderExtraHeight = 20;
  static const juce::Colour ControlPanelBackground{ 35, 35, 45 };
}