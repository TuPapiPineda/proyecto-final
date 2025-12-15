#include "DisplayTFT.h"

DisplayTFT::DisplayTFT(TFT_eSPI &tft) : _tft(tft) {}

void DisplayTFT::begin() {
  _tft.init();
  _tft.setRotation(1);
  _tft.fillScreen(TFT_BLACK);
}

uint16_t DisplayTFT::colorFor(uint8_t v) const {
  switch (v) {
    case 1: return TFT_CYAN;
    case 2: return TFT_YELLOW;
    case 3: return TFT_MAGENTA;
    case 4: return TFT_GREEN;
    case 5: return TFT_RED;
    case 6: return TFT_BLUE;
    case 7: return TFT_ORANGE;
    default: return TFT_BLACK;
  }
}

void DisplayTFT::draw(const TetrisGame &g) {
  // UI
  _tft.fillRect(0, 0, SCREEN_W, UI_TOP, TFT_BLACK);
  _tft.setTextColor(TFT_WHITE, TFT_BLACK);
  _tft.setTextSize(2);
  _tft.drawString("TETRIS", 10, 8);
  _tft.drawString("SCORE:", 160, 8);
  _tft.setCursor(240, 10);
  _tft.print(g.score());

  // Zona tablero
  _tft.fillRect(ORIGIN_X, ORIGIN_Y, BOARD_W, BOARD_H, TFT_BLACK);
  _tft.drawRect(ORIGIN_X - 1, ORIGIN_Y - 1, BOARD_W + 2, BOARD_H + 2, TFT_DARKGREY);

  // Bloques fijos
  for (int y = 0; y < TetrisGame::H; y++) {
    for (int x = 0; x < TetrisGame::W; x++) {
      uint8_t v = g.cell(x, y);
      if (v) {
        int px = ORIGIN_X + x * BLOCK;
        int py = ORIGIN_Y + y * BLOCK;
        _tft.fillRect(px, py, BLOCK, BLOCK, colorFor(v));
        _tft.drawRect(px, py, BLOCK, BLOCK, TFT_BLACK);
      }
    }
  }

  // Pieza activa
  int ax[4], ay[4];
  g.getActiveBlocks(ax, ay);
  for (int i = 0; i < 4; i++) {
    if (ax[i] < 0 || ax[i] >= TetrisGame::W || ay[i] < 0 || ay[i] >= TetrisGame::H) continue;
    int px = ORIGIN_X + ax[i] * BLOCK;
    int py = ORIGIN_Y + ay[i] * BLOCK;
    // La pieza activa usa el color “tipo” (derivado del board sería más complejo; aquí la pintamos blanco para resaltar)
    _tft.fillRect(px, py, BLOCK, BLOCK, TFT_WHITE);
    _tft.drawRect(px, py, BLOCK, BLOCK, TFT_BLACK);
  }

  if (g.isGameOver()) {
    _tft.setTextColor(TFT_RED, TFT_BLACK);
    _tft.setTextSize(3);
    _tft.drawString("GAME OVER", 130, 140);
    _tft.setTextSize(2);
    _tft.setTextColor(TFT_WHITE, TFT_BLACK);
    _tft.drawString("FIRE = REINICIAR", 125, 180);
  }
}