#include "AudioPlayer.h"

Audio audio;
SPIClass sdSPI(HSPI);

void readDirectory(String path) {
    if (xSemaphoreTake(dataMutex, portMAX_DELAY) == pdTRUE) {
        fileList.clear();
        selectedIndex = 0;

        File root = SD.open(path);
        if (root && root.isDirectory()) {
            File file = root.openNextFile();
            while (file) {
                String fname = String(file.name());
                if (file.isDirectory() || fname.endsWith(".mp3") || fname.endsWith(".wav") || fname.endsWith(".flac")) {
                    fileList.push_back({fname, file.isDirectory()});
                }
                file.close();
                file = root.openNextFile();
            }
            root.close();
        }
        xSemaphoreGive(dataMutex);
    }
}

void playSelectedItem() {
    if (fileList.empty()) return;
    
    FileItem selected = fileList[selectedIndex];

    if (selected.isDir) {
        if (currentPath == "/") currentPath += selected.name;
        else currentPath += "/" + selected.name;
        readDirectory(currentPath);
    } else {
        String fullPath = currentPath;
        if (!fullPath.endsWith("/")) fullPath += "/";
        fullPath += selected.name;

        snprintf(currentTrackName, sizeof(currentTrackName), "%s", selected.name.c_str());

        Serial.print("Opening file: ");
        Serial.println(fullPath);

        audio.stopSong(); 
        vTaskDelay(50 / portTICK_PERIOD_MS);

        if (xSemaphoreTake(dataMutex, portMAX_DELAY) == pdTRUE) {
            audio.connecttoFS(SD, fullPath.c_str());
            isPlaying = true;
            currentState = STATE_NOW_PLAYING;
            audioCurrentTime = 0;
            audioTotalTime = 0;
            xSemaphoreGive(dataMutex);
        }
    }
}

void playNextTrack() {
    if (fileList.empty()) return;

    int initialIndex = selectedIndex;
    bool foundNextAudio = false;

    for (size_t i = 0; i < fileList.size(); i++) {
        selectedIndex = (selectedIndex + 1) % fileList.size();
        if (!fileList[selectedIndex].isDir) {
            foundNextAudio = true;
            break;
        }
    }

    if (foundNextAudio) {
        playSelectedItem();
        Serial.println("Play next item");
    } else {
        selectedIndex = initialIndex;
        isPlaying = false;
    }
}

void playPrevTrack() {
    if (fileList.empty()) return;
    selectedIndex = (selectedIndex - 1 + fileList.size()) % fileList.size();
    if (fileList[selectedIndex].isDir) playPrevTrack();
    else playSelectedItem();
}


void my_audio_info(Audio::msg_t m) {
    switch (m.e) {
        case Audio::evt_eof:
            isPlaying = false;
            nextTrackRequested = true;
            break;
        default:
            break;
    }
}

void AudioTask(void *pvParameters) {
    sdSPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);

    if (!SD.begin(SD_CS, sdSPI, 20000000)) {
        Serial.println("SD Card Error!");
        vTaskDelete(NULL);
    }
    Audio::audio_info_callback = my_audio_info;

    audio.settings.DMA_DESC_NUM = 16;
    audio.settings.DMA_FRAME_NUM = 512;
    audio.settings.VOL_FADING_SPEED = 20.0;

    audio.settings.IIR_FILTER = true;
    audio.settings.QUALITY_SLOPE = 0.707;
    audio.settings.FREQ_LS_HZ = 200;
    audio.settings.FREQ_PEAK_HZ = 1000;
    audio.settings.FREQ_HS_HZ = 8000;
    audio.setTone(0.0, 0.0, 4.0);

    audio.setPinout(I2S_BCLK, I2S_LRCK, I2S_DOUT);
    audio.setVolume(currentVolume);
    
    readDirectory(currentPath);

    for (;;) {
        audio.loop();

        if (xSemaphoreTake(dataMutex, (TickType_t)2) == pdTRUE) {
            audioCurrentTime = audio.getAudioCurrentTime();
            audioTotalTime   = audio.getAudioFileDuration();
            xSemaphoreGive(dataMutex);
        }

        if (nextTrackRequested) {
            nextTrackRequested = false;
            vTaskDelay(100 / portTICK_PERIOD_MS); 
            Serial.println("[AudioTask] Playing next track safely...");
            playNextTrack();
        }

        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}