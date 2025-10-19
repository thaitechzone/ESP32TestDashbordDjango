# ⚠️ คำเตือนสำคัญ: GPIO 34 และ 35 (Input Only Pins)

## 🔴 ข้อจำกัดของ GPIO 34 และ 35

### ESP32 มี Pin พิเศษบางตัวที่เป็น **Input Only** ได้แก่:
- **GPIO 34**
- **GPIO 35**
- **GPIO 36 (VP)**
- **GPIO 39 (VN)**

### ข้อจำกัด:
1. ❌ **ไม่สามารถใช้เป็น Output ได้**
2. ❌ **ไม่มี Internal Pull-up Resistor**
3. ❌ **ไม่มี Internal Pull-down Resistor**
4. ✅ **ใช้เป็น Input ได้เท่านั้น**
5. ✅ **รองรับ ADC (Analog to Digital Converter)**

---

## 🔧 วิธีแก้ไขสำหรับการใช้เป็นปุ่มกด

### วิธีที่ 1: ใช้ External Pull-down Resistor (แนะนำ)

```
        3.3V
         │
      [Button]
         │
         ├──────────> GPIO 34 (or 35)
         │
        10kΩ (Pull-down Resistor)
         │
        GND
```

**โค้ด:**
```cpp
// Setup
pinMode(SW1_PIN, INPUT);  // ไม่ใช้ INPUT_PULLUP
pinMode(SW2_PIN, INPUT);

// Check button (Active HIGH)
if (digitalRead(SW1_PIN) == HIGH) {  // กดปุ่ม = HIGH
  // Button pressed
  toggleRelay(1);
  while(digitalRead(SW1_PIN) == HIGH) { delay(10); }
}
```

### วิธีที่ 2: ใช้ External Pull-up Resistor

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
```

**โค้ด:**
```cpp
// Setup
pinMode(SW1_PIN, INPUT);  // ไม่ใช้ INPUT_PULLUP

// Check button (Active LOW)
if (digitalRead(SW1_PIN) == LOW) {  // กดปุ่ม = LOW
  // Button pressed
  toggleRelay(1);
  while(digitalRead(SW1_PIN) == LOW) { delay(10); }
}
```

### วิธีที่ 3: เปลี่ยนเป็น GPIO อื่นที่มี Internal Pull-up (แนะนำที่สุด)

แทนที่จะใช้ GPIO 34, 35 ให้เปลี่ยนเป็น GPIO ที่มี Internal Pull-up:

**GPIO ที่แนะนำสำหรับปุ่ม:**
- GPIO 12, 13, 14
- GPIO 25, 26, 27
- GPIO 32, 33

**ตัวอย่าง:**
```cpp
// เปลี่ยนจาก GPIO 34, 35 เป็น GPIO 25, 26
#define SW1_PIN 25  // แทน GPIO 34
#define SW2_PIN 26  // แทน GPIO 35
#define SW3_PIN 32  // เหมือนเดิม

// Setup
pinMode(SW1_PIN, INPUT_PULLUP);  // ใช้ได้ปกติ
pinMode(SW2_PIN, INPUT_PULLUP);  // ใช้ได้ปกติ
pinMode(SW3_PIN, INPUT_PULLUP);  // ใช้ได้ปกติ

// Check button (Active LOW)
if (digitalRead(SW1_PIN) == LOW) {  // กดปุ่ม = LOW
  // Button pressed
}
```

---

## 📊 ตารางเปรียบเทียบ GPIO Types

| GPIO Type | Output | Input | Pull-up | Pull-down | ADC | ใช้สำหรับ |
|-----------|--------|-------|---------|-----------|-----|-----------|
| **GPIO 34, 35, 36, 39** | ❌ | ✅ | ❌ | ❌ | ✅ | Sensor Input, ADC |
| **GPIO 0, 2, 4, 5, 12-19, 21-23, 25-27, 32, 33** | ✅ | ✅ | ✅ | ✅ | ✅* | General Purpose I/O |
| **GPIO 1, 3** | ✅ | ✅ | ✅ | ✅ | ❌ | UART (TX/RX) |
| **GPIO 6-11** | ❌ | ❌ | ❌ | ❌ | ❌ | Flash Memory (ห้ามใช้!) |

*บาง GPIO มี ADC บางตัวไม่มี

---

## 🛠️ การแก้ไขโค้ดปัจจุบัน

### ปัญหา: ใช้ INPUT_PULLUP กับ GPIO 34, 35
```cpp
// ❌ ไม่ได้ผล - GPIO 34, 35 ไม่มี Internal Pull-up
pinMode(SW1_PIN, INPUT_PULLUP);  // GPIO 34
pinMode(SW2_PIN, INPUT_PULLUP);  // GPIO 35
```

### แก้ไข Option 1: ใช้ External Pull-down
```cpp
// ✅ ได้ผล - ต่อ Pull-down 10kΩ ภายนอก
pinMode(SW1_PIN, INPUT);  // GPIO 34
pinMode(SW2_PIN, INPUT);  // GPIO 35

