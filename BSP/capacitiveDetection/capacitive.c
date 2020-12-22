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

/**
 * @brief list of TOP pins corresponding ADC channels from top1 to top8 and topA to topH
 */
static const uint32_t topADCchanTable[NB_MULTIPLEX_PINS] = {
        ADC_CHANNEL_8, ADC_CHANNEL_7, ADC_CHANNEL_6, ADC_CHANNEL_5, ADC_CHANNEL_4, ADC_CHANNEL_3, ADC_CHANNEL_2, ADC_CHANNEL_1,
        ADC_CHANNEL_16, ADC_CHANNEL_15, ADC_CHANNEL_14, ADC_CHANNEL_13, ADC_CHANNEL_12, ADC_CHANNEL_11, ADC_CHANNEL_10, ADC_CHANNEL_9
    };


ADC_HandleTypeDef AdcHandle;

bool capacitive_init(void)
{
    bool resultSuccess = true;

    /* **** Init the GPIOs **** */

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

    /* **** Init the ADC **** */
    __HAL_RCC_ADC_CLK_ENABLE();
    __HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_SYSCLK);

    AdcHandle.Instance          = ADC1;

    if (HAL_ADC_DeInit(&AdcHandle) != HAL_OK)
    {
        resultSuccess = false;
    }

    AdcHandle.Init.ClockPrescaler        = ADC_CLOCK_ASYNC_DIV1;          /* Asynchronous clock mode, input ADC clock not divided */
    AdcHandle.Init.Resolution            = ADC_RESOLUTION_12B;            /* 12-bit resolution for converted data */
    AdcHandle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;           /* Right-alignment for converted data */
    AdcHandle.Init.ScanConvMode          = DISABLE;                       /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
    AdcHandle.Init.EOCSelection          = ADC_EOC_SINGLE_CONV;           /* EOC flag picked-up to indicate conversion end */
    AdcHandle.Init.LowPowerAutoWait      = DISABLE;                       /* Auto-delayed conversion feature disabled */
    AdcHandle.Init.ContinuousConvMode    = DISABLE;                       /* Continuous mode disabled to have only 1 conversion at each conversion trig */
    //AdcHandle.Init.NbrOfConversion       = 1;                             /* Parameter discarded because sequencer is disabled */
    //AdcHandle.Init.DiscontinuousConvMode = DISABLE;                       /* Parameter discarded because sequencer is disabled */
    //AdcHandle.Init.NbrOfDiscConversion   = 1;                             /* Parameter discarded because sequencer is disabled */
    AdcHandle.Init.ExternalTrigConv      = ADC_SOFTWARE_START;            /* Software start to trig the 1st conversion manually, without external event */
    //AdcHandle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE; /* Parameter discarded because software trigger chosen */
    AdcHandle.Init.DMAContinuousRequests = DISABLE;                       /* DMA one-shot mode selected (not applied to this example) */
    AdcHandle.Init.Overrun               = ADC_OVR_DATA_OVERWRITTEN;      /* DR register is overwritten with the last conversion result in case of overrun */
    AdcHandle.Init.OversamplingMode      = DISABLE;                       /* No oversampling */

    if (HAL_ADC_Init(&AdcHandle) != HAL_OK)
    {
        resultSuccess = false;
    }

    if (HAL_ADCEx_Calibration_Start(&AdcHandle, ADC_SINGLE_ENDED) != HAL_OK)
    {
        resultSuccess = false;
    }

    return resultSuccess;
}

// discharge the sample and hold cap by making a measurement to GND
// charge the correct TOP plate to VCC
// Enable corresponding OPAMP
// the charged TOP pin configures to ADC input
// sample and hold the TOP plate (will discharge "parasitic" cap into sample and hold)
bool capacitive_getADCvalue(uint16_t *ADCrawMeas)
{
    bool resultSuccess = true;

    ADC_ChannelConfTypeDef sConfig;
    sConfig.Channel      = topADCchanTable[0];          /* Sampled channel number */
    sConfig.Rank         = ADC_REGULAR_RANK_1;          /* Rank of sampled channel number ADCx_CHANNEL */
    // ===> TODO need to check how long the OPAMP needs to stanilize
    sConfig.SamplingTime = ADC_SAMPLETIME_640CYCLES_5;  /* Sampling time (number of clock cycles unit) */
    sConfig.SingleDiff   = ADC_SINGLE_ENDED;            /* Single-ended input channel */
    sConfig.OffsetNumber = ADC_OFFSET_NONE;             /* No offset subtraction */

    if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
    {
        resultSuccess = false;
    }

    if (HAL_ADC_Start(&AdcHandle) != HAL_OK)
    {
        resultSuccess = false;
    }

    if (HAL_ADC_PollForConversion(&AdcHandle, 10) != HAL_OK || ADCrawMeas==NULL)
    {
        resultSuccess = false;
    }
    else
    {
        *ADCrawMeas = HAL_ADC_GetValue(&AdcHandle);
    }

    return resultSuccess;
}

