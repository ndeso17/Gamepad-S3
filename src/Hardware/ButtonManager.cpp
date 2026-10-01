#include "../../Config.h"
#include "ButtonManager.h"
#include <Arduino.h>
#if ENABLE_BUTTON
#if BUTTON_PIN < 0
#error "Set BUTTON_PIN to a verified free GPIO before enabling the button."
#endif
#include <driver/gpio.h>
static_assert(GPIO_IS_VALID_GPIO(BUTTON_PIN), "BUTTON_PIN is not a valid GPIO");
namespace {
bool lastRaw = true, stable = true;
uint32_t lastChange = 0, pressedAt = 0;
}
void buttonBegin() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  lastRaw = digitalRead(BUTTON_PIN);
  stable = true;
  lastChange = millis();
}
void buttonTick() {
  bool raw = digitalRead(BUTTON_PIN);
  uint32_t now = millis();
  if (raw != lastRaw) { lastRaw = raw; lastChange = now; }
  if (raw != stable && now - lastChange >= 30) {
    stable = raw;
    if (!stable) pressedAt = now;
  }
  if (!stable && now - pressedAt >= 2000) ESP.restart();
}
#else
void buttonBegin() {}
void buttonTick() {}
#endif
