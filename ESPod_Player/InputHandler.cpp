#include "InputHandler.h"
#include "BluetoothManager.h"

Button buttons[] = {
    {BTN_UP, false, 0, false},
    {BTN_DOWN, false, 0, false},
    {BTN_MENU, false, 0, false},
    {BTN_PLAY, false, 0, false},
    {BTN_CENTER, false, 0, false}
};

void enterDeepSleep() {
    Serial.println("Entering Deep Sleep...");

    audio.stopSong();
    setDisplayPower(false);

    esp_sleep_enable_ext0_wakeup((gpio_num_t)BTN_PLAY, 0); // 0 = LOW
    delay(3000);
    gpio_hold_en(GPIO_NUM_27);
    gpio_deep_sleep_hold_en();
    esp_deep_sleep_start();
}

void InputTask(void *pvParameters) {
    for (int i = 0; i < 5; i++) pinMode(buttons[i].pin, INPUT_PULLUP);

    const unsigned long LONG_PRESS_TIME = 800;
    const unsigned long FAST_SCROLL_INTERVAL = 80;
    unsigned long lastFastScroll = 0;

    for (;;) {
        unsigned long now = millis();

        if (isBacklightOn && (now - lastInteractionTime >= AUTO_OFF_TIMEOUT)) {
            setDisplayPower(false);
            Serial.println("Auto Display OFF (30s Timeout)");
        }

        for (int i = 0; i < 5; i++) {
            int pinState = digitalRead(buttons[i].pin);

            if (pinState == LOW && !buttons[i].isPressed) {
                if (!isBacklightOn) {
                    setDisplayPower(true);
                    lastInteractionTime = now;
                    buttons[i].isPressed = true;
                    buttons[i].longPressHandled = true;
                    continue;
                }

                lastInteractionTime = now;
                buttons[i].isPressed = true;
                buttons[i].pressStartTime = now;
                buttons[i].longPressHandled = false;
            }
            else if (pinState == LOW && buttons[i].isPressed) {
                if (now - buttons[i].pressStartTime >= LONG_PRESS_TIME && !buttons[i].longPressHandled) {
                    lastInteractionTime = now;

                    if (currentState == STATE_MENU_VIEW) {
                        if (buttons[i].pin == BTN_UP && (now - lastFastScroll > FAST_SCROLL_INTERVAL)) {
                            if (!fileList.empty()) selectedIndex = (selectedIndex - 1 + fileList.size()) % fileList.size();
                            lastFastScroll = now;
                        } else if (buttons[i].pin == BTN_DOWN && (now - lastFastScroll > FAST_SCROLL_INTERVAL)) {
                            if (!fileList.empty()) selectedIndex = (selectedIndex + 1) % fileList.size();
                            lastFastScroll = now;
                        } else if (buttons[i].pin == BTN_MENU) {
                            currentPath = "/";
                            readDirectory(currentPath);
                            buttons[i].longPressHandled = true;
                        }else if (buttons[i].pin == BTN_PLAY) {
                            buttons[i].longPressHandled = true;
                            enterDeepSleep();
                        }
                    } 
                    else if (currentState == STATE_NOW_PLAYING) {
                        if (buttons[i].pin == BTN_UP) {
                            if (audioCurrentTime > 3) audio.setAudioPlayTime(0);
                            else playPrevTrack();
                            buttons[i].longPressHandled = true;
                        } else if (buttons[i].pin == BTN_DOWN) {
                            playNextTrack();
                            buttons[i].longPressHandled = true;
                        } else if (buttons[i].pin == BTN_PLAY) {
                            buttons[i].longPressHandled = true;
                            enterDeepSleep();
                        }else if (buttons[i].pin == BTN_MENU) {
                            ESP.restart();
                            buttons[i].longPressHandled = true;
                        }
                    }
                    else if (currentState == STATE_SCRUBBER_MODE) {
                        if (buttons[i].pin == BTN_UP && (now - lastFastScroll > FAST_SCROLL_INTERVAL)) {
                            uint32_t newPos = constrain(audioCurrentTime + 5, 0, audioTotalTime);
                            audio.setAudioPlayTime(newPos);
                            lastFastScroll = now;
                        } else if (buttons[i].pin == BTN_DOWN && (now - lastFastScroll > FAST_SCROLL_INTERVAL)) {
                            uint32_t newPos = (audioCurrentTime > 1) ? (audioCurrentTime - 5) : 0;
                            audio.setAudioPlayTime(newPos);
                            lastFastScroll = now;
                        }
                    }

                    else if (currentState == STATE_BLUETOOTH_MODE) {
                        if (buttons[i].pin == BTN_UP) {
                            a2dp_sink.next();
                            buttons[i].longPressHandled = true;
                        }
                        else if (buttons[i].pin == BTN_DOWN) {
                            a2dp_sink.previous();
                            buttons[i].longPressHandled = true;
                        }else if (buttons[i].pin == BTN_MENU) {
                            ESP.restart();
                            buttons[i].longPressHandled = true;
                        }else if (buttons[i].pin == BTN_PLAY) {
                            buttons[i].longPressHandled = true;
                            enterDeepSleep();
                        } 
                    }
                }
            }
            else if (pinState == HIGH && buttons[i].isPressed) {
                unsigned long pressDuration = now - buttons[i].pressStartTime;

                if (pressDuration < LONG_PRESS_TIME && !buttons[i].longPressHandled) {
                    lastInteractionTime = now;
                    
                    if (currentState == STATE_MENU_VIEW) {
                        if (buttons[i].pin == BTN_UP) {
                            if (!fileList.empty()) selectedIndex = (selectedIndex - 1 + fileList.size()) % fileList.size();
                        } else if (buttons[i].pin == BTN_DOWN) {
                            if (!fileList.empty()) selectedIndex = (selectedIndex + 1) % fileList.size();
                        } else if (buttons[i].pin == BTN_CENTER) {
                            if (!fileList.empty() && fileList[selectedIndex].name == "[ Bluetooth Mode ]") {
                                currentState = STATE_BLUETOOTH_MODE;
                                startBluetoothMode();
                            } else {
                                if (isSdAvailable) {
                                    playSelectedItem();
                                }else{
                                currentState = STATE_BLUETOOTH_MODE;
                                startBluetoothMode();
                                }
                            }
                        } else if (buttons[i].pin == BTN_MENU) {
                            if (currentPath != "/") {
                                int lastSlash = currentPath.lastIndexOf('/');
                                if (lastSlash == 0) currentPath = "/";
                                else currentPath = currentPath.substring(0, lastSlash);
                                readDirectory(currentPath);
                            }
                        } else if (buttons[i].pin == BTN_PLAY) {
                            if (isPlaying) currentState = STATE_NOW_PLAYING;
                            else playSelectedItem();
                        }
                    }
                    else if (currentState == STATE_NOW_PLAYING) {
                        if (buttons[i].pin == BTN_UP) {
                            if (currentVolume < 21) { currentVolume++; audio.setVolume(currentVolume); }
                        } else if (buttons[i].pin == BTN_DOWN) {
                            if (currentVolume > 0) { currentVolume--; audio.setVolume(currentVolume); }
                        } else if (buttons[i].pin == BTN_PLAY) {
                            audio.pauseResume();
                            isPlaying = !isPlaying;
                        } else if (buttons[i].pin == BTN_CENTER) {
                            currentState = STATE_SCRUBBER_MODE;
                        } else if (buttons[i].pin == BTN_MENU) {
                            currentState = STATE_MENU_VIEW;
                        }
                    }
                    else if (currentState == STATE_SCRUBBER_MODE) {
                        if (buttons[i].pin == BTN_UP) {
                            uint32_t newPos = constrain(audioCurrentTime + 5, 0, audioTotalTime);
                            audio.setAudioPlayTime(newPos);
                        } else if (buttons[i].pin == BTN_DOWN) {
                            uint32_t newPos = (audioCurrentTime > 1) ? (audioCurrentTime - 5) : 0;
                            audio.setAudioPlayTime(newPos);
                        } else if (buttons[i].pin == BTN_CENTER) {
                            currentState = STATE_NOW_PLAYING;
                        }
                    }

                    else if (currentState == STATE_BLUETOOTH_MODE) {
                        if (buttons[i].pin == BTN_MENU) {
                            stopBluetoothMode(); 
                        }
                        else if (buttons[i].pin == BTN_PLAY) {
                            handleBluetoothPlayback();
                        }
                        else if (buttons[i].pin == BTN_UP) {
                            increaseBluetoothVolume();
                        }
                        else if (buttons[i].pin == BTN_DOWN) {;
                            decreaseBluetoothVolume();
                        }
                    }
                }

                buttons[i].isPressed = false;
            }
        }

        vTaskDelay(15 / portTICK_PERIOD_MS);
    }
}