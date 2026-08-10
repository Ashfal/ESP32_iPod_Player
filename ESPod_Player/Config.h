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
#define R1_VALUE        10.0 // kOhm
#define R2_VALUE        10.0 // kOhm

// ==========================================
// COLOR DEFINITIONS
// ==========================================
#define COLOR_IPOD_BLUE     0x231F  
#define COLOR_IPOD_SEL      0x041F  
#define COLOR_IPOD_BG       0xFFFF  
#define COLOR_IPOD_DARK     0x18E3  
#define COLOR_IPOD_BAR      0x8410  
#define COLOR_SCRUBBER      0xF800  

#define COLOR_CASSETTE_BODY  0x39E7  
#define COLOR_CASSETTE_LABEL 0xFD20  
#define COLOR_CASSETTE_WIN   0x2104  
#define COLOR_TAPE_BROWN     0x59A0  

// ==========================================
// STRUCTURES & ENUMS
// ==========================================
enum PlayerState { 
    STATE_MENU_VIEW, 
    STATE_NOW_PLAYING, 
    STATE_SCRUBBER_MODE 
};

struct FileItem {
    String name;
    bool isDir;
};

// ==========================================
// EXTERN GLOBALS
// ==========================================
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

extern volatile uint8_t batteryPercent;
extern volatile bool isBacklightOn;
extern volatile unsigned long lastInteractionTime;
extern const unsigned long AUTO_OFF_TIMEOUT;

extern SemaphoreHandle_t dataMutex;

#endif