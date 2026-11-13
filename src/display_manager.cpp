#include "display_manager.h"

#include "app_config.h"

DisplayManager::DisplayManager()
    : display_(U8G2_R0, U8X8_PIN_NONE) {}

void DisplayManager::begin() {
  // 設定 I2C 速度並初始化 SSD1306 OLED。
  display_.setBusClock(400000);
  display_.begin();
  display_.setFont(u8g2_font_6x10_tf);

  // U8g2 先畫到記憶體 buffer，再一次送到螢幕，避免畫面閃爍。
  display_.clearBuffer();
  display_.drawStr(0, 12, "ESP32 Navigation");
  display_.drawStr(0, 28, "Waiting for iPhone");
  display_.sendBuffer();
}

void DisplayManager::render(const NavigationData& data, bool bleConnected) {
  // 每次重繪前清空 buffer，避免前一筆較長文字殘留在畫面上。
  display_.clearBuffer();
  display_.setFont(u8g2_font_6x10_tf);
  display_.drawStr(0, 10, bleConnected ? "BLE: connected" : "BLE: waiting");
  display_.setFont(u8g2_font_helvB14_tf);
  display_.drawStr(0, 32, actionLabel(data.action));
  display_.setFont(u8g2_font_6x10_tf);
  display_.drawStr(0, 46, data.distance.c_str());
  display_.drawUTF8(0, 60, data.street.c_str());
  // 將完整畫面送到 OLED。
  display_.sendBuffer();
}

const char* DisplayManager::actionLabel(NavigationAction action) {
  // 第一版先顯示文字；後續可在這裡改成方向箭頭 bitmap。
  switch (action) {
    case NavigationAction::Straight:
      return "STRAIGHT";
    case NavigationAction::TurnLeft:
      return "LEFT";
    case NavigationAction::TurnRight:
      return "RIGHT";
    case NavigationAction::SlightLeft:
      return "SLIGHT LEFT";
    case NavigationAction::SlightRight:
      return "SLIGHT RIGHT";
    case NavigationAction::UTurn:
      return "U-TURN";
    default:
      return "WAITING";
  }
}
