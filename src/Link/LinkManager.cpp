#include "LinkManager.h"

#if ENABLE_LINK
#include "LinkStorage.h"
#include <ctype.h>

namespace GamepadLink {
namespace {
CarLink binding;
LinkMode mode = LinkMode::Off;
LinkState state = LinkState::Off;

void showStatus() {
  Serial.printf("[link] state=%s mode=%s locked=%u channel=%u\n",
                linkStateName(state), linkModeName(mode),
                static_cast<unsigned>(binding.locked),
                static_cast<unsigned>(binding.channel));
}
}

void linkBegin() {
  mode = LinkMode::Off;
  state = LinkState::Off;
  Serial.println(loadCarLink(binding) ? "[link] NVS loaded" :
                                      "[link] NVS unavailable or invalid; defaults loaded");
}

void linkTick() {}

LinkState linkState() { return state; }

void printLinkHelp() {
  Serial.println("link status | link help");
}

bool processLinkCommand(const String &line) {
  String command = line;
  command.trim();
  unsigned end = 0;
  while (end < command.length() &&
         !isspace(static_cast<unsigned char>(command[end]))) ++end;
  if (!command.substring(0, end).equalsIgnoreCase("link")) return false;
  String argument = command.substring(end);
  argument.trim();
  if (argument.equalsIgnoreCase("status")) showStatus();
  else if (argument.length() == 0 || argument.equalsIgnoreCase("help")) printLinkHelp();
  else {
    Serial.println("[link] Unknown command");
    printLinkHelp();
  }
  return true;
}

}
#endif
