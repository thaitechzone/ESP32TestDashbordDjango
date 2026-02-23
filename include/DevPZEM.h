/**
 * @file DevPZEM.h
 * @brief PZEM-016 AC Power Monitor wrapper class for ESP32 using ModbusMaster
 * @note Uses the same Serial port as programming (Serial/UART0)
 * @note Uses MAX13487 for RS232 to RS485 conversion (Auto Direction)
 * 
 * PZEM-016 Specifications:
 * - Voltage: 80-260V AC
 * - Current: 0-100A (with external CT)
 * - Power: 0-23kW
 * - Communication: Modbus RTU (9600 8N1)
 * - Slave Address: 0x01 (default)
 * 
 * Modbus Register Map:
 * - 0x0000: Voltage (V) - 1 register, scale: 0.1
 * - 0x0001: Current (A) - 2 registers (32-bit), scale: 0.001
 * - 0x0003: Power (W) - 2 registers (32-bit), scale: 0.1
 * - 0x0005: Energy (Wh) - 2 registers (32-bit), scale: 1
 * - 0x0007: Frequency (Hz) - 1 register, scale: 0.1
 * - 0x0008: Power Factor - 1 register, scale: 0.01
 * - 0x0009: Alarm Status - 1 register
 * 
 * Hardware Connection (MAX13487 Auto Direction):
 * - ESP32 TXD0 (GPIO1) -> MAX13487 DI (Driver Input)
 * - ESP32 RXD0 (GPIO3) -> MAX13487 RO (Receiver Output)
 * - MAX13487 A -> PZEM-016 A
 * - MAX13487 B -> PZEM-016 B
 * 
 * MAX13487 Features:
 * - Auto direction detection (no DE/RE control needed)
 * - Slew-rate limited for reduced EMI
 * - Works with standard UART (no GPIO for direction control)
 */

#ifndef DEVPZEM_H
#define DEVPZEM_H

#include <Arduino.h>
#include <ModbusMaster.h>

// Callback functions for ModbusMaster (MAX13487 auto direction - no control needed)
void preTransmission() {
  // MAX13487 handles direction automatically - no action needed
  // Just ensure any pending data is sent
  Serial.flush();
}

void postTransmission() {
  // MAX13487 handles direction automatically - no action needed  
  // Small delay to ensure transmission is complete before listening
  delayMicroseconds(100);
}

class DevPZEM {
private:
  ModbusMaster node;
  HardwareSerial* serial;
  uint8_t slaveAddress;
  bool initialized;
  bool dataValid;
  unsigned long lastReadTime;
  const unsigned long readInterval = 2000;  // อ่านทุก 2 วินาที
  
  // ข้อมูลที่อ่านได้
  float voltage;      // แรงดัน (V)
  float current;      // กระแส (A)
  float power;        // กำลังไฟฟ้า (W)
  float energy;       // พลังงาน (Wh -> converted to kWh)
  float frequency;    // ความถี่ (Hz)
  float powerFactor;  // Power Factor (0-1)
  uint16_t alarmStatus; // Alarm status
  
  /**
   * @brief อ่านค่า 32-bit จาก 2 registers ต่อเนื่อง
   * @param startReg Register เริ่มต้น
   * @return ค่า 32-bit
   * @note PZEM-016 uses Low Word first, High Word second
   */
  uint32_t read32BitValue(uint16_t startReg) {
    uint8_t result = node.readInputRegisters(startReg, 2);
    if (result == node.ku8MBSuccess) {
      // PZEM-016: Register[0] = Low 16 bits, Register[1] = High 16 bits
      uint32_t value = node.getResponseBuffer(0) | ((uint32_t)node.getResponseBuffer(1) << 16);
      return value;
    }
    return 0;
  }

public:
  /**
   * @brief Constructor - สร้าง PZEM object โดยใช้ ModbusMaster
   * @param serial ตัวชี้ไปยัง HardwareSerial object (default: &Serial)
   * @param addr Modbus slave address ของ PZEM (default: 0x01)
   */
  DevPZEM(HardwareSerial* serial = &Serial, uint8_t addr = 0x01) {
    this->serial = serial;
    this->slaveAddress = addr;
    initialized = false;
    dataValid = false;
    lastReadTime = 0;
    voltage = 0.0;
    current = 0.0;
    power = 0.0;
    energy = 0.0;
    frequency = 0.0;
    powerFactor = 0.0;
    alarmStatus = 0;
  }

