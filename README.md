# ESP32_iPod_Player

esp32-ipod-audio-player ESP32 Audio Player project with iPod-style UI, animated cassette tape, multi-task FreeRTOS architecture, I2S DAC output, and SD card integration.
Instructions
ESP32 iPod & Cassette Audio Player
Proyek ini adalah pemutar musik portabel berbasis ESP32 yang mengombinasikan antarmuka UI bergaya iPod Classic dan animasi kaset pita retro yang berputar secara dinamis. Proyek ini dibangun di atas sistem operasi FreeRTOS (multithreading) pada ESP32 untuk menangani decoding audio I2S, rendering tampilan grafis, dan pemrosesan tombol navigasi secara responsif.

📸 Fitur Utama

Antarmuka iPod & Animasi Kaset Retro: Memiliki indikator baterai, header iPod, daftar lagu, status pemutaran, serta kaset dengan kerek pita (spool) yang berputar dinamis mengikuti durasi lagu.
Multithreading FreeRTOS: Pemisahan tugas audio, UI, dan input tombol ke dalam core terpisah (AudioTask, UITask, InputTask) agar playback audio lancar tanpa stuttering.
Dukungan Format Audio: Mendukung file .mp3, .wav, dan .flac dari MicroSD Card.
Scrubber Mode: Kemampuan untuk melompat (fast forward / rewind) durasi lagu sebesar ±5 atau ±10 detik.
Penghemat Daya Otomatis (Auto Off Backlight): Tampilan layar otomatis mati setelah 30 detik tanpa interaksi tombol.
Manajemen Daya Baterai: Pembacaan kapasitas baterai Li-Ion/LiPo via ADC dengan pembagi tegangan (voltage divider).


🛠️ Hardware & Komponen yang Digunakan

KomponenSpesifikasi / KeteranganMicrocontrollerESP32-WROVER (Direkomendasikan dengan PSRAM aktif)DisplaySPI Display TFT (240x240 pixel, didukung oleh library TFT_eSPI)Audio DAC / AmpI2S Module (misal: MAX98357A / PCM5102A)StorageMicroSD Card Module (Interface HSPI)Tombol5x Push Button (Navigasi & Kontrol)Power/BateraiBaterai LiPo / Li-Ion (3.7V - 4.2V) + Pembagi Tegangan ADC


📌 Pinout & Skema Koneksi
1. Modul Audio I2S

Nama Pin I2SPin ESP32KeteranganBCLK (Bit Clock)GPIO 26Bit Clock untuk I2S DACLRCK / WS (Word Select)GPIO 25Left/Right ClockDOUT / SD (Data Out)GPIO 22Data Audio Digital Out

2. Modul MicroSD (SPI / HSPI)

Nama Pin SDPin ESP32KeteranganSD_CSGPIO 15Chip SelectSD_MOSIGPIO 13SPI Data InputSD_MISOGPIO 12SPI Data OutputSD_SCKGPIO 14SPI Clock

3. Tombol Navigasi (Push Buttons)
(Semua tombol dikonfigurasi menggunakan INPUT_PULLUP internal ESP32, dihubungkan ke GND ketika ditekan)

Nama TombolPin ESP32Fungsi UtamaBTN_UPGPIO 32Navigasi Atas / Tambah Volume / Fast ForwardBTN_DOWNGPIO 21Navigasi Bawah / Kurang Volume / RewindBTN_MENUGPIO 5Kembali / Menu / Ke Root DirectoryBTN_PLAYGPIO 33Play / Pause / On-Off Screen BacklightBTN_CENTERGPIO 0Pilih Item / Masuk Scrubber Mode

4. Monitor Baterai & Display Backlight

FungsiPin ESP32KeteranganPIN_BATTERYGPIO 35Pembacaan ADC Tegangan Baterai (R1 = 10kΩ, R2 = 10kΩ)TFT_BL(Sesuai setup TFT_eSPI)Kontrol Backlight Display (HIGH = On, LOW = Off)


🕹️ Fungsi & Cara Penggunaan Tombol Navigasi
Sistem memiliki 3 Mode Tampilan (State) yang membedakan fungsi masing-masing tombol:

STATE_MENU_VIEW (Daftar File / Folder)
STATE_NOW_PLAYING (Tampilan Pemutar Lagu & Animasi Kaset)
STATE_SCRUBBER_MODE (Mode Melompati Durasi Lagu)

