#pragma once
#include <Arduino.h>

// Preserve v0.7 layout: stored directly in NVS. Do not reorder or pack.
enum InputType : uint8_t {
  INPUT_NONE = 0,
  INPUT_AXIS = 1,
  INPUT_DIGITAL = 2,
  INPUT_HAT = 3
};

// ============================================================
// INPUT MAPPING
// ============================================================

struct InputMapping {
  bool valid;

  InputType type;

  uint8_t byteIndex;

  // DIGITAL
  uint8_t mask;
  uint8_t releasedValue;

  // AXIS
  uint8_t centerValue;
  uint8_t negativeValue;
  uint8_t positiveValue;
};

// ============================================================
// STICK
// ============================================================

struct StickMapping {
  InputMapping left;
  InputMapping right;
  InputMapping up;
  InputMapping down;
  InputMapping click;
};

// ============================================================
// DPAD
// ============================================================

struct DPadMapping {
  InputMapping up;
  InputMapping down;
  InputMapping left;
  InputMapping right;
};

// ============================================================
// PROFILE
// ============================================================

struct GamepadProfile {
  uint32_t magic;

  uint8_t reportLength;

  StickMapping leftStick;
  StickMapping rightStick;

  InputMapping triangle;
  InputMapping circle;
  InputMapping cross;
  InputMapping square;

  InputMapping l1;
  InputMapping r1;
  InputMapping l2;
  InputMapping r2;

  InputMapping selectButton;
  InputMapping startButton;

  DPadMapping dpad;
};
