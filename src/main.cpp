#include <Arduino.h>

#include "app_config.h"
#include "ble_manager.h"
#include "display_manager.h"
#include "navigation_parser.h"
#include "navigation_state.h"

// 全域模組物件：讓 BLE、解析器、狀態與顯示器在整個韌體生命週期中持續存在。
BleManager bleManager;
DisplayManager displayManager;
NavigationParser navigationParser;
NavigationState navigationState;
uint32_t lastDisplayUpdate = 0;

// BLE 連線狀態回呼只做快速通知；實際的重連工作由 BleManager::loop() 處理。
void onConnectionChanged(bool connected) {
  Serial.println(connected ? "BLE connected." : "BLE disconnected.");
}

// ANCS 收到通知時執行。函式庫提供的 notification 指標只在回呼期間有效，
// 因此只在這裡讀取欄位，解析後由 NavigationState 保存需要的資料。
void onNotification(const ArduinoNotification* notification) {
  // 只處理兩個導航 App，避免社群、電話或其他通知更新導航畫面。
  const bool isNavigationApp =
      notification->type == AppConfig::AppleMapsAppId ||
      notification->type == AppConfig::GoogleMapsAppId;
  if (!isNavigationApp) {
    return;
  }

  Serial.println("========== 收到導航通知 ==========");
  Serial.println("App ID  : " + notification->type);
  Serial.println("Title   : " + notification->title);
  Serial.println("Message : " + notification->message);
  Serial.println("==================================");

  // 解析成功後，將結果複製到狀態物件，供主迴圈與 OLED 使用。
  NavigationData parsed;
  if (navigationParser.parse(notification->type, notification->title,
                             notification->message, notification->uuid,
                             parsed)) {
    navigationState.update(parsed);
  }
}

void setup() {
  // 開啟序列埠，供除錯與查看 ANCS 原始通知內容。
  Serial.begin(AppConfig::SerialBaudRate);
  delay(500);
  Serial.println();
  Serial.println("系統啟動中...");

  // 先顯示 OLED 啟動畫面，再啟動 BLE。
  displayManager.begin();

  // 註冊回呼並開始廣播，等待 iPhone 配對與通知授權。
  bleManager.begin(onNotification, onConnectionChanged);
  Serial.println("BLE ANCS 初始化完成！請打開 iPhone 藍牙進行配對...");
}

void loop() {
  // 處理 BLE 斷線後的重新廣播旗標。
  bleManager.loop();

  // 導航通知超過指定時間沒有更新時，清除舊資料。
  navigationState.expire(millis(), AppConfig::NavigationDataTimeoutMs);

  // OLED 不需要每次 loop 都重繪，固定週期更新即可降低處理負擔。
  if (millis() - lastDisplayUpdate >= AppConfig::DisplayRefreshIntervalMs) {
    lastDisplayUpdate = millis();
    displayManager.render(navigationState.data(), bleManager.isConnected());
  }

  // 讓 BLE 背景工作與其他系統任務有執行機會。
  delay(10);
}
