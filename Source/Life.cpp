#include "Life.h"
#include "GOLEQ_Constants.h"

Life::Life(int width, int height)
  : _gridWidth(width),
  _gridHeight(height),
  _currentGen(width, std::vector<int>(height, 0)),
  _nextGen(width, std::vector<int>(height, 0)),
  _caVariant(Constants::CAVariant::Conway)
{
}

void Life::Randomize()
{
  for (int x = 0; x < _gridWidth; ++x)
  {
    for (int y = 0; y < _gridHeight; ++y)
    {
      _currentGen[x][y] = juce::Random::getSystemRandom().nextInt(kMaxCellStateExclusive);
    }
  }
}

void Life::Update()
{
  switch (_caVariant)
  {
  case Constants::Conway:
    Update_Conway();
    break;
  case Constants::HighLife:
    Update_Highlife();
    break;
  case Constants::Seeds:
    Update_Seeds();
    break;
  default:
    Update_Conway();
    break;
  }
}

void Life::Update_Conway()
{
  // Conway's rules:
  // If alive:
  //   - Survives with 2 or 3 live neighbours
  // If dead:
  //   - Comes to life with exactly 3 live neighbours
  for (int x = 0; x < _gridWidth; ++x)
  {
    for (int y = 0; y < _gridHeight; ++y)
    {
      const int currentCellState = _currentGen[x][y];
      const int liveNeighbours = CountLiveNeighbours(x, y);

      _nextGen[x][y] = (currentCellState == 1)
        ? ((liveNeighbours == 2 || liveNeighbours == 3) ? 1 : 0)
        : (liveNeighbours == 3 ? 1 : 0);
    }
  }
  std::swap(_currentGen, _nextGen);
}

void Life::Update_Highlife()
{
  Update_Conway();
}

void Life::Update_Seeds()
{
  Update_Conway();
}

void Life::ToggleCell(int x, int y)
{
  _currentGen[x][y] = (_currentGen[x][y] == 0) ? 1 : 0;
}

int Life::GetCell(int x, int y) const
{
  return _currentGen[x][y];
}

int Life::GetWidth() const
{
  return _gridWidth;
}

int Life::GetHeight() const
{
  return _gridHeight;
}

void Life::SetSize(int newWidth, int newHeight)
{
  const int widthDiff = newWidth - _gridWidth;
  const int heightDiff = newHeight - _gridHeight;

  // Resize columns
  if (widthDiff != 0)
  {
    const int prepend = widthDiff > 0 ? widthDiff / 2 : -widthDiff / 2;
    const int append = widthDiff > 0 ? (widthDiff + 1) / 2 : (-widthDiff + 1) / 2;

    for (int i = 0; i < prepend; ++i)
    {
      std::vector<int> newCol(newHeight, 0);
      _currentGen.insert(_currentGen.begin(), newCol);
      _nextGen.insert(_nextGen.begin(), newCol);
    }

    if (widthDiff < 0)
    {
      _currentGen.erase(_currentGen.begin(), _currentGen.begin() + prepend);
      _nextGen.erase(_nextGen.begin(), _nextGen.begin() + prepend);
    }

    for (int i = 0; i < append; ++i)
    {
      std::vector<int> newCol(newHeight, 0);
      _currentGen.push_back(newCol);
      _nextGen.push_back(newCol);
    }

    if (widthDiff < 0)
    {
      _currentGen.resize(newWidth);
      _nextGen.resize(newWidth);
    }
  }

  // Resize rows
  for (int x = 0; x < newWidth; ++x)
  {
    const int rowDiff = newHeight - _gridHeight;
    const int prepend = rowDiff > 0 ? rowDiff / 2 : -rowDiff / 2;
    const int append = rowDiff > 0 ? (rowDiff + 1) / 2 : (-rowDiff + 1) / 2;

    if (rowDiff > 0)
    {
      _currentGen[x].insert(_currentGen[x].begin(), prepend, 0);
      _nextGen[x].insert(_nextGen[x].begin(), prepend, 0);
    }
    else if (rowDiff < 0)
    {
      _currentGen[x].erase(_currentGen[x].begin(), _currentGen[x].begin() + prepend);
      _nextGen[x].erase(_nextGen[x].begin(), _nextGen[x].begin() + prepend);
    }

    if (rowDiff > 0)
    {
      _currentGen[x].insert(_currentGen[x].end(), append, 0);
      _nextGen[x].insert(_nextGen[x].end(), append, 0);
    }
    else if (rowDiff < 0)
    {
      _currentGen[x].resize(newHeight);
      _nextGen[x].resize(newHeight);
    }
  }

  _gridWidth = newWidth;
  _gridHeight = newHeight;
}

int Life::CountLiveNeighbours(int x, int y) const
{
  int liveCount = 0;

  for (int dx = -1; dx <= 1; ++dx)
  {
    for (int dy = -1; dy <= 1; ++dy)
    {
      if (dx == 0 && dy == 0)
        continue;

      const int neighbourX = x + dx;
      const int neighbourY = y + dy;

      if (neighbourX >= 0 && neighbourY >= 0 &&
        neighbourX < _gridWidth && neighbourY < _gridHeight)
      {
        liveCount += _currentGen[neighbourX][neighbourY];
      }
    }
  }

  return liveCount;
}

void Life::SetCAVariant(Constants::CAVariant caVariant)
{
  _caVariant = caVariant;
}