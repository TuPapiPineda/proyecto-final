#pragma once
#include <Arduino.h>
#include "InputADKeyboard.h"

class TetrisGame {
public:
  static constexpr int W = 10;
  static constexpr int H = 20;

  void reset();
  void tick();                 // caída automática
  void handleButton(Button b); // izquierda/derecha/abajo/rotar/restart
  bool isGameOver() const { return _gameOver; }
  uint32_t score() const { return _score; }

  // acceso para dibujar
  uint8_t cell(int x, int y) const { return _board[y][x]; }
  // pieza actual (4 bloques)
  void getActiveBlocks(int outX[4], int outY[4]) const;

private:
  uint8_t _board[H][W]{};
  bool _gameOver = false;
  uint32_t _score = 0;

  // pieza actual
  uint8_t _type = 0;   // 0..6
  uint8_t _rot = 0;    // 0..3
  int _px = 4;
  int _py = 0;

  // helpers
  void spawn();
  bool collides(int nx, int ny, uint8_t ntype, uint8_t nrot) const;
  void lockPiece();
  void clearLines();
};