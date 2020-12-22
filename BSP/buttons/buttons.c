#include "buttons.h"
#include <stm32l4xx_hal.h>

/**
 * @brief list of BUTTON ports in same order as @ref button_e
 */
static GPIO_TypeDef *const buttonPortTable[NB_BUTTONS] = {
        GPIOH, GPIOB, GPIOD, GPIOE, GPIOD
    };

/**
 * @brief list of BUTTON pins in same order as @ref button_e
 */
static const uint32_t buttonPinTable[NB_BUTTONS] = {
        GPIO_PIN_3, GPIO_PIN_15, GPIO_PIN_8, GPIO_PIN_8, GPIO_PIN_15
    };

void buttons_init(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    for(uint8_t i = 0; i<NB_BUTTONS; i++)
    {
        GPIO_InitStruct.Pin = buttonPinTable[i];
        HAL_GPIO_Init(buttonPortTable[i], &GPIO_InitStruct);
    }
}

bool buttons_isPressed(button_e button)
{
    GPIO_PinState state = HAL_GPIO_ReadPin(buttonPortTable[button], buttonPinTable[button]);

    return state == GPIO_PIN_SET;
}
