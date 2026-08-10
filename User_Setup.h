// =========================================================================
// 1. DRIVER SELECTION
// =========================================================================
#define ST7789_DRIVER      // Gunakan driver ST7789

// Set resolusi persegi 240x240
#define TFT_WIDTH  240
#define TFT_HEIGHT 240

// =========================================================================
// 2. PIN CONFIGURATION (ESP32-WROVER)
// =========================================================================
#define TFT_MOSI 23        // SDA / MOSI
#define TFT_SCLK 18        // SCL / SCK
#define TFT_CS   -1        // Set -1 karena layar TIDAK MEMILIKI PIN CS
#define TFT_DC    2        // DC / RS Pin
#define TFT_RST   4        // RES / RST Pin
#define TFT_BL   19       // BL / Backlight Pin 

// =========================================================================
// 3. FONT SELECTION
// =========================================================================
#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define SMOOTH_FONT

// =========================================================================
// 4. SPI FREQUENCY
// =========================================================================
#define SPI_FREQUENCY  40000000  // ST7789 sangat stabil di 40MHz