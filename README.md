# Gamepad-S3 — v0.7 modular

Refactor dari file unggahan `Gamepad-S3.ino` v0.7. Target: **Arduino IDE, ESP32S3 Dev Module, Arduino-ESP32 3.3.12**. Tidak memakai PlatformIO. Tidak ada driver atau output motor.

## Membuka dan compile

1. Ekstrak ZIP. Pertahankan seluruh folder `Gamepad-S3` beserta `src/`.
2. Buka `Gamepad-S3/Gamepad-S3.ino` di Arduino IDE.
3. Pilih board **ESP32S3 Dev Module**, core Espressif **3.3.12**.
4. Pilih **USB Mode: USB-OTG (TinyUSB)**; **USB CDC On Boot: Disabled**, **USB MSC On Boot: Disabled**, **USB DFU On Boot: Disabled**.
5. Serial Monitor: **115200 baud**, newline. Gunakan koneksi UART/USB-UART yang sesuai board untuk console; port USB OTG dipakai sebagai host gamepad. Pertahankan wiring USB/VBUS yang sudah berhasil pada v0.7.
6. Compile dan upload seperti sketch biasa. `USBHost`, `USBHostHIDGamepad`, `Preferences`, dan `WiFi` memakai library bawaan core 3.3.12; tidak perlu library HID pihak ketiga.

