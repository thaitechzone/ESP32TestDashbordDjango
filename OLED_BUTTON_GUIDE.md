# ESP32 OLED Display & Button Control Guide

คู่มือการเพิ่มจอ OLED และปุ่มควบคุม Relay สำหรับโปรเจกต์ ESP32 IoT Dashboard

---

## 🔧 อุปกรณ์ที่ต้องใช้

### Hardware ที่เพิ่มเติม:
- **OLED Display 128x64** (I2C, SSD1306)
- **Push Button Switch x 3** (สำหรับ SW1, SW2, SW3)
- **Resistor 10kΩ x 3** (Pull-down resistor - ถ้าไม่ใช้ Internal Pull-up)
- **สาย Jumper Wire**
- **Breadboard**

---

## 🔌 การเชื่อมต่อ Hardware

### 1. OLED Display (I2C - SSD1306)
```
OLED Display -> ESP32
============================
VCC  -> 3.3V
GND  -> GND
SCL  -> GPIO 22 (I2C Clock)
SDA  -> GPIO 21 (I2C Data)
```

### 2. Push Buttons (SW1, SW2, SW3)
```
Button -> ESP32
============================
SW1  -> GPIO 34 (with External Pull-up 10kΩ)
SW2  -> GPIO 35 (with External Pull-up 10kΩ)
SW3  -> GPIO 32 (ใช้ Internal Pull-up)

การต่อปุ่ม SW1, SW2 (GPIO 34, 35):
        3.3V
         │
        10kΩ (External Pull-up)
         │
         ├──────> GPIO 34 (or 35)
         │
      [Button]
         │
        GND

การต่อปุ่ม SW3 (GPIO 32):
┌─────────┐
│  SW3    │
│  ┌───┐  │
│  │   │  │
└──┴───┴──┘
   │   │
   │   └──── GPIO 32
   └──────── GND
```

**หมายเหตุ:** 
- **GPIO 34 และ 35** ต้องมี **External Pull-up resistor 10kΩ** เชื่อมต่อไปยัง 3.3V
- ปุ่มทั้งหมดทำงานแบบ **Active Low** (กดปุ่ม = LOW, ปล่อย = HIGH)
- **GPIO 32** ใช้ Internal Pull-up ได้ปกติ
```
Button -> ESP32
============================
SW1  -> GPIO 34 (Input Only - มี Pull-down ภายใน)
SW2  -> GPIO 35 (Input Only - มี Pull-down ภายใน)
SW3  -> GPIO 32 (ใช้ Internal Pull-up)

การต่อปุ่ม:
┌─────────┐
│  SW1    │
│  ┌───┐  │
│  │   │  │
└──┴───┴──┘
   │   │
   │   └──── GPIO 34
   └──────── 3.3V (สำหรับ GPIO 34, 35)

┌─────────┐
│  SW3    │
│  ┌───┐  │
│  │   │  │
└──┴───┴──┘
   │   │
   │   └──── GPIO 32
   └──────── GND (สำหรับ GPIO 32)
```

**หมายเหตุสำคัญ:** 
- **GPIO 34 และ 35** เป็น **Input Only** และไม่มี Internal Pull-up/Pull-down ที่แข็งแรง
- แนะนำต่อ **External Pull-down resistor 10kΩ** สำหรับ GPIO 34, 35
- หรือต่อปุ่มแบบ **Active High** (ปกติ LOW, กดเป็น HIGH)
- **GPIO 32** สามารถใช้ Internal Pull-up ได้ปกติ

---

## 📐 Pinout Diagram แบบเต็ม