  /**
   * @brief Initialize PZEM sensor
   * @return true ถ้าสำเร็จ, false ถ้าล้มเหลว
   */
  bool begin() {
    // Clear serial buffer
    while (serial->available()) {
      serial->read();
    }
    
    // กำหนด Modbus slave address
    node.begin(slaveAddress, *serial);
    
    // Set callbacks for MAX13487 (auto direction - just flush/delay)
    node.preTransmission(preTransmission);
    node.postTransmission(postTransmission);
    
    delay(200);  // Wait for serial and MAX13487 to stabilize
    
    // ทดสอบการเชื่อมต่อโดยพยายามอ่านแรงดัน (retry 3 times)
    for (int attempt = 0; attempt < 3; attempt++) {
      uint8_t result = node.readInputRegisters(0x0000, 1);
      
      if (result == node.ku8MBSuccess) {
        uint16_t rawVoltage = node.getResponseBuffer(0);
        if (rawVoltage > 0 && rawVoltage < 3000) {  // ช่วง 0-300V (raw: 0-3000)
          initialized = true;
          dataValid = false;  // ยังไม่ได้อ่านครบทุกค่า
          Serial.printf("PZEM-016: Initialized successfully (Raw V=%d)\n", rawVoltage);
          Serial.println("  Using MAX13487 Auto Direction Mode");
          return true;
        }
      }
      
      Serial.printf("  Attempt %d failed (0x%02X), retrying...\n", attempt + 1, result);
      delay(100);
    }
    
    initialized = false;
    dataValid = false;
    Serial.println("PZEM-016: No response from sensor after 3 attempts");
    Serial.println("  Check: 1) Switch is in RS485 mode");
    Serial.println("         2) PZEM wiring (A-A, B-B)");
    Serial.println("         3) PZEM power supply");
    return false;
  }

  /**
   * @brief อ่านค่าทั้งหมดจาก PZEM
   * @return true ถ้าอ่านสำเร็จ, false ถ้าล้มเหลว
   */
  bool update() {
    if (!initialized) {
      return false;
    }

    // ตรวจสอบว่าถึงเวลาอ่านหรือยัง
    unsigned long currentTime = millis();
    if (currentTime - lastReadTime < readInterval) {
      return dataValid;  // ยังไม่ถึงเวลาอ่าน ใช้ค่าเดิม
    }
    lastReadTime = currentTime;

    // Clear any stale data in serial buffer
    while (serial->available()) {
      serial->read();
    }

    bool readSuccess = true;
    
    // อ่าน Voltage (Register 0x0000, 1 register)
    uint8_t result = node.readInputRegisters(0x0000, 1);
    if (result == node.ku8MBSuccess) {
      voltage = node.getResponseBuffer(0) * 0.1;  // Scale: 0.1
    } else {
      readSuccess = false;
      Serial.printf("PZEM: Failed to read voltage (0x%02X)\n", result);
    }
    
    delay(100);  // Delay between requests (MAX13487 needs time to switch direction)
    
    // อ่าน Current (Register 0x0001, 2 registers, 32-bit)
    uint32_t rawCurrent = read32BitValue(0x0001);
    current = rawCurrent * 0.001;  // Scale: 0.001 A
    
    delay(100);
    
    // อ่าน Power (Register 0x0003, 2 registers, 32-bit)
    uint32_t rawPower = read32BitValue(0x0003);
    power = rawPower * 0.1;  // Scale: 0.1
    
    delay(100);
    
    // อ่าน Energy (Register 0x0005, 2 registers, 32-bit)
    uint32_t rawEnergy = read32BitValue(0x0005);
    energy = rawEnergy;  // Wh (will convert to kWh when needed)
    
    delay(100);
    
    // อ่าน Frequency (Register 0x0007, 1 register)
    result = node.readInputRegisters(0x0007, 1);
    if (result == node.ku8MBSuccess) {
      frequency = node.getResponseBuffer(0) * 0.1;  // Scale: 0.1
    }
    
    delay(100);
    
    // อ่าน Power Factor (Register 0x0008, 1 register)
    result = node.readInputRegisters(0x0008, 1);
    if (result == node.ku8MBSuccess) {
      powerFactor = node.getResponseBuffer(0) * 0.01;  // Scale: 0.01
    }
    
    delay(100);
    
    // อ่าน Alarm Status (Register 0x0009, 1 register)
    result = node.readInputRegisters(0x0009, 1);
    if (result == node.ku8MBSuccess) {
      alarmStatus = node.getResponseBuffer(0);
    }

    // ตรวจสอบความถูกต้องของข้อมูล
    if (!readSuccess || voltage < 0 || voltage > 300) {
      dataValid = false;
      Serial.println("PZEM: Data validation failed");
      return false;
    }

    dataValid = true;
    return true;
  }

