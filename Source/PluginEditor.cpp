#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Constants.h"

GOLEQAudioProcessorEditor::GOLEQAudioProcessorEditor(GOLEQAudioProcessor& p)
  : AudioProcessorEditor(&p), audioProcessor(p)
{
  setSize(Constants::WindowWidth, Constants::WindowHeight);
  addAndMakeVisible(_lifeGridComponent);
  addAndMakeVisible(_controlPanelComponent);
  AttachListeners();

  _lifeGridComponent.setBounds(getLocalBounds());
}

GOLEQAudioProcessorEditor::~GOLEQAudioProcessorEditor()
{
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
  auto bounds = getLocalBounds();
  auto simHeight = bounds.getHeight() * 2 / 3;
  auto simBounds = bounds.removeFromTop(simHeight);
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