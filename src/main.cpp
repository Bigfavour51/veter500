// #include <Arduino.h>
// #include "verterlib.h"


// void setup() 
// {
//     VERTER_init();
// }

// void loop() {
//    // VERTER_AuthenticateBattery();
//     VERTER_updateEnergyReadings();
//     VERTER_CalculateBatteryPercentage();
//     VERTER_SetChargingState();
//     VERTER_uart_master_send();
//     delay(100); // Delay for a second before the next loop
   
// }
#include <Arduino.h>
#include <Wire.h>
#define EEPROM_BASE 0x50 // base I2C address (A2,A1,A0 = 0)



void writeEEPROM(uint8_t deviceBaseAddr, unsigned int eeaddress, byte data);
byte readEEPROM(uint8_t deviceBaseAddr, unsigned int eeaddress);



void setup() {
  Wire.begin();
  Serial.begin(9600);
  delay(200);

  Serial.println("We write the zip code 22222, a zip code");

  // Write characters '2' to addresses 0..4
  for (unsigned int address = 0; address < 5; address++) {
    writeEEPROM(EEPROM_BASE, address, '2');
    delay(10); // allow write cycle to complete (10 ms is safe)
  }

  // Read them back and print as HEX (as you did originally)
  for (unsigned int address = 0; address < 5; address++) {
    byte b = readEEPROM(EEPROM_BASE, address);
    Serial.print(b, HEX);
  }
  Serial.println();
}

void loop() {
  // nothing
}

// Write a single byte to a 24C08 (device uses block bits in dev address)
void writeEEPROM(uint8_t deviceBaseAddr, unsigned int eeaddress, byte data) {
  uint8_t devAddr = deviceBaseAddr | ((eeaddress >> 8) & 0x07); // high address bits
  uint8_t lowAddr = eeaddress & 0xFF;

  Wire.beginTransmission(devAddr);
  Wire.write(lowAddr);    // ONLY low address byte
  Wire.write(data);
  Wire.endTransmission();
  delay(10); // ensure EEPROM finishes internal write
}

// Read a single byte from a 24C08
byte readEEPROM(uint8_t deviceBaseAddr, unsigned int eeaddress) {
  uint8_t devAddr = deviceBaseAddr | ((eeaddress >> 8) & 0x07);
  uint8_t lowAddr = eeaddress & 0xFF;
  byte rdata = 0xFF;

  Wire.beginTransmission(devAddr);
  Wire.write(lowAddr);   // ONLY low address byte
  Wire.endTransmission();

  Wire.requestFrom(devAddr, (uint8_t)1);
  if (Wire.available()) rdata = Wire.read();
  return rdata;
}
