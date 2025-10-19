# 🔘 วงจรปุ่มกด (Push Button Circuit) สำหรับ GPIO 34, 35, 32

## ✅ การต่อวงจรที่ถูกต้อง

### วงจร SW1 และ SW2 (GPIO 34, 35) - ต้องใช้ External Pull-up

```
                    ESP32
                     │
        3.3V ────────┤
         │           │
        10kΩ         │
         │           │
         ├───────────┤ GPIO 34 (SW1)
         │           │
      ┌──┴──┐        │
      │ SW1 │        │
      └──┬──┘        │
         │           │
        GND ─────────┤ GND
                     │


        3.3V ────────┤
         │           │
        10kΩ         │
         │           │
         ├───────────┤ GPIO 35 (SW2)
         │           │
      ┌──┴──┐        │
      │ SW2 │        │
      └──┬──┘        │
         │           │
        GND ─────────┤ GND
```

### วงจร SW3 (GPIO 32) - ใช้ Internal Pull-up

```
                    ESP32
                     │
                     │ (Internal Pull-up)
                     │
         ┌───────────┤ GPIO 32 (SW3)
         │           │
      ┌──┴──┐        │
      │ SW3 │        │
      └──┬──┘        │
         │           │
        GND ─────────┤ GND
```

---

## 🔧 รายละเอียดอุปกรณ์

### สำหรับ SW1 และ SW2:
- **Resistor:** 10kΩ (สีน้ำตาล-ดำ-ส้ม-ทอง)
- **Push Button:** Tactile Switch 6x6mm (4 ขา)
- **การต่อ:**
  1. 3.3V → Resistor 10kΩ → GPIO Pin
  2. GPIO Pin → Push Button → GND

### สำหรับ SW3:
- **Push Button:** Tactile Switch 6x6mm (4 ขา)
- **การต่อ:**
  1. GPIO 32 → Push Button → GND
  2. ใช้ `INPUT_PULLUP` ในโค้ด

---

## 📊 ตารางสรุปการต่อ

| ปุ่ม | GPIO | Pull-up Type | Resistor | การต่อปุ่ม |
|------|------|--------------|----------|------------|
| **SW1** | 34 | External 10kΩ | ✅ ต้องมี | GPIO → Button → GND |
| **SW2** | 35 | External 10kΩ | ✅ ต้องมี | GPIO → Button → GND |
| **SW3** | 32 | Internal | ❌ ไม่ต้อง | GPIO → Button → GND |

---

## 🔍 วิธีการทำงาน (Active Low)

### สถานะปกติ (ไม่กดปุ่ม):
```
3.3V ──[10kΩ]──┬─> GPIO = HIGH (1)
               │
            [Button] (Open)
               │
              GND
```
**ผลลัพธ์:** `digitalRead(PIN) = HIGH`

### สถานะกดปุ่ม:
```
3.3V ──[10kΩ]──┬─> GPIO = LOW (0)
               │
            [Button] (Closed/Pressed)
               │
              GND ←── กระแสไหลผ่านปุ่มไป GND
```
**ผลลัพธ์:** `digitalRead(PIN) = LOW`

---

## 💻 โค้ดที่ใช้

```cpp
// Pin Definitions
#define SW1_PIN 34  // External Pull-up
#define SW2_PIN 35  // External Pull-up
#define SW3_PIN 32  // Internal Pull-up

void setup() {
  // Setup buttons
  pinMode(SW1_PIN, INPUT_PULLUP);  // ใช้ได้เพราะมี External Pull-up
  pinMode(SW2_PIN, INPUT_PULLUP);  // ใช้ได้เพราะมี External Pull-up
  pinMode(SW3_PIN, INPUT_PULLUP);  // ใช้ Internal Pull-up
}

void loop() {
  // Check buttons (Active Low)
  if (digitalRead(SW1_PIN) == LOW) {
    // SW1 pressed
  }
  
  if (digitalRead(SW2_PIN) == LOW) {
    // SW2 pressed
  }
  
  if (digitalRead(SW3_PIN) == LOW) {
    // SW3 pressed
  }
}
```

---

## 🛠️ วัสดุที่ต้องใช้

### อุปกรณ์:
- [ ] ESP32 Development Board x 1
- [ ] Push Button (Tactile Switch) x 3
- [ ] Resistor 10kΩ x 2 (สำหรับ GPIO 34, 35)
- [ ] Breadboard x 1
- [ ] Jumper Wires

