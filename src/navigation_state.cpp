#include "navigation_state.h"

// 將解析器產生的資料複製到目前導航狀態。
void NavigationState::update(const NavigationData& data) { current_ = data; }

// 清除導航資料，讓顯示器回到等待狀態。
void NavigationState::clear() { current_ = NavigationData(); }

void NavigationState::expire(uint32_t now, uint32_t timeoutMs) {
  // 使用無號整數差值可正確處理 millis() 溢位後的時間比較。
  if (current_.isValid() && now - current_.updatedAt >= timeoutMs) {
    clear();
  }
}
