#pragma once

#include <JuceHeader.h>

class ControlPanelComponent : public juce::Component
{
public:
  ControlPanelComponent();

  juce::TextButton& GetPlayPauseButton();
  juce::TextButton& GetRandomizeButton();
  juce::TextButton& GetNextGenerationButton();
  juce::ComboBox& GetCAVariantDropdown();
  juce::ComboBox& GetEffectTypeDropdown();
  juce::Slider& GetWidthSlider();
  juce::Slider& GetHeightSlider();

  void paint(juce::Graphics& g) override;
  void resized() override;

private:
  void InitializeControls();

  juce::ToggleButton _tempoToggle;
  juce::Slider _timeSlider;
  juce::ComboBox _caVariantDropdown;
  juce::ComboBox _effectDropdown;

  juce::TextButton _playPauseButton;
  juce::TextButton _randomizeButton;
  juce::TextButton _nextGenerationButton;

  juce::Slider _widthSlider;
  juce::Slider _heightSlider;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ControlPanelComponent)
};
