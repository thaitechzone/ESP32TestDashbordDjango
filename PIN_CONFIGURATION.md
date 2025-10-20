# ESP32 Pin Configuration Summary

## 📌 การกำหนด Pin ทั้งหมดในโปรเจกต์

### ตารางสรุป Pin Configuration

| GPIO Pin | อุปกรณ์ | หมายเหตุ |
|----------|---------|----------|
| **GPIO 2** | Built-in LED | Active High (HIGH = ON) |
| **GPIO 4** | Relay 3 | Relay Control Channel 3 (Active Low) |
| **GPIO 15** | DHT22 Sensor | Temperature & Humidity Sensor (Data Pin) |
| **GPIO 16** | Relay 2 | Relay Control Channel 2 (Active Low) |
| **GPIO 17** | Relay 1 | Relay Control Channel 1 (Active Low) |
| **GPIO 21** | OLED SDA | I2C Data Line for OLED Display |
| **GPIO 22** | OLED SCL | I2C Clock Line for OLED Display |
| **GPIO 32** | Button SW3 | Control Relay 3 (Internal Pull-up) |
| **GPIO 34** | Button SW1 | Control Relay 1 (External Pull-up 10kΩ) |
| **GPIO 35** | Button SW2 | Control Relay 2 (External Pull-up 10kΩ) |

---

## 🔌 การต่อสายทั้งหมด

### 1. Built-in LED (GPIO 2)
```
GPIO 2 -> Built-in LED (บนบอร์ด ESP32)
ไม่ต้องต่อสายเพิ่มเติม
```

### 2. DHT22 Temperature & Humidity Sensor (GPIO 15)
```
DHT22          ESP32
━━━━━━━━━━━━━━━━━━━━━━━━━
Pin 1 (VCC)  -> 3.3V
Pin 2 (DATA) -> GPIO 15 (+ Pull-up resistor 10kΩ ไป 3.3V)
Pin 3        -> ไม่ใช้
Pin 4 (GND)  -> GND
```

### 3. Relay Module 3 Channels
```
Relay Module   ESP32
━━━━━━━━━━━━━━━━━━━━━━━━━
VCC           -> 5V (หรือ 3.3V ตาม Relay Module)
GND           -> GND
IN1           -> GPIO 17 (Relay 1)
IN2           -> GPIO 16 (Relay 2)
IN3           -> GPIO 4  (Relay 3)
```

---

## 📊 Pinout Diagram แบบเต็ม

```
                    ESP32
         ┌──────────────────────┐
         │                      │
   3.3V  ├──────────────┐       │
         │              │       │
         │         [DHT22]      │
         │          VCC  DATA   │
         │           │    │     │
         │           └────┼─────┤ GPIO 15
         │                │     │
         │           10kΩ │     │
         │                │     │
    GND  ├────────────────┴─────┤ GND
         │                      │
         │    [Built-in LED]    │
         │           │          │
         │           └──────────┤ GPIO 2
         │                      │
    5V   ├──────┐               │
         │      │               │
         │  [Relay Module]      │
         │   VCC  IN1  IN2 IN3  │
         │    │    │    │   │   │
         │    │    │    │   └───┤ GPIO 4  (Relay 3)
         │    │    │    └───────┤ GPIO 16 (Relay 2)
         │    │    └────────────┤ GPIO 17 (Relay 1)
         │    │                 │
    GND  ├────┴─────────────────┤ GND
         │                      │
         └──────────────────────┘
```

---

## 🔧 โค้ดใน main.cpp

```cpp
// ===== Pin Definitions =====
#define LED_PIN 2      // The onboard LED is on GPIO2 (Active High)
#define DHT_PIN 15     // DHT sensor pin (GPIO15)
#define DHT_TYPE DHT22 // DHT22 (AM2302)
#define RELAY1_PIN 17  // Relay 1 pin (GPIO17)
#define RELAY2_PIN 16  // Relay 2 pin (GPIO16)
#define RELAY3_PIN 4   // Relay 3 pin (GPIO4)
```

