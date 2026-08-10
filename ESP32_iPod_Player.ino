#include "Config.h"
#include "AudioPlayer.h"
#include "UIRenderer.h"
#include "InputHandler.h"

// Definisi variabel global
volatile PlayerState currentState = STATE_MENU_VIEW;
std::vector<FileItem> fileList;
int selectedIndex = 0;
String currentPath = "/";

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

    setDisplayPower(true);
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