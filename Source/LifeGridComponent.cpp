#include "LifeGridComponent.h"
#include "UIConstants.h"

LifeGridComponent::LifeGridComponent() :
  _life(UIConstants::SimWidthMax, UIConstants::SimHeightMax)
{
  _life.Randomize();
  //setCellSize(_grid.GetWidth(), _grid.GetHeight());
  setCellPaintSize(UIConstants::CellPaintSize, UIConstants::CellPaintSize);
  startTimerHz(10); // 10 times per second for now
}

void LifeGridComponent::paint(juce::Graphics& g)
{
  g.fillAll(juce::Colours::black);

  // Red boundary anchored to local bounds
  juce::Rectangle<float> bounds = getLocalBounds().toFloat();

  // Optional: calculate offset inside redBounds for padding or centering
  const float originX = bounds.getX();
  const float originY = bounds.getY();
  const juce::Colour deadColour = juce::Colours::darkgrey.withAlpha(0.2f);

  for (int y = 0; y < _life.GetHeight(); ++y)
  {
    for (int x = 0; x < _life.GetWidth(); ++x)
    {
      const float left = originX + x * _cellPaintSize.x;
      const float top = originY + y * _cellPaintSize.y;

      const juce::Rectangle<float> cellRect(left, top, _cellPaintSize.x, _cellPaintSize.y);

      const bool alive = (_life.GetCell(x, y) == 1);
      g.setColour(alive ? juce::Colours::green : deadColour);
      g.fillRect(cellRect);
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
    _life.Update();
  }
  repaint();
}

void LifeGridComponent::toggleplayPause()
{
  _isPlaying = !_isPlaying;
}

void LifeGridComponent::setCellPaintSize(int width, int height)
{
  //_cellPaintSize.x = static_cast<float>(UIConstants::SimWidthMax) / static_cast<float>(width);
  //_cellPaintSize.y = static_cast<float>(UIConstants::SimHeightMax) / static_cast<float>(height);

  _cellPaintSize.x = width;
  _cellPaintSize.y = height;
}

void LifeGridComponent::setGridSize(int width, int height)
{
  _life.SetSize(width, height);
  //setCellPaintSize(width, height);
}

void LifeGridComponent::mouseDown(const juce::MouseEvent& event)
{
  const auto [gridX, gridY] = GetGridCoordsFromMouse(event);

  _dragModeErase = _life.GetCell(gridX, gridY) == 1;
  _isDragging = true;

  _life.ToggleCell(gridX, gridY);
  repaint();

}

void LifeGridComponent::mouseDrag(const juce::MouseEvent& event)
{
  if (!_isDragging)
  {
    return;
  }

  const auto [gridX, gridY] = GetGridCoordsFromMouse(event);

  _life.ToggleCell(gridX, gridY);
  repaint();

}

void LifeGridComponent::mouseUp(const juce::MouseEvent&)
{
  _isDragging = false;
}

juce::Point<int> LifeGridComponent::GetGridCoordsFromMouse(const juce::MouseEvent& event)
{
  const int clampedX = std::clamp(event.x, 0, static_cast<int>(UIConstants::SimWidthMax * _cellPaintSize.x) - 1);
  const int clampedY = std::clamp(event.y, 0, static_cast<int>(UIConstants::SimHeightMax * _cellPaintSize.y) - 1);

  return {
      static_cast<int>(clampedX / _cellPaintSize.x),
      static_cast<int>(clampedY / _cellPaintSize.y)
  };
}
