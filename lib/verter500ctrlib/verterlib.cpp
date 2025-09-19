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
    digitalWrite(VERTER_powerControl_pin, LOW); // Ensure power control is off initially
}

void VERTER_AuthenticateBattery() 
{
    // Authenticate battery by reading its EEPROM
    Wire.beginTransmission(VERTER_batteryI2C_address);
    Wire.write(0x00); // Start reading from address 0
    Wire.endTransmission();
    
    Wire.requestFrom(VERTER_batteryI2C_address, 1);
    if (Wire.available()) {
        uint8_t authByte = Wire.read();
        if (authByte == 0x01) { // Assuming 0x01 is the authentication byte
            Serial.println("Battery authenticated successfully.");
        } else {
            Serial.println("Battery authentication failed.");
            digitalWrite(VERTER_buzzer_pin, HIGH); // Sound alarm
            delay(3000); // Wait for 3 seconds
            digitalWrite(VERTER_buzzer_pin, LOW); // Turn off buzzer
            digitalWrite(VERTER_powerControl_pin, LOW); // Ensure power control is off
            return; // Exit if authentication fails
        }
    }
}

void VERTER_CalculateBatteryPercentage() 
{
    const float ADC_REF = 5.0;        // Nano default
    const float ADC_MAX = 1023.0;
    const float SCALE   = 6.087f;     // = Vbat_measured / Vadc_measured = 12.6 / 2.070

    float raw = analogRead(VERTER_batteryVoltage_pin);
    float Vadc = (raw / ADC_MAX) * ADC_REF;  // volts at ADC pin
    float Vbat = Vadc * SCALE;               // calibrated pack voltage
    batteryVoltage = (Vbat-0.6);

    float pct = ((Vbat - 9.0f) / (12.6f - 9.0f)) * 100.0f;
    if (pct < 0) pct = 0;
    if (pct > 100) pct = 100;
    batteryPercentage = pct;
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



  