// เปลี่ยนการตรวจสอบเป็น Active HIGH
void checkButtons() {
  bool sw1Reading = digitalRead(SW1_PIN);
  if (sw1Reading == HIGH) {  // เปลี่ยนจาก LOW เป็น HIGH
    toggleRelay(1);
    while(digitalRead(SW1_PIN) == HIGH) { delay(10); }
  }
  
  bool sw2Reading = digitalRead(SW2_PIN);
  if (sw2Reading == HIGH) {  // เปลี่ยนจาก LOW เป็น HIGH
    toggleRelay(2);
    while(digitalRead(SW2_PIN) == HIGH) { delay(10); }
  }
  
  // GPIO 32 ใช้แบบเดิมได้
  bool sw3Reading = digitalRead(SW3_PIN);
  if (sw3Reading == LOW) {  // Active LOW
    toggleRelay(3);
    while(digitalRead(SW3_PIN) == LOW) { delay(10); }
  }
}
```

### แก้ไข Option 2: เปลี่ยน GPIO (แนะนำ)
```cpp
// ✅ ง่ายที่สุด - เปลี่ยน GPIO
#define SW1_PIN 25  // เปลี่ยนจาก GPIO 34
#define SW2_PIN 26  // เปลี่ยนจาก GPIO 35
#define SW3_PIN 32  // เหมือนเดิม

// ใช้โค้ดเดิมได้เลย
pinMode(SW1_PIN, INPUT_PULLUP);
pinMode(SW2_PIN, INPUT_PULLUP);
pinMode(SW3_PIN, INPUT_PULLUP);

// ตรวจสอบแบบ Active LOW
if (digitalRead(SW1_PIN) == LOW) { ... }
```

---

## 🎯 คำแนะนำ

### สำหรับโปรเจกต์นี้:
1. **แนะนำ:** เปลี่ยน GPIO 34, 35 เป็น GPIO 25, 26
2. **ทางเลือก:** ต่อ External Pull-down resistor 10kΩ
3. **ไม่แนะนำ:** ใช้ GPIO 34, 35 โดยไม่ต่อ External Resistor

### GPIO ที่ปลอดภัยสำหรับปุ่ม:
✅ GPIO 12, 13, 14, 15  
✅ GPIO 25, 26, 27  
✅ GPIO 32, 33  

### GPIO ที่ควรหลีกเลี่ยง:
❌ GPIO 34, 35, 36, 39 (Input Only - ต้องต่อ External Resistor)  
❌ GPIO 6-11 (Flash - ห้ามใช้!)  
⚠️ GPIO 0, 2, 12, 15 (Boot Pins - ใช้ได้แต่ระวัง)  

---

## 🔬 การทดสอบ GPIO 34, 35

### ทดสอบว่าปุ่มทำงานหรือไม่:
```cpp
void loop() {
  int sw1 = digitalRead(34);
  int sw2 = digitalRead(35);
  int sw3 = digitalRead(32);
  
  Serial.print("SW1(34): "); Serial.print(sw1);
  Serial.print(" | SW2(35): "); Serial.print(sw2);
  Serial.print(" | SW3(32): "); Serial.println(sw3);
  
  delay(500);
}
```

**ผลลัพธ์ที่คาดหวัง (ไม่มี External Resistor):**
- SW1, SW2: ค่าลอย (Floating) - แสดงค่าสุ่ม 0/1
- SW3: แสดงค่าคงที่ 1 (เพราะมี Internal Pull-up)

**ผลลัพธ์ที่คาดหวัง (มี External Pull-down):**
- ไม่กดปุ่ม: SW1=0, SW2=0, SW3=1
- กดปุ่ม: SW1=1, SW2=1, SW3=0

---

## 📚 สรุป

| วิธี | ข้อดี | ข้อเสีย | คะแนน |
|------|-------|---------|-------|
| **เปลี่ยน GPIO** | ง่าย, ไม่ต้องต่อภายนอก | ต้องเปลี่ยนสาย | ⭐⭐⭐⭐⭐ |
| **External Pull-down** | ใช้ GPIO เดิมได้ | ต้องต่อ Resistor | ⭐⭐⭐⭐ |
| **External Pull-up** | ใช้ GPIO เดิมได้ | ต้องต่อ Resistor | ⭐⭐⭐⭐ |
| **ไม่ต่อ Resistor** | ไม่ต้องทำอะไร | ❌ ไม่ทำงาน | ❌ |

**คำแนะนำสุดท้าย:** เปลี่ยน GPIO 34, 35 เป็น GPIO 25, 26 จะง่ายและเสถียรที่สุด! 🎯

---

**Created by:** ThaiTechZone  
**Date:** 2025-10-19  
**Version:** 1.0  
**Project:** ESP32TestDashboardDjango
