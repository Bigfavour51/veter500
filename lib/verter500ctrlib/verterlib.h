#ifndef __VERTERLIB_H__
#define __VERTERLIB_H__



/*
Firmware design for Verter500C Energy Meter Controller

Scope of Work:
- Implement I2C communication between Verter500C and the Battery Packs EEPROM 24C08
- Read and write energy values to the EEPROM
- Implement energy meter functionality to read voltage, current, power, and energy
- Implement UART communication for sending data to the display screen CYD 2.8 inches
- Implement a Battery percent calculation using voltage divider method
- Implement charging and not charging stated using diode resistor method


*/




/*
               PIN CONNECTION MAPPING
               ------------------------
    screen UART TX  -> Verter500C RX- D0
    screen UART RX  -> Verter500C TX- D1
    PZEM RX         -> Verter500C TX- D7
    PZEM TX         -> Verter500C RX- D8
    Battery EEPROM  -> Verter500C I2C pins (SDA-A4, SCL-A5)
    Battery Voltage -> Verter500C VERTER_batteryVoltage_pin - A0
    Battery Charge State -> Verter500C VERTER_chargeState_pin - D2
    Buzzer          -> Verter500C VERTER_buzzer_pin - D3
    Power Control   -> Verter500C VERTER_powerControl_pin - D4


*/




#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <PZEM004Tv30.h>


// === User Configuration ===
#define PZEM_RX_PIN 7   // Connect to TX of NANO
#define PZEM_TX_PIN 8   // Connect to RX of NANO

// === Constants ===
#define NUM_THRESHOLDS 3
#define INPUT_BUFFER_SIZE 64
#define MSG_HEADER '#' // optional: use for identifying start of command
#define MSG_TERMINATOR '\n'

#define VERTER_chargeState_pin A1
#define VERTER_batteryVoltage_pin A0
#define VERTER_buzzer_pin 5
#define VERTER_powerControl_pin 6

#define VERTER_batteryI2C_address 0x50 // I2C address for the battery EEPROM
#define EEPROM_BASE 0x50
#define VERTER_ID_ADDR 0
#define VERTER_ID_LEN 8




// === Function Declarations ===
void VERTER_init();
String VERTER_AuthenticateBattery();
void VERTER_ReadEnergyValues();
void VERTER_CalculateBatteryPercentage();
void VERTER_SetChargingState();
void VERTER_updateEnergyReadings();
void VERTER_uart_master_send();
void VERTER_authBATTERY();
bool VERTER_checkWhitelist(String id);
void VERTER_PowerInverterON();
void VERTER_PowerInverterOFF();

// === Global Variables declaration===
extern float batteryPercentage;
extern float batteryVoltage;
extern bool  isCharging;
extern float voltage;
extern float current;
extern float power;
extern float energy;
extern float frequency;
extern float powerFactor;
extern int   slaveValues [NUM_THRESHOLDS] ;// Array to hold slave values
extern int   PowerThreshold, DistanceThreshold, TempThreshold;

extern String whitelist[]; // station-side allowed IDs


#endif // __VERTERLIB_H__


// End of file: lib/verter500ctrlib/verterlib.h
// This file is part of the Verter500C library.
// It contains the declarations for the Verter500C library functions and classes.