#include "../Core/GamepadCore.h"
#include "../Core/ProfileStorage.h"
#include "../Modules/Pairing.h"
#include "../Modules/ProfileManager.h"
#include "../Modules/GamepadTest.h"
#include "../Modules/RawMonitor.h"

#if ENABLE_PAIRING
bool waitForNeutral(
  uint32_t timeout
) {

  unsigned long start =
    millis();

  unsigned long stableSince = 0;

  while (
    millis() - start <
    timeout
  ) {

    updateHID();

    if (reportIsNeutral()) {

      if (
        stableSince == 0
      ) {
        stableSince =
          millis();
      }

      if (
        millis() -
        stableSince >=
        NEUTRAL_STABLE_MS
      ) {
        return true;
      }

    } else {

      stableSince = 0;
    }

    delay(2);
  }

  return false;
}

bool captureAction(
  uint8_t *captured,
  uint8_t &capturedLength,
  uint32_t timeout
) {

  unsigned long start =
    millis();

  while (
    millis() - start <
    timeout
  ) {

    updateHID();

    if (
      !haveReport ||
      !haveNeutral
    ) {
      delay(2);
      continue;
    }

    if (
      currentLength !=
      neutralLength
    ) {
      delay(2);
      continue;
    }

    if (
      memcmp(
        currentReport,
        neutralReport,
        neutralLength
      ) != 0
    ) {

      // Give report a little time
      // to stabilize.
      delay(40);

      updateHID();

      memcpy(
        captured,
        currentReport,
        currentLength
      );

      capturedLength =
        currentLength;

      return true;
    }

    delay(2);
  }

  return false;
}

bool captureNeutral() {

  separator();

  Serial.println(
    "LANGKAH AWAL"
  );

  Serial.println();

  Serial.println(
    "1. Pastikan gamepad terhubung."
  );

  Serial.println(
    "2. Aktifkan lampu/mode ANALOG."
  );

  Serial.println(
    "3. Lepaskan SEMUA kontrol."
  );

  Serial.println(
    "4. Jangan sentuh stik atau D-Pad."
  );

  Serial.println();

  Serial.println(
    "Merekam posisi netral..."
  );

  haveNeutral = false;

  unsigned long start =
    millis();

  while (
    millis() - start <
    1500
  ) {

    updateHID();
    delay(2);
  }

  if (!haveReport) {

    Serial.println();

    Serial.println(
      "[GAGAL] Tidak ada report HID."
    );

    return false;
  }

  neutralLength =
    currentLength;

  memcpy(
    neutralReport,
    currentReport,
    neutralLength
  );

  haveNeutral = true;

  Serial.println();

  Serial.println(
    "[OK] Posisi netral direkam."
  );

  Serial.print(
    "Neutral : "
  );

  printReport(
    neutralReport,
    neutralLength
  );

  return true;
}

InputMapping createDigitalMapping(
  const uint8_t *actionReport
) {

  InputMapping result = {};

  if (
    countChangedBytes(
      neutralReport,
      actionReport,
      neutralLength
    ) != 1
  ) {
    return result;
  }

  int index =
    firstChangedByte(
      neutralReport,
      actionReport,
      neutralLength
    );

  if (index < 0) {
    return result;
  }

  uint8_t mask =
    neutralReport[index] ^
    actionReport[index];

  if (mask == 0) {
    return result;
  }

  result.valid = true;

  result.type =
    INPUT_DIGITAL;

  result.byteIndex =
    index;

  result.mask =
    mask;

  result.releasedValue =
    neutralReport[index];

  return result;
}

bool capturePhysicalAction(
  const char *instruction,
  uint8_t *report
) {

  uint8_t length = 0;

  Serial.println();

  Serial.print(">>> ");
  Serial.println(
    instruction
  );

  Serial.println(
    "    Tahan sebentar..."
  );

  if (
    !captureAction(
      report,
      length
    )
  ) {

    Serial.println();

    Serial.println(
      "[GAGAL] Input tidak terdeteksi."
    );

    return false;
  }

  Serial.println();

  Serial.println(
    "[OK] Input terdeteksi."
  );

  Serial.println();

  Serial.println(
    ">>> LEPASKAN kontrol."
  );

  if (
    !waitForNeutral()
  ) {

    Serial.println();

    Serial.println(
      "[GAGAL] Tidak kembali netral."
    );

    return false;
  }

  Serial.println(
    "[OK] Netral."
  );

  delay(250);

  return true;
}

