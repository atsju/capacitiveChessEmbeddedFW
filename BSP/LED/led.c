#include "led.h"
#include <stm32l4xx_hal.h>

#define NB_LED_PER_EDGE (9)

/**
 * @brief list of ANODE LED ports from LED_A to LED_I
 */
static GPIO_TypeDef *const anodeLedPortTable[NB_LED_PER_EDGE] = {
        GPIOD, GPIOD, GPIOD, GPIOB, GPIOB, GPIOE, GPIOE, GPIOE, GPIOE
    };

/**
 * @brief list of ANODE LED pins from LED_A to LED_I
 */
static const uint32_t anodeLedPinTable[NB_LED_PER_EDGE] = {
        GPIO_PIN_14, GPIO_PIN_12, GPIO_PIN_10, GPIO_PIN_14, GPIO_PIN_12, GPIO_PIN_15, GPIO_PIN_13, GPIO_PIN_11, GPIO_PIN_9
    };

/**
 * @brief list of CATHODE LED ports from LED_1 to LED_9
 */
static GPIO_TypeDef *const cathodeLedPortTable[NB_LED_PER_EDGE] = {
        GPIOE, GPIOH, GPIOC, GPIOC, GPIOE, GPIOE, GPIOB, GPIOB, GPIOB
    };

/**
 * @brief list of CATHODE LED ppin from LED_1 to LED_9
 */
static const uint32_t cathodeLedPinTable[NB_LED_PER_EDGE] = {
        GPIO_PIN_7, GPIO_PIN_1, GPIO_PIN_15, GPIO_PIN_13, GPIO_PIN_5, GPIO_PIN_3, GPIO_PIN_9, GPIO_PIN_7, GPIO_PIN_5
    };

void led_init(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Pull = GPIO_NOPULL;

    // cathode needs to be set high before anode to avoid anyblinking during powerup
    for(uint8_t i=0; i<NB_LED_PER_EDGE; i++)
    {
        // init TOP pins to output GND
        GPIO_InitStruct.Pin = cathodeLedPinTable[i];
        HAL_GPIO_Init(cathodeLedPortTable[i], &GPIO_InitStruct);
        HAL_GPIO_WritePin(cathodeLedPortTable[i], cathodeLedPinTable[i], GPIO_PIN_SET);
    }
    for(uint8_t i=0; i<NB_LED_PER_EDGE; i++)
    {
        // init TOP pins to output GND
        GPIO_InitStruct.Pin = anodeLedPinTable[i];
        HAL_GPIO_Init(anodeLedPortTable[i], &GPIO_InitStruct);
        HAL_GPIO_WritePin(anodeLedPortTable[i], anodeLedPinTable[i], GPIO_PIN_RESET);
    }
}

void led_blinkTest(void)
{
    ;
}
