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

  void toggleplayPause();
  void setCellSize(int width, int height);
  void setGridSize(int width, int height);

  void mouseDown(const juce::MouseEvent& event);
  void mouseDrag(const juce::MouseEvent& event);
  void mouseUp(const juce::MouseEvent& event);

private:
  LifeGrid _grid;
  bool _isPlaying = true;
  juce::Point<float> _cellPaintSize;
  bool _isDragging = false;
  bool _dragModeErase = false;
};