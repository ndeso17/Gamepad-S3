#pragma once

// NVS "gp-link" untuk binding remote-mobil 1:1. Terpisah dari "gamepad" (profil)
// dan "gp-network" (Wi-Fi). Hanya satu key: "link".
#ifdef ARDUINO
#include <Arduino.h>
#else
#include <cstdint>
#endif

#include "LinkTypes.h"

namespace GamepadLink {

CarLink defaultCarLink();

// Load binding tersimpan. Selalu mengisi out.
// Return true hanya bila NVS punya data valid versi sekarang.
bool loadCarLink(CarLink &out);

// Simpan binding. Tolak data versi salah atau tidak konsisten.
bool saveCarLink(const CarLink &link);

// Kosongkan binding: set default dan simpan. Hanya menyentuh "gp-link".
bool unbindCarLink();

}  // namespace GamepadLink
