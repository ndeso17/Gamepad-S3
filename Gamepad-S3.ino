#include "Config.h"
#include "src/Core/GamepadCore.h"
#include "src/Commands/CommandManager.h"
#include "src/Modules/GamepadTest.h"
#include "src/Network/NetworkManager.h"
#include "src/Hardware/ButtonManager.h"
#include "src/Hardware/NeoPixelManager.h"
#include "src/Hardware/DisplayManager.h"

void setup() {
  gamepadBegin();
  networkBegin();
  buttonBegin();
  neoPixelBegin();
  displayBegin();
  printHelp();
}

void loop() {
  updateHID();
  processSerial();
#if ENABLE_TEST
  processLiveTest();
#endif
  networkTick();
  buttonTick();
  neoPixelTick();
  displayTick();
  delay(1);
}
