#include "../Core/GamepadCore.h"
#include "../Core/ProfileStorage.h"
#include "../Modules/Pairing.h"
#include "../Modules/ProfileManager.h"
#include "../Modules/GamepadTest.h"
#include "../Modules/RawMonitor.h"

#include <USBHost.h>
#include <USBHostHIDGamepad.h>

USBHostHIDGamepad Gamepad;
uint8_t currentReport[MAX_REPORT] = {0};
uint8_t neutralReport[MAX_REPORT] = {0};

uint8_t currentLength = 0;
uint8_t neutralLength = 0;

bool haveReport = false;
bool haveNeutral = false;

// ============================================================
// MODES
// ============================================================

bool rawMode = false;
bool testMode = false;

void separator() {
  Serial.println();
  Serial.println(
    "========================================"
  );
}

void smallSeparator() {
  Serial.println(
    "----------------------------------------"
  );
}

void printReport(
  const uint8_t *data,
  uint8_t len
) {
  Serial.print("HID [");
  Serial.print(len);
  Serial.print("]: ");

  for (uint8_t i = 0; i < len; i++) {

    if (data[i] < 0x10) {
      Serial.print("0");
    }

    Serial.print(
      data[i],
      HEX
    );

    if (i < len - 1) {
      Serial.print(" ");
    }
  }

  Serial.println();
}

void updateHID() {

  USBHost.task();

  if (!Gamepad.available()) {
    return;
  }

  uint16_t len =
    Gamepad.reportLength();

  if (len > MAX_REPORT) {
    len = MAX_REPORT;
  }

  const uint8_t *data =
    Gamepad.reportData();

  memcpy(
    currentReport,
    data,
    len
  );

  currentLength = len;

  haveReport = true;

#if ENABLE_RAW
  processRawReport(data, len);
#endif

  Gamepad.clear();
}

bool reportIsNeutral() {

  if (
    !haveReport ||
    !haveNeutral
  ) {
    return false;
  }

  if (
    currentLength !=
    neutralLength
  ) {
    return false;
  }

  return memcmp(
    currentReport,
    neutralReport,
    neutralLength
  ) == 0;
}

uint8_t countChangedBytes(
  const uint8_t *base,
  const uint8_t *changed,
  uint8_t len
) {

  uint8_t count = 0;

  for (
    uint8_t i = 0;
    i < len;
    i++
  ) {

    if (
      base[i] !=
      changed[i]
    ) {
      count++;
    }
  }

  return count;
}

int firstChangedByte(
  const uint8_t *base,
  const uint8_t *changed,
  uint8_t len
) {

  for (
    uint8_t i = 0;
    i < len;
    i++
  ) {

    if (
      base[i] !=
      changed[i]
    ) {
      return i;
    }
  }

  return -1;
}

bool digitalPressed(
  const InputMapping &input
) {

  if (
    !input.valid ||
    input.type !=
      INPUT_DIGITAL ||
    input.byteIndex >=
      currentLength
  ) {
    return false;
  }

  uint8_t difference =
    currentReport[
      input.byteIndex
    ] ^
    input.releasedValue;

  return (
    difference &
    input.mask
  ) != 0;
}

int axisValue(
  const InputMapping &input
) {

  if (
    !input.valid ||
    input.type !=
      INPUT_AXIS ||
    input.byteIndex >=
      currentLength
  ) {
    return 0;
  }

  int raw =
    currentReport[
      input.byteIndex
    ];

  int center =
    input.centerValue;

  int negative =
    input.negativeValue;

  int positive =
    input.positiveValue;

  int result = 0;

  bool normal =
    negative < center &&
    positive > center;

  bool reverse =
    negative > center &&
    positive < center;

  if (normal) {

    if (raw < center) {

      int range =
        center - negative;

      if (range > 0) {

        result =
          -(
            (center - raw) *
            100
          ) / range;
      }

    } else if (
      raw > center
    ) {

      int range =
        positive - center;

      if (range > 0) {

        result =
          (
            (raw - center) *
            100
          ) / range;
      }
    }

  } else if (reverse) {

    if (raw > center) {

      int range =
        negative - center;

      if (range > 0) {

        result =
          -(
            (raw - center) *
            100
          ) / range;
      }

    } else if (
      raw < center
    ) {

      int range =
        center - positive;

      if (range > 0) {

        result =
          (
            (center - raw) *
            100
          ) / range;
      }
    }
  }

  result =
    constrain(
      result,
      -100,
      100
    );

  if (
    abs(result) <=
    AXIS_DEADZONE
  ) {
    result = 0;
  }

  return result;
}

bool directionPressed(
  const InputMapping &input,
  bool negativeDirection
) {

  if (!input.valid) {
    return false;
  }

  if (
    input.type ==
    INPUT_DIGITAL
  ) {

    return digitalPressed(
      input
    );
  }

  if (
    input.type ==
    INPUT_AXIS
  ) {

    int value =
      axisValue(
        input
      );

    if (negativeDirection) {
      return value < -50;
    }

    return value > 50;
  }

  return false;
}

void gamepadBegin() {

  Serial.begin(
    115200
  );

  delay(1200);

  separator();

  Serial.println(
    "             Gamepad-S3"
  );

  Serial.println(
    "      Adaptive HID Controller"
  );

  Serial.println(
    "               v0.7"
  );

  separator();

  clearProfile();

  Gamepad.registerWithHost();

  if (
    !USBHost.begin()
  ) {

    Serial.println();

    Serial.println(
      "[ERROR] USB Host gagal."
    );

    while (true) {
      delay(1000);
    }
  }

  Serial.println();

  Serial.println(
    "[OK] USB Host aktif."
  );

  Serial.println(
    "[INFO] Hubungkan gamepad."
  );

  // ----------------------------------------------------------
  // LOAD NVS
  // ----------------------------------------------------------

  if (
    loadProfile()
  ) {

    Serial.println();

    Serial.println(
      "[OK] Profile v0.7 ditemukan."
    );

    Serial.println();

#if ENABLE_TEST
    Serial.println("Ketik 'test' untuk pengujian.");
#endif

  } else {

    Serial.println();

    Serial.println(
      "[INFO] Profile v0.7 belum tersedia."
    );

    Serial.println();

#if ENABLE_PAIRING
    Serial.println("Ketik 'pair' untuk pairing.");
#endif
  }


}
