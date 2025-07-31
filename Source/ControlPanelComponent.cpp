#include "ControlPanelComponent.h"
#include "GOLEQ_Constants.h"

ControlPanelComponent::ControlPanelComponent()
{
  InitializeControls();
}

juce::TextButton& ControlPanelComponent::GetPlayPauseButton()
{
  return _playPauseButton;
}

juce::TextButton& ControlPanelComponent::GetRandomizeButton()
{
  return _randomizeButton;
}

juce::TextButton& ControlPanelComponent::GetNextGenerationButton()
{
  return _nextGenerationButton;
}

juce::ComboBox& ControlPanelComponent::GetCAVariantDropdown()
{
  return _caVariantDropdown;
}

juce::ComboBox& ControlPanelComponent::GetEffectTypeDropdown()
{
  return _effectDropdown;
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
  g.fillAll(Constants::ControlPanelBackground);
}

void ControlPanelComponent::resized()
{
  auto area = getLocalBounds().reduced(Constants::ComponentPadding);
  auto topArea = area.removeFromTop(area.getHeight() / 2);
  auto bottomArea = area;

  juce::FlexBox topRow, bottomRow;
  topRow.flexDirection = juce::FlexBox::Direction::row;
  bottomRow.flexDirection = juce::FlexBox::Direction::row;

  topRow.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
  bottomRow.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;

  const int knobHeight = Constants::ControlHeight + Constants::RotarySliderExtraHeight;

  topRow.items.add(juce::FlexItem(_tempoToggle).withMinHeight(Constants::ControlHeight).withMinWidth(Constants::ControlMinWidth));
  topRow.items.add(juce::FlexItem(_timeSlider).withMinHeight(knobHeight).withMinWidth(Constants::ControlMinWidth));
  topRow.items.add(juce::FlexItem(_caVariantDropdown).withMinHeight(Constants::ControlHeight).withMinWidth(Constants::DropdownWidth));
  topRow.items.add(juce::FlexItem(_effectDropdown).withMinHeight(Constants::ControlHeight).withMinWidth(Constants::DropdownWidth));
  topRow.items.add(juce::FlexItem(_playPauseButton).withMinHeight(Constants::ControlHeight).withMinWidth(Constants::ButtonWidth));

  bottomRow.items.add(juce::FlexItem(_randomizeButton).withMinHeight(Constants::ControlHeight).withMinWidth(Constants::ButtonWidth));
  bottomRow.items.add(juce::FlexItem(_nextGenerationButton).withMinHeight(Constants::ControlHeight).withMinWidth(Constants::ButtonWidth));
  bottomRow.items.add(juce::FlexItem(_widthSlider).withMinHeight(knobHeight).withMinWidth(Constants::ControlMinWidth));
  bottomRow.items.add(juce::FlexItem(_heightSlider).withMinHeight(knobHeight).withMinWidth(Constants::ControlMinWidth));

  topRow.performLayout(topArea);
  bottomRow.performLayout(bottomArea);
}

void ControlPanelComponent::InitializeControls()
{
  _tempoToggle.setButtonText(Constants::TempoToggleLabel);

  _timeSlider.setRange(Constants::TimeSliderMin, Constants::TimeSliderMax, Constants::TimeSliderInterval);
  _timeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  _timeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, Constants::TextBoxWidth, Constants::TextBoxHeight);

  //_widthSlider.setRange(UIConstants::SimWidthMin, UIConstants::SimWidthMax, UIConstants::SimSizeSliderInterval);
  //_widthSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  //_widthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, UIConstants::TextBoxWidth, UIConstants::TextBoxHeight);
  //_widthSlider.setValue(UIConstants::SimWidthMax, juce::NotificationType::dontSendNotification);
  //
  //_heightSlider.setRange(UIConstants::SimHeightMin, UIConstants::SimHeightMax, UIConstants::SimSizeSliderInterval);
  //_heightSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  //_heightSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, UIConstants::TextBoxWidth, UIConstants::TextBoxHeight);
  //_heightSlider.setValue(UIConstants::SimHeightMax, juce::NotificationType::dontSendNotification);

  _caVariantDropdown.addItem(Constants::CA_Conway_Label, Constants::CAVariant::Conway);
  _caVariantDropdown.addItem(Constants::CA_HighLife_Label, Constants::CAVariant::HighLife);
  _caVariantDropdown.addItem(Constants::CA_Seeds_Label, Constants::CAVariant::Seeds);
  _caVariantDropdown.setSelectedId(Constants::CAVariant::Conway);

  _effectDropdown.addItem(Constants::Effect_EQ_Label, Constants::EffectType::EQ);
  _effectDropdown.addItem(Constants::Effect_Bitcrusher_Label, Constants::EffectType::Bitcrusher);
  _effectDropdown.addItem(Constants::Effect_Reverb_Label, Constants::EffectType::Reverb);
  _effectDropdown.setSelectedId(Constants::EffectType::EQ);

  _playPauseButton.setButtonText(Constants::PlayPauseLabel);
  _randomizeButton.setButtonText(Constants::RandomizeLabel);
  _nextGenerationButton.setButtonText(Constants::NextGenerationLabel);

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