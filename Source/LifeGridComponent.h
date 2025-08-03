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
  void paint(juce::Graphics& g) override;
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

private:
  Life _life;
  bool _isPlaying;
  LifeRenderer _lifeRenderer;
  bool _isDragging;
  bool _dragModeErase;
  juce::Point<float> _cellPaintSize;
  juce::OpenGLContext _glContext;

  //juce::Colour GetAgeColour(int age);
};