```
                    ESP32
         ┌──────────────────────┐
         │                      │
   3.3V  ├──┐                   │
         │  │  [OLED Display]   │
         │  │   VCC  SCL  SDA   │
         │  │    │    │    │    │
         │  └────┼────┼────┼────┤
         │       │    │    │    │
         │       │    │    └────┤ GPIO 21 (SDA)
         │       │    └─────────┤ GPIO 22 (SCL)
    GND  ├───────┴──────────────┤ GND
         │                      │
         │   [Push Buttons]     │
         │    SW1  SW2  SW3     │
         │     │    │    │      │
         │     ├────┼────┼──────┤ GPIO 25 (SW1)
         │     │    ├────┼──────┤ GPIO 26 (SW2)
         │     │    │    └──────┤ GPIO 27 (SW3)
         │     │    │           │
    GND  ├─────┴────┴───────────┤ GND
         │                      │
         │   [DHT22 Sensor]     │
         │     DATA             │
         │      │               │
         │      └───────────────┤ GPIO 15
         │                      │
         │   [Relay Module]     │
         │   IN1  IN2  IN3      │
         │    │    │    │       │
         │    ├────┼────┼───────┤ GPIO 17 (Relay 1)
         │    │    ├────┼───────┤ GPIO 16 (Relay 2)
         │    │    │    └───────┤ GPIO 4  (Relay 3)
         │                      │
         └──────────────────────┘
```

---

## 📌 Pin Configuration สรุป

| GPIO Pin | อุปกรณ์ | หมายเหตุ |
|----------|---------|----------|
| **GPIO 2** | Built-in LED | Active High |
| **GPIO 4** | Relay 3 | Active Low |
| **GPIO 15** | DHT22 Data | Temperature & Humidity |
| **GPIO 16** | Relay 2 | Active Low |
| **GPIO 17** | Relay 1 | Active Low |
| **GPIO 21** | OLED SDA | I2C Data |
| **GPIO 22** | OLED SCL | I2C Clock |
| **GPIO 32** | Button SW3 | Control Relay 3 (Internal Pull-up) |
| **GPIO 34** | Button SW1 | Control Relay 1 (External Pull-up 10kΩ) |
| **GPIO 35** | Button SW2 | Control Relay 2 (External Pull-up 10kΩ) |

---

## 📊 OLED Display Layout

```
┌──────────────────────────────┐
│ ESP32 IoT Control            │ <- Header
├──────────────────────────────┤
│ WiFi: OK -67dBm              │ <- WiFi Status
│ MQTT: Connected              │ <- MQTT Status
├──────────────────────────────┤
│ R1:ON  R2:OFF R3:ON          │ <- Relay Status
│ T:25.3 H:65.2%               │ <- Sensor Data
│ LED: OFF                     │ <- LED Status
└──────────────────────────────┘
```

### ข้อมูลที่แสดงบน OLED:
1. **Header**: ชื่อโปรเจกต์
2. **WiFi Status**: สถานะการเชื่อมต่อ WiFi และความแรงสัญญาณ (RSSI)
3. **MQTT Status**: สถานะการเชื่อมต่อ MQTT Broker
4. **Relay Status**: สถานะของ Relay ทั้ง 3 ตัว (ON/OFF)
5. **Sensor Data**: อุณหภูมิและความชื้น
6. **LED Status**: สถานะของ LED

---

## 🎮 การใช้งานปุ่ม

### ฟังก์ชันของแต่ละปุ่ม:

| ปุ่ม | ฟังก์ชัน | GPIO |
|------|----------|------|
| **SW1** | Toggle Relay 1 (ON ⇄ OFF) | GPIO 25 |
| **SW2** | Toggle Relay 2 (ON ⇄ OFF) | GPIO 26 |
| **SW3** | Toggle Relay 3 (ON ⇄ OFF) | GPIO 27 |

### วิธีการทำงาน:
1. กดปุ่ม SW1 -> Relay 1 เปลี่ยนสถานะ (ถ้า OFF จะเป็น ON, ถ้า ON จะเป็น OFF)
2. สถานะใหม่จะแสดงบน OLED ทันที
3. ส่งสถานะไปยัง MQTT Dashboard
4. มี Debounce 50ms เพื่อป้องกันการกดซ้ำ

---

## 💻 การเปลี่ยนแปลงในโค้ด

### 1. เพิ่ม Libraries:
```cpp
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
```

### 2. เพิ่ม Pin Definitions:
```cpp
// Button pins (All Active Low with Pull-up)
#define SW1_PIN 34 // Button 1 for Relay 1 (with External Pull-up 10kΩ)
#define SW2_PIN 35 // Button 2 for Relay 2 (with External Pull-up 10kΩ)
#define SW3_PIN 32 // Button 3 for Relay 3 (Internal Pull-up)

// OLED Display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
```

