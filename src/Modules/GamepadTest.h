#pragma once
#include "../../Config.h"
#include "../Core/GamepadTypes.h"

#if ENABLE_TEST
void printButtonEvent(
  const char *name,
  bool current,
  bool &previous
);
void printAxisEvent(
  const char *name,
  int current,
  int &previous
);
void resetLiveState();
void processStickLive(
  const char *name,
  const StickMapping &stick,
  int &previousHorizontal,
  int &previousVertical,
  bool &previousLeft,
  bool &previousRight,
  bool &previousUp,
  bool &previousDown
);
void processDpadLive();
void processLiveTest();
void startLiveTest();
void stopLiveTest();
#endif