  /**
   * @brief รีเซ็ตค่า Energy counter
   * @return true ถ้าสำเร็จ, false ถ้าล้มเหลว
   */
  bool resetEnergy() {
    if (!initialized) {
      return false;
    }
    
    // PZEM-016 Reset Command: Write to register 0x0042
    // Using function code 0x06 (Write Single Register)
    uint8_t result = node.writeSingleRegister(0x0042, 0x0000);
    
    if (result == node.ku8MBSuccess) {
      energy = 0.0;
      Serial.println("PZEM: Energy counter reset successfully");
      return true;
    }
    
    Serial.printf("PZEM: Failed to reset energy (0x%02X)\n", result);
    return false;
  }

  // Getter methods
  bool isInitialized() const { return initialized; }
  bool isDataValid() const { return dataValid; }
  uint8_t getSlaveAddress() const { return slaveAddress; }
  float getVoltage() const { return dataValid ? voltage : 0.0; }
  float getCurrent() const { return dataValid ? current : 0.0; }
  float getPower() const { return dataValid ? power : 0.0; }
  float getEnergy() const { return dataValid ? (energy / 1000.0) : 0.0; }  // Convert Wh to kWh
  float getFrequency() const { return dataValid ? frequency : 0.0; }
  float getPowerFactor() const { return dataValid ? powerFactor : 0.0; }
  uint16_t getAlarmStatus() const { return dataValid ? alarmStatus : 0; }

  /**
   * @brief แสดงข้อมูลทั้งหมดใน Serial Monitor
   */
  void printData() {
    if (!dataValid) {
      Serial.println("PZEM: No valid data");
      return;
    }

    Serial.println("========== PZEM-016 Data ==========");
    Serial.printf("Voltage:      %.2f V\n", voltage);
    Serial.printf("Current:      %.3f A\n", current);
    Serial.printf("Power:        %.2f W\n", power);
    Serial.printf("Energy:       %.3f kWh\n", getEnergy());
    Serial.printf("Frequency:    %.1f Hz\n", frequency);
    Serial.printf("Power Factor: %.2f\n", powerFactor);
    Serial.printf("Alarm Status: 0x%04X\n", alarmStatus);
    Serial.println("===================================");
  }

  /**
   * @brief สร้าง JSON string สำหรับส่งข้อมูล
   * @return JSON string
   */
  String toJSON() {
    String json = "{";
    json += "\"slaveId\":" + String(slaveAddress) + ",";
    json += "\"voltage\":" + String(voltage, 2) + ",";
    json += "\"current\":" + String(current, 3) + ",";
    json += "\"power\":" + String(power, 2) + ",";
    json += "\"energy\":" + String(getEnergy(), 3) + ",";
    json += "\"frequency\":" + String(frequency, 1) + ",";
    json += "\"powerFactor\":" + String(powerFactor, 2) + ",";
    json += "\"alarmStatus\":" + String(alarmStatus) + ",";
    json += "\"valid\":" + String(dataValid ? "true" : "false");
    json += "}";
    return json;
  }
};

#endif // DEVPZEM_H
