#ifndef UI_RENDERER_H
#define UI_RENDERER_H

#include <TFT_eSPI.h>
#include "Config.h"

void drawHeader(String title);
void setDisplayPower(bool turnOn);
void showSplashScreen();
void drawProgressBar(int percentage);
void UITask(void *pvParameters);

#endif