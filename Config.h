#pragma once
#include <Arduino.h>

// Development defaults. Set 0 to remove a module and its commands.
// Hardware pins must be verified against your exact board before enabling.
#ifndef ENABLE_PAIRING
#define ENABLE_PAIRING 1
#endif
#ifndef ENABLE_RAW
#define ENABLE_RAW 1
#endif
#ifndef ENABLE_PROFILE
#define ENABLE_PROFILE 1
#endif
#ifndef ENABLE_TEST
#define ENABLE_TEST 1
#endif
#ifndef ENABLE_ERASE
#define ENABLE_ERASE 1
#endif
#ifndef ENABLE_CONFIG_COMMAND
#define ENABLE_CONFIG_COMMAND 1
#endif
#ifndef ENABLE_NETWORK
#define ENABLE_NETWORK 1
#endif
#ifndef ENABLE_LINK
#define ENABLE_LINK 0
#endif
#ifndef ENABLE_LINK_ESP_NOW
#define ENABLE_LINK_ESP_NOW 0
#endif
#ifndef ENABLE_BUTTON
#define ENABLE_BUTTON 0
#endif
#ifndef ENABLE_NEOPIXEL
#define ENABLE_NEOPIXEL 0
#endif
#ifndef ENABLE_DISPLAY
#define ENABLE_DISPLAY 0
#endif
#ifndef BUTTON_PIN
#define BUTTON_PIN -1
#endif
#ifndef NEOPIXEL_PIN
#define NEOPIXEL_PIN -1
#endif

static const uint8_t MAX_REPORT = 32;

static const uint32_t PROFILE_MAGIC =
  0x47503730; // GP70

static const uint32_t ACTION_TIMEOUT = 15000;
static const uint32_t NEUTRAL_TIMEOUT = 15000;

static const uint16_t NEUTRAL_STABLE_MS = 350;

static const int AXIS_DEADZONE = 7;
static const int AXIS_PRINT_STEP = 3;
