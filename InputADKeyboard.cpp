#include "InputADKeyboard.h"

InputADKeyboard::InputADKeyboard(uint8_t adcPin) : _pin(adcPin) {}

void InputADKeyboard::begin() {
  pinMode(_pin, INPUT);
  int v = analogRead(_pin);
  _sum = 0;
  for (int i = 0; i < AVG_N; i++)
  { _buf[i] = v;
    _sum += v; 
  }
  _idx = 0;
  _lastAdc = v;
  _rawBtn = _candidateBtn = _stableBtn = Button::NONE;
  _stableCount = 0;
  _lastEventMs = _lastRepeatMs = millis();
  _held = false;
}

int InputADKeyboard::readAveraged() {
  int v = analogRead(_pin);
  _lastAdc = v;

  _sum -= _buf[_idx];
  _buf[_idx] = v;
  _sum += v;
  _idx = (_idx + 1) % AVG_N;

  return _sum / AVG_N;
}

// Mapeo con “bandas” separadas (tus rangos) + un poco de margen implícito por promedio+debounce
Button InputADKeyboard::adcToButton(int adc) const {
  // IZQ: 0–300, ARRIBA: 300–900, ABAJO: 900–1600, DER: 1600–2600, FIRE: >2600
  if (adc < 250) return Button::LEFT;       // margen para evitar ruido en borde
  if (adc < 850) return Button::UP;
  if (adc < 1550) return Button::DOWN;
  if (adc < 2500) return Button::RIGHT;
  if (adc > 3000) return Button::FIRE;      // deja “zona muerta” 2500–3000
  return Button::NONE;                      // zona muerta para evitar cruces
}

bool InputADKeyboard::poll(ButtonEvent &ev) {
  ev = {Button::NONE, false};

  const uint32_t now = millis();
  const int avg = readAveraged();
  _rawBtn = adcToButton(avg);

  // ---- Debounce por estabilidad (3 lecturas iguales) ----
  if (_rawBtn != _candidateBtn) {
    _candidateBtn = _rawBtn;
    _stableCount = 0;
  } else {
    if (_stableCount < 255) _stableCount++;
  }

  const bool becameStable = (_stableCount == 3); // ~3*20ms=60ms si poll cada 20ms

  // ---- Generación de evento de “press” + control de repeat ----
  // Repeat: al mantener, un delay inicial y luego repetición más rápida.
  const uint32_t initialDelay = 180;
  const uint32_t repeatEvery  = 90;

  if (becameStable) {
    if (_stableBtn != _candidateBtn) {
      _stableBtn = _candidateBtn;
      _held = (_stableBtn != Button::NONE);
      _lastEventMs = now;
      _lastRepeatMs = now;

      if (_stableBtn != Button::NONE) {
        ev = {_stableBtn, true};
        return true;
      }
      return false;
    }

    // Si se mantiene presionado, repetimos para movimiento lateral/abajo sin “cruces”
    if (_stableBtn != Button::NONE) {
      if (!_held) {
        _held = true;
        _lastEventMs = now;
        _lastRepeatMs = now;
        ev = {_stableBtn, true};
        return true;
      } else {
        // esperar delay inicial
        if ((now - _lastEventMs) > initialDelay && (now - _lastRepeatMs) > repeatEvery) {
          _lastRepeatMs = now;
          ev = {_stableBtn, true};
          return true;
        }
      }
    } else {
      _held = false;
    }
  }

  return false;
} 