bool detectAxisPair(
  const uint8_t *negativeReport,
  const uint8_t *positiveReport,
  InputMapping &negative,
  InputMapping &positive
) {

  if (
    countChangedBytes(
      neutralReport,
      negativeReport,
      neutralLength
    ) != 1
  ) {
    return false;
  }

  if (
    countChangedBytes(
      neutralReport,
      positiveReport,
      neutralLength
    ) != 1
  ) {
    return false;
  }

  int negativeByte =
    firstChangedByte(
      neutralReport,
      negativeReport,
      neutralLength
    );

  int positiveByte =
    firstChangedByte(
      neutralReport,
      positiveReport,
      neutralLength
    );

  if (
    negativeByte < 0 ||
    positiveByte < 0 ||
    negativeByte !=
      positiveByte
  ) {
    return false;
  }

  uint8_t index =
    negativeByte;

  uint8_t center =
    neutralReport[index];

  uint8_t negativeValue =
    negativeReport[index];

  uint8_t positiveValue =
    positiveReport[index];

  // Axis should cross center.
  bool centerBetween =
    (
      negativeValue <
        center &&
      positiveValue >
        center
    )
    ||
    (
      positiveValue <
        center &&
      negativeValue >
        center
    );

  if (!centerBetween) {
    return false;
  }

  negative = {};
  positive = {};

  negative.valid = true;

  negative.type =
    INPUT_AXIS;

  negative.byteIndex =
    index;

  negative.centerValue =
    center;

  negative.negativeValue =
    negativeValue;

  negative.positiveValue =
    positiveValue;

  positive = negative;

  return true;
}

bool mapDirectionPair(
  const char *controlName,
  const char *negativeName,
  const char *positiveName,
  InputMapping &negativeMap,
  InputMapping &positiveMap
) {

  uint8_t negativeReport[
    MAX_REPORT
  ] = {0};

  uint8_t positiveReport[
    MAX_REPORT
  ] = {0};

  char instruction[100];

  snprintf(
    instruction,
    sizeof(instruction),
    "%s ke %s",
    controlName,
    negativeName
  );

  if (
    !capturePhysicalAction(
      instruction,
      negativeReport
    )
  ) {
    return false;
  }

  snprintf(
    instruction,
    sizeof(instruction),
    "%s ke %s",
    controlName,
    positiveName
  );

  if (
    !capturePhysicalAction(
      instruction,
      positiveReport
    )
  ) {
    return false;
  }

  // ----------------------------------------------------------
  // Try ANALOG
  // ----------------------------------------------------------

  if (
    detectAxisPair(
      negativeReport,
      positiveReport,
      negativeMap,
      positiveMap
    )
  ) {

    Serial.println();

    Serial.print(
      "[OK] "
    );

    Serial.print(
      controlName
    );

    Serial.print(" ");

    Serial.print(
      negativeName
    );

    Serial.print("/");

    Serial.print(
      positiveName
    );

    Serial.println(
      " = ANALOG"
    );

    return true;
  }

  // ----------------------------------------------------------
  // Try DIGITAL
  // ----------------------------------------------------------

  negativeMap =
    createDigitalMapping(
      negativeReport
    );

  positiveMap =
    createDigitalMapping(
      positiveReport
    );

  if (
    negativeMap.valid &&
    positiveMap.valid
  ) {

    Serial.println();

    Serial.print(
      "[OK] "
    );

    Serial.print(
      controlName
    );

    Serial.print(" ");

    Serial.print(
      negativeName
    );

    Serial.print("/");

    Serial.print(
      positiveName
    );

    Serial.println(
      " = DIGITAL"
    );

    return true;
  }

  Serial.println();

  Serial.println(
    "[GAGAL] Format arah tidak dikenali."
  );

  return false;
}

bool mapButton(
  const char *instruction,
  InputMapping &mapping
) {

  uint8_t report[
    MAX_REPORT
  ] = {0};

  separator();

  if (
    !capturePhysicalAction(
      instruction,
      report
    )
  ) {
    return false;
  }

  mapping =
    createDigitalMapping(
      report
    );

  if (!mapping.valid) {

    Serial.println();

    Serial.println(
      "[GAGAL] Tombol tidak dikenali."
    );

    return false;
  }

  Serial.println();

  Serial.println(
    "[OK] Tombol dipetakan."
  );

  return true;
}

