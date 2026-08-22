#include "UIRenderer.h"

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprUI = TFT_eSprite(&tft);

// ==========================================
// LAYOUT CONSTANTS
// ==========================================
static const int HEADER_H     = 28;
static const int CARD_X       = 20;
static const int CARD_Y       = 34;
static const int CARD_W       = 200;
static const int CARD_H       = 106;
static const int SCREEN_W     = 240;

// ==========================================
// SMALL, REUSABLE FLAT-UI PRIMITIVES
// ==========================================

static void drawFlatBar(int x, int y, int w, int h, int pct, uint16_t trackColor, uint16_t fillColor) {
    pct = constrain(pct, 0, 100);
    sprUI.fillRoundRect(x, y, w, h, h / 2, trackColor);
    int fillW = map(pct, 0, 100, 0, w);
    if (fillW > 0) {
        fillW = max(fillW, h); // keep the rounded cap visible even at low %
        fillW = min(fillW, w);
        sprUI.fillRoundRect(x, y, fillW, h, h / 2, fillColor);
    }
}

static void drawPlayGlyph(int cx, int cy, int r, bool playing, uint16_t color) {
    sprUI.drawCircle(cx, cy, r, color);
    sprUI.drawCircle(cx, cy, r - 1, color);
    if (playing) {
        sprUI.fillRoundRect(cx - 8, cy - 9, 5, 18, 2, color);
        sprUI.fillRoundRect(cx + 3, cy - 9, 5, 18, 2, color);
    } else {
        sprUI.fillTriangle(cx - 6, cy - 10, cx - 6, cy + 10, cx + 10, cy, color);
    }
}

static void drawEqualizerBars(int x, int y, int w, int h, bool active) {
    static float level[5] = {0.35f, 0.55f, 0.42f, 0.6f, 0.38f};
    static float phase = 0.0f;

    int gap = 4;
    int barW = (w - gap * 4) / 5;

    for (int i = 0; i < 5; i++) {
        float target = active ? (0.25f + 0.75f * fabsf(sinf(phase + i * 0.9f))) : 0.14f;
        level[i] += (target - level[i]) * 0.35f;

        int barH = (int)(h * level[i]);
        barH = max(barH, 3);
        int bx = x + i * (barW + gap);
        int by = y + h - barH;
        sprUI.fillRoundRect(bx, by, barW, barH, 2, COLOR_ACCENT);
    }
    phase += active ? 0.20f : 0.05f;
}

static void drawFolderIcon(int x, int y, uint16_t color) {
    sprUI.fillRoundRect(x, y + 3, 7, 4, 1, color);
    sprUI.fillRoundRect(x, y + 6, 16, 10, 2, color);
}

static void drawMusicIcon(int x, int y, uint16_t color) {
    sprUI.fillCircle(x, y + 15, 3, color);
    sprUI.fillCircle(x + 10, y + 13, 3, color);
    sprUI.fillRect(x + 2, y, 2, 15, color);
    sprUI.fillRect(x + 2, y, 10, 2, color);
    sprUI.fillRect(x + 10, y - 2, 2, 15, color);
}

static void drawBTGlyph(int cx, int cy, int s, uint16_t color) {
    int top = cy - s;
    int bot = cy + s;
    sprUI.drawLine(cx, top, cx, bot, color);
    sprUI.drawLine(cx, top, cx + s, top + s / 2, color);
    sprUI.drawLine(cx + s, top + s / 2, cx - s, bot - s / 2, color);
    sprUI.drawLine(cx, bot, cx + s, bot - s / 2, color);
    sprUI.drawLine(cx + s, bot - s / 2, cx - s, top + s / 2, color);
}

static void drawBatteryIcon(int x, int y) {
    const int w = 24, h = 12;
    sprUI.drawRoundRect(x, y, w, h, 2, COLOR_TEXT_MUTED);
    sprUI.fillRoundRect(x + w, y + 3, 2, h - 6, 1, COLOR_TEXT_MUTED);

    int fillW = map(batteryPercent, 0, 100, 0, w - 4);
    fillW = constrain(fillW, 0, w - 4);
    uint16_t batColor = (batteryPercent <= 20) ? COLOR_WARN : COLOR_GOOD;
    if (fillW > 0) sprUI.fillRoundRect(x + 2, y + 2, fillW, h - 4, 1, batColor);
}

