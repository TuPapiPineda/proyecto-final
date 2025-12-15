#pragma once
#include <Arduino.h>

enum class Button : uint8_t {
  NONE, LEFT, UP, DOWN, RIGHT, FIRE
};

struct ButtonEvent {
  Button btn;
  bool pressed;
};

class InputADKeyboard {
public:
  InputADKeyboard(uint8_t adcPin);

  void begin();
  bool poll(ButtonEvent &ev);

  int lastAdc() const { return _lastAdc; }
  Button stableButton() const { return _stableBtn; }

private:
  uint8_t _pin;

  static constexpr int AVG_N = 8;
  int _buf[AVG_N]{};
  int _idx = 0;
  int _sum = 0;
  int _lastAdc = 4095;

  Button _rawBtn = Button::NONE;
  Button _candidateBtn = Button::NONE;
  Button _stableBtn = Button::NONE;
  uint8_t _stableCount = 0;

  uint32_t _lastEventMs = 0;
  uint32_t _lastRepeatMs = 0;
  bool _held = false;

  Button adcToButton(int adc) const;

  int readAveraged();
};