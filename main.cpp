#include <Arduino.h>
#include <TFT_eSPI.h>
#include "InputADKeyboard.h"
#include "TetrisGame.h"
#include "DisplayTFT.h"

#define AD_PIN 33

TFT_eSPI tft = TFT_eSPI();
InputADKeyboard input(AD_PIN);
TetrisGame game;
DisplayTFT display(tft);

// Cola de eventos de botones
static QueueHandle_t qButtons;

// Mutex para TFT (por si luego agregas más tareas que dibujen)
static SemaphoreHandle_t mTFT;

static void TaskInput(void *pv) {
  input.begin();
  ButtonEvent ev;

  for (;;) {
    if (input.poll(ev)) {
      // enviamos solo eventos de “press”
      xQueueSend(qButtons, &ev, 0);
    }
    vTaskDelay(pdMS_TO_TICKS(20)); // 20ms -> debounce estable
  }
}

static void TaskGame(void *pv) {
  const uint32_t tickMs = 450;     // caída normal
  uint32_t lastTick = millis();

  game.reset();

  for (;;) {
    // Procesar botones disponibles
    ButtonEvent ev;
    while (xQueueReceive(qButtons, &ev, 0) == pdTRUE) {
      if (ev.pressed) game.handleButton(ev.btn);
    }

    // caída automática
    const uint32_t now = millis();
    if (!game.isGameOver() && (now - lastTick) >= tickMs) {
      lastTick = now;
      game.tick();
    }

    // dibujar
    if (xSemaphoreTake(mTFT, pdMS_TO_TICKS(50)) == pdTRUE) {
      display.draw(game);
      xSemaphoreGive(mTFT);
    }

    vTaskDelay(pdMS_TO_TICKS(33)); // ~30 FPS (suficiente)
  }
}

void setup() {
  Serial.begin(115200);
  delay(200);

  display.begin();

  mTFT = xSemaphoreCreateMutex();
  qButtons = xQueueCreate(12, sizeof(ButtonEvent));

  // Tareas en diferentes cores (ESP32)
  xTaskCreatePinnedToCore(TaskInput, "Input", 4096, nullptr, 2, nullptr, 0);
  xTaskCreatePinnedToCore(TaskGame,  "Game",  8192, nullptr, 1, nullptr, 1);
}

void loop() {
  // vacío: todo corre en FreeRTOS tasks
  vTaskDelay(pdMS_TO_TICKS(1000));
}