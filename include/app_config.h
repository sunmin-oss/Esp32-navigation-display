#pragma once

#include <Arduino.h>

namespace AppConfig {
// 所有可調整的硬體與韌體參數集中在這裡，避免散落在各個模組。
constexpr uint32_t SerialBaudRate = 115200;
constexpr uint32_t DisplayRefreshIntervalMs = 100;
constexpr uint32_t NavigationDataTimeoutMs = 30000;
constexpr char BleDeviceName[] = "ESP32_Navi";
constexpr char AppleMapsAppId[] = "com.apple.Maps";
constexpr char GoogleMapsAppId[] = "com.google.Maps";

// OLED 控制器目前由 DisplayManager 固定使用 SSD1306，確認硬體後再切換 SH1106。
constexpr bool UseSh1106 = false;
constexpr uint8_t I2cSdaPin = 21;
constexpr uint8_t I2cSclPin = 22;
}  // namespace AppConfig
