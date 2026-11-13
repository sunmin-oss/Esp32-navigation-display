#include "navigation_parser.h"

#include "app_config.h"

bool NavigationParser::parse(const String& appId, const String& title,
                             const String& message, uint32_t notificationId,
                             NavigationData& output) const {
  // 解析器本身再次確認 App，讓它即使被其他呼叫端直接使用也保持安全。
  if (!isSupportedApp(appId)) {
    return false;
  }

  // 將 Title 與 Message 合併，因為兩個 Maps App 可能把方向和距離放在不同欄位。
  const String combined = title + " " + message;
  output.action = parseAction(combined);
  output.distance = parseDistance(combined);
  output.street = parseStreet(title, message);
  output.sourceApp = appId;
  output.notificationId = notificationId;
  output.updatedAt = millis();
  // 至少解析出方向、距離或路名其中一項，才接受這筆通知。
  return output.isValid();
}

NavigationAction NavigationParser::parseAction(const String& text) {
  // 先判斷較具體的方向，再判斷一般左／右轉，避免關鍵字互相誤判。
  if (text.indexOf("迴轉") >= 0 || text.indexOf("掉頭") >= 0 ||
      text.indexOf("U-turn") >= 0) {
    return NavigationAction::UTurn;
  }
  if (text.indexOf("微右轉") >= 0 || text.indexOf("稍向右") >= 0 ||
      text.indexOf("slight right") >= 0) {
    return NavigationAction::SlightRight;
  }
  if (text.indexOf("微左轉") >= 0 || text.indexOf("稍向左") >= 0 ||
      text.indexOf("slight left") >= 0) {
    return NavigationAction::SlightLeft;
  }
  if (text.indexOf("右轉") >= 0 || text.indexOf("向右") >= 0 ||
      text.indexOf("turn right") >= 0) {
    return NavigationAction::TurnRight;
  }
  if (text.indexOf("左轉") >= 0 || text.indexOf("向左") >= 0 ||
      text.indexOf("turn left") >= 0) {
    return NavigationAction::TurnLeft;
  }
  if (text.indexOf("直行") >= 0 || text.indexOf("繼續") >= 0 ||
      text.indexOf("straight") >= 0 || text.indexOf("continue") >= 0) {
    return NavigationAction::Straight;
  }
  return NavigationAction::Unknown;
}

String NavigationParser::parseDistance(const String& text) {
  // 目前支援常見的中英文距離單位；實際格式需再用手機通知樣本校正。
  const char* units[] = {"公里", "公尺", "km", "m", "mi", "ft"};
  for (const char* unit : units) {
    const int unitIndex = text.indexOf(unit);
    if (unitIndex < 0) {
      continue;
    }

    // 從單位往左找數字、小數點與空白，擷取完整距離字串。
    int start = unitIndex - 1;
    while (start >= 0 &&
           (isDigit(text[start]) || text[start] == '.' || text[start] == ' ')) {
      --start;
    }
    String distance = text.substring(start + 1, unitIndex + strlen(unit));
    distance.trim();
    if (distance.length() > 0) {
      return distance;
    }
  }
  return String();
}

String NavigationParser::parseStreet(const String& title, const String& message) {
  // The exact Maps format varies by locale. Keep the original fields until
  // format-specific rules are added from real ANCS samples.
  if (message.length() > 0) {
    return message;
  }
  return title;
}

bool NavigationParser::isSupportedApp(const String& appId) {
  // 只允許文件規劃中的 Apple Maps 與 Google Maps。
  return appId == AppConfig::AppleMapsAppId ||
         appId == AppConfig::GoogleMapsAppId;
}
