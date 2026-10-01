#include "../../Config.h"
#include "NetworkManager.h"
#if ENABLE_NETWORK
#include "ModemManager.h"
#include <WiFi.h>
namespace {
using namespace GamepadNetwork;
Settings settings;
bool apRunning = false;
uint32_t connectStarted = 0;
uint32_t lastAPAttempt = 0;
bool attemptedAP = false;
constexpr uint32_t fallbackMs = 15000;
void startAP() {
  lastAPAttempt = millis();
  attemptedAP = true;
  apRunning = WiFi.softAP(settings.apSSID, settings.apPassword);
  Serial.println(apRunning ? "Wi-Fi AP active." : "Wi-Fi AP failed.");
  if (apRunning) {
    Serial.print("AP IP: "); Serial.println(WiFi.softAPIP());
    if (!settings.apPassword[0]) Serial.println("AP has no password (open network).");
  }
}
void apply() {
  WiFi.disconnect(false, false);
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_OFF);
  apRunning = false;
  attemptedAP = false;
  if (settings.mode == Mode::Off) return;
  WiFi.persistent(false);
  if (settings.mode == Mode::AP) { WiFi.mode(WIFI_AP); startAP(); return; }
  WiFi.mode(settings.mode == Mode::Auto ? WIFI_AP_STA : WIFI_STA);
  connectStarted = millis();
  if (settings.clientSSID[0]) {
    WiFi.setAutoReconnect(true);
    WiFi.begin(settings.clientSSID, settings.clientPassword);
  } else if (settings.mode == Mode::Auto) startAP();
}
// Quoted arguments retain spaces and letter case; backslash escapes quotes/backslashes.
bool tokenize(const String &line, String *args, unsigned &count) {
  count = 0; unsigned i = 0;
  while (i < line.length()) {
    while (i < line.length() && isspace(static_cast<unsigned char>(line[i]))) ++i;
    if (i == line.length()) break;
    if (count == 5) return false;
    String token; char quote = 0;
    if (line[i] == '\'' || line[i] == '"') quote = line[i++];
    bool closed = !quote;
    while (i < line.length()) {
      char c = line[i++];
      if (quote && c == quote) { closed = true; break; }
      if (!quote && isspace(static_cast<unsigned char>(c))) break;
      if (c == '\\' && i < line.length() && (line[i] == '\\' || line[i] == quote)) c = line[i++];
      token += c;
    }
    if (!closed || (quote && i < line.length() && !isspace(static_cast<unsigned char>(line[i])))) return false;
    args[count++] = token;
  }
  return true;
}
void show() {
  Serial.printf("Network mode: %s\nAP SSID: %s\nClient SSID: %s\n", modeName(settings.mode), settings.apSSID, settings.clientSSID);
  Serial.printf("AP security: %s\nClient credentials: %s\n", settings.apPassword[0] ? "password configured" : "open", settings.clientPassword[0] ? "password configured" : "open / unset");
  Serial.printf("Client connected: %s\n", WiFi.status() == WL_CONNECTED ? "yes" : "no");
  if (WiFi.status() == WL_CONNECTED) { Serial.print("Client IP: "); Serial.println(WiFi.localIP()); }
  if (apRunning) { Serial.print("AP IP: "); Serial.println(WiFi.softAPIP()); }
}
}
void networkBegin() { GamepadNetwork::loadSettings(settings); apply(); }
void networkTick() {
  if (settings.mode == GamepadNetwork::Mode::Auto && !apRunning && WiFi.status() != WL_CONNECTED && millis() - connectStarted >= fallbackMs && (!attemptedAP || millis() - lastAPAttempt >= fallbackMs)) startAP();
}
#else
void networkBegin() {}
void networkTick() {}
#endif
void printConfigHelp() {
#if ENABLE_CONFIG_COMMAND
#if ENABLE_NETWORK
  Serial.println("config [show|help]");
  Serial.println("config mode off|ap|client|auto");
  Serial.println("config ap \"SSID\" \"password\" | config client \"SSID\" \"password\"");
  Serial.println("config apply | config reset (network settings only)");
  Serial.println("Changes save to NVS and apply immediately. Empty password selects an open network.");
#else
  Serial.println("config: network disabled at compile time.");
#endif
#endif
}
bool processConfigCommand(const String &line) {
#if ENABLE_CONFIG_COMMAND
  String first = line; first.trim();
  int space = first.indexOf(' '); int tab = first.indexOf('\t');
  if (tab >= 0 && (space < 0 || tab < space)) space = tab;
  if (space >= 0) first = first.substring(0, space);
  if (!first.equalsIgnoreCase("config")) return false;
#if ENABLE_NETWORK
  String a[5]; unsigned n;
  if (!tokenize(line, a, n)) { Serial.println("Invalid config syntax; quote SSID/password containing spaces."); return true; }
  if (n == 1 || (n == 2 && a[1].equalsIgnoreCase("show"))) { show(); return true; }
  if (n == 2 && a[1].equalsIgnoreCase("help")) { printConfigHelp(); return true; }
  if (n == 2 && a[1].equalsIgnoreCase("apply")) { apply(); return true; }
  Settings next = settings;
  if (n == 2 && a[1].equalsIgnoreCase("reset")) next = defaultSettings();
  else if (n == 3 && a[1].equalsIgnoreCase("mode")) {
    if (a[2].equalsIgnoreCase("off")) next.mode = Mode::Off;
    else if (a[2].equalsIgnoreCase("ap")) next.mode = Mode::AP;
    else if (a[2].equalsIgnoreCase("client")) next.mode = Mode::Client;
    else if (a[2].equalsIgnoreCase("auto")) next.mode = Mode::Auto;
    else { printConfigHelp(); return true; }
  } else if (n == 4 && (a[1].equalsIgnoreCase("ap") || a[1].equalsIgnoreCase("client"))) {
    if (a[2].length() < 1 || a[2].length() > 32 || (a[3].length() != 0 && (a[3].length() < 8 || a[3].length() > 63))) {
      Serial.println("SSID must be 1-32 bytes; password must be empty or 8-63 bytes."); return true;
    }
    bool ap = a[1].equalsIgnoreCase("ap");
    strlcpy(ap ? next.apSSID : next.clientSSID, a[2].c_str(), 33);
    strlcpy(ap ? next.apPassword : next.clientPassword, a[3].c_str(), 65);
  } else { printConfigHelp(); return true; }
  if (next.mode == Mode::Client && !next.clientSSID[0]) { Serial.println("Set client SSID before selecting client mode."); return true; }
  if (!saveSettings(next)) { Serial.println("Failed to save network settings; current settings retained."); return true; }
  settings = next; apply(); Serial.println("Network settings saved and applied.");
#else
  printConfigHelp();
#endif
  return true;
#else
  (void)line; return false;
#endif
}
