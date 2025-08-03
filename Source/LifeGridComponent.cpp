#include "LifeGridComponent.h"
#include "Constants.h"

LifeGridComponent::LifeGridComponent() :
  _life(Constants::SimWidthMax, Constants::SimHeightMax),
  _isPlaying(false),
  _cellPaintSize({Constants::CellPaintSize, Constants::CellPaintSize }),
  _isDragging(false),
  _dragModeErase(false)
{
  _life.Randomize();

  addAndMakeVisible(_renderer);
  startTimerHz(10); // 10 times per second for now
}

LifeGridComponent::~LifeGridComponent()
{
  //_glContext.detach();
}


void LifeGridComponent::paint(juce::Graphics& g)
{
  //g.fillAll(juce::Colours::black);

  //juce::Rectangle<float> bounds = getLocalBounds().toFloat();
  //const float originX = bounds.getX();
  //const float originY = bounds.getY();
  //const juce::Colour deadColour = juce::Colours::transparentBlack;

  //for (int y = 0; y < _life.GetHeight(); ++y)
  //{
  //  for (int x = 0; x < _life.GetWidth(); ++x)
  //  {
  //    const float left = originX + x * _cellPaintSize.x;
  //    const float top = originY + y * _cellPaintSize.y;
  //    const juce::Rectangle<float> cellRect(left, top, _cellPaintSize.x, _cellPaintSize.y);

  //    const Cell cell = _life.GetCell(x, y);
  //    const bool alive = (cell.alive == 1);

  //    if (alive)
  //    {
  //      juce::Colour cellColour;

  //      if (cell.age == -1)
  //      {
  //        cellColour = juce::Colours::green;
  //      }
  //      else
  //      {
  //        // Normalize age to [0, 1]
  //        const float ageRatio = juce::jlimit(0.0f, 1.0f, (float)(cell.age - Constants::MinAge) / (float)(Constants::MaxAge - Constants::MinAge));

  //        juce::Colour startColour = juce::Colours::green;
  //        juce::Colour midColour = juce::Colours::green.darker(0.6f);
  //        juce::Colour endColour = juce::Colours::brown.darker(0.8f);

  //        if (ageRatio < 0.5f)
  //        {
  //          float t = ageRatio * 2.0f;
  //          cellColour = startColour.interpolatedWith(midColour, t);
  //        }
  //        else
  //        {
  //          float t = (ageRatio - 0.5f) * 2.0f;
  //          cellColour = midColour.interpolatedWith(endColour, t);
  //        }
  //      }

  //      g.setColour(cellColour);
  //    }
  //    else
  //    {
  //      g.setColour(deadColour);
  //    }

  //    g.fillRect(cellRect);
  //  }
  //}
}

void LifeGridComponent::resized()
{
  //repaint();
  //_renderer.setBounds(getLocalBounds());
  _renderer.setBounds(0, 0, Constants::WindowWidth, Constants::SimulationHeight);
}

void LifeGridComponent::timerCallback()
{
  if(_isPlaying)
  {
    _life.Update();
  }
  Render();
}

void LifeGridComponent::TogglePlayPause()
{
  _isPlaying = !_isPlaying;
}

void LifeGridComponent::SetCellPaintSize(int width, int height)
{
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
  Render();
}

void LifeGridComponent::RandomizeMeaningfully()
{
  _life.RandomizeMeaningfully();
  Render();
}

void LifeGridComponent::NextGeneration()
{
  _life.Update();
  Render();
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
  Render();
}

void LifeGridComponent::mouseDrag(const juce::MouseEvent& event)
{
  if (!_isDragging)
  {
    return;
  }

  const auto [gridX, gridY] = GetGridCoordsFromMouse(event);

  _life.ToggleCell(gridX, gridY);
  Render();
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

void LifeGridComponent::Render()
{
  UpdateLifeData(_life.GetRenderData(), _life.GetWidth(), _life.GetHeight());
}

void LifeGridComponent::UpdateLifeData(const std::vector<CellRenderData>& data, int width, int height)
{
  _renderer.SetLifeData(data, width, height);
}
