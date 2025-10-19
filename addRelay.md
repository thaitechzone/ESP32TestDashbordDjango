# ESP32 Relay Control สำหรับ Django Dashboard

เอกสารนี้อธิบายการเพิ่มฟังก์ชันควบคุม Relay 3 ตัว (Relay1, Relay2, Relay3) เพื่อควบคุมอุปกรณ์ไฟฟ้าผ่าน MQTT Dashboard

## 🔧 อุปกรณ์ที่ต้องใช้

### Hardware:
- **ESP32 Development Board**
- **Relay Module 3 Channel** (หรือ 1 Channel x 3 ตัว)
- **สาย Jumper Wire**
- **DHT22 Sensor** (ถ้ามีอยู่แล้ว)

### Relay Module:
- แนะนำใช้ **Active Low Relay Module** (ส่วนใหญ่เป็นแบบนี้)
- หรือ **Active High Relay Module** (ต้องปรับโค้ดเล็กน้อย)

---

## 🔌 การเชื่อมต่อ Hardware

```
Relay Module -> ESP32
============================
VCC         -> 5V (หรือ 3.3V ขึ้นอยู่กับ Relay Module)
GND         -> GND
IN1         -> GPIO 17 (Relay 1)
IN2         -> GPIO 16 (Relay 2)
IN3         -> GPIO 4  (Relay 3)
```

### 📐 Pinout Diagram:
```
ESP32 GPIO Pins:
- GPIO 17 -> Relay 1 (IN1)
- GPIO 16 -> Relay 2 (IN2)
- GPIO 4  -> Relay 3 (IN3)
- GPIO 2  -> Built-in LED
- GPIO 15 -> DHT22 Sensor

Relay Module:
┌─────────────────────┐
│   3-Channel Relay   │
│                     │
│  VCC  GND  IN1-IN3  │
│   │    │    │       │
│   5V  GND  GPIO     │
│              17,16,4│
└─────────────────────┘
      ESP32
```

---

## 📊 MQTT Topics สำหรับ Relay

| Topic | Direction | ความหมาย | Format |
|-------|-----------|----------|---------|
| `thaitechzone/v2_board/control/relay1` | Dashboard → ESP32 | คำสั่งควบคุม Relay 1 | `ON` / `OFF` |
| `thaitechzone/v2_board/control/relay2` | Dashboard → ESP32 | คำสั่งควบคุม Relay 2 | `ON` / `OFF` |
| `thaitechzone/v2_board/control/relay3` | Dashboard → ESP32 | คำสั่งควบคุม Relay 3 | `ON` / `OFF` |
| `thaitechzone/v2_board/state/relay1` | ESP32 → Dashboard | สถานะ Relay 1 | `ON` / `OFF` |
| `thaitechzone/v2_board/state/relay2` | ESP32 → Dashboard | สถานะ Relay 2 | `ON` / `OFF` |
| `thaitechzone/v2_board/state/relay3` | ESP32 → Dashboard | สถานะ Relay 3 | `ON` / `OFF` |

---

## 💻 การเปลี่ยนแปลงในโค้ด

### 1. เพิ่ม Pin Definitions:
```cpp
#define LED_PIN 2      // The onboard LED is on GPIO2 (Active High)
#define DHT_PIN 15     // DHT sensor pin (GPIO15)
#define DHT_TYPE DHT22 // DHT22 (AM2302)
#define RELAY1_PIN 17  // Relay 1 pin (GPIO17)
#define RELAY2_PIN 16  // Relay 2 pin (GPIO16)
#define RELAY3_PIN 4   // Relay 3 pin (GPIO4)
```

### 2. เพิ่ม MQTT Topics:
```cpp
// Relay control topics
const char* RELAY1_CONTROL_TOPIC = "thaitechzone/v2_board/control/relay1";
const char* RELAY2_CONTROL_TOPIC = "thaitechzone/v2_board/control/relay2";
const char* RELAY3_CONTROL_TOPIC = "thaitechzone/v2_board/control/relay3";
// Relay state topics
const char* RELAY1_STATE_TOPIC = "thaitechzone/v2_board/state/relay1";
const char* RELAY2_STATE_TOPIC = "thaitechzone/v2_board/state/relay2";
const char* RELAY3_STATE_TOPIC = "thaitechzone/v2_board/state/relay3";
```

