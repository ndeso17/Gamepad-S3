#pragma once
#include "../../Config.h"
#include "../Core/GamepadTypes.h"

#if ENABLE_RAW
void processRawReport(const uint8_t *data, uint16_t len);
void toggleRawMonitor();
#endif
