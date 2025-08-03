#include "Life.h"
#include "Constants.h"

Life::Life(int width, int height) :
  _gridWidth(width),
  _gridHeight(height),
  _currentGen(width, std::vector<Cell>(height, { 0, 0 })),
  _nextGen(width, std::vector<Cell>(height, { 0, 0 })),
  _caVariant(CAVariant::Conway)
{
}

void Life::KillAll()
{
  for (int x = 0; x < _gridWidth; ++x)
  {
    for (int y = 0; y < _gridHeight; ++y)
    {
      _currentGen[x][y].alive = 0;
    }
  }
}

void Life::Randomize()
{
  for (int x = 0; x < _gridWidth; ++x)
  {
    for (int y = 0; y < _gridHeight; ++y)
    {
      _currentGen[x][y].alive = juce::Random::getSystemRandom().nextInt(kMaxCellStateExclusive);
    }
  }
}

void Life::RandomizeMeaningfully()
{
  KillAll();
  int numOfPatterns = 5;
  int pattern = rand() % numOfPatterns;

  switch (pattern)
  {
    case 0: GenerateClusterBlobs(); break;
    case 1: GenerateDiagonalBands(); break;
    case 2: GenerateGliderSeeds(); break;
    case 3: GenerateCornerBias(); break;
    case 4: GenerateRingFormations(); break;
  }
}

void Life::GenerateClusterBlobs()
{
  for (int i = 0; i < 12; ++i)
  {
    int cx = rand() % _gridWidth;
    int cy = rand() % _gridHeight;

    for (int dx = -2; dx <= 2; ++dx)
    {
      for (int dy = -2; dy <= 2; ++dy)
      {
        int x = cx + dx;
        int y = cy + dy;
        if (IsInBounds(x, y) && rand() % 100 < 40)
        {
          _currentGen[x][y].alive = 1;
        }
      }
    }
  }
}

void Life::GenerateDiagonalBands()
{
  for (int x = 0; x < _gridWidth; ++x)
  {
    for (int y = 0; y < _gridHeight; ++y)
    {
      if ((x + y) % 10 < 3 && rand() % 100 < 50)
      {
        _currentGen[x][y].alive = 1;
      }
    }
  }
}

void Life::GenerateGliderSeeds()
{
  for (int i = 0; i < 8; ++i)
  {
    int x = rand() % (_gridWidth - 3);
    int y = rand() % (_gridHeight - 3);

    // Simple glider pattern
    _currentGen[x + 1][y].alive     = 1;
    _currentGen[x + 2][y + 1].alive = 1;
    _currentGen[x][y + 2].alive     = 1;
    _currentGen[x + 1][y + 2].alive = 1;
    _currentGen[x + 2][y + 2].alive = 1;
  }
}

void Life::GenerateCornerBias()
{
  for (int x = 0; x < _gridWidth / 4; ++x)
  {
    for (int y = 0; y < _gridHeight / 4; ++y)
    {
      if (rand() % 100 < 60)
        _currentGen[x][y].alive = 1; // Top-left bias
    }
  }

  for (int x = _gridWidth - _gridWidth / 4; x < _gridWidth; ++x)
  {
    for (int y = _gridHeight - _gridHeight / 4; y < _gridHeight; ++y)
    {
      if (rand() % 100 < 60)
        _currentGen[x][y].alive = 1; // Bottom-right bias
    }
  }
}

void Life::GenerateRingFormations()
{
  // Random center within grid bounds
  int cx = juce::Random::getSystemRandom().nextInt({ 0, _gridWidth });
  int cy = juce::Random::getSystemRandom().nextInt({ 0, _gridHeight });

  // Random radius between 1/4 and ~45% of the smaller grid dimension
  int minRadius = std::min(_gridWidth, _gridHeight) / 4;
  int maxRadius = static_cast<int>(std::min(_gridWidth, _gridHeight) * 0.45f);
  int radius = juce::Random::getSystemRandom().nextInt({ minRadius, maxRadius + 1 });

  for (int angle = 0; angle < 360; angle += 10)
  {
    float rad = angle * 3.14159f / 180.0f;
    int x = cx + static_cast<int>(radius * std::cos(rad));
    int y = cy + static_cast<int>(radius * std::sin(rad));

    if (IsInBounds(x, y))
    {
      _currentGen[x][y].alive = 1;
    }

    for (int dx = -2; dx <= 2; dx += 2)
    {
      for (int dy = -2; dy <= 2; dy += 2)
      {
        int nx = x + dx;
        int ny = y + dy;
        if (IsInBounds(nx, ny))
        {
          _currentGen[nx][ny].alive = 1;
        }
      }
    }
  }
}