### 3. เพิ่มฟังก์ชัน publishRelayState:
```cpp
void publishRelayState(int relayNum) {
  bool relayState;
  const char* stateTopic;
  
  switch(relayNum) {
    case 1:
      relayState = digitalRead(RELAY1_PIN);
      stateTopic = RELAY1_STATE_TOPIC;
      break;
    case 2:
      relayState = digitalRead(RELAY2_PIN);
      stateTopic = RELAY2_STATE_TOPIC;
      break;
    case 3:
      relayState = digitalRead(RELAY3_PIN);
      stateTopic = RELAY3_STATE_TOPIC;
      break;
    default:
      return;
  }
  
  String stateMessage = relayState ? "ON" : "OFF";
  mqttClient.publish(stateTopic, stateMessage.c_str(), true);
}
```

### 4. อัปเดต callback function เพื่อรองรับ Relay:
```cpp
void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  message.toUpperCase();
  
  // LED Control
  if (String(topic) == LED_CONTROL_TOPIC) {
    // ... existing LED code ...
  }
  // Relay 1 Control
  else if (String(topic) == RELAY1_CONTROL_TOPIC) {
    if (message == "ON") {
      digitalWrite(RELAY1_PIN, HIGH);
    } else if (message == "OFF") {
      digitalWrite(RELAY1_PIN, LOW);
    }
    publishRelayState(1);
  }
  // Relay 2 Control
  else if (String(topic) == RELAY2_CONTROL_TOPIC) {
    if (message == "ON") {
      digitalWrite(RELAY2_PIN, HIGH);
    } else if (message == "OFF") {
      digitalWrite(RELAY2_PIN, LOW);
    }
    publishRelayState(2);
  }
  // Relay 3 Control
  else if (String(topic) == RELAY3_CONTROL_TOPIC) {
    if (message == "ON") {
      digitalWrite(RELAY3_PIN, HIGH);
    } else if (message == "OFF") {
      digitalWrite(RELAY3_PIN, LOW);
    }
    publishRelayState(3);
  }
}
```

---

## 🎯 วิธีการใช้งาน

### 1. Upload โค้ดลง ESP32:
```bash
platformio run --target upload
```

### 2. เปิด Serial Monitor:
```bash
platformio device monitor
```

### 3. ทดสอบผ่าน MQTT:

#### ใช้ MQTT Explorer หรือ mosquitto_pub:
```bash
# เปิด Relay 1
mosquitto_pub -h broker.hivemq.com -t "thaitechzone/v2_board/control/relay1" -m "ON"

# ปิด Relay 1
mosquitto_pub -h broker.hivemq.com -t "thaitechzone/v2_board/control/relay1" -m "OFF"

# เปิด Relay 2
mosquitto_pub -h broker.hivemq.com -t "thaitechzone/v2_board/control/relay2" -m "ON"

# เปิด Relay 3
mosquitto_pub -h broker.hivemq.com -t "thaitechzone/v2_board/control/relay3" -m "ON"
```

### 4. ตรวจสอบสถานะ Relay:
```bash
# Subscribe เพื่อดูสถานะ Relay ทั้งหมด
mosquitto_sub -h broker.hivemq.com -t "thaitechzone/v2_board/state/#" -v
```

---

## 🔧 การแก้ไขปัญหา

### ✅ **Relay Module นี้ใช้ Active Low แล้ว**

โค้ดได้ปรับให้ทำงานกับ Active Low Relay Module แล้ว:
```cpp
// สำหรับ Active Low Relay Module (ถูกต้องแล้ว)
if (message == "ON") {
  digitalWrite(RELAY1_PIN, LOW);  // LOW = ON ✅
} else if (message == "OFF") {
  digitalWrite(RELAY1_PIN, HIGH); // HIGH = OFF ✅
}
```

### ❌ **ปัญหา: Relay ไม่ทำงาน**

