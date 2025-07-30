#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "LifeGridComponent.h"
#include "ControlPanelComponent.h"

class GOLEQAudioProcessorEditor :
  public juce::AudioProcessorEditor,
  public juce::Slider::Listener,
  public juce::Button::Listener,
  public juce::ComboBox::Listener
{
public:
  GOLEQAudioProcessorEditor(GOLEQAudioProcessor&);
  ~GOLEQAudioProcessorEditor() override;

  void ResizeGrid(int newWidth, int newHeight);
  void AttachListeners();

  void paint(juce::Graphics&) override;
  void resized() override;
  void sliderValueChanged(juce::Slider* slider) override;
  void buttonClicked(juce::Button* button) override;
  void comboBoxChanged(juce::ComboBox* comboBox) override;

private:
  GOLEQAudioProcessor& audioProcessor;

  LifeGridComponent _lifeGridComponent;
  ControlPanelComponent _controlPanelComponent;

  int _pendingGridWidth = 72;
  int _pendingGridHeight = 128;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GOLEQAudioProcessorEditor)
};
