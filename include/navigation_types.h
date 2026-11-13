#pragma once

#include <Arduino.h>

enum class NavigationAction : uint8_t {
  Unknown,
  Straight,
  TurnLeft,
  TurnRight,
  SlightLeft,
  SlightRight,
  UTurn,
};

struct NavigationData {
  NavigationAction action = NavigationAction::Unknown;
  String distance;
  String street;
  String sourceApp;
  uint32_t notificationId = 0;
  uint32_t updatedAt = 0;

  bool isValid() const {
    return action != NavigationAction::Unknown || distance.length() > 0 ||
           street.length() > 0;
  }
};
