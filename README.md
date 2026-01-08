# ADV-MitutoyoSPC
> [!CAUTION]
> **NOT YET TESTED**

Parser for data received via Mitutoyo SPC Protocol

## Reading procedure
1. Connect the device to the microcontroller using 3 pins:
   1. `REQ`: output pin that is triggered by the user when a new measurement is requested
   2. `CLK`: input pin where the signal clock line of the device is connected
   3. `DAT`: input pin where the data line of the device is connected 
2. When the user requires a measure, set `REQ` pin low and and call `MitutoyoSPC_startReception`. Release `REQ` after at least 250 us (in any case stop after the first 4-bits nibble has been received)
3. Data bits are then transmitted on the `DAT` line synchronized with the rising edge of the `CLK` line
4. As soon as a rising edge is detected on the `CLK` line, read the value of the `DAT` line (0 is low, 1 is high) and pass it to `MitutoyoSPC_processBit`
5. If `SPC_STATUS_SUCCESS` is returned, measurement data can be read via `MitutoyoSPC_processFrame` followed by `MitutoyoSPC_getMeasurementValue` and `MitutoyoSPC_getUnitOfMeasure`

## Example on STM32
- Initialization
```cpp
#include "MitutoyoSPC.h"
MitutoyoSPC_t spc;
float measure;
MitutoyoSPC_unit_t uom;
uint8_t dataReceivedFlag = 0;
```  

- Measurement request
```cpp
HAL_GPIO_WritePin(REQ_GPIO_Port, REQ_Pin, GPIO_PIN_RESET);
MitutoyoSPC_startReception(&spc);
HAL_delay(250);
HAL_GPIO_WritePin(REQ_GPIO_Port, REQ_Pin, GPIO_PIN_SET);
```  

- Interrupt callback
```cpp
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    switch (GPIO_Pin) {
        case CLK_Pin:
            if (MitutoyoSPC_processBit(&spc, HAL_GPIO_ReadPin(DAT_GPIO_Port, DAT_Pin)) == SPC_STATUS_SUCCESS) {
                dataReceivedFlag = 1;
            }
            break;

        default: 
            break;
    }
    return;
}
```

- Data reading
```cpp
if (dataReceivedFlag == 1) {
    dataReceivedFlag == 0;
    if (MitutoyoSPC_processFrame(&spc) == SPC_STATUS_SUCCESS) {
        measure = MitutoyoSPC_getMeasurementValue(&spc);
        uom = MitutoyoSPC_getUnitOfMeasure(&spc);
    }
}
```