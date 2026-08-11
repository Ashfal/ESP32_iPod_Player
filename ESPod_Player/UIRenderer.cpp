#include "UIRenderer.h"

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprUI = TFT_eSprite(&tft);
float cassetteAngle = 0.0;

void drawProgressBar(int percentage) {
    int barX = 30;
    int barY = 170;
    int barW = 180;
    int barH = 12;

    sprUI.drawRoundRect(barX, barY, barW, barH, 4, TFT_WHITE);
    
    int fillW = map(percentage, 0, 100, 0, barW - 4);
    fillW = constrain(fillW, 0, barW - 4);
    if (fillW > 0) {
        sprUI.fillRoundRect(barX + 2, barY + 2, fillW, barH - 4, 2, COLOR_IPOD_BLUE);
    }
}

void showSplashScreen() {
    uint32_t bootStartTime = millis();
    uint32_t bootDuration  = 2500; 

    while (millis() - bootStartTime < bootDuration) {

        int progress = map(millis() - bootStartTime, 0, bootDuration, 0, 100);
        progress = constrain(progress, 0, 100);

        sprUI.fillSprite(COLOR_IPOD_BG);

        sprUI.setTextColor(COLOR_IPOD_BLUE, COLOR_IPOD_BG);
        sprUI.setTextDatum(MC_DATUM);
        sprUI.drawString("ESPod Player", 120, 100, 4); 

        sprUI.setTextColor(COLOR_IPOD_DARK, COLOR_IPOD_BG);
        sprUI.drawString("System Starting...", 120, 135, 2);

        drawProgressBar(progress);

        sprUI.pushSprite(0, 0);

        sprUI.setTextDatum(TL_DATUM);

        vTaskDelay(33 / portTICK_PERIOD_MS);
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);
}

void drawCassetteSpool(int cx, int cy, float angle, int tapeRadius) {
    if (tapeRadius > 10) {
        sprUI.fillCircle(cx, cy, tapeRadius, COLOR_TAPE_BROWN);
    }

    sprUI.fillCircle(cx, cy, 10, TFT_WHITE);
    sprUI.drawCircle(cx, cy, 10, TFT_BLACK);

    for (int i = 0; i < 6; i++) {
        float rad = angle + (i * 60.0 * DEG_TO_RAD);
        int x1 = cx + cos(rad) * 4;
        int y1 = cy + sin(rad) * 4;
        int x2 = cx + cos(rad) * 9;
        int y2 = cy + sin(rad) * 9;
        sprUI.drawLine(x1, y1, x2, y2, TFT_BLACK);
    }

    sprUI.fillCircle(cx, cy, 4, COLOR_CASSETTE_WIN);
}

void drawCassetteAnimation(int x, int y, int w, int h) {
    sprUI.fillRoundRect(x, y, w, h, 6, COLOR_CASSETTE_BODY);
    sprUI.drawRoundRect(x, y, w, h, 6, TFT_BLACK);

    sprUI.fillRect(x + 5, y + 5, w - 10, 28, COLOR_CASSETTE_LABEL);
    sprUI.setTextColor(TFT_BLACK, COLOR_CASSETTE_LABEL);

    String title = String(currentTrackName);
    if (title.length() > 25) title = title.substring(0, 20) + "..";
    
    sprUI.drawString(title, x + 10, y + 8, 1);
    sprUI.drawString("C-90", x + w - 35, y + 8, 1);

    int winX = x + 15;
    int winY = y + 38;
    int winW = w - 30;
    int winH = h - 48;
    sprUI.fillRect(winX, winY, winW, winH, COLOR_CASSETTE_WIN);
    sprUI.drawRect(winX, winY, winW, winH, TFT_SILVER);

    int leftTapeRadius = 18;
    int rightTapeRadius = 11;

    if (audioTotalTime > 0) {
        int progressPercent = map(audioCurrentTime, 0, audioTotalTime, 0, 100);
        leftTapeRadius  = map(progressPercent, 0, 100, 18, 11);
        rightTapeRadius = map(progressPercent, 0, 100, 11, 18);
    }

    if (isPlaying) {
        cassetteAngle -= 0.25;
        if (cassetteAngle >= TWO_PI) cassetteAngle = 0;
    }

    int leftSpoolX  = winX + 25;
    int rightSpoolX = winX + winW - 25;
    int spoolY      = winY + (winH / 2);

    sprUI.drawFastHLine(leftSpoolX, spoolY + 14, rightSpoolX - leftSpoolX, COLOR_TAPE_BROWN);

    drawCassetteSpool(leftSpoolX, spoolY, cassetteAngle, leftTapeRadius);
    drawCassetteSpool(rightSpoolX, spoolY, cassetteAngle, rightTapeRadius);

    sprUI.fillTriangle(x + 25, y + h, x + 35, y + h - 10, x + w - 35, y + h - 10, COLOR_CASSETTE_BODY);
    sprUI.fillTriangle(x + 25, y + h, x + w - 35, y + h - 10, x + w - 25, y + h, COLOR_CASSETTE_BODY);
    sprUI.drawRect(x + 35, y + h - 10, w - 70, 10, TFT_BLACK);
}


