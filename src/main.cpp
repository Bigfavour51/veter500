    /*
        FILE NAME : main.cpp
        FILE DESCRIPTION : Source file for Verter500C Portable Power Station Controller library.
        FILE AUTHOR : Aina Favour Olalekan.
        DATE CREATED : 2024-10-01.
        DATE LAST MODIFIED : 2024-10-10.
*/
    
    
    
    #include <Arduino.h>
    #include "verterlib.h"


      void setup() 
      {
        VERTER_init();
      
      }

    void loop() {
        VERTER_updateEnergyReadings();
        VERTER_CalculateBatteryPercentage();
        VERTER_SetChargingState();
        VERTER_uart_master_send();
        delay(100); // Delay for a second before the next loop
    }

 