### ตัวต้านทาน 10kΩ:
```
รหัสสี: น้ำตาล-ดำ-ส้ม-ทอง
┌─────────────────────┐
│  │  │  │  │         │
│ BRN BLK ORG GLD     │
│  1  0  ×1k  ±5%     │
│                     │
│    = 10,000Ω        │
│    = 10kΩ           │
└─────────────────────┘
```

---

## 🔬 การทดสอบวงจร

### ทดสอบว่าปุ่มทำงานถูกต้อง:

```cpp
void loop() {
  Serial.print("SW1(34): ");
  Serial.print(digitalRead(SW1_PIN));
  Serial.print(" | SW2(35): ");
  Serial.print(digitalRead(SW2_PIN));
  Serial.print(" | SW3(32): ");
  Serial.println(digitalRead(SW3_PIN));
  
  delay(500);
}
```

**ผลลัพธ์ที่ถูกต้อง:**
```
ไม่กดปุ่ม: SW1(34): 1 | SW2(35): 1 | SW3(32): 1
กด SW1:    SW1(34): 0 | SW2(35): 1 | SW3(32): 1
กด SW2:    SW1(34): 1 | SW2(35): 0 | SW3(32): 1
กด SW3:    SW1(34): 1 | SW2(35): 1 | SW3(32): 0
```

---

## ⚡ ทำไมต้องใช้ Pull-up Resistor?

### ถ้าไม่มี Pull-up Resistor:
```
GPIO Pin ───> ลอยๆ (Floating) ← ไม่มีอะไรดึงเป็น HIGH หรือ LOW
```
**ปัญหา:**
- ค่าที่อ่านได้จะสุ่ม (0 หรือ 1)
- อาจรับสัญญาณรบกวน (Noise)
- ไม่สามารถอ่านค่าที่แน่นอนได้

### เมื่อมี Pull-up Resistor:
```
3.3V ──[10kΩ]─── GPIO Pin ← ถูกดึงให้เป็น HIGH (1) อยู่เสมอ
```
**ข้อดี:**
- ค่าเริ่มต้นเป็น HIGH (1) อยู่เสมอ
- เมื่อกดปุ่มจะกลายเป็น LOW (0) ชัดเจน
- ป้องกันสัญญาณรบกวน

---

## 📐 การคำนวณค่า Resistor

### ทำไมใช้ 10kΩ?

```
กระแสที่ไหล = V / R
            = 3.3V / 10,000Ω
            = 0.33 mA
            = 0.00033 A
```

**ข้อดี:**
- กระแสต่ำ → ประหยัดพลังงาน
- ค่าต้านทานพอดี → ไม่รบกวนการอ่านค่า
- ป้องกันกระแสสูงเกินไป

**ค่าที่ใช้ได้:**
- ✅ 4.7kΩ - 47kΩ (แนะนำ 10kΩ)
- ⚠️ < 1kΩ (กระแสสูงเกินไป)
- ⚠️ > 100kΩ (รับสัญญาณรบกวนง่าย)

---

## 🎯 สรุป

### ✅ การต่อที่ถูกต้อง:
1. **GPIO 34, 35:** ต้องใช้ External Pull-up Resistor 10kΩ
2. **GPIO 32:** ใช้ Internal Pull-up (ไม่ต้อง Resistor ภายนอก)
3. **ปุ่มทั้งหมด:** ต่อแบบ Active Low (GPIO → Button → GND)

### 🔧 การตั้งค่าในโค้ด:
```cpp
pinMode(SW1_PIN, INPUT_PULLUP);  // ✅ ทำงานเพราะมี External Pull-up
pinMode(SW2_PIN, INPUT_PULLUP);  // ✅ ทำงานเพราะมี External Pull-up
pinMode(SW3_PIN, INPUT_PULLUP);  // ✅ ใช้ Internal Pull-up

// ตรวจสอบแบบ Active Low
if (digitalRead(SW1_PIN) == LOW) { /* กดปุ่ม */ }
```

### 📊 แผนภาพรวม:
```
SW1 (GPIO 34): [3.3V]──[10kΩ]──[GPIO]──[Button]──[GND]
SW2 (GPIO 35): [3.3V]──[10kΩ]──[GPIO]──[Button]──[GND]
SW3 (GPIO 32): [Internal Pull-up]──[GPIO]──[Button]──[GND]
```

---

**Created by:** ThaiTechZone  
**Date:** 2025-10-19  
**Version:** 1.0  
**Project:** ESP32TestDashboardDjango
