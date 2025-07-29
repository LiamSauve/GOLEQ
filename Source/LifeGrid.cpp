#include "LifeGrid.h"

LifeGrid::LifeGrid(int width, int height) :
  _gridWidth(width),
  _gridHeight(height),
  _currentGen(width, std::vector<int>(height, 0)),
  _nextGen(width, std::vector<int>(height, 0))
{
}

void LifeGrid::Randomize()
{
  for (int x = 0; x < _gridWidth; ++x)
  {
    for (int y = 0; y < _gridHeight; ++y)
    {
      _currentGen[x][y] = juce::Random::getSystemRandom().nextInt(kMaxCellStateExclusive);
    }
  }
}

void LifeGrid::Update()
{
  for (int x = 0; x < _gridWidth; ++x)
  {
    for (int y = 0; y < _gridHeight; ++y)
    {
      int currentCellState = _currentGen[x][y];
      int liveNeighbours = CountLiveNeighbours(x, y);

      // Conway's rules:
      // If alive:
      //   - Survives with 2 or 3 live neighbours
      // If dead:
      //   - Comes to life with exactly 3 live neighbours
      _nextGen[x][y] = (currentCellState == 1) ?
                       (liveNeighbours == 2 || liveNeighbours == 3 ? 1 : 0) :
                       (liveNeighbours == 3 ? 1 : 0);
    }
  }
  std::swap(_currentGen, _nextGen);
}

void LifeGrid::ToggleCell(int x, int y)
{
  if (_currentGen[x][y] == 0)
  {
    _currentGen[x][y] = 1;
  }
  else
  {
    _currentGen[x][y] = 0;
  }
}

int LifeGrid::GetCell(int x, int y) const
{
  return _currentGen[x][y];
}


int LifeGrid::GetWidth() const
{
  return _gridWidth;
}

int LifeGrid::GetHeight() const
{
  return _gridHeight;
}

int LifeGrid::CountLiveNeighbours(int x, int y) const
{
  int liveCount = 0;

  for (int dx = -1; dx <= 1; ++dx)
  {
    for (int dy = -1; dy <= 1; ++dy)
    {
      // Skip the cell itself
      if (dx == 0 && dy == 0)
      {
        continue;
      }

      int neighbourX = x + dx;
      int neighbourY = y + dy;

      // Skip out-of-bounds accesses
      if (neighbourX >= 0 && neighbourY >= 0 &&
          neighbourX < _gridWidth && neighbourY < _gridHeight)
      {
        liveCount += _currentGen[neighbourX][neighbourY];
      }
    }
  }
  return liveCount;
}
