#pragma once
#include <vector>
#include <juce_core/juce_core.h>
#include "Constants.h"

class Life
{
public:
  Life(int width, int height);

  void KillAll();
  void Randomize();
  void RandomizeMeaningfully();
  void GenerateClusterBlobs();
  void GenerateDiagonalBands();
  void GenerateGliderSeeds();
  void GenerateCornerBias();
  void GenerateRingFormations();
  void Update();
  void Update_Conway();
  void Update_Highlife();
  void Update_Seeds();
  void ToggleCell(int x, int y);
  int GetCell(int x, int y) const;
  int GetWidth() const;
  int GetHeight() const;
  void SetSize(int newWidth, int newHeight);
  void SetCAVariant(CAVariant caVariant);

private:
  int CountLiveNeighbours(int x, int y) const;
  bool IsInBounds(int x, int y);

  static constexpr int kMaxCellStateExclusive = 2;
  int _gridWidth;
  int _gridHeight;
  std::vector<std::vector<int>> _currentGen;
  std::vector<std::vector<int>> _nextGen;
  CAVariant _caVariant;
};