void Life::Update()
{
  switch (_caVariant)
  {
  case CAVariant::Conway:
    Update_Conway();
    break;
  case CAVariant::HighLife:
    Update_Highlife();
    break;
  case CAVariant::Seeds:
    Update_Seeds();
    break;
  case CAVariant::Maze:
    Update_Maze();
    break;
  case CAVariant::DataMosh:
    Update_DataMosh();
    break;
  case CAVariant::OrganicLife:
    Update_OrganicLife();
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
      const Cell current = _currentGen[x][y];
      const int liveNeighbours = CountLiveNeighbours(x, y);
      Cell next = current;
      if (current.alive == 1)
      {
        // Survives with 2 or 3 neighbors
        next.alive = (liveNeighbours == 2 || liveNeighbours == 3) ? 1 : 0;
      }
      else
      {
        // Born with exactly 3 neighbors
        next.alive = (liveNeighbours == 3) ? 1 : 0;
      }
      next.age = -1;
      _nextGen[x][y] = next;
    }
  }
  std::swap(_currentGen, _nextGen);
}

void Life::Update_Highlife()
{
  // HighLife rules:
  // If alive:
  //   - Survives with 2 or 3 live neighbours
  // If dead:
  //   - Comes to life with 3 or 6 live neighbours
  for (int x = 0; x < _gridWidth; ++x)
  {
    for (int y = 0; y < _gridHeight; ++y)
    {
      const Cell current = _currentGen[x][y];
      const int liveNeighbours = CountLiveNeighbours(x, y);
      Cell next = current;

      if (current.alive == 1)
      {
        // Survives with 2 or 3 neighbors
        next.alive = (liveNeighbours == 2 || liveNeighbours == 3) ? 1 : 0;
      }
      else
      {
        // Born with 3 or 6 neighbors
        next.alive = (liveNeighbours == 3 || liveNeighbours == 6) ? 1 : 0;
      }
      next.age = -1;
      _nextGen[x][y] = next;
    }
  }
  std::swap(_currentGen, _nextGen);
}

void Life::Update_Seeds()
{
  // Seeds rules:
  // If dead:
  //   - Comes to life with exactly 2 live neighbours
  // All live cells die every generation
  for (int x = 0; x < _gridWidth; ++x)
  {
    for (int y = 0; y < _gridHeight; ++y)
    {
      const Cell current = _currentGen[x][y];
      const int liveNeighbours = CountLiveNeighbours(x, y);
      Cell next = current;

      // Seeds: only dead cells with exactly 2 neighbors come to life
      next.alive = (current.alive == 0 && liveNeighbours == 2) ? 1 : 0;
      next.age = -1;

      _nextGen[x][y] = next;
    }
  }
  std::swap(_currentGen, _nextGen);
}

void Life::Update_Maze()
{
  // Maze rules:
  // If alive:
  //   - Survives with 1 to 5 live neighbours
  // If dead:
  //   - Comes to life with exactly 3 live neighbours
  for (int x = 0; x < _gridWidth; ++x)
  {
    for (int y = 0; y < _gridHeight; ++y)
    {
      const int currentCellState = _currentGen[x][y].alive;
      const int liveNeighbours = CountLiveNeighbours(x, y);
  
      _nextGen[x][y].alive = (currentCellState == 1)
        ? ((liveNeighbours >= 1 && liveNeighbours <= 5) ? 1 : 0)
        : (liveNeighbours == 3 ? 1 : 0);
    }
  }
  std::swap(_currentGen, _nextGen);
}

