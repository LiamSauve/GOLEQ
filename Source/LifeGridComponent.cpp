#include "LifeGridComponent.h"
#include "Constants.h"

LifeGridComponent::LifeGridComponent() :
  _life(Constants::SimWidthMax, Constants::SimHeightMax)
{
  _life.Randomize();
  //setCellSize(_grid.GetWidth(), _grid.GetHeight());
  SetCellPaintSize(Constants::CellPaintSize, Constants::CellPaintSize);
  startTimerHz(10); // 10 times per second for now
}

void LifeGridComponent::paint(juce::Graphics& g)
{
  g.fillAll(juce::Colours::black);

  juce::Rectangle<float> bounds = getLocalBounds().toFloat();
  const float originX = bounds.getX();
  const float originY = bounds.getY();
  const juce::Colour deadColour = juce::Colours::transparentBlack;

  for (int y = 0; y < _life.GetHeight(); ++y)
  {
    for (int x = 0; x < _life.GetWidth(); ++x)
    {
      const float left = originX + x * _cellPaintSize.x;
      const float top = originY + y * _cellPaintSize.y;
      const juce::Rectangle<float> cellRect(left, top, _cellPaintSize.x, _cellPaintSize.y);

      const Cell cell = _life.GetCell(x, y);
      const bool alive = (cell.alive == 1);

      if (alive)
      {
        juce::Colour cellColour;

        if (cell.age == -1)
        {
          // Age not used — default bright green
          cellColour = juce::Colours::green;
        }
        else
        {
          // Age used — brighten with age
          float brightness = juce::jlimit(0.2f, 1.0f, 0.2f + (cell.age / 20.0f)); // age 0 = 0.2, age 20 = 1.0
          cellColour = juce::Colours::green.withBrightness(brightness);
        }

        g.setColour(cellColour);
      }
      else
      {
        g.setColour(deadColour);
      }

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

void LifeGridComponent::TogglePlayPause()
{
  _isPlaying = !_isPlaying;
}

void LifeGridComponent::SetCellPaintSize(int width, int height)
{
  //_cellPaintSize.x = static_cast<float>(UIConstants::SimWidthMax) / static_cast<float>(width);
  //_cellPaintSize.y = static_cast<float>(UIConstants::SimHeightMax) / static_cast<float>(height);

  _cellPaintSize.x = static_cast<float>(width);
  _cellPaintSize.y = static_cast<float>(height);
}

void LifeGridComponent::SetGridSize(int width, int height)
{
  _life.SetSize(width, height);
  //setCellPaintSize(width, height);
}

void LifeGridComponent::Randomize()
{
  _life.Randomize();
  repaint();
}

void LifeGridComponent::RandomizeMeaningfully()
{
  _life.RandomizeMeaningfully();
  repaint();
}

void LifeGridComponent::NextGeneration()
{
  _life.Update();
  repaint();
}

void LifeGridComponent::SetCAVariant(CAVariant variant)
{
  _life.SetCAVariant(variant);
}

void LifeGridComponent::SetEffectType(EffectType effectType)
{
  // send this to the processor
}

const ILife* LifeGridComponent::GetILife() const
{
  return &_life;
}

void LifeGridComponent::mouseDown(const juce::MouseEvent& event)
{
  const auto [gridX, gridY] = GetGridCoordsFromMouse(event);

  _dragModeErase = _life.GetCell(gridX, gridY).alive == 1;
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
  const int clampedX = std::clamp(event.x, 0, static_cast<int>(Constants::SimWidthMax * _cellPaintSize.x) - 1);
  const int clampedY = std::clamp(event.y, 0, static_cast<int>(Constants::SimHeightMax * _cellPaintSize.y) - 1);

  return
  {
      static_cast<int>(clampedX / _cellPaintSize.x),
      static_cast<int>(clampedY / _cellPaintSize.y)
  };
}