---

## ⚠️ ข้อควรระวัง

### GPIO Pins ที่ไม่ควรใช้:
- **GPIO 0**: ใช้สำหรับ Boot Mode (กดค้างเพื่อเข้า Flash Mode)
- **GPIO 1 (TX)**: ใช้สำหรับ Serial Communication
- **GPIO 3 (RX)**: ใช้สำหรับ Serial Communication
- **GPIO 6-11**: เชื่อมต่อกับ Flash Memory (ห้ามใช้!)
- **GPIO 12**: อาจทำให้ Boot ล้มเหลวถ้ามีแรงดันสูงขณะ Boot

### GPIO Pins ที่แนะนำ:
- **GPIO 13, 14, 15, 16, 17**: ✅ เหมาะสำหรับ Input/Output
- **GPIO 4, 5**: ✅ เหมาะสำหรับ I2C หรือ Digital I/O
- **GPIO 2**: ✅ Built-in LED (มี LED ติดอยู่แล้ว)
- **GPIO 21, 22**: ✅ เหมาะสำหรับ I2C (SDA, SCL)

---

## 📱 MQTT Topics ที่เกี่ยวข้อง

### LED Control:
- Control: `thaitechzone/v2_board/control/led`
- State: `thaitechzone/v2_board/state/led`

### Relay Control:
- Relay 1 Control: `thaitechzone/v2_board/control/relay1`
- Relay 1 State: `thaitechzone/v2_board/state/relay1`
- Relay 2 Control: `thaitechzone/v2_board/control/relay2`
- Relay 2 State: `thaitechzone/v2_board/state/relay2`
- Relay 3 Control: `thaitechzone/v2_board/control/relay3`
- Relay 3 State: `thaitechzone/v2_board/state/relay3`

### Sensor Data:
- Temperature: `thaitechzone/v2_board/sensor/temperature`
- Humidity: `thaitechzone/v2_board/sensor/humidity`
- JSON Data: `thaitechzone/v2_board/sensor/data`

---

## 🎯 สรุปฟังก์ชันการทำงาน

### 1. LED Control (GPIO 2)
- รับคำสั่ง ON/OFF จาก Dashboard
- ส่งสถานะกลับไปยัง Dashboard

### 2. DHT22 Sensor (GPIO 15)
- อ่านค่าอุณหภูมิและความชื้นทุก 5 วินาที
- ส่งข้อมูลในรูปแบบ JSON
- รูปแบบ: `{"temperature": 25.5, "humidity": 71.3, "device_name": "ESP_01"}`

### 3. Relay Control (GPIO 17, 16, 4)
- ควบคุม Relay 3 ตัวผ่าน MQTT
- แต่ละ Relay สามารถควบคุมแยกอิสระ
- ส่งสถานะกลับเมื่อมีการเปลี่ยนแปลง
- **Active Low:** LOW = ON, HIGH = OFF

---

## 🔍 การทดสอบ Pin

### ทดสอบ LED:
```cpp
digitalWrite(LED_PIN, HIGH); // เปิด LED
delay(1000);
digitalWrite(LED_PIN, LOW);  // ปิด LED
```

### ทดสอบ DHT22:
```cpp
float temp = dht.readTemperature();
float hum = dht.readHumidity();
Serial.print("Temp: "); Serial.println(temp);
Serial.print("Humidity: "); Serial.println(hum);
```

### ทดสอบ Relay (Active Low):
```cpp
digitalWrite(RELAY1_PIN, LOW);  // เปิด Relay 1 (LOW = ON)
delay(1000);
digitalWrite(RELAY1_PIN, HIGH); // ปิด Relay 1 (HIGH = OFF)
```

---

**Created by:** ThaiTechZone  
**Date:** 2025-10-19  
**Version:** 1.0  
**Project:** ESP32TestDashboardDjango
