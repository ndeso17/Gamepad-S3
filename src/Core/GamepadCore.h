#pragma once
#include "../../Config.h"
#include "../Core/GamepadTypes.h"

extern uint8_t currentReport[MAX_REPORT], neutralReport[MAX_REPORT];
extern uint8_t currentLength, neutralLength;
extern bool haveReport, haveNeutral, rawMode, testMode;
void gamepadBegin();
void separator();
void smallSeparator();
void printReport(
  const uint8_t *data,
  uint8_t len
);
void updateHID();
bool reportIsNeutral();
uint8_t countChangedBytes(
  const uint8_t *base,
  const uint8_t *changed,
  uint8_t len
);
int firstChangedByte(
  const uint8_t *base,
  const uint8_t *changed,
  uint8_t len
);
bool digitalPressed(
  const InputMapping &input
);
int axisValue(
  const InputMapping &input
);
bool directionPressed(
  const InputMapping &input,
  bool negativeDirection
);
