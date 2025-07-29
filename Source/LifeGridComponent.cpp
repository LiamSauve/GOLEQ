#include "LifeGridComponent.h"

LifeGridComponent::LifeGridComponent() :
  _grid(kTempGridSizeX, kTempGridSizeY)
{
  _grid.Randomize();
  startTimerHz(10); // 10 times per second for now
}

void LifeGridComponent::paint(juce::Graphics& g)
{
  g.fillAll(juce::Colours::black);
  for (int x = 0; x < _grid.GetWidth(); ++x)
  {
    for (int y = 0; y < _grid.GetHeight(); ++y)
    {
      if (_grid.GetCell(x, y) == 1)
      {
        g.setColour(juce::Colours::green);
        g.fillRect(x * _cellSize, y * _cellSize, _cellSize, _cellSize);
      }
    }
  }
}

void LifeGridComponent::resized()
{
  //
}

void LifeGridComponent::timerCallback()
{ 
  _grid.Update();
  repaint();
}