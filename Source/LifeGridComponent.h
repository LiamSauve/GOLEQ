#pragma once
#include "Life.h"
#include "LifeRenderer.h"

class LifeGridComponent :
  public juce::Component,
  public juce::Timer
{
public:
  LifeGridComponent();
  ~LifeGridComponent();
  void resized() override;
  void timerCallback() override;

  void TogglePlayPause();
  void SetCellPaintSize(int width, int height);
  void SetGridSize(int width, int height);
  void Randomize();
  void RandomizeMeaningfully();
  void NextGeneration();
  void SetCAVariant(CAVariant variant);
  void SetEffectType(EffectType effectType);
  const ILife* GetILife() const;

  void mouseDown(const juce::MouseEvent& event);
  void mouseDrag(const juce::MouseEvent& event);
  void mouseUp(const juce::MouseEvent& event);
  juce::Point<int> GetGridCoordsFromMouse(const juce::MouseEvent& event);

  void Render();
  void UpdateLifeData(const std::vector<std::vector<Cell>>& data, int width, int height);

private:
  Life _life;
  LifeRenderer _renderer;
  bool _isPlaying;
  bool _isDragging;
  bool _dragModeErase;
  juce::Point<float> _cellPaintSize;

  //juce::Colour GetAgeColour(int age);
};
