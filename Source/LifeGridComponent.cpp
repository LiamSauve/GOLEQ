#include "LifeGridComponent.h"
#include "UIConstants.h"

LifeGridComponent::LifeGridComponent() :
  _grid(UIConstants::SimWidthMin, UIConstants::SimHeightMin)
{
  _grid.Randomize();
  setCellSize(_grid.GetWidth(), _grid.GetHeight());
  startTimerHz(10); // 10 times per second for now
}

void LifeGridComponent::paint(juce::Graphics& g)
{
  g.fillAll(juce::Colours::black);

  const float gridPixelWidth = _grid.GetWidth() * _cellPaintSize.x;
  const float gridPixelHeight = _grid.GetHeight() * _cellPaintSize.y;

  // Use SimWidthMax and SimHeightMax for consistent centering
  const float offsetX = (UIConstants::SimWidthMax - gridPixelWidth) / 2.0f;
  const float offsetY = (UIConstants::SimHeightMax - gridPixelHeight) / 2.0f;

  for (int y = 0; y < _grid.GetHeight(); ++y)
  {
    for (int x = 0; x < _grid.GetWidth(); ++x)
    {
      if (_grid.GetCell(x, y) == 1)
      {
        g.setColour(juce::Colours::green);

        g.fillRect(offsetX + x * _cellPaintSize.x,
          offsetY + y * _cellPaintSize.y,
          _cellPaintSize.x,
          _cellPaintSize.y);
      }
    }
  }
}

void LifeGridComponent::resized()
{
  repaint();
}

void LifeGridComponent::timerCallback()
{
  if(_isPlaying)
  {
    _grid.Update();
  }
  repaint();
}

void LifeGridComponent::toggleplayPause()
{
  _isPlaying = !_isPlaying;
}

void LifeGridComponent::setCellSize(int width, int height)
{
  _cellPaintSize.x = static_cast<float>(UIConstants::SimWidthMax) / static_cast<float>(width);
  _cellPaintSize.y = static_cast<float>(UIConstants::SimHeightMax) / static_cast<float>(height);

  DBG("x:" << _cellPaintSize.x);
  DBG("y:" << _cellPaintSize.y);
}

void LifeGridComponent::setGridSize(int width, int height)
{
  _grid.SetSize(width, height);
  setCellSize(width, height);
}

void LifeGridComponent::mouseDown(const juce::MouseEvent& event)
{
  const int clampedMouseX = std::clamp(event.x, 0, UIConstants::SimWidthMax-1);
  const int clampedMouseY = std::clamp(event.y, 0, UIConstants::SimHeightMax-1);
  const int gridX = clampedMouseX / static_cast<int>(_cellPaintSize.x);
  const int gridY = clampedMouseY / static_cast<int>(_cellPaintSize.y);

  _dragModeErase = _grid.GetCell(gridX, gridY) == 1;
  _isDragging = true;

  _grid.ToggleCell(gridX, gridY);
  repaint();
}

void LifeGridComponent::mouseDrag(const juce::MouseEvent& event)
{
  if (!_isDragging)
  {
    return;
  }

  const int clampedMouseX = std::clamp(event.x, 0, UIConstants::SimWidthMax-1);
  const int clampedMouseY = std::clamp(event.y, 0, UIConstants::SimHeightMax-1);

  const int gridX = static_cast<int>(clampedMouseX / _cellPaintSize.x);
  const int gridY = static_cast<int>(clampedMouseY / _cellPaintSize.y);

  _grid.ToggleCell(gridX, gridY);
  repaint();
}

void LifeGridComponent::mouseUp(const juce::MouseEvent&)
{
  _isDragging = false;
}
