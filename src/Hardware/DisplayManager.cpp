#include "../../Config.h"
#include "DisplayManager.h"
#include <Arduino.h>
void __attribute__((weak)) gamepadDisplayBeginHook() {
#if ENABLE_DISPLAY
  Serial.println("Display enabled, but no board-specific display driver is installed.");
#endif
}
void __attribute__((weak)) gamepadDisplayTickHook() {}
void displayBegin() {
#if ENABLE_DISPLAY
  gamepadDisplayBeginHook();
#endif
}
void displayTick() {
#if ENABLE_DISPLAY
  gamepadDisplayTickHook();
#endif
}
