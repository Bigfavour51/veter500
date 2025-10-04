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

 
