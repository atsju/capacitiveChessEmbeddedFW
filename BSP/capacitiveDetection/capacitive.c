#include "capacitive.h"

#include <stm32l4xx_hal.h>

/**
 * @brief This is just to remove the magic "16"
 */
#define NB_MULTIPLEX_PINS    (16)

/**
 * @brief list of TOP ports from top1 to top8 and topA to topH
 */
static GPIO_TypeDef *const topPortTable[NB_MULTIPLEX_PINS] = {
        GPIOA, GPIOA, GPIOA, GPIOA, GPIOC, GPIOC, GPIOC, GPIOC,
        GPIOB, GPIOB, GPIOC, GPIOC, GPIOA, GPIOA, GPIOA, GPIOA
    };

/**
 * @brief list of EN ports from en1 to en8 and enA to enH
 */
static GPIO_TypeDef *const enPortTable[NB_MULTIPLEX_PINS] = {
        GPIOB, GPIOH, GPIOC, GPIOE, GPIOE, GPIOE, GPIOB, GPIOB,
        GPIOD, GPIOD, GPIOD, GPIOB, GPIOB, GPIOE, GPIOE, GPIOE
    };

/**
 * @brief list of TOP pins from top1 to top8 and topA to topH
 */
static const uint32_t topPinTable[NB_MULTIPLEX_PINS] = {
        GPIO_PIN_3, GPIO_PIN_2, GPIO_PIN_1, GPIO_PIN_0, GPIO_PIN_3, GPIO_PIN_2, GPIO_PIN_1, GPIO_PIN_0,
        GPIO_PIN_1, GPIO_PIN_0, GPIO_PIN_5, GPIO_PIN_4, GPIO_PIN_7, GPIO_PIN_6, GPIO_PIN_5, GPIO_PIN_4
    };

/**
 * @brief list of EN pins from top1 to top8 and topA to topH
 */
static const uint32_t enPinTable[NB_MULTIPLEX_PINS] = {
        GPIO_PIN_2, GPIO_PIN_0, GPIO_PIN_14, GPIO_PIN_6, GPIO_PIN_4, GPIO_PIN_0, GPIO_PIN_8, GPIO_PIN_6,
        GPIO_PIN_13, GPIO_PIN_11, GPIO_PIN_9, GPIO_PIN_13, GPIO_PIN_10, GPIO_PIN_14, GPIO_PIN_12, GPIO_PIN_10
    };


void capacitive_init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_ADC_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Pull = GPIO_NOPULL;

    for(uint8_t i=0; i<NB_MULTIPLEX_PINS; i++)
    {
        // init TOP pins to output GND
        GPIO_InitStruct.Pin = topPinTable[i];
        HAL_GPIO_Init(topPortTable[i], &GPIO_InitStruct);
        HAL_GPIO_WritePin(topPortTable[i], topPinTable[i], GPIO_PIN_RESET);

        // init EN pins to output VCC (MCP6043 has CSN pin)
        GPIO_InitStruct.Pin = enPinTable[i];
        HAL_GPIO_Init(enPortTable[i], &GPIO_InitStruct);
        HAL_GPIO_WritePin(enPortTable[i], enPinTable[i], GPIO_PIN_SET);
    }

    //TODO init ADC peripheral here if possible
}

// discharge the sample and hold cap by making a measurement to GND
// charge the correct TOP plate to VCC
// Enable corresponding OPAMP
// the charged TOP pin configures to ADC input
// sample and hold the TOP plate (will discharge "parasitic" cap into sample and hold)
uint16_t capacitive_getADCvalue(void)
{

    return 42;
}

