#include "../Core/GamepadCore.h"
#include "../Core/ProfileStorage.h"
#include "../Modules/Pairing.h"
#include "../Modules/ProfileManager.h"
#include "../Modules/GamepadTest.h"
#include "../Modules/RawMonitor.h"

#include <Preferences.h>
static Preferences prefs;
GamepadProfile profile;
void clearProfile() {

  memset(
    &profile,
    0,
    sizeof(profile)
  );

  profile.magic =
    PROFILE_MAGIC;
}

void saveProfile() {

  prefs.begin(
    "gamepad",
    false
  );

  prefs.putBytes(
    "profile",
    &profile,
    sizeof(profile)
  );

  prefs.end();

  Serial.println();

  Serial.println(
    "[OK] Profile disimpan ke NVS."
  );
}

bool loadProfile() {

  prefs.begin(
    "gamepad",
    true
  );

  size_t size =
    prefs.getBytesLength(
      "profile"
    );

  if (
    size !=
    sizeof(profile)
  ) {

    prefs.end();

    return false;
  }

  prefs.getBytes(
    "profile",
    &profile,
    sizeof(profile)
  );

  prefs.end();

  return (
    profile.magic ==
    PROFILE_MAGIC
  );
}

#if ENABLE_ERASE
void eraseProfile() {

  prefs.begin(
    "gamepad",
    false
  );

  prefs.clear();

  prefs.end();

  clearProfile();

  Serial.println();

  Serial.println(
    "[OK] Profile dihapus."
  );
}

#endif
