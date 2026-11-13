#pragma once

#include <U8g2lib.h>

#include "navigation_types.h"

class DisplayManager {
 public:
  DisplayManager();
  void begin();
  void render(const NavigationData& data, bool bleConnected);

 private:
  U8G2_SSD1306_128X64_NONAME_F_HW_I2C display_;
  static const char* actionLabel(NavigationAction action);
};
