#include <Arduino.h>
#include "verterlib.h"


void setup() 
{
    VERTER_init();
}

void loop() {
   // VERTER_AuthenticateBattery();
    VERTER_updateEnergyReadings();
    VERTER_CalculateBatteryPercentage();
    VERTER_SetChargingState();
    VERTER_uart_master_send();
    delay(100); // Delay for a second before the next loop
   
}

// Nano: ADC Signature Auth (3-channel, counts-based)
// Resistor dividers chosen for 12.6V pack:
// Ch1: Rtop=120k, Rbot=10k  => expected ADC ~198
// Ch2: Rtop=39k,  Rbot=10k  => expected ADC ~527
// Ch3: Rtop=27k,  Rbot=10k  => expected ADC ~696
//
// Hardware notes:
// - Put a 4.7k series resistor between each SIGNAL node and the corresponding ADC pin.
// - Optionally add a small Schottky clamp from SIGNAL to Vcc for protection.
// - Ensure MCU ground is connected to pack ground BEFORE sampling (use dedicated ground pin on connector if you plan to switch main negative).
// - Use 1% resistors for better consistency.
