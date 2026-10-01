#pragma once
#include "../../Config.h"

#if ENABLE_LINK
#include "LinkTypes.h"

namespace GamepadLink {

void linkBegin();
void linkTick();
bool processLinkCommand(const String &line);
void printLinkHelp();
LinkState linkState();

}
#endif
