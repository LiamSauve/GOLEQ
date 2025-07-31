#pragma once

class ILife
{
public:
  virtual ~ILife() = default;
  virtual int GetCell(int x, int y) const = 0;
  virtual int GetWidth() const = 0;
  virtual int GetHeight() const = 0;
};
