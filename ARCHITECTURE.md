# ESP32 導航顯示器程式架構

## 1. 系統資料流

```text
iPhone Maps
    │ iOS ANCS
    ▼
BLE Manager
    │ 複製 App ID、Title、Message、UUID
    ▼
Navigation Parser
    │ App 過濾、方向／距離／路名解析
    ▼
Navigation State
    │ 保留目前導航資料、逾時清除
    ▼
Display Manager
    │ U8g2
    ▼
OLED SSD1306／SH1106
```

## 2. 模組責任

| 模組 | 檔案 | 責任 |
|------|------|------|
| 應用設定 | `include/app_config.h` | 裝置名稱、序列埠、I2C 腳位、逾時與更新週期 |
| 導航資料模型 | `include/navigation_types.h` | 定義方向列舉與 `NavigationData` |
| BLE/ANCS | `include/ble_manager.h`, `src/ble_manager.cpp` | 啟動 ANCS、管理連線、重新廣播、轉交通知回呼 |
| 通知解析 | `include/navigation_parser.h`, `src/navigation_parser.cpp` | 過濾 Apple Maps／Google Maps 並解析導航欄位 |
| 導航狀態 | `include/navigation_state.h`, `src/navigation_state.cpp` | 保存目前資料並處理逾時清除 |
| OLED 顯示 | `include/display_manager.h`, `src/display_manager.cpp` | 初始化 U8g2、顯示連線狀態與導航資料 |
| 系統協調 | `src/main.cpp` | 初始化模組、執行主迴圈與模組間事件串接 |

## 3. 事件與生命週期

### 啟動

1. 初始化 Serial。
2. 初始化 OLED 並顯示等待畫面。
3. 註冊 ANCS 連線與通知回呼。
4. 啟動 BLE 廣播，等待 iPhone 配對。

### 收到通知

1. `BleManager` 收到 ANCS callback。
2. 立即複製 callback 內的資料，不保存原始指標。
3. `NavigationParser` 過濾非 Apple Maps／Google Maps 通知。
4. 解析成功後更新 `NavigationState`。
5. 主迴圈定期將狀態交給 `DisplayManager`。

### 斷線

1. BLE callback 將連線狀態設為 disconnected。
2. `BleManager::loop()` 在 callback 外重新啟動廣播。
3. OLED 顯示等待連線狀態。
4. 重新連線後恢復接收通知。

## 4. 已完成的骨架功能

- PlatformIO `esp32dev` 建置環境。
- ANCS BLE 函式庫初始化與 callback 接口；目前使用 `ArduinoNotification` API。
- Apple Maps／Google Maps App ID 過濾。
- 基本中文與英文方向關鍵字解析。
- 距離單位的基本解析。
- 導航資料狀態保存與 30 秒逾時清除。
- OLED 啟動畫面、BLE 狀態、方向文字、距離與路名顯示。
- BLE 斷線後重新廣播的事件流程。

## 5. 後續實作順序

1. 連接實機並確認 ANCS 配對與通知授權。
2. 收集 Apple Maps／Google Maps 的真實 Title、Message 樣本。
3. 依真實樣本修正距離與路名解析規則。
4. 將方向文字替換為箭頭 bitmap。
5. 加入通知移除事件與導航狀態清除規則。
6. 增加解析器單元測試與固定測試樣本。
7. 實作 BLE 重新連線退避與錯誤狀態提示。
8. 確認 OLED 控制器後補上 SH1106 顯示設定。
9. 進行長時間執行、記憶體與實機路測。

## 6. 目前設計限制

- 路名目前保留完整 Message，尚未依兩個 Maps App 的實際格式精確切割。
- 顯示器目前使用 SSD1306 128x64 硬體 I2C 建構子。
- 顯示方向目前為文字，箭頭圖示尚未加入。
- 尚未連接實體 ESP32，因此尚未驗證 iPhone ANCS 授權與通知格式。
- ANCS 函式庫會在背景工作執行緒處理封包，主程式不呼叫 `notifications.loop()`；
  `BleManager::loop()` 僅負責在 callback 外重新啟動廣播。