bool mapStick(
  const char *name,
  StickMapping &stick,
  const char *clickName
) {

  separator();

  Serial.print(
    "PAIRING "
  );

  Serial.println(
    name
  );

  if (
    !mapDirectionPair(
      name,
      "KIRI",
      "KANAN",
      stick.left,
      stick.right
    )
  ) {
    return false;
  }

  if (
    !mapDirectionPair(
      name,
      "ATAS",
      "BAWAH",
      stick.up,
      stick.down
    )
  ) {
    return false;
  }

  char instruction[100];

  snprintf(
    instruction,
    sizeof(instruction),
    "Klik %s / %s",
    name,
    clickName
  );

  if (
    !mapButton(
      instruction,
      stick.click
    )
  ) {
    return false;
  }

  return true;
}

bool mapDPad() {

  separator();

  Serial.println(
    "PAIRING D-PAD"
  );

  Serial.println();

  Serial.println(
    "D-Pad akan diperiksa sebagai"
  );

  Serial.println(
    "ANALOG atau DIGITAL secara otomatis."
  );

  // Horizontal
  if (
    !mapDirectionPair(
      "D-PAD",
      "KIRI",
      "KANAN",
      profile.dpad.left,
      profile.dpad.right
    )
  ) {
    return false;
  }

  // Vertical
  if (
    !mapDirectionPair(
      "D-PAD",
      "ATAS",
      "BAWAH",
      profile.dpad.up,
      profile.dpad.down
    )
  ) {
    return false;
  }

  return true;
}

void startPairing() {

  testMode = false;
  rawMode = false;

  clearProfile();

  separator();

  Serial.println(
    "       GAMEPAD-S3 PAIRING"
  );

  Serial.println(
    "       Adaptive Mapper v0.7"
  );

  separator();

  Serial.println();

  Serial.println(
    "Ikuti satu instruksi setiap kali."
  );

  Serial.println(
    "Jangan menekan kontrol lain."
  );

  // ----------------------------------------------------------
  // NEUTRAL
  // ----------------------------------------------------------

  if (!captureNeutral()) {
    return;
  }

  profile.reportLength =
    neutralLength;

  // ----------------------------------------------------------
  // LEFT STICK
  // ----------------------------------------------------------

  if (
    !mapStick(
      "ANALOG KIRI",
      profile.leftStick,
      "L3"
    )
  ) {
    return;
  }

  // ----------------------------------------------------------
  // RIGHT STICK
  // ----------------------------------------------------------

  if (
    !mapStick(
      "ANALOG KANAN",
      profile.rightStick,
      "R3"
    )
  ) {
    return;
  }

  // ----------------------------------------------------------
  // FACE
  // ----------------------------------------------------------

  if (
    !mapButton(
      "Tekan TRIANGLE / Button 1",
      profile.triangle
    )
  ) {
    return;
  }

  if (
    !mapButton(
      "Tekan CIRCLE / Button 2",
      profile.circle
    )
  ) {
    return;
  }

  if (
    !mapButton(
      "Tekan X / Button 3",
      profile.cross
    )
  ) {
    return;
  }

  if (
    !mapButton(
      "Tekan SQUARE / Button 4",
      profile.square
    )
  ) {
    return;
  }

  // ----------------------------------------------------------
  // SHOULDER
  // ----------------------------------------------------------

  if (
    !mapButton(
      "Tekan L1 / Button 5",
      profile.l1
    )
  ) {
    return;
  }

  if (
    !mapButton(
      "Tekan R1 / Button 6",
      profile.r1
    )
  ) {
    return;
  }

  if (
    !mapButton(
      "Tekan L2 / Button 7",
      profile.l2
    )
  ) {
    return;
  }

  if (
    !mapButton(
      "Tekan R2 / Button 8",
      profile.r2
    )
  ) {
    return;
  }

  // ----------------------------------------------------------
  // SYSTEM
  // ----------------------------------------------------------

  if (
    !mapButton(
      "Tekan SELECT / Button 9",
      profile.selectButton
    )
  ) {
    return;
  }

  if (
    !mapButton(
      "Tekan START / Button 10",
      profile.startButton
    )
  ) {
    return;
  }

  // ----------------------------------------------------------
  // DPAD
  // ----------------------------------------------------------

  if (!mapDPad()) {
    return;
  }

  // ----------------------------------------------------------
  // SAVE
  // ----------------------------------------------------------

  saveProfile();

  separator();

  Serial.println(
    "          PAIRING SELESAI"
  );

  separator();

  Serial.println();

  Serial.println(
    "Mapping berhasil dibuat."
  );

  Serial.println(
    "Profile disimpan ke ESP32."
  );

#if ENABLE_PROFILE
  showProfile();
#endif

  separator();

  Serial.println(
    "GAMEPAD READY"
  );

  Serial.println();

#if ENABLE_TEST
  Serial.println("Ketik 'test' untuk Live Test.");
#endif
}

#endif