void drawIpodHeader(String title) {
    sprUI.fillRect(0, 0, 240, 28, COLOR_IPOD_BLUE);
    sprUI.drawFastHLine(0, 27, 240, TFT_NAVY);
    
    sprUI.setTextColor(TFT_WHITE, COLOR_IPOD_BLUE);
    sprUI.drawString(title, 8, 6, 2);

    if (isPlaying) {
        sprUI.fillTriangle(175, 8, 175, 20, 183, 14, TFT_WHITE);
    } else {
        sprUI.fillRect(175, 8, 3, 12, TFT_WHITE);
        sprUI.fillRect(181, 8, 3, 12, TFT_WHITE);
    }

    int batX = 195;
    int batY = 8;
    int batW = 28;
    int batH = 13;

    sprUI.drawRect(batX, batY, batW, batH, TFT_WHITE);
    sprUI.fillRect(batX + batW, batY + 3, 2, 7, TFT_WHITE); 

    int fillW = map(batteryPercent, 0, 100, 0, batW - 4);
    fillW = constrain(fillW, 0, batW - 4);

    uint16_t batColor = (batteryPercent <= 20) ? TFT_RED : TFT_GREEN;
    
    if (fillW > 0) {
        sprUI.fillRect(batX + 2, batY + 2, fillW, batH - 4, batColor);
    }
}

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

uint8_t readBatteryPercentage() {

    uint32_t rawADC = analogRead(PIN_BATTERY);
    
    float pinVoltage = (rawADC / 4095.0) * 3.3;
    
    float batteryVoltage = pinVoltage * ((R1_VALUE + R2_VALUE) / R2_VALUE);

    if (batteryVoltage >= 4.2) return 100;
    if (batteryVoltage <= 3.3) return 0;

    int percent = (int)((batteryVoltage - 3.3) / (4.2 - 3.3) * 100.0);
    return constrain(percent, 0, 100);
}

