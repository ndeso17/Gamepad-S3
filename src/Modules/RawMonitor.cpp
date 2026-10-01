#include "../Core/GamepadCore.h"
#include "../Core/ProfileStorage.h"
#include "../Modules/Pairing.h"
#include "../Modules/ProfileManager.h"
#include "../Modules/GamepadTest.h"
#include "../Modules/RawMonitor.h"

#if ENABLE_RAW
void processRawReport(const uint8_t *data, uint16_t len) {
  // RAW monitor
  if (rawMode) {

    static uint8_t previous[MAX_REPORT] = {0};
    static uint8_t previousLength = 0;
    static bool first = true;

    if (
      first ||
      previousLength != len ||
      memcmp(
        previous,
        data,
        len
      ) != 0
    ) {

      printReport(
        data,
        len
      );

      memcpy(
        previous,
        data,
        len
      );

      previousLength = len;
      first = false;
    }
  }

}

void toggleRawMonitor() {
  testMode = false;
  rawMode = !rawMode;
  Serial.println();
  Serial.print("RAW HID: ");
  Serial.println(rawMode ? "ON" : "OFF");
}
#endif