### ✅ **วงจร Pull-up สำหรับ GPIO 34 และ 35:**
เนื่องจาก GPIO 34 และ 35 เป็น Input Only และไม่มี Internal Pull-up ต้องต่อ External Pull-up Resistor:

```
        3.3V
         │
        10kΩ (Pull-up Resistor)
         │
         ├──────────> GPIO 34 (or 35)
         │
      [Button]
         │
        GND

การทำงาน:
- ปกติ (ไม่กดปุ่ม): GPIO = HIGH (3.3V)
- กดปุ่ม: GPIO = LOW (0V) ← Active Low
```

### 3. เพิ่มฟังก์ชันหลัก:

#### `updateDisplay()`
- อัปเดตข้อมูลบน OLED ทุก 500ms
- แสดงสถานะ WiFi, MQTT, Relay, Sensor, LED

#### `checkButtons()`
- ตรวจสอบปุ่มทั้ง 3 ตัวอย่างต่อเนื่อง
- มี Debounce เพื่อป้องกัน bouncing

#### `toggleRelay(int relayNum)`
- สลับสถานะ Relay (ON ⇄ OFF)
- อัปเดต OLED และ MQTT

---

## 🔧 การทดสอบ

### ทดสอบ OLED Display:
```cpp
void testDisplay() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("HELLO"));
  display.println(F("ESP32!"));
  display.display();
}
```

### ทดสอบปุ่ม:
```cpp
void loop() {
  if (digitalRead(SW1_PIN) == LOW) {
    Serial.println("SW1 Pressed!");
  }
  if (digitalRead(SW2_PIN) == LOW) {
    Serial.println("SW2 Pressed!");
  }
  if (digitalRead(SW3_PIN) == LOW) {
    Serial.println("SW3 Pressed!");
  }
  delay(100);
}
```

---

## 🐛 การแก้ไขปัญหา

### ❌ **ปัญหา: OLED ไม่แสดงผล**

**สาเหตุ:**
1. I2C Address ไม่ถูกต้อง
2. การต่อสาย SDA/SCL ผิด
3. OLED ได้รับไฟไม่เพียงพอ

**วิธีแก้:**
1. สแกนหา I2C Address:
```cpp
#include <Wire.h>

void setup() {
  Serial.begin(115200);
  Wire.begin();
  
  Serial.println("Scanning I2C devices...");
  for(byte i = 0; i < 128; i++) {
    Wire.beginTransmission(i);
    if(Wire.endTransmission() == 0) {
      Serial.print("Found device at 0x");
      Serial.println(i, HEX);
    }
  }
}

void loop() {}
```

2. เปลี่ยน I2C Address ในโค้ด (0x3C หรือ 0x3D):
```cpp
#define SCREEN_ADDRESS 0x3D  // ลองเปลี่ยนเป็น 0x3D
```

3. ตรวจสอบการต่อสาย:
   - SCL -> GPIO 22
   - SDA -> GPIO 21

### ❌ **ปัญหา: ปุ่มไม่ทำงาน**

**สาเหตุ:**
1. ปุ่มต่อผิด
2. GPIO 34, 35 ต้องใช้ External Pull-down (ไม่มี Internal Pull-up)
3. Pull-up/Pull-down ไม่ทำงาน

**วิธีแก้:**
1. **สำหรับ GPIO 34 และ 35** ต้องต่อ External Pull-down resistor 10kΩ:
```
3.3V ──[Button]──┬── GPIO 34 (or 35)
                 │
               10kΩ
                 │
                GND
```

2. **หรือแก้โค้ดเป็นแบบ Active High:**
```cpp
// ใน setup()
pinMode(SW1_PIN, INPUT);  // ไม่ใช้ INPUT_PULLUP เพราะ GPIO 34/35 ไม่มี
pinMode(SW2_PIN, INPUT);

// ใน checkButtons()
if (sw1Reading == HIGH) { // เปลี่ยนจาก LOW เป็น HIGH
  toggleRelay(1);
  while(digitalRead(SW1_PIN) == HIGH) { delay(10); }
}
```

