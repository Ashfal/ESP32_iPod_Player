#ifndef UI_RENDERER_H
#define UI_RENDERER_H

#include <TFT_eSPI.h>
#include "Config.h"

void drawCassetteSpool(int cx, int cy, float angle, int tapeRadius);
void drawCassetteAnimation(int x, int y, int w, int h);
void drawIpodHeader(String title);
void setDisplayPower(bool turnOn);
void showSplashScreen();
void drawProgressBar(int percentage);
void UITask(void *pvParameters);

#endif