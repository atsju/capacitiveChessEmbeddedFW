#include "buttons.h"
#include <stm32l4xx_hal.h>

typedef struct {
    GPIO_TypeDef *const port;
    uint32_t pin;
} bspIO_s;

/**
 * @brief list of BUTTON IOs in same order as @ref button_e
 */
static const bspIO_s buttonIOtable[NB_BUTTONS]={
    {GPIOH, GPIO_PIN_3},
    {GPIOB, GPIO_PIN_15},
    {GPIOD, GPIO_PIN_8},
    {GPIOE, GPIO_PIN_8},
    {GPIOD, GPIO_PIN_15}
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
        GPIO_InitStruct.Pin = buttonIOtable[i].pin;
        HAL_GPIO_Init(buttonIOtable[i].port, &GPIO_InitStruct);
    }
}

bool buttons_isPressed(button_e button)
{
    GPIO_PinState state = HAL_GPIO_ReadPin(buttonIOtable[button].port, buttonIOtable[button].pin);

    return state == GPIO_PIN_SET;
}
