#pragma once

// Dual-compile: Arduino (ESP32 core) dan host (g++ test).
#ifdef ARDUINO
#include <Arduino.h>
#else
#include <cstdint>
#include <cstddef>
#endif

namespace GamepadLink {

// Transport link. Hanya ESP-NOW untuk F1-F4. BLE ditunda (F5).
enum class LinkMode : uint8_t {
  Off = 0,
  EspNow = 1
};

// State machine link, lihat Architecture.md §5.
enum class LinkState : uint8_t {
  Off = 0,
  Pairing = 1,
  Locked = 2,
  Armed = 3,
  LinkLost = 4
};

// Bitmask buttons, PRD §9.
enum CtrlButton : uint8_t {
  BTN_BRAKE_SOFT = 1 << 0,  // L1
  BTN_BRAKE_HARD = 1 << 1,  // L2
  BTN_GEAR_UP = 1 << 2,     // R1
  BTN_GEAR_DOWN = 1 << 3,   // R2
  BTN_HORN = 1 << 4,
  BTN_RESERVE = 1 << 5
};

// Kode gear. 0 netral, 1..3 maju, 255 mundur.
enum CtrlGear : uint8_t {
  GEAR_NEUTRAL = 0,
  GEAR_1 = 1,
  GEAR_2 = 2,
  GEAR_3 = 3,
  GEAR_REVERSE = 255
};

static const uint8_t CTRL_MAGIC = 0x43;  // 'C'
static const uint8_t CTRL_VERSION = 1;
static const int8_t CTRL_STEER_MIN = -127;
static const int8_t CTRL_STEER_MAX = 127;
static const int8_t CTRL_THROTTLE_MIN = 0;
static const int8_t CTRL_THROTTLE_MAX = 100;

// Header 9 byte + crc 2 byte, natural packing.
struct CtrlPacket {
  uint8_t  magic;
  uint8_t  version;
  uint8_t  carId;
  uint8_t  seq;
  int8_t   steer;
  int8_t   throttle;
  uint8_t  gear;
  uint8_t  buttons;
  uint8_t  arm;
  uint16_t crc;
};

// Ukuran natural = 12 byte. PRD §9 menyebut 17; lihat Log WP-01.
static_assert(sizeof(CtrlPacket) == 12, "CtrlPacket layout changed");

// Binding 1:1, NVS namespace "gp-link".
struct CarLink {
  uint8_t version;
  bool    locked;
  uint8_t carMac[6];
  uint8_t psk[16];
  uint8_t channel;
  uint8_t lastSeq;
};

static const uint8_t CARLINK_VERSION = 1;
static const uint8_t CARLINK_DEFAULT_CHANNEL = 1;

const char *linkModeName(LinkMode mode);
const char *linkStateName(LinkState state);

// CRC16-CCITT, poly 0x1021, init 0xFFFF, tanpa reflect.
uint16_t ctrlCrc(const CtrlPacket &p);

// Padding pkt dengan crc, lalu validasi magic+versi+crc.
void ctrlSeal(CtrlPacket &p);
bool ctrlValid(const CtrlPacket &p);

// Reset field kontrol ke netral. Tidak menyentuh magic/versi/seq/crc.
void ctrlNeutral(CtrlPacket &p);

}  // namespace GamepadLink
