#include "TetrisGame.h"

static const int8_t SHAPES[7][4][4][2] = {
  // I
  {
    {{0,1},{1,1},{2,1},{3,1}},
    {{2,0},{2,1},{2,2},{2,3}},
    {{0,2},{1,2},{2,2},{3,2}},
    {{1,0},{1,1},{1,2},{1,3}}
  },
  // O
  {
    {{1,1},{2,1},{1,2},{2,2}},
    {{1,1},{2,1},{1,2},{2,2}},
    {{1,1},{2,1},{1,2},{2,2}},
    {{1,1},{2,1},{1,2},{2,2}}
  },
  // T
  {
    {{1,1},{0,2},{1,2},{2,2}},
    {{1,1},{1,2},{2,2},{1,3}},
    {{0,2},{1,2},{2,2},{1,3}},
    {{1,1},{0,2},{1,2},{1,3}}
  },
  // S
  {
    {{1,1},{2,1},{0,2},{1,2}},
    {{1,1},{1,2},{2,2},{2,3}},
    {{1,2},{2,2},{0,3},{1,3}},
    {{0,1},{0,2},{1,2},{1,3}}
  },
  // Z
  {
    {{0,1},{1,1},{1,2},{2,2}},
    {{2,1},{1,2},{2,2},{1,3}},
    {{0,2},{1,2},{1,3},{2,3}},
    {{1,1},{0,2},{1,2},{0,3}}
  },
  // J
  {
    {{0,1},{0,2},{1,2},{2,2}},
    {{1,1},{2,1},{1,2},{1,3}},
    {{0,2},{1,2},{2,2},{2,3}},
    {{1,1},{1,2},{0,3},{1,3}}
  },
  // L
  {
    {{2,1},{0,2},{1,2},{2,2}},
    {{1,1},{1,2},{1,3},{2,3}},
    {{0,2},{1,2},{2,2},{0,3}},
    {{0,1},{1,1},{1,2},{1,3}}
  }
};

void TetrisGame::reset() {
  memset(_board, 0, sizeof(_board));
  _gameOver = false;
  _score = 0;
  randomSeed(analogRead(0));
  spawn();
}

void TetrisGame::spawn() {
  _type = random(0, 7);
  _rot = 0;
  _px = 3;  // caja 4x4, centrada
  _py = 0;

  if (collides(_px, _py, _type, _rot)) {
    _gameOver = true;
  }
}

bool TetrisGame::collides(int nx, int ny, uint8_t ntype, uint8_t nrot) const {
  for (int i = 0; i < 4; i++) {
    int x = nx + SHAPES[ntype][nrot][i][0];
    int y = ny + SHAPES[ntype][nrot][i][1];

    if (x < 0 || x >= W || y < 0 || y >= H) return true;
    if (_board[y][x] != 0) return true;
  }
  return false;
}

void TetrisGame::lockPiece() {
  for (int i = 0; i < 4; i++) {
    int x = _px + SHAPES[_type][_rot][i][0];
    int y = _py + SHAPES[_type][_rot][i][1];
    if (x >= 0 && x < W && y >= 0 && y < H) {
      _board[y][x] = _type + 1; // 1..7
    }
  }
  clearLines();
  spawn();
}

void TetrisGame::clearLines() {
  int cleared = 0;
  for (int y = H - 1; y >= 0; y--) {
    bool full = true;
    for (int x = 0; x < W; x++) {
      if (_board[y][x] == 0) { full = false; break; }
    }
    if (full) {
      cleared++;
      // bajar todo
      for (int yy = y; yy > 0; yy--) {
        memcpy(_board[yy], _board[yy - 1], W);
      }
      memset(_board[0], 0, W);
      y++; // re-check misma fila tras bajar
    }
  }

  if (cleared > 0) {
    // puntuación simple
    static const uint16_t pts[5] = {0, 100, 300, 500, 800};
    _score += pts[min(cleared, 4)];
  }
}

void TetrisGame::tick() {
  if (_gameOver) return;

  if (!collides(_px, _py + 1, _type, _rot)) {
    _py++;
  } else {
    lockPiece();
  }
}

void TetrisGame::handleButton(Button b) {
  if (b == Button::NONE) return;

  if (_gameOver) {
    if (b == Button::FIRE) reset();
    return;
  }

  if (b == Button::LEFT) {
    if (!collides(_px - 1, _py, _type, _rot)) _px--;
  } else if (b == Button::RIGHT) {
    if (!collides(_px + 1, _py, _type, _rot)) _px++;
  } else if (b == Button::DOWN) {
    // soft drop
    if (!collides(_px, _py + 1, _type, _rot)) _py++;
    else lockPiece();
  } else if (b == Button::UP) {
    // rotate (simple) con “wall kick” básico
    uint8_t nr = (_rot + 1) & 3;
    if (!collides(_px, _py, _type, nr)) {
      _rot = nr;
    } else if (!collides(_px - 1, _py, _type, nr)) {
      _px--; _rot = nr;
    } else if (!collides(_px + 1, _py, _type, nr)) {
      _px++; _rot = nr;
    }
  } else if (b == Button::FIRE) {
    // si quieres hard drop, descomenta:
    // while (!collides(_px, _py + 1, _type, _rot)) _py++;
    // lockPiece();
  }
}

void TetrisGame::getActiveBlocks(int outX[4], int outY[4]) const {
  for (int i = 0; i < 4; i++) {
    outX[i] = _px + SHAPES[_type][_rot][i][0];
    outY[i] = _py + SHAPES[_type][_rot][i][1];
  }
}