#include "../../Config.h"
#include "NeoPixelManager.h"
#include <Arduino.h>
#if ENABLE_NEOPIXEL
#if NEOPIXEL_PIN < 0
#error "Set NEOPIXEL_PIN to the verified onboard RGB LED GPIO before enabling NeoPixel."
#endif
#include <driver/gpio.h>
static_assert(GPIO_IS_VALID_OUTPUT_GPIO(NEOPIXEL_PIN), "NEOPIXEL_PIN is not an output GPIO");
void neoPixelSet(uint8_t red, uint8_t green, uint8_t blue) { rgbLedWrite(NEOPIXEL_PIN, red, green, blue); }
void neoPixelBegin() { neoPixelSet(0, 0, 0); }
void neoPixelTick() {}
#else
void neoPixelSet(uint8_t, uint8_t, uint8_t) {}
void neoPixelBegin() {}
void neoPixelTick() {}
#endif
