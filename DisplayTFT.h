#pragma once
#include <Arduino.h>
#include <TFT_eSPI.h>
#include "TetrisGame.h"

class DisplayTFT {
public:
  DisplayTFT(TFT_eSPI &tft);

  void begin();
  void draw(const TetrisGame &g);

private:
  TFT_eSPI &_tft;

  // Ajustes pantalla (rot=1 => 480x320)
  static constexpr int SCREEN_W = 480;
  static constexpr int SCREEN_H = 320;

  // Para que quepa 10x20 con UI:
  static constexpr int BLOCK = 14;      // 20*14 = 280
  static constexpr int UI_TOP = 40;     // 40 + 280 = 320 exacto

  static constexpr int BOARD_W = TetrisGame::W * BLOCK; // 140
  static constexpr int BOARD_H = TetrisGame::H * BLOCK; // 280
  static constexpr int ORIGIN_X = (SCREEN_W - BOARD_W) / 2; // 170
  static constexpr int ORIGIN_Y = UI_TOP;

  uint16_t colorFor(uint8_t v) const;
};