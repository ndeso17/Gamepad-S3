#include "../Core/GamepadCore.h"
#include "../Core/ProfileStorage.h"
#include "../Modules/Pairing.h"
#include "../Modules/ProfileManager.h"
#include "../Modules/GamepadTest.h"
#include "../Modules/RawMonitor.h"

#if ENABLE_TEST
int prevLeftHorizontal = 999;
int prevLeftVertical = 999;

int prevRightHorizontal = 999;
int prevRightVertical = 999;

int prevDpadHorizontal = 999;
int prevDpadVertical = 999;

bool prevLeftLeft = false;
bool prevLeftRight = false;
bool prevLeftUp = false;
bool prevLeftDown = false;

bool prevRightLeft = false;
bool prevRightRight = false;
bool prevRightUp = false;
bool prevRightDown = false;

bool prevDpadLeft = false;
bool prevDpadRight = false;
bool prevDpadUp = false;
bool prevDpadDown = false;

bool prevL3 = false;
bool prevR3 = false;

bool prevTriangle = false;
bool prevCircle = false;
bool prevCross = false;
bool prevSquare = false;

bool prevL1 = false;
bool prevR1 = false;
bool prevL2 = false;
bool prevR2 = false;

bool prevSelect = false;
bool prevStart = false;

void printButtonEvent(
  const char *name,
  bool current,
  bool &previous
) {

  if (
    current ==
    previous
  ) {
    return;
  }

  Serial.print(
    "[BUTTON] "
  );

  Serial.print(name);

  Serial.print(
    " = "
  );

  Serial.println(
    current
      ? "PRESSED"
      : "RELEASED"
  );

  previous =
    current;
}

void printAxisEvent(
  const char *name,
  int current,
  int &previous
) {

  if (
    previous == 999
  ) {

    previous =
      current;

    return;
  }

  bool centerChanged =
    current == 0 &&
    previous != 0;

  bool movedFromCenter =
    previous == 0 &&
    current != 0;

  bool enoughChange =
    abs(
      current -
      previous
    ) >=
    AXIS_PRINT_STEP;

  if (
    !centerChanged &&
    !movedFromCenter &&
    !enoughChange
  ) {
    return;
  }

  Serial.print(
    "[AXIS] "
  );

  Serial.print(name);

  Serial.print(
    " = "
  );

  Serial.println(
    current
  );

  previous =
    current;
}

void resetLiveState() {

  prevLeftHorizontal = 999;
  prevLeftVertical = 999;

  prevRightHorizontal = 999;
  prevRightVertical = 999;

  prevDpadHorizontal = 999;
  prevDpadVertical = 999;

  prevLeftLeft = false;
  prevLeftRight = false;
  prevLeftUp = false;
  prevLeftDown = false;

  prevRightLeft = false;
  prevRightRight = false;
  prevRightUp = false;
  prevRightDown = false;

  prevDpadLeft = false;
  prevDpadRight = false;
  prevDpadUp = false;
  prevDpadDown = false;

  prevL3 = false;
  prevR3 = false;

  prevTriangle = false;
  prevCircle = false;
  prevCross = false;
  prevSquare = false;

  prevL1 = false;
  prevR1 = false;
  prevL2 = false;
  prevR2 = false;

  prevSelect = false;
  prevStart = false;
}

void processStickLive(
  const char *name,
  const StickMapping &stick,
  int &previousHorizontal,
  int &previousVertical,
  bool &previousLeft,
  bool &previousRight,
  bool &previousUp,
  bool &previousDown
) {

  char label[80];

  // ----------------------------------------------------------
  // Horizontal
  // ----------------------------------------------------------

  if (
    stick.left.type ==
    INPUT_AXIS
  ) {

    snprintf(
      label,
      sizeof(label),
      "%s Kiri/Kanan",
      name
    );

    printAxisEvent(
      label,
      axisValue(
        stick.left
      ),
      previousHorizontal
    );

  } else {

    snprintf(
      label,
      sizeof(label),
      "%s KIRI",
      name
    );

    printButtonEvent(
      label,
      directionPressed(
        stick.left,
        true
      ),
      previousLeft
    );

    snprintf(
      label,
      sizeof(label),
      "%s KANAN",
      name
    );

    printButtonEvent(
      label,
      directionPressed(
        stick.right,
        false
      ),
      previousRight
    );
  }

  // ----------------------------------------------------------
  // Vertical
  // ----------------------------------------------------------

  if (
    stick.up.type ==
    INPUT_AXIS
  ) {

    snprintf(
      label,
      sizeof(label),
      "%s Atas/Bawah",
      name
    );

    printAxisEvent(
      label,
      axisValue(
        stick.up
      ),
      previousVertical
    );

  } else {

    snprintf(
      label,
      sizeof(label),
      "%s ATAS",
      name
    );

    printButtonEvent(
      label,
      directionPressed(
        stick.up,
        true
      ),
      previousUp
    );

    snprintf(
      label,
      sizeof(label),
      "%s BAWAH",
      name
    );

    printButtonEvent(
      label,
      directionPressed(
        stick.down,
        false
      ),
      previousDown
    );
  }
}

