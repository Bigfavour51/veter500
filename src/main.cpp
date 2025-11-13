//     /*
//         FILE NAME : main.cpp
//         FILE DESCRIPTION : Source file for Verter500C Portable Power Station Controller library.
//         FILE AUTHOR : Aina Favour Olalekan.
//         DATE CREATED : 2024-10-01.
//         DATE LAST MODIFIED : 2024-10-10.
// */
    
    
    
    #include <Arduino.h>
    #include "verterlib.h"


      void setup() 
      {
        VERTER_init();
      
      }

    void loop() 
    {
        VERTER_updateEnergyReadings();
        VERTER_CalculateBatteryPercentage();
        VERTER_SetChargingState();
        VERTER_uart_master_send();
        delay(100); // Delay for a second before the next loop
    }

// Relay and Buzzer Test Code

// const int relayPin = 6;    // Relay connected to pin 6
// const int buzzerPin = 5;   // Buzzer connected to pin 5

// void setup() {
//   pinMode(relayPin, OUTPUT);
//   pinMode(buzzerPin, OUTPUT);
  
//   Serial.begin(9600);
//   Serial.println("Relay and Buzzer Test Starting...");
// }

// void loop() {
//   Serial.println("Relay ON, Buzzer ON");
//   digitalWrite(relayPin, HIGH);
//   digitalWrite(buzzerPin, HIGH);
//   delay(1000); // ON for 1 second
  
//   Serial.println("Relay OFF, Buzzer OFF");
//   digitalWrite(relayPin, LOW);
//   digitalWrite(buzzerPin, LOW);
//   delay(1000); // OFF for 1 second
// }


// #include <SoftwareSerial.h>
// #include <PZEM004Tv30.h>

// // Define Software Serial pins for PZEM
// #define PZEM_RX_PIN 7   // Connect to TX of PZEM
// #define PZEM_TX_PIN 8   // Connect to RX of PZEM

// // Create SoftwareSerial and PZEM instances
// SoftwareSerial pzemSerial(PZEM_RX_PIN, PZEM_TX_PIN);
// PZEM004Tv30 pzem(pzemSerial);

// void setup() {
//   Serial.begin(9600);
//   Serial.println("PZEM004T Test Starting...");
//   delay(1000);
// }

// void loop() {
//   float voltage = pzem.voltage();
//   float current = pzem.current();
//   float power   = pzem.power();
//   float energy  = pzem.energy();
//   float frequency = pzem.frequency();
//   float pf      = pzem.pf();

//   if(isnan(voltage)) {
//     Serial.println("Error reading PZEM sensor data!");
//   } else {
//     Serial.print("Voltage: "); Serial.print(voltage); Serial.println(" V");
//     Serial.print("Current: "); Serial.print(current); Serial.println(" A");
//     Serial.print("Power:   "); Serial.print(power); Serial.println(" W");
//     Serial.print("Energy:  "); Serial.print(energy); Serial.println(" Wh");
//     Serial.print("Freq:    "); Serial.print(frequency); Serial.println(" Hz");
//     Serial.print("PF:      "); Serial.println(pf);
//     Serial.println("------------------------------------");
//   }

//   delay(2000); // Wait 2 seconds before next read
// }