void Life::Update_DataMosh()
{
  for (int x = 0; x < _gridWidth; ++x)
  {
    for (int y = 0; y < _gridHeight; ++y)
    {
      int srcX = (x + rand() % 3 - 1 + _gridWidth) % _gridWidth;
      int srcY = (y + rand() % 3 - 1 + _gridHeight) % _gridHeight;

      const Cell& current = _currentGen[x][y];
      const Cell& neighbor = _currentGen[srcX][srcY];

      // Blend alive state and age
      int blendedAlive = (current.alive + neighbor.alive) / 2;
      int blendedAge = (current.age + neighbor.age) / 2;

      // Add noise or motion bias
      if (rand() % 10 < 2)
        blendedAlive = 1 - blendedAlive; // occasional inversion

      // Assign to nextGen
      _nextGen[x][y].alive = blendedAlive;
      _nextGen[x][y].age = blendedAge;
    }
  }

  std::swap(_currentGen, _nextGen);
}

void Life::Update_OrganicLife()
{
  for (int x = 0; x < _gridWidth; ++x)
  {
    for (int y = 0; y < _gridHeight; ++y)
    {
      int liveNeighbours = CountLiveNeighbours(x, y);
      Cell current = _currentGen[x][y];
      Cell next = current;

      if (current.alive == 1)
      {
        next.age++;

        // Death by isolation or overcrowding
        if (liveNeighbours <= 1 || liveNeighbours >= 5)
          next.alive = 0;
        // Death by age
        else if (next.age > 20)
          next.alive = 0;
        else
        {
          // Try to spawn nearby — increased spread chance
          for (int i = 0; i < 2; ++i) // try multiple directions
          {
            int dx = rand() % 3 - 1;
            int dy = rand() % 3 - 1;
            int nx = (x + dx + _gridWidth) % _gridWidth;
            int ny = (y + dy + _gridHeight) % _gridHeight;

            if (_currentGen[nx][ny].alive == 0 && rand() % 100 < 50) // 50% chance to spread
              _nextGen[nx][ny] = { 1, 0 };
          }
        }
      }
      else
      {
        // Rarer spontaneous birth
        if (rand() % 1000 < 1) // 0.1% chance
          next = { 1, 0 };
      }

      _nextGen[x][y] = next;
    }
  }

  std::swap(_currentGen, _nextGen);
}

void Life::ToggleCell(int x, int y)
{
  _currentGen[x][y].alive = (_currentGen[x][y].alive == 0) ? 1 : 0;
}

int Life::GetActiveLiveCellCount() const
{
  int count = 0;
  for (int x = 0; x < _currentGen.size(); ++x)
  {
    for (int y = 0; y < _currentGen[x].size(); ++y)
    {
      if (_currentGen[x][y].alive != 0)
      {
        ++count;
      }
    }
  }
  return count;
}


Cell Life::GetCell(int x, int y) const
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
      std::vector<Cell> newCol(newHeight, { 0, 0 });
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
      std::vector<Cell> newCol(newHeight, { 0, 0 });
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
      _currentGen[x].insert(_currentGen[x].begin(), prepend, Cell{ 0, 0 });
      _nextGen[x].insert(_nextGen[x].begin(), prepend, Cell{ 0, 0 });
    }
    else if (rowDiff < 0)
    {
      _currentGen[x].erase(_currentGen[x].begin(), _currentGen[x].begin() + prepend);
      _nextGen[x].erase(_nextGen[x].begin(), _nextGen[x].begin() + prepend);
    }

    if (rowDiff > 0)
    {
      _currentGen[x].insert(_currentGen[x].end(), append, Cell{ 0, 0 });
      _nextGen[x].insert(_nextGen[x].end(), append, Cell{ 0, 0 });
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

void Life::SetCAVariant(CAVariant caVariant)
{
  _caVariant = caVariant;
}

std::vector<CellRenderData> Life::GetRenderData() const
{
  std::vector<CellRenderData> data;
  data.reserve(_currentGen.size() * _currentGen[0].size());

  for (const auto& row : _currentGen)
  {
    for (const auto& cell : row)
    {
      CellRenderData renderCell;
      renderCell.alive = cell.alive;
      renderCell.age = cell.age;
      data.push_back(renderCell);
    }
  }
  return data;
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
        liveCount += _currentGen[neighbourX][neighbourY].alive;
      }
    }
  }

  return liveCount;
}

bool Life::IsInBounds(int x, int y)
{
  return x >= 0 && x < _gridWidth &&
         y >= 0 && y < _gridHeight;
}