void UITask(void *pvParameters) {
    tft.init();
    tft.setRotation(2);
    tft.fillScreen(COLOR_IPOD_BG);

    gpio_hold_dis(GPIO_NUM_27);
    gpio_deep_sleep_hold_dis();
    setDisplayPower(true);

    sprUI.setAttribute(PSRAM_ENABLE, true);
    sprUI.createSprite(240, 240);

    showSplashScreen();

    uint32_t lastUpdate = 0;

    pinMode(PIN_BATTERY,INPUT);

    static unsigned long lastBatCheck = 0;

    for (;;) {

        if (millis() - lastBatCheck > 5000) { // Cek tiap 5 detik
            batteryPercent = readBatteryPercentage();
            lastBatCheck = millis();
        }

        if (millis() - lastUpdate > 40) {
            lastUpdate = millis();
            sprUI.fillSprite(COLOR_IPOD_BG);

            if (currentState == STATE_MENU_VIEW) {
                drawIpodHeader(currentPath == "/" ? "ESPod" : currentPath);

                if (xSemaphoreTake(dataMutex, (TickType_t)5) == pdTRUE) {
                    int yOffset = 32;
                    int visibleCount = 7;
                    int startIdx = max(0, selectedIndex - 3);
                    int endIdx = min((int)fileList.size(), startIdx + visibleCount);

                    for (int i = startIdx; i < endIdx; i++) {
                        bool isSel = (i == selectedIndex);

                        if (isSel) {
                            sprUI.fillRect(0, yOffset - 2, 240, 26, COLOR_IPOD_SEL);
                            sprUI.setTextColor(TFT_WHITE, COLOR_IPOD_SEL);
                        } else {
                            sprUI.setTextColor(TFT_BLACK, COLOR_IPOD_BG);
                        }

                        String displayName = fileList[i].name;
                        if (displayName.length() > 28) displayName = displayName.substring(0, 26) + "..";
                        
                        sprUI.drawString(displayName, 10, yOffset + 2, 2);

                        if (fileList[i].isDir) {
                            sprUI.drawString(">", 225, yOffset + 2, 2);
                        }

                        if (!isSel) {
                            sprUI.drawFastHLine(10, yOffset + 23, 230, 0xE71C);
                        }

                        yOffset += 26;
                    }
                    xSemaphoreGive(dataMutex);
                }
            } 
            else if (currentState == STATE_NOW_PLAYING || currentState == STATE_SCRUBBER_MODE) {
                drawIpodHeader(currentState == STATE_SCRUBBER_MODE ? "Scrubber Mode" : "Now Playing");

                drawCassetteAnimation(20, 50, 200, 95);

                uint16_t barColor = (currentState == STATE_SCRUBBER_MODE) ? COLOR_SCRUBBER : COLOR_IPOD_BLUE;
                sprUI.drawRoundRect(12, 160, 216, 12, 3, COLOR_IPOD_BAR);
                
                uint32_t cTime = audioCurrentTime;
                uint32_t tTime = audioTotalTime;

                if (tTime > 0) {
                    int barWidth = map(cTime, 0, tTime, 0, 210);
                    sprUI.fillRoundRect(15, 162, constrain(barWidth, 0, 210), 8, 2, barColor);
                }

                char timeCurr[10], timeTot[10];
                snprintf(timeCurr, sizeof(timeCurr), "%02d:%02d", cTime / 60, cTime % 60);
                snprintf(timeTot, sizeof(timeTot), "-%02d:%02d", (tTime > cTime ? (tTime - cTime) / 60 : 0), (tTime > cTime ? (tTime - cTime) % 60 : 0));

                sprUI.setTextColor(COLOR_IPOD_DARK, COLOR_IPOD_BG);
                sprUI.drawString(timeCurr, 12, 177, 1);
                sprUI.drawString(timeTot, 195, 177, 1);

                sprUI.drawFastHLine(12, 198, 216, 0xE71C);
                sprUI.drawString("Vol:", 12, 207, 1);
                
                int volWidth = map(currentVolume, 0, 21, 0, 160);
                sprUI.fillRect(45, 209, 160, 6, 0xE71C);
                sprUI.fillRect(45, 209, volWidth, 6, COLOR_IPOD_SEL);
            }
            
           else if (currentState == STATE_BLUETOOTH_MODE) {
                drawIpodHeader("ESPod BT");

                static float btCassetteAngle = 0.0;
                if (isBtPlaying) {
                    btCassetteAngle += 5.0;
                    if (btCassetteAngle >= 360.0) btCassetteAngle -= 360.0;
                }

                int cassX = 20;
                int cassY = 60;
                int cassW = 200;
                int cassH = 110;

                sprUI.fillRoundRect(cassX, cassY, cassW, cassH, 6, COLOR_CASSETTE_BODY);
                sprUI.drawRoundRect(cassX, cassY, cassW, cassH, 6, COLOR_IPOD_DARK);

                int labelX = cassX + 10;
                int labelY = cassY + 8;
                int labelW = cassW - 20;
                int labelH = 34;

                sprUI.fillRect(labelX, labelY, labelW, labelH, COLOR_CASSETTE_LABEL);
                sprUI.drawRect(labelX, labelY, labelW, labelH, COLOR_IPOD_DARK);

                sprUI.setTextDatum(MC_DATUM);

                if (isBtConnected) {

                    String titleStr = String(btTrackTitle);
                    if (titleStr.length() > 18) {
                        titleStr = titleStr.substring(0, 16) + "..";
                    }

                    String artistStr = String(btTrackArtist);
                    if (artistStr.length() > 22) {
                        artistStr = artistStr.substring(0, 20) + "..";
                    }

                    sprUI.setTextColor(COLOR_IPOD_DARK, COLOR_CASSETTE_LABEL);
                    sprUI.drawString(titleStr, 120, labelY + 10, 2);

                    sprUI.setTextColor(COLOR_IPOD_DARK, COLOR_CASSETTE_LABEL);
                    sprUI.drawString(artistStr, 120, labelY + 24, 1);
                } else {

                    sprUI.setTextColor(COLOR_IPOD_DARK, COLOR_CASSETTE_LABEL);
                    sprUI.drawString("PAIRING...", 120, labelY + 12, 2);
                    sprUI.setTextColor(COLOR_IPOD_DARK, COLOR_CASSETTE_LABEL);
                    sprUI.drawString("ESPod_BT", 120, labelY + 25, 1);
                }

                int winX = cassX + 25;
                int winY = cassY + 48;
                int winW = cassW - 50;
                int winH = 48;

                sprUI.fillRect(winX, winY, winW, winH, COLOR_CASSETTE_WIN);
                sprUI.drawRect(winX, winY, winW, winH, COLOR_IPOD_DARK);

                int leftTapeRadius  = 16;
                int rightTapeRadius = 16;

                int leftSpoolX = winX + 32;
                int rightSpoolX = winX + winW - 32;
                int spoolY = winY + (winH / 2);

                drawCassetteSpool(leftSpoolX, spoolY, btCassetteAngle, leftTapeRadius);
                drawCassetteSpool(rightSpoolX, spoolY, btCassetteAngle, rightTapeRadius);

                uint32_t curSec = btPlayPosMs / 1000;
                char timeStr[32];
                
                snprintf(timeStr, sizeof(timeStr), "%02d:%02d", curSec / 60, curSec % 60);

                sprUI.setTextColor(TFT_WHITE,COLOR_CASSETTE_WIN );
                sprUI.drawString(timeStr, 120, spoolY, 2);

                sprUI.drawFastHLine(12, 198, 216, 0xE71C);
                
                int volWidth = map(currentVolume, 0, 21, 0, 160);
                sprUI.fillRect(45, 209, 160, 6, 0xE71C);
                sprUI.fillRect(45, 209, volWidth, 6, COLOR_IPOD_SEL);
                
                sprUI.setTextDatum(TL_DATUM);
                sprUI.setTextColor(COLOR_IPOD_DARK, COLOR_IPOD_BG);
                sprUI.drawString("Vol:", 12, 207, 1);
            }

            sprUI.pushSprite(0, 0);
        }

        vTaskDelay(15 / portTICK_PERIOD_MS);
    }
}
