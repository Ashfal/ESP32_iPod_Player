#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

#include <Audio.h>
#include <SPI.h>
#include <SD.h>
#include "Config.h"

extern Audio audio;

void readDirectory(String path);
void playSelectedItem();
void playNextTrack();
void playPrevTrack();
void my_audio_info(Audio::msg_t m);
void AudioTask(void *pvParameters);

#endif