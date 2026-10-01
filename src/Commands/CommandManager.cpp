#include "../Core/GamepadCore.h"
#include "../Core/ProfileStorage.h"
#include "../Modules/Pairing.h"
#include "../Modules/ProfileManager.h"
#include "../Modules/GamepadTest.h"
#include "../Modules/RawMonitor.h"
#include "CommandManager.h"
#include "../Network/NetworkManager.h"
#include "../Link/LinkManager.h"

void printHelp() {
  separator();
  Serial.println("GAMEPAD-S3 v0.7 MODULAR COMMANDS");
#if ENABLE_PAIRING
  Serial.println("pair");
#endif
#if ENABLE_PROFILE
  Serial.println("profile");
#endif
#if ENABLE_PROFILE
  Serial.println("conflicts");
#endif
#if ENABLE_TEST
  Serial.println("test");
#endif
#if ENABLE_TEST
  Serial.println("stop");
#endif
#if ENABLE_RAW
  Serial.println("raw");
#endif
#if ENABLE_ERASE
  Serial.println("erase");
#endif
#if ENABLE_CONFIG_COMMAND
  printConfigHelp();
#endif
#if ENABLE_LINK
  GamepadLink::printLinkHelp();
#endif
  Serial.println("help");
}

void processSerial() {
  if (!Serial.available()) return;
  String command = Serial.readStringUntil('\n');
  command.trim();
  String lower = command;
  lower.toLowerCase();
#if ENABLE_PAIRING
  if (lower == "pair") { startPairing(); return; }
#endif
#if ENABLE_PROFILE
  if (lower == "profile") { showProfile(); return; }
#endif
#if ENABLE_PROFILE
  if (lower == "conflicts") { showConflicts(); return; }
#endif
#if ENABLE_TEST
  if (lower == "test") { startLiveTest(); return; }
#endif
#if ENABLE_TEST
  if (lower == "stop") { stopLiveTest(); return; }
#endif
#if ENABLE_RAW
  if (lower == "raw") { toggleRawMonitor(); return; }
#endif
#if ENABLE_ERASE
  if (lower == "erase") { testMode = false; rawMode = false; eraseProfile(); return; }
#endif
#if ENABLE_CONFIG_COMMAND
  if (processConfigCommand(command)) return;
#endif
#if ENABLE_LINK
  if (GamepadLink::processLinkCommand(command)) return;
#endif
  if (lower == "help") { printHelp(); return; }
  Serial.println();
  Serial.print("Perintah tidak dikenal: ");
  Serial.println(command);
}
