#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Constants.h"

GOLEQAudioProcessorEditor::GOLEQAudioProcessorEditor(GOLEQAudioProcessor& p)
  : AudioProcessorEditor(&p), audioProcessor(p)
{
  p.SetLifeInterface(_lifeGridComponent.GetILife());
  setSize(Constants::WindowWidth, Constants::WindowHeight);
  addAndMakeVisible(_lifeGridComponent);
  addAndMakeVisible(_controlPanelComponent);
  AttachListeners();

  _lifeGridComponent.setBounds(getLocalBounds());

  startTimerHz(60);
}

GOLEQAudioProcessorEditor::~GOLEQAudioProcessorEditor()
{
  stopTimer();
}

void GOLEQAudioProcessorEditor::ResizeGrid(int newWidth, int newHeight)
{
  _lifeGridComponent.SetGridSize(newWidth, newHeight);
}

void GOLEQAudioProcessorEditor::paint(juce::Graphics& g)
{
  g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void GOLEQAudioProcessorEditor::resized()
{
  auto bounds             = getLocalBounds();
  auto simHeight          = bounds.getHeight() * 2 / 3;
  auto simBounds          = bounds.removeFromTop(simHeight);
  auto controlPanelBounds = bounds;

  _lifeGridComponent.setBounds(simBounds);
  _controlPanelComponent.setBounds(controlPanelBounds);
}

void GOLEQAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
  if (slider == &_controlPanelComponent.GetWidthSlider())
  {
    _pendingGridWidth = static_cast<int>(slider->getValue());
    ResizeGrid(_pendingGridWidth, _pendingGridHeight);
    return;
  }

  if (slider == &_controlPanelComponent.GetHeightSlider())
  {
    _pendingGridHeight = static_cast<int>(slider->getValue());
    ResizeGrid(_pendingGridWidth, _pendingGridHeight);
    return;
  }
}

void GOLEQAudioProcessorEditor::buttonClicked(juce::Button* button)
{
  if (button == &_controlPanelComponent.GetPlayPauseButton())
  {
    _lifeGridComponent.TogglePlayPause();
    return;
  }

  if (button == &_controlPanelComponent.GetRandomizeButton())
  {
    _lifeGridComponent.Randomize();
    return;
  }

  if (button == &_controlPanelComponent.GetRandomizeMeaningfullyButton())
  {
    _lifeGridComponent.RandomizeMeaningfully();
    return;
  }
  
  if (button == &_controlPanelComponent.GetNextGenerationButton())
  {
    _lifeGridComponent.NextGeneration();
    return;
  }
}

void GOLEQAudioProcessorEditor::comboBoxChanged(juce::ComboBox* comboBox)
{
  int value = comboBox->getSelectedId();
  if (comboBox == &_controlPanelComponent.GetCAVariantDropdown())
  {
    _lifeGridComponent.SetCAVariant(static_cast<CAVariant>(value));
    return;
  }

  if (comboBox == &_controlPanelComponent.GetEffectTypeDropdown())
  {
    _lifeGridComponent.SetEffectType(static_cast<EffectType>(value));
    return;
  }
}

void GOLEQAudioProcessorEditor::AttachListeners()
{
  _controlPanelComponent.GetWidthSlider().addListener(this);
  _controlPanelComponent.GetHeightSlider().addListener(this);
  _controlPanelComponent.GetPlayPauseButton().addListener(this);
  _controlPanelComponent.GetRandomizeButton().addListener(this);
  _controlPanelComponent.GetRandomizeMeaningfullyButton().addListener(this);
  _controlPanelComponent.GetNextGenerationButton().addListener(this);
  _controlPanelComponent.GetCAVariantDropdown().addListener(this);
  _controlPanelComponent.GetEffectTypeDropdown().addListener(this);
}

void GOLEQAudioProcessorEditor::UpdateAudioBands()
{
  if (!_windowInit)
  {
    for (int i = 0; i < Constants::FFTSize; ++i)
    {
      _window[i] = 0.5f * (1.0f - std::cos(2.0f * juce::MathConstants<float>::pi * i / (Constants::FFTSize - 1)));
    }
    _windowInit = true;
  }

  std::array<float, Constants::FFTSize> mono {};
  int got = audioProcessor.ReadAudioForFFT(mono.data(), Constants::FFTSize);
  if (got < Constants::FFTSize)
  {
    return;
  }

  for (int i = 0; i < Constants::FFTSize; ++i)
  {
    _FFTData[i] = mono[i] * _window[i];
  }
  std::fill(_FFTData.begin() + Constants::FFTSize, _FFTData.end(), 0.0f);

  // magnitudes in-place
  _fft.performFrequencyOnlyForwardTransform(_FFTData.data());

  // bin helpers
  const float sampleRate = (float)audioProcessor.getSampleRate();
  const int nyquistBin = Constants::FFTSize / 2;
  auto hzToBin = [sampleRate, nyquistBin](float hz)
    {
      const int bin = static_cast<int>(std::lround(hz * Constants::FFTSize / sampleRate));
      return juce::jlimit(0, nyquistBin, bin);
    };

  // Band edges (low / mid / high)
  // todo: define these values in constants.h
  const int lowStartBin   = hzToBin(Constants::Bands::LowStartHz);
  const int lowEndBin     = hzToBin(Constants::Bands::LowEndHz);
  const int midStartBin   = std::min(lowEndBin + 1, nyquistBin);
  const int midEndBin     = hzToBin(Constants::Bands::MidEndHz);
  const int highStartBin  = std::min(midEndBin + 1, nyquistBin);
  const int highEndBin    = hzToBin(Constants::Bands::HighEndHz);

  auto bandAverage = [&](int start, int end)
    {
      start = juce::jlimit(0, nyquistBin, start);
      end = juce::jlimit(0, nyquistBin, end);
      if (end <= start)
      {
        return 0.0f;
      }
      double s = 0.0f;
      for (int i = start; i <= end; ++i)
      {
        s += _FFTData[i];
      }
      float v = (float)(s / double(end - start + 1));
      return juce::jlimit(0.0f, 1.0f, v * 3.0f);
    };

  const float rawB = bandAverage(lowStartBin, lowEndBin);
  const float rawM = bandAverage(midStartBin, midEndBin);
  const float rawH = bandAverage(highStartBin, highEndBin);

  _smoothBands.B = Constants::Smoothing::apply(rawB, _smoothBands.B);
  _smoothBands.M = Constants::Smoothing::apply(rawM, _smoothBands.M);
  _smoothBands.H = Constants::Smoothing::apply(rawH, _smoothBands.H);
}

void GOLEQAudioProcessorEditor::timerCallback()
{
  UpdateAudioBands();


  if ((++debugPrintEveryNFrames % 15) == 0)
  {
    const float uAudio = juce::jlimit(0.0f, 1.0f, audioProcessor._rmsAtomic.load());
    //DBG(juce::String::formatted("Bands  B=%.2f  M=%.2f  H=%.2f  |  Level=%.2f",
    //  _smoothBands.B, _smoothBands.M, _smoothBands.H, uAudio));
    audioProcessor.Log(juce::String::formatted("Bands  B=%.2f  M=%.2f  H=%.2f  |  Level=%.2f",
      _smoothBands.B, _smoothBands.M, _smoothBands.H, uAudio));
  }
}