static String truncate(String s, int maxLen) {
    if (s.length() > (unsigned)maxLen) return s.substring(0, maxLen - 2) + "..";
    return s;
}

// ==========================================
// HEADER (shared across every screen)
// ==========================================
void drawHeader(String title) {
    sprUI.setTextDatum(TL_DATUM);
    sprUI.setTextColor(COLOR_TEXT, COLOR_BG);
    sprUI.drawString(truncate(title, 14), 14, 7, 2);

    // playback status dot
    uint16_t dotColor = isPlaying ? COLOR_ACCENT : COLOR_TEXT_MUTED;
    sprUI.fillCircle(180, 13, 4, dotColor);

    drawBatteryIcon(196, 8);

    sprUI.drawFastHLine(0, HEADER_H - 1, SCREEN_W, COLOR_DIVIDER);
}

// ==========================================
// SPLASH SCREEN
// ==========================================
void drawProgressBar(int percentage) {
    drawFlatBar(40, 165, 160, 6, percentage, COLOR_DIVIDER, COLOR_ACCENT);
}

void showSplashScreen() {
    uint32_t bootStartTime = millis();
    uint32_t bootDuration  = 2000;

    while (millis() - bootStartTime < bootDuration) {
        int progress = map(millis() - bootStartTime, 0, bootDuration, 0, 100);
        progress = constrain(progress, 0, 100);

        sprUI.fillSprite(COLOR_BG);

        sprUI.setTextDatum(MC_DATUM);
        sprUI.setTextColor(COLOR_TEXT, COLOR_BG);
        sprUI.drawString("ESPod Audio", 120, 100, 4);

        sprUI.setTextColor(COLOR_TEXT_MUTED, COLOR_BG);
        sprUI.drawString("Starting up", 120, 132, 2);

        drawProgressBar(progress);

        sprUI.setTextColor(COLOR_TEXT, COLOR_BG);
        sprUI.setTextDatum(BC_DATUM);
        sprUI.drawString("Created by @ashfal_f", 120, 220, 2);

        sprUI.pushSprite(0, 0);
        sprUI.setTextDatum(TL_DATUM);

        vTaskDelay(33 / portTICK_PERIOD_MS);
    }
    vTaskDelay(300 / portTICK_PERIOD_MS);
}

// ==========================================
// MENU / FILE BROWSER
// ==========================================
static void renderMenu() {
    if (!isSdAvailable) {
        sprUI.setTextDatum(MC_DATUM);
        sprUI.setTextColor(COLOR_ACCENT, COLOR_BG);
        sprUI.drawString("NO SD CARD", 120, 100, 4);
        
        sprUI.setTextColor(COLOR_TEXT_MUTED, COLOR_BG);
        sprUI.drawString("Press CENTER for Bluetooth", 120, 140, 2);
    } 
    else {
         String header = "Library";
        if (currentPath != "/") {
            int lastSlash = currentPath.lastIndexOf('/');
            header = currentPath.substring(lastSlash + 1);
        }
        drawHeader(header);

        if (xSemaphoreTake(dataMutex, (TickType_t)5) == pdTRUE) {
            const int top = HEADER_H + 6;
            const int rowH = 32;
            const int rowGap = 4;
            const int visibleCount = (SCREEN_W - top) / rowH;

            int startIdx = max(0, selectedIndex - visibleCount / 2);
            int endIdx = min((int)fileList.size(), startIdx + visibleCount);
            startIdx = max(0, min(startIdx, endIdx - visibleCount));
            startIdx = max(0, startIdx);

            int y = top;
            for (int i = startIdx; i < endIdx; i++) {
                bool isSel = (i == selectedIndex);
                uint16_t rowBg = isSel ? COLOR_SURFACE : COLOR_BG;
                uint16_t fg = isSel ? COLOR_TEXT : COLOR_TEXT_MUTED;
                uint16_t iconColor = isSel ? COLOR_ACCENT : COLOR_TEXT_MUTED;

                if (isSel) {
                    sprUI.fillRoundRect(10, y, 220, rowH - rowGap, 10, COLOR_SURFACE);
                    sprUI.fillRoundRect(10, y, 4, rowH - rowGap, 2, COLOR_ACCENT);
                }

                if (fileList[i].isDir) drawFolderIcon(26, y + 8, iconColor);
                else drawMusicIcon(26, y + 6, iconColor);

                sprUI.setTextColor(fg, rowBg);
                sprUI.setTextDatum(TL_DATUM);
                sprUI.drawString(truncate(fileList[i].name, 24), 48, y + 9, 2);

                if (fileList[i].isDir) {
                    int cx = 216, cy = y + (rowH - rowGap) / 2;
                    sprUI.drawLine(cx, cy - 5, cx + 5, cy, iconColor);
                    sprUI.drawLine(cx + 5, cy, cx, cy + 5, iconColor);
                }

                y += rowH;
            }

            if (fileList.empty()) {
                sprUI.setTextDatum(MC_DATUM);
                sprUI.setTextColor(COLOR_TEXT_MUTED, COLOR_BG);
                sprUI.drawString("No tracks found, Entering Bluetooth Mode", 120, 130, 2);
                sprUI.setTextDatum(TL_DATUM);
            }

            xSemaphoreGive(dataMutex);
        }
    }
    
   
}

