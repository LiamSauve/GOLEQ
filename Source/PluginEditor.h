#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "LifeGridComponent.h"
#include "ControlPanelComponent.h"
#include "Constants.h"

struct Bands
{
  float B = 0;
  float M = 0;
  float H = 0;
};

class GOLEQAudioProcessorEditor :
  public juce::AudioProcessorEditor,
  private juce::Timer,
  public juce::Slider::Listener,
  public juce::Button::Listener,
  public juce::ComboBox::Listener
{
public:
  //static float smoothAR(float x, float y) { const float a = 0.35f, r = 0.08f; return y + (x - y) * ((x > y) ? a : r); }

  GOLEQAudioProcessorEditor(GOLEQAudioProcessor&);
  ~GOLEQAudioProcessorEditor() override;

  void ResizeGrid(int newWidth, int newHeight);
  void AttachListeners();
  void UpdateAudioBands();

  void paint(juce::Graphics&) override;
  void resized() override;
  void sliderValueChanged(juce::Slider* slider) override;
  void buttonClicked(juce::Button* button) override;
  void comboBoxChanged(juce::ComboBox* comboBox) override;
  

private:
  int debugPrintEveryNFrames = 0;
  void timerCallback() override;

  GOLEQAudioProcessor& audioProcessor;

  LifeGridComponent _lifeGridComponent;
  ControlPanelComponent _controlPanelComponent;

  int _pendingGridWidth = 72;
  int _pendingGridHeight = 128;

  juce::dsp::FFT _fft{ Constants::FFTOrder };
  std::array<float, 2 * Constants::FFTSize> _FFTData {};
  std::array<float, Constants::FFTSize> _window{};
  bool _windowInit = false;
  Bands smoothBands;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GOLEQAudioProcessorEditor)
};
