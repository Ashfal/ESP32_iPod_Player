#ifndef BLUETOOTH_MANAGER_H
#define BLUETOOTH_MANAGER_H

#include "Config.h"
#include "ESP_I2S.h"
#include "BluetoothA2DPSink.h"

extern I2SClass i2s;
extern BluetoothA2DPSink a2dp_sink;

void startBluetoothMode();
void stopBluetoothMode();
void handleBluetoothPlayback();

void setBluetoothVolume(uint8_t vol);
void increaseBluetoothVolume();
void decreaseBluetoothVolume();

#endif