// ==========================================
// NOW PLAYING / SCRUBBER
// ==========================================
static void renderNowPlaying(bool scrubMode) {
    drawHeader(scrubMode ? "Seeking" : "Now Playing");

    uint16_t accent = scrubMode ? COLOR_ACCENT_ALT : COLOR_ACCENT;

    sprUI.fillRoundRect(CARD_X, CARD_Y, CARD_W, CARD_H, 14, COLOR_SURFACE);
    if (scrubMode) sprUI.drawRoundRect(CARD_X, CARD_Y, CARD_W, CARD_H, 14, COLOR_ACCENT_ALT);

    drawPlayGlyph(120, CARD_Y + 38, 24, isPlaying, accent);
    drawEqualizerBars(70, CARD_Y + 72, 100, 24, isPlaying);

    sprUI.setTextDatum(MC_DATUM);
    sprUI.setTextColor(COLOR_TEXT, COLOR_BG);
    sprUI.drawString(truncate(String(localTrackTitle), 20), 120, 156, 2);

    sprUI.setTextColor(COLOR_TEXT_MUTED, COLOR_BG);
    sprUI.drawString(truncate(String(localTrackArtist), 30), 120, 178, 1);
    sprUI.setTextDatum(TL_DATUM);

    uint32_t cTime = audioCurrentTime;
    uint32_t tTime = audioTotalTime;
    int pct = (tTime > 0) ? map(cTime, 0, tTime, 0, 100) : 0;

    drawFlatBar(15, 194, 210, 6, pct, COLOR_DIVIDER, accent);

    char timeCurr[8], timeTot[8];
    snprintf(timeCurr, sizeof(timeCurr), "%02lu:%02lu", cTime / 60, cTime % 60);
    snprintf(timeTot, sizeof(timeTot), "%02lu:%02lu", tTime / 60, tTime % 60);

    sprUI.setTextColor(COLOR_TEXT_MUTED, COLOR_BG);
    sprUI.drawString(timeCurr, 15, 204, 1);
    sprUI.setTextDatum(TR_DATUM);
    sprUI.drawString(timeTot, 225, 204, 1);
    sprUI.setTextDatum(TL_DATUM);

    sprUI.drawFastHLine(15, 216, 210, COLOR_DIVIDER);

    sprUI.setTextColor(COLOR_TEXT_MUTED, COLOR_BG);
    sprUI.drawString("VOL", 15, 222, 1);
    int volPct = map(currentVolume, 0, MAX_VOLUME, 0, 100);
    drawFlatBar(45, 221, 165, 6, volPct, COLOR_DIVIDER, COLOR_ACCENT);
}

