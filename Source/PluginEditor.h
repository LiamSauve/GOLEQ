#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "LifeGridComponent.h"
#include "ControlPanelComponent.h"

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

  // Configuration // this is duplicate code, fix it
  static constexpr int kFFTOrder                    = 11;             // 2048-point FFT
  static constexpr int kFFTSize                     = 1 << kFFTOrder;
  static constexpr int kAnalysisFifoCapacitySamples = kFFTSize * 8;   // 8 windows of headroom
  static float smoothAR(float x, float y) { const float a = 0.35f, r = 0.08f; return y + (x - y) * ((x > y) ? a : r); }

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

  juce::dsp::FFT _fft{ kFFTOrder };
  std::array<float, 2 * kFFTSize> _FFTData {};
  std::array<float, kFFTSize> _window{};
  bool _windowInit = false;
  Bands bandsSm;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GOLEQAudioProcessorEditor)
};
