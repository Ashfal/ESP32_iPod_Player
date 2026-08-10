#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include "Config.h"
#include "AudioPlayer.h"
#include "UIRenderer.h"

struct Button {
    uint8_t pin;
    bool isPressed;
    unsigned long pressStartTime;
    bool longPressHandled;
};

void InputTask(void *pvParameters);

#endif