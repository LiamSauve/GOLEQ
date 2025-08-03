#pragma once
#include <vector>
#include <juce_core/juce_core.h>
#include "ILife.h"
#include "Constants.h"
#include "Cell.h"

class Life : public ILife
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
  void Update_Maze();
  void Update_DataMosh();
  void Update_OrganicLife();
  void ToggleCell(int x, int y);
  int GetActiveLiveCellCount() const override;
  Cell GetCell(int x, int y) const override;
  int GetWidth() const override;
  int GetHeight() const override;
  void SetSize(int newWidth, int newHeight);
  void SetCAVariant(CAVariant caVariant);
  std::vector<CellRenderData> GetRenderData() const;

private:
  int CountLiveNeighbours(int x, int y) const;
  bool IsInBounds(int x, int y);

  static constexpr int kMaxCellStateExclusive = 2;
  int _gridWidth;
  int _gridHeight;
  std::vector<std::vector<Cell>> _currentGen;
  std::vector<std::vector<Cell>> _nextGen;
  CAVariant _caVariant;
};
