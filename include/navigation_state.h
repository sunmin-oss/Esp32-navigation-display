#pragma once

#include "navigation_types.h"

class NavigationState {
 public:
  void update(const NavigationData& data);
  void clear();
  void expire(uint32_t now, uint32_t timeoutMs);
  const NavigationData& data() const { return current_; }

 private:
  NavigationData current_;
};
