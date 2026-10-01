#pragma once
void displayBegin();
void displayTick();
// Optional board-specific implementation supplies strong definitions of these
// hooks in its own .cpp. No TFT model, driver, bus or GPIO is assumed here.
void gamepadDisplayBeginHook();
void gamepadDisplayTickHook();