void processDpadLive() {

  // ----------------------------------------------------------
  // Horizontal
  // ----------------------------------------------------------

  if (
    profile.dpad.left.type ==
    INPUT_AXIS
  ) {

    printAxisEvent(
      "D-Pad Kiri/Kanan",
      axisValue(
        profile.dpad.left
      ),
      prevDpadHorizontal
    );

  } else {

    printButtonEvent(
      "D-Pad KIRI",
      directionPressed(
        profile.dpad.left,
        true
      ),
      prevDpadLeft
    );

    printButtonEvent(
      "D-Pad KANAN",
      directionPressed(
        profile.dpad.right,
        false
      ),
      prevDpadRight
    );
  }

  // ----------------------------------------------------------
  // Vertical
  // ----------------------------------------------------------

  if (
    profile.dpad.up.type ==
    INPUT_AXIS
  ) {

    printAxisEvent(
      "D-Pad Atas/Bawah",
      axisValue(
        profile.dpad.up
      ),
      prevDpadVertical
    );

  } else {

    printButtonEvent(
      "D-Pad ATAS",
      directionPressed(
        profile.dpad.up,
        true
      ),
      prevDpadUp
    );

    printButtonEvent(
      "D-Pad BAWAH",
      directionPressed(
        profile.dpad.down,
        false
      ),
      prevDpadDown
    );
  }
}

void processLiveTest() {

  if (
    !testMode ||
    !haveReport
  ) {
    return;
  }

  // LEFT STICK

  processStickLive(
    "Analog Kiri",
    profile.leftStick,
    prevLeftHorizontal,
    prevLeftVertical,
    prevLeftLeft,
    prevLeftRight,
    prevLeftUp,
    prevLeftDown
  );

  // L3

  printButtonEvent(
    "L3",
    digitalPressed(
      profile.leftStick.click
    ),
    prevL3
  );

  // RIGHT STICK

  processStickLive(
    "Analog Kanan",
    profile.rightStick,
    prevRightHorizontal,
    prevRightVertical,
    prevRightLeft,
    prevRightRight,
    prevRightUp,
    prevRightDown
  );

  // R3

  printButtonEvent(
    "R3",
    digitalPressed(
      profile.rightStick.click
    ),
    prevR3
  );

  // FACE BUTTONS

  printButtonEvent(
    "Triangle / Button 1",
    digitalPressed(
      profile.triangle
    ),
    prevTriangle
  );

  printButtonEvent(
    "Circle / Button 2",
    digitalPressed(
      profile.circle
    ),
    prevCircle
  );

  printButtonEvent(
    "X / Button 3",
    digitalPressed(
      profile.cross
    ),
    prevCross
  );

  printButtonEvent(
    "Square / Button 4",
    digitalPressed(
      profile.square
    ),
    prevSquare
  );

  // SHOULDER

  printButtonEvent(
    "L1 / Button 5",
    digitalPressed(
      profile.l1
    ),
    prevL1
  );

  printButtonEvent(
    "R1 / Button 6",
    digitalPressed(
      profile.r1
    ),
    prevR1
  );

  printButtonEvent(
    "L2 / Button 7",
    digitalPressed(
      profile.l2
    ),
    prevL2
  );

  printButtonEvent(
    "R2 / Button 8",
    digitalPressed(
      profile.r2
    ),
    prevR2
  );

  // SYSTEM

  printButtonEvent(
    "SELECT / Button 9",
    digitalPressed(
      profile.selectButton
    ),
    prevSelect
  );

  printButtonEvent(
    "START / Button 10",
    digitalPressed(
      profile.startButton
    ),
    prevStart
  );

  // DPAD

  processDpadLive();
}

void startLiveTest() {

  if (
    profile.magic !=
    PROFILE_MAGIC
  ) {

    Serial.println();

    Serial.println(
      "[ERROR] Profile tidak tersedia."
    );

    return;
  }

  rawMode = false;

  resetLiveState();

  testMode = true;

  separator();

  Serial.println(
    "LIVE EVENT TEST : ON"
  );

  separator();

  Serial.println();

  Serial.println(
    "Gerakkan stik / D-Pad"
  );

  Serial.println(
    "atau tekan tombol."
  );

  Serial.println();

  Serial.println(
    "Ketik 'stop' untuk keluar."
  );

  Serial.println();
}

void stopLiveTest() {

  testMode = false;

  Serial.println();

  Serial.println(
    "[TEST MODE: OFF]"
  );
}

#endif
