#pragma once
#include <Arduino.h>
void networkBegin();
void networkTick();
bool processConfigCommand(const String &line);
void printConfigHelp();