// ==========================================
// BLUETOOTH MODE
// ==========================================
static void renderBluetooth() {
    drawHeader("ESPod Audio");

    sprUI.fillRoundRect(CARD_X, CARD_Y, CARD_W, CARD_H, 14, COLOR_SURFACE);

    sprUI.setTextDatum(MC_DATUM);

    if (isBtConnected) {
        drawBTGlyph(36, CARD_Y + 18, 7, COLOR_ACCENT);
        sprUI.fillCircle(50, CARD_Y + 12, 3, COLOR_GOOD);

        sprUI.setTextColor(COLOR_TEXT, COLOR_SURFACE);
        sprUI.drawString(truncate(String(btTrackTitle), 20), 120, CARD_Y + 22, 2);

        sprUI.setTextColor(COLOR_TEXT_MUTED, COLOR_SURFACE);
        sprUI.drawString(truncate(String(btTrackArtist), 26), 120, CARD_Y + 40, 1);

        drawEqualizerBars(70, CARD_Y + 62, 100, 30, isBtPlaying);
    } else {
        drawBTGlyph(120, CARD_Y + 32, 16, COLOR_TEXT_MUTED);
        sprUI.setTextColor(COLOR_TEXT, COLOR_SURFACE);
        sprUI.drawString("Waiting for device", 120, CARD_Y + 66, 2);
        sprUI.setTextColor(COLOR_TEXT_MUTED, COLOR_SURFACE);
        sprUI.drawString("ESPod_Audio", 120, CARD_Y + 86, 1);
    }

    uint32_t curSec = btPlayPosMs / 1000;
    char timeStr[8];
    snprintf(timeStr, sizeof(timeStr), "%02lu:%02lu", curSec / 60, curSec % 60);

    sprUI.setTextColor(COLOR_TEXT, COLOR_BG);
    sprUI.drawString(timeStr, 120, 168, 4);

    sprUI.setTextColor(COLOR_TEXT_MUTED, COLOR_BG);
    sprUI.drawString(isBtConnected ? "Streaming from phone" : "Not connected", 120, 190, 1);
    sprUI.setTextDatum(TL_DATUM);

    sprUI.drawFastHLine(15, 204, 210, COLOR_DIVIDER);
    sprUI.setTextColor(COLOR_TEXT_MUTED, COLOR_BG);
    sprUI.drawString("VOL", 15, 212, 1);
    int volPct = map(currentVolume, 0, MAX_VOLUME, 0, 100);
    drawFlatBar(45, 211, 165, 6, volPct, COLOR_DIVIDER, COLOR_ACCENT);
}

// ==========================================
// DISPLAY POWER
// ==========================================
void setDisplayPower(bool turnOn) {
    if (xSemaphoreTake(dataMutex, portMAX_DELAY) == pdTRUE) {
        if (turnOn) {
            tft.writecommand(ST7789_SLPOUT);
            tft.writecommand(ST7789_DISPON);
            vTaskDelay(120 / portTICK_PERIOD_MS);
        } else {
            tft.writecommand(ST7789_DISPOFF);
            tft.writecommand(ST7789_SLPIN);
            vTaskDelay(120 / portTICK_PERIOD_MS);
        }
        digitalWrite(TFT_BL, turnOn ? HIGH : LOW);
        isBacklightOn = turnOn;
        xSemaphoreGive(dataMutex);
    }
}

static uint8_t readBatteryPercentage() {
    uint32_t rawADC = analogRead(PIN_BATTERY);
    float pinVoltage = (rawADC / 4095.0) * 3.3;
    float batteryVoltage = pinVoltage * ((R1_VALUE + R2_VALUE) / R2_VALUE);

    if (batteryVoltage >= 4.2) return 100;
    if (batteryVoltage <= 3.3) return 0;

    int percent = (int)((batteryVoltage - 3.3) / (4.2 - 3.3) * 100.0);
    return constrain(percent, 0, 100);
}

// ==========================================
// MAIN UI TASK
// ==========================================
void UITask(void *pvParameters) {
    tft.init();
    tft.setRotation(2);
    tft.fillScreen(COLOR_BG);

    gpio_hold_dis(GPIO_NUM_27);
    gpio_deep_sleep_hold_dis();
    setDisplayPower(true);

    sprUI.setAttribute(PSRAM_ENABLE, true);
    sprUI.createSprite(240, 240);

    showSplashScreen();

    uint32_t lastUpdate = 0;
    static unsigned long lastBatCheck = 0;

    pinMode(PIN_BATTERY, INPUT);

    for (;;) {
        if (millis() - lastBatCheck > 5000) {
            batteryPercent = readBatteryPercentage();
            lastBatCheck = millis();
        }

        if (millis() - lastUpdate > 40) {
            lastUpdate = millis();
            sprUI.fillSprite(COLOR_BG);

            switch (currentState) {
                case STATE_MENU_VIEW:
                    renderMenu();
                    break;
                case STATE_NOW_PLAYING:
                    renderNowPlaying(false);
                    break;
                case STATE_SCRUBBER_MODE:
                    renderNowPlaying(true);
                    break;
                case STATE_BLUETOOTH_MODE:
                    renderBluetooth();
                    break;
            }

            sprUI.pushSprite(0, 0);
        }

        vTaskDelay(15 / portTICK_PERIOD_MS);
    }
}
