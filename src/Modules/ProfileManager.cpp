#include "../Core/GamepadCore.h"
#include "../Core/ProfileStorage.h"
#include "../Modules/Pairing.h"
#include "../Modules/ProfileManager.h"
#include "../Modules/GamepadTest.h"
#include "../Modules/RawMonitor.h"

#if ENABLE_PROFILE
bool sameDigitalMapping(
  const InputMapping &a,
  const InputMapping &b
) {

  if (
    !a.valid ||
    !b.valid
  ) {
    return false;
  }

  if (
    a.type != INPUT_DIGITAL ||
    b.type != INPUT_DIGITAL
  ) {
    return false;
  }

  return (
    a.byteIndex ==
      b.byteIndex &&
    a.mask ==
      b.mask &&
    a.releasedValue ==
      b.releasedValue
  );
}

bool sameAxisMapping(
  const InputMapping &a,
  const InputMapping &b
) {

  if (
    !a.valid ||
    !b.valid
  ) {
    return false;
  }

  if (
    a.type != INPUT_AXIS ||
    b.type != INPUT_AXIS
  ) {
    return false;
  }

  return (
    a.byteIndex ==
      b.byteIndex &&
    a.centerValue ==
      b.centerValue &&
    a.negativeValue ==
      b.negativeValue &&
    a.positiveValue ==
      b.positiveValue
  );
}

bool sameMapping(
  const InputMapping &a,
  const InputMapping &b
) {

  if (
    sameDigitalMapping(
      a,
      b
    )
  ) {
    return true;
  }

  if (
    sameAxisMapping(
      a,
      b
    )
  ) {
    return true;
  }

  return false;
}

bool digitalMatchesAxisDirection(
  const InputMapping &digital,
  const InputMapping &axis,
  bool negativeDirection
) {

  if (
    !digital.valid ||
    !axis.valid
  ) {
    return false;
  }

  if (
    digital.type !=
      INPUT_DIGITAL ||
    axis.type !=
      INPUT_AXIS
  ) {
    return false;
  }

  if (
    digital.byteIndex !=
    axis.byteIndex
  ) {
    return false;
  }

  if (
    digital.releasedValue !=
    axis.centerValue
  ) {
    return false;
  }

  uint8_t expected =
    negativeDirection
      ? axis.negativeValue
      : axis.positiveValue;

  uint8_t digitalPressedValue =
    digital.releasedValue ^
    digital.mask;

  return (
    digitalPressedValue ==
    expected
  );
}

bool directionAlias(
  const InputMapping &a,
  const InputMapping &b,
  bool bNegative
) {

  if (
    sameMapping(
      a,
      b
    )
  ) {
    return true;
  }

  if (
    a.type ==
      INPUT_DIGITAL &&
    b.type ==
      INPUT_AXIS
  ) {

    return digitalMatchesAxisDirection(
      a,
      b,
      bNegative
    );
  }

  if (
    a.type ==
      INPUT_AXIS &&
    b.type ==
      INPUT_DIGITAL
  ) {

    return digitalMatchesAxisDirection(
      b,
      a,
      bNegative
    );
  }

  return false;
}

void printAlias(
  const char *a,
  const char *b
) {

  Serial.print(
    "[ALIAS] "
  );

  Serial.print(a);

  Serial.print(
    " == "
  );

  Serial.println(b);
}

