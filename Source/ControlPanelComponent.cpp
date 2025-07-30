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
  g.fillAll(GOLEQ_Constants::ControlPanelBackground);
}

void ControlPanelComponent::resized()
{
  auto area = getLocalBounds().reduced(GOLEQ_Constants::ComponentPadding);
  auto topArea = area.removeFromTop(area.getHeight() / 2);
  auto bottomArea = area;

  juce::FlexBox topRow, bottomRow;
  topRow.flexDirection = juce::FlexBox::Direction::row;
  bottomRow.flexDirection = juce::FlexBox::Direction::row;

  topRow.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
  bottomRow.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;

  const int knobHeight = GOLEQ_Constants::ControlHeight + GOLEQ_Constants::RotarySliderExtraHeight;

  topRow.items.add(juce::FlexItem(_tempoToggle).withMinHeight(GOLEQ_Constants::ControlHeight).withMinWidth(GOLEQ_Constants::ControlMinWidth));
  topRow.items.add(juce::FlexItem(_timeSlider).withMinHeight(knobHeight).withMinWidth(GOLEQ_Constants::ControlMinWidth));
  topRow.items.add(juce::FlexItem(_caVariantDropdown).withMinHeight(GOLEQ_Constants::ControlHeight).withMinWidth(GOLEQ_Constants::DropdownWidth));
  topRow.items.add(juce::FlexItem(_effectDropdown).withMinHeight(GOLEQ_Constants::ControlHeight).withMinWidth(GOLEQ_Constants::DropdownWidth));
  topRow.items.add(juce::FlexItem(_playPauseButton).withMinHeight(GOLEQ_Constants::ControlHeight).withMinWidth(GOLEQ_Constants::ButtonWidth));

  bottomRow.items.add(juce::FlexItem(_randomizeButton).withMinHeight(GOLEQ_Constants::ControlHeight).withMinWidth(GOLEQ_Constants::ButtonWidth));
  bottomRow.items.add(juce::FlexItem(_nextGenerationButton).withMinHeight(GOLEQ_Constants::ControlHeight).withMinWidth(GOLEQ_Constants::ButtonWidth));
  bottomRow.items.add(juce::FlexItem(_widthSlider).withMinHeight(knobHeight).withMinWidth(GOLEQ_Constants::ControlMinWidth));
  bottomRow.items.add(juce::FlexItem(_heightSlider).withMinHeight(knobHeight).withMinWidth(GOLEQ_Constants::ControlMinWidth));

  topRow.performLayout(topArea);
  bottomRow.performLayout(bottomArea);
}

void ControlPanelComponent::InitializeControls()
{
  _tempoToggle.setButtonText(GOLEQ_Constants::TempoToggleLabel);

  _timeSlider.setRange(GOLEQ_Constants::TimeSliderMin, GOLEQ_Constants::TimeSliderMax, GOLEQ_Constants::TimeSliderInterval);
  _timeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  _timeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, GOLEQ_Constants::TextBoxWidth, GOLEQ_Constants::TextBoxHeight);

  //_widthSlider.setRange(UIConstants::SimWidthMin, UIConstants::SimWidthMax, UIConstants::SimSizeSliderInterval);
  //_widthSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  //_widthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, UIConstants::TextBoxWidth, UIConstants::TextBoxHeight);
  //_widthSlider.setValue(UIConstants::SimWidthMax, juce::NotificationType::dontSendNotification);
  //
  //_heightSlider.setRange(UIConstants::SimHeightMin, UIConstants::SimHeightMax, UIConstants::SimSizeSliderInterval);
  //_heightSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  //_heightSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, UIConstants::TextBoxWidth, UIConstants::TextBoxHeight);
  //_heightSlider.setValue(UIConstants::SimHeightMax, juce::NotificationType::dontSendNotification);

  _caVariantDropdown.addItem(GOLEQ_Constants::CA_Conway_Label, GOLEQ_Constants::CAVariant::Conway);
  _caVariantDropdown.addItem(GOLEQ_Constants::CA_HighLife_Label, GOLEQ_Constants::CAVariant::HighLife);
  _caVariantDropdown.addItem(GOLEQ_Constants::CA_Seeds_Label, GOLEQ_Constants::CAVariant::Seeds);
  _caVariantDropdown.setSelectedId(GOLEQ_Constants::CAVariant::Conway);

  _effectDropdown.addItem(GOLEQ_Constants::Effect_EQ_Label, GOLEQ_Constants::EffectType::EQ);
  _effectDropdown.addItem(GOLEQ_Constants::Effect_Bitcrusher_Label, GOLEQ_Constants::EffectType::Bitcrusher);
  _effectDropdown.addItem(GOLEQ_Constants::Effect_Reverb_Label, GOLEQ_Constants::EffectType::Reverb);
  _effectDropdown.setSelectedId(GOLEQ_Constants::EffectType::EQ);

  _playPauseButton.setButtonText(GOLEQ_Constants::PlayPauseLabel);
  _randomizeButton.setButtonText(GOLEQ_Constants::RandomizeLabel);
  _nextGenerationButton.setButtonText(GOLEQ_Constants::NextGenerationLabel);

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