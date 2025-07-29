#pragma once
#include <vector>
#include <juce_core/juce_core.h>

class LifeGrid
{
public:
  LifeGrid(int width, int height);

  void Randomize();
  void Update();
  void ToggleCell(int x, int y);
  int GetCell(int x, int y) const;
  int GetWidth() const;
  int GetHeight() const;

private:
  int CountLiveNeighbours(int x, int y) const;

  static constexpr int kMaxCellState = 2; // We use binary states: 0 = dead, 1 = alive
  int _gridWidth;
  int _gridHeight;
  std::vector<std::vector<int>> _currentGen;
  std::vector<std::vector<int>> _nextGen;
};
