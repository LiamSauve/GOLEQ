#include "ControlPanelComponent.h"
#include "UIConstants.h"

ControlPanelComponent::ControlPanelComponent()
{
  InitializeControls();
}

juce::TextButton& ControlPanelComponent::GetPlayPauseButton()
{
  return _playPauseButton;
}

juce::Slider& ControlPanelComponent::GetWidthSlider()
{
  return _widthSlider;
}

juce::Slider& ControlPanelComponent::GetHeightSlider()
{
  return _heightSlider;
}

void ControlPanelComponent::paint(juce::Graphics& g)
{
  g.fillAll(UIConstants::ControlPanelBackground);
}

void ControlPanelComponent::resized()
{
  auto area = getLocalBounds().reduced(UIConstants::ComponentPadding);
  auto topArea = area.removeFromTop(area.getHeight() / 2);
  auto bottomArea = area;

  juce::FlexBox topRow, bottomRow;
  topRow.flexDirection = juce::FlexBox::Direction::row;
  bottomRow.flexDirection = juce::FlexBox::Direction::row;

  topRow.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
  bottomRow.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;

  const int knobHeight = UIConstants::ControlHeight + UIConstants::RotarySliderExtraHeight;

  topRow.items.add(juce::FlexItem(_tempoToggle).withMinHeight(UIConstants::ControlHeight).withMinWidth(UIConstants::ControlMinWidth));
  topRow.items.add(juce::FlexItem(_timeSlider).withMinHeight(knobHeight).withMinWidth(UIConstants::ControlMinWidth));
  topRow.items.add(juce::FlexItem(_caVariantDropdown).withMinHeight(UIConstants::ControlHeight).withMinWidth(UIConstants::DropdownWidth));
  topRow.items.add(juce::FlexItem(_effectDropdown).withMinHeight(UIConstants::ControlHeight).withMinWidth(UIConstants::DropdownWidth));
  topRow.items.add(juce::FlexItem(_playPauseButton).withMinHeight(UIConstants::ControlHeight).withMinWidth(UIConstants::ButtonWidth));

  bottomRow.items.add(juce::FlexItem(_randomizeButton).withMinHeight(UIConstants::ControlHeight).withMinWidth(UIConstants::ButtonWidth));
  bottomRow.items.add(juce::FlexItem(_nextGenerationButton).withMinHeight(UIConstants::ControlHeight).withMinWidth(UIConstants::ButtonWidth));
  bottomRow.items.add(juce::FlexItem(_widthSlider).withMinHeight(knobHeight).withMinWidth(UIConstants::ControlMinWidth));
  bottomRow.items.add(juce::FlexItem(_heightSlider).withMinHeight(knobHeight).withMinWidth(UIConstants::ControlMinWidth));

  topRow.performLayout(topArea);
  bottomRow.performLayout(bottomArea);
}

void ControlPanelComponent::InitializeControls()
{
  _tempoToggle.setButtonText(UIConstants::TempoToggleLabel);

  _timeSlider.setRange(UIConstants::TimeSliderMin, UIConstants::TimeSliderMax, UIConstants::TimeSliderInterval);
  _timeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  _timeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, UIConstants::TextBoxWidth, UIConstants::TextBoxHeight);

  //_widthSlider.setRange(UIConstants::SimWidthMin, UIConstants::SimWidthMax, UIConstants::SimSizeSliderInterval);
  //_widthSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  //_widthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, UIConstants::TextBoxWidth, UIConstants::TextBoxHeight);
  //_widthSlider.setValue(UIConstants::SimWidthMax, juce::NotificationType::dontSendNotification);
  //
  //_heightSlider.setRange(UIConstants::SimHeightMin, UIConstants::SimHeightMax, UIConstants::SimSizeSliderInterval);
  //_heightSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  //_heightSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, UIConstants::TextBoxWidth, UIConstants::TextBoxHeight);
  //_heightSlider.setValue(UIConstants::SimHeightMax, juce::NotificationType::dontSendNotification);

  _caVariantDropdown.addItem(UIConstants::CA_Conway, 1);
  _caVariantDropdown.addItem(UIConstants::CA_HighLife, 2);
  _caVariantDropdown.addItem(UIConstants::CA_Seeds, 3);

  _effectDropdown.addItem(UIConstants::Effect_EQ, 1);
  _effectDropdown.addItem(UIConstants::Effect_Bitcrusher, 2);
  _effectDropdown.addItem(UIConstants::Effect_Reverb, 3);

  _playPauseButton.setButtonText(UIConstants::PlayPauseLabel);
  _randomizeButton.setButtonText(UIConstants::RandomizeLabel);
  _nextGenerationButton.setButtonText(UIConstants::NextGenerationLabel);

  addAndMakeVisible(_tempoToggle);
  addAndMakeVisible(_timeSlider);
  addAndMakeVisible(_caVariantDropdown);
  addAndMakeVisible(_effectDropdown);
  addAndMakeVisible(_playPauseButton);
  addAndMakeVisible(_randomizeButton);
  addAndMakeVisible(_nextGenerationButton);
  //addAndMakeVisible(_widthSlider);
  //addAndMakeVisible(_heightSlider);
}