#include <stm32l4xx_hal.h>

#include "LS013B7DH03.h"
#include "buttons.h"
#include "capacitive.h"
#include <stdio.h>


int main(void)
{
    HAL_Init();

    //TODO configure clock to max speed (reduce when everything is debugged)

    sharpMemoryLCD_init();
    sharpMemoryLCD_printTextLine(0, "hello world", 11);

    //TODO test the buttons
    buttons_init();
    //TODO test the LEDs
    //TODO test the SMPS
    //TODO test the USB
    capacitive_init();
    //TODO test the EPD screen
    //TODO write of find some delay function because it will be needed


    while(1)
    {
        char printBuffer[11];
        uint16_t rawADC = capacitive_getADCvalue();
        sprintf(printBuffer, "%i", rawADC);
        sharpMemoryLCD_printTextLine(1, printBuffer, 10);

        uint8_t c = buttons_isPressed(BUTTON_CENTER) ? 1:0;
        uint8_t u = buttons_isPressed(BUTTON_UP) ? 1:0;
        uint8_t d = buttons_isPressed(BUTTON_DOWN) ? 1:0;
        uint8_t l = buttons_isPressed(BUTTON_LEFT) ? 1:0;
        uint8_t r = buttons_isPressed(BUTTON_RIGHT) ? 1:0;
        sprintf(printBuffer, "%i %i %i %i %i",c,u,d,l,r);
        sharpMemoryLCD_printTextLine(5, "C U D L R", 11);
        sharpMemoryLCD_printTextLine(6, printBuffer, 11);

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
