#include "../../Config.h"
#if ENABLE_LINK
#include "LinkStorage.h"
#include <Preferences.h>
#include <cstring>

namespace GamepadLink {

CarLink defaultCarLink() {
  CarLink link{};
  link.version = CARLINK_VERSION;
  link.channel = CARLINK_DEFAULT_CHANNEL;
  return link;
}

bool loadCarLink(CarLink &out) {
  out = defaultCarLink();
  Preferences p;
  if (!p.begin("gp-link", true)) return false;
  unsigned char bytes[sizeof(CarLink)]{};
  bool ok = p.getBytesLength("link") == sizeof(bytes) &&
      p.getBytes("link", bytes, sizeof(bytes)) == sizeof(bytes);
  p.end();
  if (!ok) return false;
  if (bytes[offsetof(CarLink, version)] != CARLINK_VERSION) {
#ifdef ARDUINO
    Serial.println("[link] NVS version unsupported");
#endif
    return false;
  }
  // Validate the stored bool representation before copying it into CarLink.
  if (bytes[offsetof(CarLink, locked)] > 1) return false;
  std::memcpy(&out, bytes, sizeof(out));
  return true;
}

bool saveCarLink(const CarLink &link) {
  if (link.version != CARLINK_VERSION) return false;
  Preferences p;
  if (!p.begin("gp-link", false)) return false;
  bool ok = p.putBytes("link", &link, sizeof(link)) == sizeof(link);
  p.end();
  return ok;
}

bool unbindCarLink() {
  return saveCarLink(defaultCarLink());
}

}  // namespace GamepadLink

#endif
