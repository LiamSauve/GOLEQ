#pragma once

struct Cell
{
  int alive;
  int age;
};

struct CellRenderData
{
  bool alive;
  uint8_t age;
};