Folder modul sengaja berada di **src/**: Arduino IDE hanya mengompilasi subfolder secara rekursif di sana. `.ino` hanya mengatur pemanggilan modul. File di `src/` dapat diedit dengan editor teks; tidak semuanya muncul sebagai tab IDE.

Rujukan: https://docs.arduino.cc/arduino-cli/sketch-specification/ dan contoh USBHostMouse bawaan Arduino-ESP32 3.3.12.

## Struktur

```
Gamepad-S3.ino
Config.h
src/
  Core/       GamepadTypes, GamepadCore, ProfileStorage
  Modules/    Pairing, RawMonitor, ProfileManager, GamepadTest
  Commands/   CommandManager
  Network/    NetworkManager, ModemManager
  Hardware/   ButtonManager, NeoPixelManager, DisplayManager
```

`GamepadCore` satu-satunya pemilik USB HID, report, dan pembacaan input. `ProfileStorage` memiliki profil aktif dan penyimpanan NVS. `ModemManager` menyimpan konfigurasi jaringan; ini modul ESP32 lokal, bukan layanan ModemManager Linux.

## Feature flags

Edit nilai default di `Config.h` dan compile ulang. Nilai 0 menghilangkan implementasi modul terkait serta menu/dispatch command-nya; core HID dan penyimpanan profil tetap tersedia bagi modul lain.

| Flag | Default | Fungsi |
|---|---:|---|
| ENABLE_PAIRING | 1 | pair |
| ENABLE_RAW | 1 | raw |
| ENABLE_PROFILE | 1 | profile, conflicts |
| ENABLE_TEST | 1 | test, stop |
| ENABLE_ERASE | 1 | erase |
| ENABLE_CONFIG_COMMAND | 1 | config |
| ENABLE_NETWORK | 1 | Mesin Wi-Fi dan NVS jaringan; mode awal off |
| ENABLE_BUTTON | 0 | Tombol restart opsional |
| ENABLE_NEOPIXEL | 0 | NeoPixel opsional |
| ENABLE_DISPLAY | 0 | Hook display opsional |

`help` selalu tersedia. `stop` menghentikan live test, sesuai v0.7; toggle `raw` untuk menghentikan RAW. `ENABLE_NETWORK=0` dengan config aktif menampilkan keterangan bahwa jaringan dinonaktifkan saat compile.

Contoh hanya pairing, config, test: set `ENABLE_RAW`, `ENABLE_PROFILE`, `ENABLE_ERASE` ke 0; pertahankan `ENABLE_PAIRING`, `ENABLE_TEST`, `ENABLE_CONFIG_COMMAND`, `ENABLE_NETWORK` = 1. Menu otomatis mengikuti flag, termasuk pesan setelah pairing.

## Commands

- `pair`: wizard kalibrasi/pemetaan HID v0.7.
- `profile`: tampilkan mapping dan konflik.
- `conflicts`: analisis alias HID.
- `test`: live event test.
- `stop`: hentikan live event test.
- `raw`: toggle report HID yang berubah; menonaktifkan test.
- `erase`: hapus profil gamepad; menonaktifkan raw/test. Tidak menghapus konfigurasi jaringan.
- `config`: pengaturan jaringan di bawah.
- `help`: daftar command yang dikompilasi.

Wizard pairing tetap sinkron/blocking seperti v0.7. Selama wizard berlangsung, loop command, pemeliharaan Wi-Fi, button, dan display menunggu wizard selesai; task Wi-Fi sistem tetap berjalan. Ini sengaja dipertahankan agar timing capture HID tidak berubah.

## Konfigurasi jaringan

Awalnya Wi-Fi **off**, walaupun modul tersedia. Command berikut menyimpan ke NVS dan langsung menerapkan perubahan; konfigurasi dimuat kembali setelah restart. Argumen dengan spasi harus diberi tanda kutip. Huruf besar/kecil SSID dan password dipertahankan.

```
config help
config show
config ap "Gamepad-S3" "PasswordAP123"
config client "WiFi Rumah" "PasswordWiFi123"
config mode ap
config mode client
config mode auto
config mode off
config apply
config reset
```

Pilih satu mode sesuai kebutuhan. `ap` membuat access point; `client` menghubungkan ke router; `auto` mencoba client dan membuka AP fallback sesudah 15 detik tanpa koneksi, atau segera jika SSID client belum diisi. Jendela 15 detik dihitung sejak mode diterapkan; bila koneksi terputus setelah jendela itu, AP fallback dimulai segera. AP fallback tetap aktif bila client kemudian tersambung; `config apply` memulai ulang strategi koneksi. Password kosong `""` berarti jaringan terbuka. SSID 1–32 byte, password kosong atau 8–63 byte. Default AP bernama `Gamepad-S3`, terbuka sampai password disetel. Password tidak dicetak melalui `config show`.

`config reset` mengembalikan pengaturan jaringan ke default off, tidak menghapus profil gamepad. Namespace jaringan `gp-network` terpisah dari `gamepad`. Tidak ada web server/menu browser; konfigurasi melalui Serial Monitor.

## Hardware opsional

- **Button:** set `BUTTON_PIN` ke GPIO bebas yang telah diverifikasi, hubungkan tombol antara GPIO dan GND, lalu aktifkan `ENABLE_BUTTON`. Input pull-up internal, debounce 30 ms, tahan 2 detik untuk restart software. Ini tidak menggantikan tombol BOOT untuk flashing dan tidak akan dilayani saat pairing blocking.
- **NeoPixel:** default off dan `NEOPIXEL_PIN=-1`. Verifikasi pin LED pada model board yang tepat, lalu isi pin dan aktifkan flag. Modul memakai `rgbLedWrite` bawaan core; awal LED mati. Panggil `neoPixelSet(r,g,b)` untuk warna; belum ada pola status otomatis. Beberapa board memerlukan pengendalian power LED terpisah sesuai wiring.
- **Display:** default off. Karena jenis TFT dan wiring belum ditentukan, tersedia hook `gamepadDisplayBeginHook()` dan `gamepadDisplayTickHook()` yang dapat diimplementasikan dalam `.cpp` baru. Mengaktifkan flag saja belum menggambar ke TFT; diperlukan driver/pin untuk layar yang dipilih. Hook default mencetak keterangan ini sekali saat boot.

## Kompatibilitas dan batas v0.7

Layout `GamepadProfile`, magic `0x47503730`, namespace `gamepad`, key `profile`, serta operasi penyimpanan dipertahankan untuk membaca profil lama. Algoritma capture, deteksi axis/digital, alias, deadzone, event test, dan polling HID dipertahankan. Ekstraksi RAW mempertahankan history report statis.

Perilaku lama juga dipertahankan: profil kosong masih memiliki magic valid setelah `clearProfile()`, sehingga `test` tidak membuktikan profil telah lengkap; pairing yang dibatalkan dapat meninggalkan mapping RAM parsial. Jalankan pairing sampai selesai sebelum mengandalkan hasil test. Tidak ditambahkan decoder HAT baru, pengendalian kendaraan, atau perubahan algoritma disconnect/failsafe.

Lihat `VALIDATION.md` untuk hasil build dan batas pengujian. Pengujian gamepad, Wi-Fi, serta hardware fisik tetap perlu dilakukan di board Anda.
