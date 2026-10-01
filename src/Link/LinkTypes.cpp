#include "../../Config.h"
#if ENABLE_LINK
#include "LinkTypes.h"

namespace GamepadLink {

namespace {

// CRC16-CCITT: poly 0x1021, init 0xFFFF, tanpa reflect, tanpa xor-out.
uint16_t crcByte(uint8_t value, uint16_t crc) {
  crc ^= static_cast<uint16_t>(value) << 8;
  for (uint8_t bit = 0; bit < 8; bit++) {
    if (crc & 0x8000) {
      crc = static_cast<uint16_t>((crc << 1) ^ 0x1021);
    } else {
      crc = static_cast<uint16_t>(crc << 1);
    }
  }
  return crc;
}

// CRC dihitung atas field yang dikirim, crc itu sendiri dikecualikan.
constexpr size_t crcOffset = offsetof(CtrlPacket, crc);
constexpr size_t crcSize = sizeof(uint16_t);

}  // namespace

const char *linkModeName(LinkMode mode) {
  switch (mode) {
    case LinkMode::EspNow:
      return "espnow";
    default:
      return "off";
  }
}

const char *linkStateName(LinkState state) {
  switch (state) {
    case LinkState::Pairing:
      return "pairing";
    case LinkState::Locked:
      return "locked";
    case LinkState::Armed:
      return "armed";
    case LinkState::LinkLost:
      return "link_lost";
    default:
      return "off";
  }
}

uint16_t ctrlCrc(const CtrlPacket &p) {
  const uint8_t *raw = reinterpret_cast<const uint8_t *>(&p);
  uint16_t crc = 0xFFFF;
  for (size_t i = 0; i < crcOffset; i++) {
    crc = crcByte(raw[i], crc);
  }
  (void)crcSize;
  return crc;
}

void ctrlSeal(CtrlPacket &p) {
  p.crc = ctrlCrc(p);
}

bool ctrlValid(const CtrlPacket &p) {
  if (p.magic != CTRL_MAGIC) {
    return false;
  }
  if (p.version != CTRL_VERSION) {
    return false;
  }
  return p.crc == ctrlCrc(p);
}

void ctrlNeutral(CtrlPacket &p) {
  p.steer = 0;
  p.throttle = 0;
  p.gear = GEAR_NEUTRAL;
  p.buttons = 0;
  p.arm = 0;
}

}  // namespace GamepadLink

#endif
