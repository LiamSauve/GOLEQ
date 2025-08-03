#pragma once
struct Cell;

class ILife
{
public:
  virtual ~ILife() = default;
  virtual int GetActiveLiveCellCount() const = 0;
  virtual Cell GetCell(int x, int y) const = 0;
  virtual int GetWidth() const = 0;
  virtual int GetHeight() const = 0;
};
