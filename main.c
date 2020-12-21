#include <stm32l4xx_hal.h>

#include "LS013B7DH03.h"

int main(void)
{
    HAL_Init();

    //TODO configure clock to max speed (reduce when everything is debugged)

    sharpMemoryLCD_init();
    sharpMemoryLCD_printTextLine(0, "hello world", 11);

    //TODO test the buttons
    //TODO test the SMPS
    //TODO test the USB
    //TODO test the ADC for capacitive measurements
    //TODO test the EPD screen


    while(1)
    {
        ;
    }

    return 0;
}


void assert_failed(uint8_t *file, uint32_t line)
{
    // TODO print on screen before dying in while
    while(1)
    {
        ;
    }
}