3. **สำหรับ GPIO 32** ใช้แบบเดิมได้ (Active Low + INPUT_PULLUP):
```cpp
pinMode(SW3_PIN, INPUT_PULLUP);  // ใช้ได้ปกติ
if (sw3Reading == LOW) { ... }   // Active Low
```

### ❌ **ปัญหา: ปุ่มกดครั้งเดียวแต่ทำงานหลายครั้ง (Bouncing)**

**วิธีแก้:**
- เพิ่มเวลา debounce:
```cpp
const unsigned long debounceDelay = 100; // เพิ่มเป็น 100ms
```

---

## 🎨 การปรับแต่ง OLED Display

### เปลี่ยนขนาดตัวอักษร:
```cpp
display.setTextSize(1);  // เล็ก
display.setTextSize(2);  // กลาง
display.setTextSize(3);  // ใหญ่
```

### แสดงกราฟิก:
```cpp
// วาดสี่เหลี่ยม
display.drawRect(x, y, width, height, SSD1306_WHITE);

// วาดวงกลม
display.drawCircle(x, y, radius, SSD1306_WHITE);

// วาดเส้น
display.drawLine(x0, y0, x1, y1, SSD1306_WHITE);
```

### แสดงไอคอน:
```cpp
// กำหนด bitmap icon
const unsigned char wifi_icon [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x42, 0x00,
  // ... bitmap data
};

display.drawBitmap(x, y, wifi_icon, width, height, SSD1306_WHITE);
```

---

## 📱 ตัวอย่างการใช้งานจริง

### Scenario 1: Smart Home Control
- **SW1** -> เปิด/ปิดไฟห้องนอน
- **SW2** -> เปิด/ปิดพัดลม
- **SW3** -> เปิด/ปิดปลั๊กไฟ
- **OLED** -> แสดงสถานะทั้งหมด Real-time

### Scenario 2: Industrial Monitor
- **OLED** -> แสดงสถานะเครื่องจักร
- **Buttons** -> สั่งงานฉุกเฉิน (Emergency Stop)
- **MQTT** -> ส่งข้อมูลไปยัง Server

### Scenario 3: Agricultural System
- **Relay 1** -> ปั๊มน้ำ
- **Relay 2** -> พัดลมระบายอากาศ
- **Relay 3** -> ไฟส่องสว่าง
- **OLED** -> แสดงอุณหภูมิ/ความชื้น
- **Buttons** -> ควบคุมแบบ Manual

---

## 🔋 การประหยัดพลังงาน (Optional)

### ปิด OLED เมื่อไม่ใช้งาน:
```cpp
// ปิดหน้าจอ
display.ssd1306_command(SSD1306_DISPLAYOFF);

// เปิดหน้าจอ
display.ssd1306_command(SSD1306_DISPLAYON);
```

### ลดความสว่าง:
```cpp
// ความสว่าง 0-255
display.ssd1306_command(SSD1306_SETCONTRAST);
display.ssd1306_command(128); // ลดเหลือ 50%
```

---

## 📚 Libraries ที่ใช้

| Library | Version | ใช้สำหรับ |
|---------|---------|-----------|
| `Adafruit SSD1306` | ^2.5.7 | ควบคุม OLED Display |
| `Adafruit GFX Library` | ^1.11.5 | Graphics สำหรับ OLED |

---

## 🎯 สรุป

✅ **OLED Display แสดง:**
- สถานะ WiFi และ MQTT
- สถานะ Relay ทั้ง 3 ตัว
- ข้อมูล Temperature & Humidity
- สถานะ LED

✅ **ปุ่มควบคุม:**
- SW1 -> Toggle Relay 1
- SW2 -> Toggle Relay 2
- SW3 -> Toggle Relay 3
- มี Debounce ป้องกัน Bouncing

✅ **ฟีเจอร์:**
- อัปเดตหน้าจอทุก 500ms
- ควบคุมผ่านปุ่มและ MQTT
- แสดงผล Real-time

---

**Created by:** ThaiTechZone  
**Date:** 2025-10-19  
**Version:** 1.0  
**Project:** ESP32TestDashboardDjango
