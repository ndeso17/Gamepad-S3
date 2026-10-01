#pragma once
#include "../../Config.h"
#include "../Core/GamepadTypes.h"

#if ENABLE_PROFILE
bool sameDigitalMapping(
  const InputMapping &a,
  const InputMapping &b
);
bool sameAxisMapping(
  const InputMapping &a,
  const InputMapping &b
);
bool sameMapping(
  const InputMapping &a,
  const InputMapping &b
);
bool digitalMatchesAxisDirection(
  const InputMapping &digital,
  const InputMapping &axis,
  bool negativeDirection
);
bool directionAlias(
  const InputMapping &a,
  const InputMapping &b,
  bool bNegative
);
void printAlias(
  const char *a,
  const char *b
);
void showConflicts();
const char *inputTypeName(
  const InputMapping &input
);
void showInput(
  const char *name,
  const InputMapping &input
);
void showProfile();
#endif