1. Tampilan Menu (STATE_MENU_VIEW)

TombolJenis TekananAksi / FungsiBTN_UPTekan SingkatMenyorot item menu di atasnya.Tahan (Long Press)Fast Scroll ke atas dengan cepat.BTN_DOWNTekan SingkatMenyorot item menu di bawahnya.Tahan (Long Press)Fast Scroll ke bawah dengan cepat.BTN_CENTERTekan SingkatMembuka folder atau memutar lagu yang dipilih.BTN_MENUTekan SingkatNaik 1 tingkat ke folder di atasnya (Directory Back).Tahan (Long Press)Langsung kembali ke folder utama (Root Directory /).BTN_PLAYTekan SingkatJika sedang ada lagu aktif: masuk ke Now Playing. Jika tidak: memutar lagu terlayani.


2. Tampilan Now Playing (STATE_NOW_PLAYING)

TombolJenis TekananAksi / FungsiBTN_UPTekan SingkatMenaikkan volume audio (Maksimal level 21).Tahan (Long Press)Restart lagu dari detik 00:00 (jika durasi > 3 detik) atau ke lagu sebelumnya.BTN_DOWNTekan SingkatMenurunkan volume audio (Minimal level 0).Tahan (Long Press)Melompat ke lagu berikutnya (Next Track).BTN_PLAYTekan SingkatPlay / Pause lagu (Pause Resume).Tahan (Long Press)Mematikan / menghidupkan Layar Backlight secara manual.BTN_CENTERTekan SingkatMasuk ke Scrubber Mode (untuk maju/mundurkan durasi lagu).BTN_MENUTekan SingkatKembali ke Tampilan Menu (STATE_MENU_VIEW).


3. Mode Scrubber (STATE_SCRUBBER_MODE)

TombolJenis TekananAksi / FungsiBTN_UPTekan SingkatMaju +5 Detik (Fast Forward).Tahan (Long Press)Maju +10 Detik (Fast Forward cepat).BTN_DOWNTekan SingkatMundur -5 Detik (Rewind).Tahan (Long Press)Mundur -10 Detik (Rewind cepat).BTN_CENTERTekan SingkatKeluar dari Scrubber Mode & kembali ke STATE_NOW_PLAYING.


💡 Fitur Khusus Tombol

Membangunkan Layar (Wake-up): Jika layar sedang mati karena Auto Timeout 30 Detik, menekan tombol apa saja hanya akan menyalakan layar tanpa mengeksekusi perintah tombol tersebut.


⚡ Arsitektur Program (FreeRTOS Tasks)

AudioTask (Core 1, Priority 2): Menangani pembacaan MicroSD Card, decoding stream audio I2S via library ESP32-audioI2S, serta pemutaran lagu berikutnya secara otomatis jika EOF (End of File) tercapai.
UITask (Core 0, Priority 1): Mengurus rendering antarmuka pada sprite TFT_eSPI (240x240) yang tersimpan di PSRAM untuk menghindari efek flickering, animasi putaran kaset, serta status bar baterai.
InputTask (Core 0, Priority 1): Melakukan polling kondisi 5 tombol navigasi, menangani debouncing, long press, fast scroll, serta manajemen timeout backlight display.


📁 Struktur Repositori
.
├── Config.h          # Konfigurasi Pinout, Warna, Enum State, dan Variabel Global
├── AudioPlayer.h     # Header fungsi pemutar audio
├── AudioPlayer.cpp   # Implementasi FreeRTOS Task Audio & pembacaan SD Card
├── UIRenderer.h      # Header rendering antarmuka iPod & animasi kaset
├── UIRenderer.cpp    # Implementasi pembacaan ADC Baterai, Sprites, & Drawing UI
├── InputHandler.h    # Header penanganan input tombol
├── InputHandler.cpp  # Logic tombol (Short Press, Long Press, Scrubber, Auto-Off)
└── main.cpp / .ino   # Setup awal FreeRTOS tasks & Semaphore mutex


🚀 Requirement Library
Sebelum mengompilasi kode pada Arduino IDE / PlatformIO, pastikan library berikut telah terinstal:

TFT_eSPI (Konfigurasikan driver layar sesuai modul TFT 240x240 Anda di User_Setup.h).
ESP32-audioI2S (Library decoder audio I2S).
