#pragma once
#include <Arduino.h>
namespace GamepadNetwork {
enum class Mode : uint8_t { Off, AP, Client, Auto };
struct Settings {
  uint32_t version;
  Mode mode;
  char apSSID[33];
  char apPassword[65];
  char clientSSID[33];
  char clientPassword[65];
};
Settings defaultSettings();
bool loadSettings(Settings &settings);
bool saveSettings(const Settings &settings);
const char *modeName(Mode mode);
}
