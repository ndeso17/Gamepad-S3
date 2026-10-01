#include "../../Config.h"
#include "ModemManager.h"
#if ENABLE_NETWORK
#include <Preferences.h>
namespace GamepadNetwork {
Settings defaultSettings() {
  Settings s{};
  s.version = 1;
  s.mode = Mode::Off;
  strlcpy(s.apSSID, "Gamepad-S3", sizeof(s.apSSID));
  return s;
}
static bool valid(const Settings &s) {
  return s.version == 1 && static_cast<uint8_t>(s.mode) <= 3 &&
      memchr(s.apSSID, 0, sizeof(s.apSSID)) && memchr(s.apPassword, 0, sizeof(s.apPassword)) &&
      memchr(s.clientSSID, 0, sizeof(s.clientSSID)) && memchr(s.clientPassword, 0, sizeof(s.clientPassword));
}
bool loadSettings(Settings &settings) {
  settings = defaultSettings();
  Preferences p;
  if (!p.begin("gp-network", true)) return false;
  Settings loaded{};
  bool ok = p.getBytesLength("settings") == sizeof(loaded) && p.getBytes("settings", &loaded, sizeof(loaded)) == sizeof(loaded);
  p.end();
  if (ok && valid(loaded)) { settings = loaded; return true; }
  return false;
}
bool saveSettings(const Settings &settings) {
  if (!valid(settings)) return false;
  Preferences p;
  if (!p.begin("gp-network", false)) return false;
  bool ok = p.putBytes("settings", &settings, sizeof(settings)) == sizeof(settings);
  p.end();
  return ok;
}
const char *modeName(Mode mode) {
  switch(mode) { case Mode::AP: return "ap"; case Mode::Client: return "client"; case Mode::Auto: return "auto"; default: return "off"; }
}
}
#endif
