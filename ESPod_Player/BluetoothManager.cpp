#include "BluetoothManager.h"
#include "AudioPlayer.h"

I2SClass i2s;
BluetoothA2DPSink a2dp_sink(i2s);

void volume_changed_callback(int volume) {
    currentVolume = map(volume, 0, 127, 0, 21);
    currentVolume = constrain(currentVolume, 0, 21);
}

void avrc_play_pos_callback(uint32_t play_pos) {
    btPlayPosMs = play_pos;
}

void avrc_playstatus_callback(esp_avrc_playback_stat_t playback) {
    isBtPlaying = (playback == ESP_AVRC_PLAYBACK_PLAYING);
}

void avrc_metadata_callback(uint8_t id, const uint8_t *text) {
    if (id == 0x1) { // Title
        snprintf(btTrackTitle, sizeof(btTrackTitle), "%s", text);
        btPlayPosMs = 0;
    } 
    else if (id == 0x2) { // Artist
        snprintf(btTrackArtist, sizeof(btTrackArtist), "%s", text);
    }
}

void connection_state_changed(esp_a2d_connection_state_t state, void *ptr) {
    if (state == ESP_A2D_CONNECTION_STATE_CONNECTED) {
        isBtConnected = true;
        snprintf(btDeviceName, sizeof(btDeviceName), "%s", a2dp_sink.get_peer_name());
        snprintf(btTrackTitle, sizeof(btTrackTitle), "Connected");
        snprintf(btTrackArtist, sizeof(btTrackArtist), "Select track on phone");
        setBluetoothVolume(currentVolume);
    } else if (state == ESP_A2D_CONNECTION_STATE_DISCONNECTED) {
        isBtConnected = false;
        snprintf(btDeviceName, sizeof(btDeviceName), "Disconnected");
        snprintf(btTrackTitle, sizeof(btTrackTitle), "Waiting...");
        snprintf(btTrackArtist, sizeof(btTrackArtist), "-");
    }
}

void startBluetoothMode() {
    Serial.println("[BT] Initializing A2DP Receiver Mode...");
    
    audio.stopSong();
    
    vTaskDelay(200 / portTICK_PERIOD_MS);

    i2s.end(); 
    vTaskDelay(50 / portTICK_PERIOD_MS);

    i2s.setPins(I2S_BCLK, I2S_LRCK, I2S_DOUT);
    if (!i2s.begin(I2S_MODE_STD, 44100, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO, I2S_STD_SLOT_BOTH)) {
        Serial.println("[BT] Critical: I2S Init Fail!");
        return;
    }

    Serial.printf("[SYS] Free Heap before BT Start: %d bytes\n", ESP.getFreeHeap());

    a2dp_sink.set_avrc_metadata_callback(avrc_metadata_callback);
    a2dp_sink.set_avrc_rn_play_pos_callback(avrc_play_pos_callback, 1);
    a2dp_sink.set_avrc_rn_playstatus_callback(avrc_playstatus_callback);
    a2dp_sink.set_on_connection_state_changed(connection_state_changed);
    a2dp_sink.set_stream_reader(NULL);
    a2dp_sink.set_on_volumechange(volume_changed_callback);

    a2dp_sink.start("ESPod_Audio");
}

void stopBluetoothMode() {
    Serial.println("[BT] Stopping A2DP...");
    a2dp_sink.end(true);
    vTaskDelay(150 / portTICK_PERIOD_MS);
    if (isSdAvailable) {
        ESP.restart();
    }else{
        startBluetoothMode();
    }
}

void handleBluetoothPlayback() {
    if (isBtConnected) {
        if (a2dp_sink.get_audio_state() == ESP_A2D_AUDIO_STATE_STARTED) {
            a2dp_sink.pause();
        } else {
            a2dp_sink.play();
        }
    }
}

void setBluetoothVolume(uint8_t vol) {
    int btVol = map(vol, 0, 21, 0, 127);
    btVol = constrain(btVol, 0, 127);
    a2dp_sink.set_volume(btVol);
}

void increaseBluetoothVolume() {
    if (currentVolume < 21) {
        currentVolume++;
        setBluetoothVolume(currentVolume);
    }
}

void decreaseBluetoothVolume() {
    if (currentVolume > 0) {
        currentVolume--;
        setBluetoothVolume(currentVolume);
    }
}