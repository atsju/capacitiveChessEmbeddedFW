#include <stm32l4xx_hal.h>

#include "LS013B7DH03.h"

int main(void)
{
    HAL_Init();

    sharpMemoryLCD_init();

    while(1)
    {
        ;
    }

    return 0;
}