**ตรวจสอบ:**
1. ✅ Relay Module ได้รับไฟเลี้ยงหรือยัง (5V หรือ 3.3V)
2. ✅ การต่อสาย GPIO ถูกต้องหรือไม่
3. ✅ Relay Module รองรับแรงดันไฟจาก ESP32 (3.3V) หรือไม่

**วิธีแก้:**
- ถ้า Relay ต้องการ 5V signal อาจต้องใช้ Level Shifter
- หรือเปลี่ยนเป็น Relay Module ที่รองรับ 3.3V

### ❌ **ปัญหา: ESP32 Restart เมื่อ Relay ทำงาน**

**สาเหตุ:** Relay ดูดกระแสมากเกินไป

**วิธีแก้:**
1. ใช้ External Power Supply สำหรับ Relay Module
2. เชื่อมต่อ GND ของ ESP32 และ Power Supply เข้าด้วยกัน (Common Ground)

```
Power Supply 5V -> Relay VCC
ESP32 GND <-----> Power Supply GND (Common Ground)
ESP32 GPIO -> Relay IN1-3
```

---

## 🎊 สรุปฟีเจอร์ที่เพิ่มเข้ามา

✅ **ควบคุม Relay 3 ตัว** ผ่าน MQTT  
✅ **รับสถานะ Real-time** จาก ESP32  
✅ **Subscribe และ Publish** อัตโนมัติเมื่อเชื่อมต่อ  
✅ **ส่งสถานะเริ่มต้น** เมื่อ ESP32 เริ่มทำงาน  
✅ **Retain Flag** เพื่อเก็บสถานะล่าสุด  

---

## 📱 ตัวอย่างการใช้งานจริง

### **ตัวอย่างที่ 1: ควบคุมหลอดไฟ 3 ดวง**
- Relay 1 -> หลอดไฟห้องนอน
- Relay 2 -> หลอดไฟห้องนั่งเล่น
- Relay 3 -> หลอดไฟห้องครัว

---

## ⚠️ **หมายเหตุสำคัญ: Active Low Relay**

Relay Module นี้ใช้แบบ **Active Low** ซึ่งหมายความว่า:
- ส่ง **LOW (0V)** -> Relay เปิด (ON) ✅
- ส่ง **HIGH (3.3V)** -> Relay ปิด (OFF) ✅

โค้ดได้ปรับให้ทำงานถูกต้องแล้ว:
```cpp
// เปิด Relay (Active Low)
digitalWrite(RELAY1_PIN, LOW);  // LOW = ON

// ปิด Relay (Active Low)  
digitalWrite(RELAY1_PIN, HIGH); // HIGH = OFF
```

---

## ⚠️ คำเตือนด้านความปลอดภัย

1. **อย่าต่อโหลดที่มีกระแสสูงเกินไป** ตรวจสอบ Rating ของ Relay Module
2. **ระวังไฟฟ้าแรงสูง (220V AC)** ใช้มาตรฐานความปลอดภัย
3. **ควรใช้ Fuse หรือ Circuit Breaker** ป้องกันไฟฟ้าลัดวงจร
4. **ติดตั้งในกล่องที่ปลอดภัย** ห้ามให้เด็กหรือสัตว์เลี้ยงเข้าถึง
5. **ทดสอบให้ดีก่อนใช้งานจริง** เริ่มจากโหลดเล็กๆ ก่อน

---

## 🚀 ขั้นตอนถัดไป

- [ ] เพิ่มฟีเจอร์ Timer สำหรับเปิด/ปิดอัตโนมัติ
- [ ] เพิ่มการควบคุมแบบ Schedule
- [ ] เพิ่ม Sensor เพื่อเปิด/ปิดอัตโนมัติตามสภาพแวดล้อม
- [ ] สร้าง Mobile App สำหรับควบคุม
- [ ] เพิ่มระบบแจ้งเตือนเมื่อมีการเปลี่ยนสถานะ

---

**Created by:** ThaiTechZone  
**Date:** 2025-10-19  
**Version:** 1.0  
**GitHub:** [DjangoDashboardFramework](https://github.com/thaitechzone/DjangoDashboardFramework)
