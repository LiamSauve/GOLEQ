#pragma once
#include <JuceHeader.h>
#include "LifeGrid.h"

class LifeGridComponent : public juce::Component, public juce::Timer
{
public:
  LifeGridComponent();
  void paint(juce::Graphics& g) override;
  void resized() override;
  void timerCallback() override;

private:
  LifeGrid _grid;
  const int _cellSize = 10;
  static constexpr int kTempGridSizeX = 80;
  static constexpr int kTempGridSizeY = 60;
};