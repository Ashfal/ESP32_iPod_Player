#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <vector>

// ==========================================
// PIN CONFIGURATION
// ==========================================
#define I2S_BCLK        26
#define I2S_LRCK        25
#define I2S_DOUT        22

#define SD_CS           15
#define SD_MOSI         13
#define SD_MISO         12
#define SD_SCK          14

#define BTN_UP          32 
#define BTN_DOWN        21 
#define BTN_MENU        5 
#define BTN_PLAY        33 
#define BTN_CENTER      0  

#define PIN_BATTERY     35
#define R1_VALUE        11.2
#define R2_VALUE        10.0
#define MAX_VOLUME          21      // top of the 0..21 volume scale used everywhere
// ==========================================
// COLOR DEFINITIONS - Modern flat dark theme (RGB565)
// ==========================================
#define COLOR_BG          0x0821   // near-black charcoal background
#define COLOR_SURFACE      0x18C5   // card / panel surface
#define COLOR_SURFACE_ALT   0x2B4A   // slightly lighter surface (pressed/hover state)
#define COLOR_ACCENT       0x3CFE   // teal accent (selection, progress, primary)
#define COLOR_ACCENT_ALT     0xFAEB   // warm accent (scrubber / seek mode)
#define COLOR_TEXT         0xF7BE   // primary text, near white
#define COLOR_TEXT_MUTED     0x8C52   // secondary / muted text
#define COLOR_DIVIDER       0x2947   // hairline dividers
#define COLOR_GOOD          0x4EF0   // battery ok / connected
#define COLOR_WARN          0xFAEB   // battery low / warnings

// Legacy aliases kept so any external reference still compiles
#define COLOR_IPOD_BG       COLOR_BG
#define COLOR_SCRUBBER       COLOR_ACCENT_ALT

// ==========================================
// STRUCTURES & ENUMS
// ==========================================
enum PlayerState { 
    STATE_MENU_VIEW, 
    STATE_NOW_PLAYING, 
    STATE_SCRUBBER_MODE,
    STATE_BLUETOOTH_MODE 
};

struct FileItem {
    String name;
    bool isDir;
};

// ==========================================
// EXTERN GLOBALS
// ==========================================
extern volatile bool isSdAvailable;
extern volatile PlayerState currentState;
extern std::vector<FileItem> fileList;
extern int selectedIndex;
extern String currentPath;

extern volatile bool isPlaying;
extern volatile uint32_t audioCurrentTime;
extern volatile uint32_t audioTotalTime;
extern volatile uint8_t currentVolume;
extern char currentTrackName[64];
extern volatile bool nextTrackRequested;

extern uint32_t btPlayPosMs;
extern uint32_t btTrackLenMs;
extern bool isBtPlaying;
extern char btTrackTitle[64];
extern char btTrackArtist[64];
extern char localTrackTitle[64];
extern char localTrackArtist[64];

extern volatile uint8_t batteryPercent;
extern volatile bool isBacklightOn;
extern volatile unsigned long lastInteractionTime;
extern const unsigned long AUTO_OFF_TIMEOUT;

extern volatile bool isBtConnected;
extern char btDeviceName[32];

extern SemaphoreHandle_t dataMutex;

#endif