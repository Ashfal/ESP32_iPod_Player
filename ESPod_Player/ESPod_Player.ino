#include "Config.h"
#include "AudioPlayer.h"
#include "UIRenderer.h"
#include "InputHandler.h"

// Definisi variabel global
volatile PlayerState currentState = STATE_MENU_VIEW;
std::vector<FileItem> fileList;
int selectedIndex = 0;
String currentPath = "/";

volatile bool isBtConnected = false;
uint32_t btPlayPosMs = 0;
uint32_t btTrackLenMs = 0;
bool isBtPlaying = false;
char btDeviceName[32] = "Waiting Connection...";
char btTrackTitle[64]  = "Unknown Title";
char btTrackArtist[64] = "Unknown Artist";

volatile bool isPlaying = false;
volatile uint32_t audioCurrentTime = 0;
volatile uint32_t audioTotalTime   = 0;
volatile uint8_t currentVolume     = 12;
char currentTrackName[64] = "Belum Ada Lagu";
volatile bool nextTrackRequested = false;

volatile uint8_t batteryPercent = 100;
volatile bool isBacklightOn = true;
volatile unsigned long lastInteractionTime = 0;
const unsigned long AUTO_OFF_TIMEOUT = 30000;

SemaphoreHandle_t dataMutex;
TaskHandle_t AudioTaskHandle = NULL;

void setup() {
    Serial.begin(115200);

    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
    if (wakeup_reason == ESP_SLEEP_WAKEUP_EXT0) {
        while (digitalRead(BTN_PLAY) == LOW) {
            delay(10);
        }
    }

    lastInteractionTime = millis();

    if (psramInit()) {
        Serial.println("PSRAM Active!");
    }

    dataMutex = xSemaphoreCreateMutex();

    xTaskCreatePinnedToCore(AudioTask, "AudioTask", 8192, NULL, 2, &AudioTaskHandle, 1);
    xTaskCreatePinnedToCore(UITask,    "UITask",    4096, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(InputTask, "InputTask", 4096, NULL, 1, NULL, 0);
}

void loop() {
    vTaskDelete(NULL);
}