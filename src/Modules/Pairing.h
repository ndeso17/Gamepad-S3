#pragma once
#include "../../Config.h"
#include "../Core/GamepadTypes.h"

#if ENABLE_PAIRING
bool waitForNeutral(
  uint32_t timeout = NEUTRAL_TIMEOUT
);
bool captureAction(
  uint8_t *captured,
  uint8_t &capturedLength,
  uint32_t timeout = ACTION_TIMEOUT
);
bool captureNeutral();
InputMapping createDigitalMapping(
  const uint8_t *actionReport
);
bool capturePhysicalAction(
  const char *instruction,
  uint8_t *report
);
bool detectAxisPair(
  const uint8_t *negativeReport,
  const uint8_t *positiveReport,
  InputMapping &negative,
  InputMapping &positive
);
bool mapDirectionPair(
  const char *controlName,
  const char *negativeName,
  const char *positiveName,
  InputMapping &negativeMap,
  InputMapping &positiveMap
);
bool mapButton(
  const char *instruction,
  InputMapping &mapping
);
bool mapStick(
  const char *name,
  StickMapping &stick,
  const char *clickName
);
bool mapDPad();
void startPairing();
#endif
