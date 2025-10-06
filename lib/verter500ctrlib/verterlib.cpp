/*
        FILE NAME : verterlib.cpp
        FILE DESCRIPTION : Definition file for Verter500C Portable Power Station Controller library.
        FILE AUTHOR : Aina Favour Olalekan.
        DATE CREATED : 2024-10-01.
        DATE LAST MODIFIED : 2024-10-10.
*/


/*
Operational WorkFlow:
1. When battery is connected, authenticate the battery pack by reading the EEPROM.
2. If authentication is successful, trigger VERTER_powerControl_pin to close relay and supply battery power.
3. If authentication fails, trigger VERTER_buzzer_pin to sound an alarm.
4. Read energy values from the PZEM004T energy meter.
5. Calculate battery percentage using voltage divider method.
6. Set the charging state based on the detection of a charger input at the DC jack.
7. Send energy values and battery percentage over UART to the display screen.
8. Continuously monitor the battery voltage and update the display with the current energy values.
*/


#include "verterlib.h"

 float batteryPercentage {};
 float batteryVoltage { };
 bool isCharging {};
 float voltage {};
 float current {};
 float power {};
 float energy {};
 float frequency {};
 float powerFactor {};
 int slaveValues [NUM_THRESHOLDS] ;// Array to hold slave values
 int  PowerThreshold, DistanceThreshold, TempThreshold;
 String whitelist[] = {"PACK1234", "PACKABCD"}; // station-side allowed IDs

// === Create Software Serial Port for PZEM ===
SoftwareSerial pzemSerial(PZEM_RX_PIN, PZEM_TX_PIN); // RX, TX
PZEM004Tv30 pzem(pzemSerial);

// === Function Definitions ===
void VERTER_init() 
{   // Initialize i2c communication and serial communication
    Wire.begin(); 
    Serial.begin(9600);

    // Initialize gpiopins
    pinMode(VERTER_chargeState_pin, INPUT);
    pinMode(VERTER_batteryVoltage_pin, INPUT);
    pinMode(VERTER_buzzer_pin, OUTPUT);
    pinMode(VERTER_powerControl_pin, OUTPUT);
    digitalWrite(VERTER_buzzer_pin, LOW); // Ensure buzzer is off initially
    digitalWrite(VERTER_powerControl_pin, HIGH); // Ensure power control is off initially


     // Authenticate battery and check whitelist
          String batteryID = VERTER_AuthenticateBattery();
          if (VERTER_checkWhitelist(batteryID)) {
              VERTER_PowerInverterON();
          } else {
              VERTER_PowerInverterOFF();
              // Optionally, you can add a delay or halt further processing
              // to prevent repeated attempts to power on with an invalid battery
              delay(3000); // Wait before next authentication attempt
              return; // Skip the rest of the loop
          }
}

String VERTER_AuthenticateBattery() 
{
   char buf[VERTER_ID_LEN+1];
  for (unsigned int i=0; i<VERTER_ID_LEN; ++i) {
    uint8_t devAddr = VERTER_batteryI2C_address | (((VERTER_ID_ADDR + i) >> 8) & 0x07);
    uint8_t lowAddr = (VERTER_ID_ADDR + i) & 0xFF;
    Wire.beginTransmission(devAddr);
    Wire.write(lowAddr);
    Wire.endTransmission();
    Wire.requestFrom(devAddr, (uint8_t)1);
    if (Wire.available()) buf[i] = (char)Wire.read();
    else buf[i] = 0;
  }
  buf[VERTER_ID_LEN] = 0;
  return String(buf);
}

bool VERTER_checkWhitelist(String id) {
  for (auto &w : whitelist) if (w == id) return true;
  return false;
}

void VERTER_PowerInverterON()
{
    digitalWrite(VERTER_powerControl_pin, LOW); // Close relay to supply power

    digitalWrite(VERTER_buzzer_pin, 1); // Ensure buzzer is On
    delay(1000);
    digitalWrite(VERTER_buzzer_pin, 0); // Ensure buzzer is Off
}

void VERTER_PowerInverterOFF()
{
    digitalWrite(VERTER_powerControl_pin, HIGH); // Open relay to cut off power
    for (int i = 0; i < 3; i++) {
        digitalWrite(VERTER_buzzer_pin, HIGH); // Sound buzzer
        delay(200);
        digitalWrite(VERTER_buzzer_pin, LOW); // Turn off buzzer
        delay(200);
    }
    digitalWrite(VERTER_buzzer_pin, LOW); // Ensure buzzer is Off
}


void VERTER_CalculateBatteryPercentage() 
{
    const float ADC_REF = 5.5;        // Nano default
    const float ADC_MAX = 1023.0;
    const float SCALE   = 6.087f;     // = Vbat_measured / Vadc_measured = 12.6 / 2.070

    float raw = analogRead(VERTER_batteryVoltage_pin);
    float Vadc = (raw / ADC_MAX) * ADC_REF;  // volts at ADC pin
    float Vbat = Vadc * SCALE;               // calibrated pack voltage
    batteryVoltage = (Vbat-0.6);

    float pct = ((Vbat - 9.0f) / (12.6f - 9.0f)) * 100.0f;
    if (pct < 0) pct = 0;
    if (pct > 100) pct = 100;
     batteryPercentage = pct;   // aina actual battery percentage
  //  batteryPercentage = 20; //dummy battery percentage test value
}

void VERTER_SetChargingState() 
{
      int raw = analogRead(VERTER_chargeState_pin);
      float voltage = (raw / 1023.0) * 5.0;

  if (voltage > 1.0) {
    isCharging = true;
  } else {
    isCharging = false;
  }
}

void VERTER_updateEnergyReadings() 
{
    voltage     = pzem.voltage();
    current     = pzem.current();
    power       = pzem.power();
    energy      = pzem.energy();
    frequency   = pzem.frequency();
    powerFactor = pzem.pf();
}

void VERTER_uart_master_send()
{
    String response = String('#') + "," + String(power) + "," + String(batteryPercentage) + "," + String(batteryVoltage) + "," +
                      String(voltage) + "," + String(energy) + "," +
                      String(isCharging)  + String("\n");

    Serial.print(response); // send to slave

    String input = "";

    while (Serial.available()) {
        char c = Serial.read();

        // Optional: skip garbage before the start character
        if (input.length() == 0 && c != MSG_HEADER) {
            continue;
        }

        input += c;

        // Only process when newline is received
        if (c == MSG_TERMINATOR) {
            input.trim();

            // Strip off header if used
            if (input.charAt(0) == MSG_HEADER) {
                input.remove(0, 1);  // remove header
            }

            char inputBuffer[INPUT_BUFFER_SIZE];
            input.toCharArray(inputBuffer, sizeof(inputBuffer));

            char *token = strtok(inputBuffer, ",");
            int idx = 0;
            while (token != NULL && idx < NUM_THRESHOLDS) {
                slaveValues[idx++] = atof(token);
                token = strtok(NULL, ",");
            }

            if (idx == NUM_THRESHOLDS) {
                // Assign thresholds from master
                PowerThreshold = slaveValues[0];
                DistanceThreshold = slaveValues[1];
                TempThreshold = slaveValues[2];

            // input = ""; // reset input buffer
            }
        }
    }
}



  