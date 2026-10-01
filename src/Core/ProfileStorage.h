#pragma once
#include "../../Config.h"
#include "../Core/GamepadTypes.h"

extern GamepadProfile profile;
void clearProfile();
void saveProfile();
bool loadProfile();
#if ENABLE_ERASE
void eraseProfile();
#endif
