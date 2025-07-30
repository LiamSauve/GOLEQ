/*
  ==============================================================================
    This file contains the basic framework code for a JUCE plugin editor.
  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "UIConstants.h"

//==============================================================================
GOLEQAudioProcessorEditor::GOLEQAudioProcessorEditor(GOLEQAudioProcessor& p)
  : AudioProcessorEditor(&p), audioProcessor(p)
{
  setSize(UIConstants::WindowWidth, UIConstants::WindowHeight);

  addAndMakeVisible(_lifeGridComponent);
  addAndMakeVisible(_controlPanelComponent);

  AttachListeners();

  _lifeGridComponent.setBounds(getLocalBounds());
}

GOLEQAudioProcessorEditor::~GOLEQAudioProcessorEditor()
{
}

//==============================================================================
void GOLEQAudioProcessorEditor::ResizeGrid(int newWidth, int newHeight)
{
  _lifeGridComponent.setGridSize(newWidth, newHeight);
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
    _lifeGridComponent.toggleplayPause();
  }
}

void GOLEQAudioProcessorEditor::AttachListeners()
{
  _controlPanelComponent.GetWidthSlider().addListener(this);
  _controlPanelComponent.GetHeightSlider().addListener(this);
  _controlPanelComponent.GetPlayPauseButton().addListener(this);
}