#include "ble_manager.h"

#include "app_config.h"

BleManager* BleManager::instance_ = nullptr;

void BleManager::begin(NotificationHandler notificationHandler,
                        ConnectionHandler connectionHandler) {
  // ANCS 函式庫使用靜態 callback，因此保存目前的 BleManager 實例。
  instance_ = this;
  notificationHandler_ = notificationHandler;
  connectionHandler_ = connectionHandler;

  // 必須先註冊 callback，再呼叫 begin() 啟動 BLE。
  notifications_.setConnectionStateChangedCallback(onState);
  notifications_.setNotificationCallback(onNotification);
  notifications_.setRemovedCallback(onRemoved);

  // begin() 會初始化 ANCS 並開始廣播；失敗時保留錯誤訊息供序列埠診斷。
  if (!notifications_.begin(AppConfig::BleDeviceName)) {
    Serial.println("ANCS initialization failed.");
  }
}

void BleManager::loop() {
  // 不在 BLE callback 中直接重啟廣播，避免阻塞藍牙工作執行緒。
  if (advertiseAgain_.exchange(false)) {
    notifications_.startAdvertising();
  }
}

void BleManager::onState(BLENotifications::State state) {
  if (instance_ == nullptr) {
    return;
  }

  // callback 只更新旗標與通知應用層，耗時工作留給主迴圈。
  const bool connected = state == BLENotifications::StateConnected;
  instance_->connected_.store(connected);
  instance_->advertiseAgain_.store(!connected);
  if (instance_->connectionHandler_ != nullptr) {
    instance_->connectionHandler_(connected);
  }
}

void BleManager::onNotification(const ArduinoNotification* notification,
                                const Notification*) {
  // notification 只保證在函式庫 callback 期間有效，不能將指標保存到之後使用。
  if (instance_ != nullptr && instance_->notificationHandler_ != nullptr &&
      notification != nullptr) {
    instance_->notificationHandler_(notification);
  }
}

void BleManager::onRemoved(const ArduinoNotification*,
                           const Notification*) {
  // 第一版只監聽新增／更新通知，移除通知暫不改變導航狀態。
}
