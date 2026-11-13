#pragma once

#include <atomic>

#include <esp32notifications.h>

class BleManager {
 public:
  using NotificationHandler = void (*)(const ArduinoNotification*);
  using ConnectionHandler = void (*)(bool connected);

  void begin(NotificationHandler notificationHandler,
             ConnectionHandler connectionHandler);
  void loop();
  bool isConnected() const { return connected_; }

 private:
  static BleManager* instance_;
  static void onState(BLENotifications::State state);
  static void onNotification(const ArduinoNotification* notification,
                             const Notification* raw);
  static void onRemoved(const ArduinoNotification* notification,
                        const Notification* raw);

  BLENotifications notifications_;
  NotificationHandler notificationHandler_ = nullptr;
  ConnectionHandler connectionHandler_ = nullptr;
  std::atomic<bool> advertiseAgain_{false};
  std::atomic<bool> connected_{false};
};
