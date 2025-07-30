#pragma once
#include <JuceHeader.h>
#include "Life.h"

class LifeGridComponent : public juce::Component, public juce::Timer
{
public:
  LifeGridComponent();
  void paint(juce::Graphics& g) override;
  void resized() override;
  void timerCallback() override;

  void TogglePlayPause();
  void SetCellPaintSize(int width, int height);
  void SetGridSize(int width, int height);
  void Randomize();
  void NextGeneration();
  void SetCAVariant(GOLEQ_Constants::CAVariant variant);
  void SetEffectType(GOLEQ_Constants::EffectType effectType);

  void mouseDown(const juce::MouseEvent& event);
  void mouseDrag(const juce::MouseEvent& event);
  void mouseUp(const juce::MouseEvent& event);
  juce::Point<int> GetGridCoordsFromMouse(const juce::MouseEvent& event);

private:
  Life _life;
  bool _isPlaying = false;
  juce::Point<float> _cellPaintSize;
  bool _isDragging = false;
  bool _dragModeErase = false;
};