void showConflicts() {

  separator();

  Serial.println(
    "HID ALIAS / CONFLICT ANALYSIS"
  );

  separator();

  bool found = false;

  // ----------------------------------------------------------
  // RIGHT STICK vs FACE BUTTONS
  // ----------------------------------------------------------

  if (
    sameMapping(
      profile.rightStick.up,
      profile.triangle
    )
  ) {

    printAlias(
      "Analog Kanan ATAS",
      "Triangle"
    );

    found = true;
  }

  if (
    sameMapping(
      profile.rightStick.right,
      profile.circle
    )
  ) {

    printAlias(
      "Analog Kanan KANAN",
      "Circle"
    );

    found = true;
  }

  if (
    sameMapping(
      profile.rightStick.down,
      profile.cross
    )
  ) {

    printAlias(
      "Analog Kanan BAWAH",
      "X"
    );

    found = true;
  }

  if (
    sameMapping(
      profile.rightStick.left,
      profile.square
    )
  ) {

    printAlias(
      "Analog Kanan KIRI",
      "Square"
    );

    found = true;
  }

  // ----------------------------------------------------------
  // DPAD vs LEFT STICK
  // ----------------------------------------------------------

  if (
    directionAlias(
      profile.dpad.left,
      profile.leftStick.left,
      true
    )
  ) {

    printAlias(
      "D-Pad KIRI",
      "Analog Kiri KIRI"
    );

    found = true;
  }

  if (
    directionAlias(
      profile.dpad.right,
      profile.leftStick.right,
      false
    )
  ) {

    printAlias(
      "D-Pad KANAN",
      "Analog Kiri KANAN"
    );

    found = true;
  }

  if (
    directionAlias(
      profile.dpad.up,
      profile.leftStick.up,
      true
    )
  ) {

    printAlias(
      "D-Pad ATAS",
      "Analog Kiri ATAS"
    );

    found = true;
  }

  if (
    directionAlias(
      profile.dpad.down,
      profile.leftStick.down,
      false
    )
  ) {

    printAlias(
      "D-Pad BAWAH",
      "Analog Kiri BAWAH"
    );

    found = true;
  }

  if (!found) {

    Serial.println();

    Serial.println(
      "Tidak ada alias yang dikenali."
    );
  }

  Serial.println();

  Serial.println(
    "Catatan:"
  );

  Serial.println(
    "ALIAS berarti dua kontrol menghasilkan"
  );

  Serial.println(
    "sinyal HID yang tidak dapat dibedakan."
  );
}

const char *inputTypeName(
  const InputMapping &input
) {

  if (!input.valid) {
    return "NOT MAPPED";
  }

  switch (
    input.type
  ) {

    case INPUT_AXIS:
      return "ANALOG";

    case INPUT_DIGITAL:
      return "DIGITAL";

    case INPUT_HAT:
      return "HAT";

    default:
      return "UNKNOWN";
  }
}

void showInput(
  const char *name,
  const InputMapping &input
) {

  Serial.print(name);
  Serial.print(" : ");

  Serial.println(
    inputTypeName(
      input
    )
  );
}

void showProfile() {

  separator();

  Serial.println(
    "GAMEPAD PROFILE v0.7"
  );

  separator();

  Serial.println(
    "ANALOG KIRI"
  );

  showInput(
    "  Kiri ",
    profile.leftStick.left
  );

  showInput(
    "  Kanan",
    profile.leftStick.right
  );

  showInput(
    "  Atas ",
    profile.leftStick.up
  );

  showInput(
    "  Bawah",
    profile.leftStick.down
  );

  showInput(
    "  Klik / L3",
    profile.leftStick.click
  );

  Serial.println();

  Serial.println(
    "ANALOG KANAN"
  );

  showInput(
    "  Kiri ",
    profile.rightStick.left
  );

  showInput(
    "  Kanan",
    profile.rightStick.right
  );

  showInput(
    "  Atas ",
    profile.rightStick.up
  );

  showInput(
    "  Bawah",
    profile.rightStick.down
  );

  showInput(
    "  Klik / R3",
    profile.rightStick.click
  );

  Serial.println();

  Serial.println(
    "TOMBOL AKSI"
  );

  showInput(
    "  Triangle / Button 1",
    profile.triangle
  );

  showInput(
    "  Circle   / Button 2",
    profile.circle
  );

  showInput(
    "  X        / Button 3",
    profile.cross
  );

  showInput(
    "  Square   / Button 4",
    profile.square
  );

  Serial.println();

  Serial.println(
    "SHOULDER"
  );

  showInput(
    "  L1 / Button 5",
    profile.l1
  );

  showInput(
    "  R1 / Button 6",
    profile.r1
  );

  showInput(
    "  L2 / Button 7",
    profile.l2
  );

  showInput(
    "  R2 / Button 8",
    profile.r2
  );

  Serial.println();

  Serial.println(
    "SYSTEM"
  );

  showInput(
    "  Select / Button 9",
    profile.selectButton
  );

  showInput(
    "  Start  / Button 10",
    profile.startButton
  );

  Serial.println();

  Serial.println(
    "D-PAD"
  );

  showInput(
    "  Atas ",
    profile.dpad.up
  );

  showInput(
    "  Bawah",
    profile.dpad.down
  );

  showInput(
    "  Kiri ",
    profile.dpad.left
  );

  showInput(
    "  Kanan",
    profile.dpad.right
  );

  showConflicts();
}

#endif
