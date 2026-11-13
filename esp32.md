# 專案開發文件：ESP32 藍牙導航顯示器 (iOS ANCS 版)

## 1. 專案概述 (Project Overview)

本專案旨在開發一款基於 ESP32 微控制器的外接式智慧型導航顯示器。透過藍牙低功耗 (BLE) 技術與 iOS 的 ANCS (Apple Notification Center Service) 協定，ESP32 能夠直接攔截並讀取 iPhone 鎖定畫面上的導航通知，**同時支援 Apple Maps 與 Google Maps (iOS版)**。

系統會將擷取到的通知內容進行解析，萃取出「轉彎方向」、「距離」與「路名」後，即時繪製於 OLED 顯示螢幕上。此方案的優勢在於**無須開發任何 iOS App**，完全依賴系統原生的藍牙通知推播機制。

## 2. 系統需求與配置 (Requirements & Configuration)

### 2.1 硬體需求 (Hardware)

* **核心控制板**：ESP32 開發板 (推薦使用 ESP32-WROOM 或 ESP32-S3，內建藍牙與足夠的快閃記憶體)。
* **顯示模組**：0.96 吋或 1.3 吋 OLED 螢幕 (SSD1306 或 SH1106 控制晶片，I2C 介面)。
* **連接線材**：杜邦線若干。
* **電源供應**：行動電源或機車/腳踏車上的 USB 供電模組 (5V 轉 3.3V 由 ESP32 開發板內建負責)。

### 2.2 軟體與開發環境 (Software & Environment)

* **開發 IDE**：Visual Studio Code (VS Code)。
* **編譯框架**：PlatformIO Extension。
* **核心函式庫**：
  * `Krnza/ESP32-ANCS-Notifications`：處理與 iOS 的 BLE 握手及 ANCS 通知訂閱。
  * `olikraus/U8g2`：用於 OLED 螢幕的高效能圖形與文字繪製。

### 2.3 專案配置參數 (platformio.ini 建議)

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
board_build.partitions = huge_app.csv  ; 擴充 APP 儲存空間
lib_deps =
    krnza/ESP32-ANCS-Notifications
    olikraus/U8g2
```

## 3. 開發流程與階段規劃 (Development Workflow)

本專案將採用迭代式開發，分為四個主要階段：

### 階段一：BLE 通訊與 ANCS 授權測試

* **目標**：確保 ESP32 能與 iPhone 成功配對，並取得讀取系統通知的權限。
* **工作項目**：
  1. 建立 PlatformIO 專案並寫入基礎 BLE Server 程式碼。
  2. 與 iPhone 進行藍牙配對，並在手機端同意「分享系統通知」。
  3. 利用 Serial Monitor 驗證是否能成功印出通知的原始字串。

### 階段二：雙地圖通知字串解析與邏輯處理 (核心)

* **目標**：將 Apple Maps 與 Google Maps 生硬的通知原始碼轉化為結構化的導航資料。
* **工作項目**：
  1. **設定攔截目標**：過濾 App Identifier，僅放行 `com.apple.Maps` 與 `com.google.Maps`。
  2. **格式差異分析與解析 (String Parsing)**：
     * **Apple Maps**：分析其 `Title` 與 `Message` 的字串結構，通常動作（如右轉）與距離/路名會分開顯示。
     * **Google Maps**：分析其字串結構，擷取規則會與 Apple Maps 不同（可能距離在 Title，動作在 Message）。
  3. **資料正規化**：透過 C++ `String` 函式 (如 `indexOf`, `substring`) 萃取出三個共用變數，供下一個階段使用：
     * **Action (動作)**：解析關鍵字（如「左轉」、「右轉」、「直行」、「微右轉」等），對應到特定的圖示 ID。
     * **Distance (距離)**：擷取數字與單位（如「300 公尺」、「2 公里」）。
     * **Street (路名)**：擷取目標道路名稱（如「中山路」）。
  4. **建立狀態機**：過濾掉非導航相關的雜訊通知（例如「GPS 訊號遺失」或廣告推播）。

### 階段三：OLED 硬體整合與 UI 繪製

* **目標**：將解析後的導航資料視覺化。
* **工作項目**：
  1. 完成 ESP32 與 OLED 螢幕的 I2C 硬體接線 (SDA, SCL, VCC, GND)。
  2. 匯入 U8g2 函式庫，設計 UI 版面配置 (Layout)。
  3. 製作並匯入方向指示的 Bitmap 陣列圖示 (直行箭頭、左右轉箭頭等)。
  4. 根據階段二產出的 `Action` 變數切換圖示，並動態顯示 `Distance`。

### 階段四：系統優化與外殼封裝 (進階)

* **目標**：提升系統穩定性與實用性。
* **工作項目**：
  1. **中文字型支援**：處理 U8g2 顯示繁體中文路名時的字庫容量問題 (初期開發建議先專注於「方向圖示」與「距離數字」)。
  2. **斷線重連機制**：當 iPhone 遠離 ESP32 後再次靠近時，實作 BLE 自動重連邏輯。
  3. **外殼設計**：設計防水或防震的實體外殼以利安裝於載具上。

## 4. 潛在風險與注意事項 (Risks & Mitigations)

1. **App 格式更新風險**：Apple Maps 或 Google Maps 若在未來的 App 更新中更改了通知字串的排列格式，可能會導致階段二的解析邏輯失效。需盡量使用「關鍵字搜尋 (`indexOf`)」而非「絕對位置切割」來增加解析的容錯率。
2. **中文字庫體積**：完整繁體中文字庫極大，開發時需謹慎評估 Flash 空間，可能需要精簡字庫工具 (Font subsetting) 來只保留導航常用的中文字。

## 5. 開發進度

詳細的階段狀態、工作清單、驗收條件與驗證紀錄請參閱 [DEVELOPMENT_PROGRESS.md](DEVELOPMENT_PROGRESS.md)。