#pragma once

#include <Arduino.h>

#include "navigation_types.h"

class NavigationParser {
 public:
  bool parse(const String& appId, const String& title, const String& message,
             uint32_t notificationId, NavigationData& output) const;

 private:
  static NavigationAction parseAction(const String& text);
  static String parseDistance(const String& text);
  static String parseStreet(const String& title, const String& message);
  static bool isSupportedApp(const String& appId);
};
