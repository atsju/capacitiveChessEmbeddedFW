#include "LS013B7DH03.h"

#include <stdbool.h>
#include <stm32l4xx_hal.h>
#include "fonts.h"

/** Update pixel data */
#define CMD_DATA_UPDATE (0x01)
/** Clear internal memory */
#define CMD_CLEAR       (0x04)

#define SCREEN_HEIGHT   (128)
#define SCREEN_LENGTH   (128)

SPI_HandleTypeDef SpiHandle;

/**
 * @brief set or unset SS (Slave Select) pin
 *
 * @param ss slave select if  true, unselect if false
 */
static void LCDslaveSelect(bool ss)
{
    if(ss)
    {
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, GPIO_PIN_RESET);
    }
}

/**
 * @brief set DISP pin
 *
 * @param en enable true to desplay info from memory
 */
static void LCDdisplayEnable(bool en)
{
    if(en)
    {
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, GPIO_PIN_RESET);
    }
}


void sharpMemoryLCD_init(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_TIM3_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    /* Pin B0 is EXTCOMIN driven by TIM3_CH1 */
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_1;
    /* Pin D1 is SPI2 CLK */
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_4;
    /* Pin D4 is SPI2 MOSI */
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    /* Pin D0 is SPI2 NSS */
    /* It is not driven by SPI2 peripheral because need to be enabled with high level */
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    LCDslaveSelect(false);

    GPIO_InitStruct.Pin = GPIO_PIN_6;
    /* Pin D6 is DISP (for enabling display) */
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    LCDdisplayEnable(false);


    /* Output a 60kHz signal to EXTCOMIN pin */
    TIM_HandleTypeDef TimHandle;
    TimHandle.Instance = TIM3;
    TimHandle.Init.Period        = 65535;
    TimHandle.Init.Prescaler     = 60000; // counter is counting at 120M/60k => 2kHz
    TimHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV4; // 500Hz
    TimHandle.Init.CounterMode   = TIM_COUNTERMODE_UP;
    TimHandle.Init.Period        = 8; //500/8 => 60Hz period

    if (HAL_TIM_PWM_Init(&TimHandle) != HAL_OK)
    {
        assert_param(false);
    }

    TIM_OC_InitTypeDef sConfig;
    sConfig.OCMode       = TIM_OCMODE_PWM1;
    sConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
    sConfig.OCFastMode   = TIM_OCFAST_DISABLE;
    sConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
    sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;
    sConfig.Pulse = 1;
    if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_1) != HAL_OK)
    {
        assert_param(false);
    }

    if (HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1) != HAL_OK)
    {
        assert_param(false);
    }

    SpiHandle.Instance               = SPI2;
    SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256; // ===> TODO set correct baudrate to 1Mbps
    SpiHandle.Init.Direction         = SPI_DIRECTION_1LINE;
    SpiHandle.Init.CLKPhase          = SPI_PHASE_1EDGE;
    SpiHandle.Init.CLKPolarity       = SPI_POLARITY_LOW;
    SpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
    SpiHandle.Init.FirstBit          = SPI_FIRSTBIT_LSB;
    SpiHandle.Init.TIMode            = SPI_TIMODE_DISABLE;
    SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
    SpiHandle.Init.NSS               = SPI_NSS_SOFT;
    SpiHandle.Init.NSSPMode          = SPI_NSS_PULSE_DISABLE;
    SpiHandle.Init.Mode              = SPI_MODE_MASTER;
    if(HAL_SPI_Init(&SpiHandle) != HAL_OK)
    {
        assert_param(false);
    }

    sharpMemoryLCD_clearScreen();

    LCDdisplayEnable(true);
}


void sharpMemoryLCD_clearScreen(void)
{
    /* To clear sreen send CMD_CLEAR (3bit) + 16 additional dummy bits */
    uint8_t buffer[] = {CMD_CLEAR, 0x00};
    LCDslaveSelect(true);
    if(HAL_SPI_Transmit(&SpiHandle, buffer, sizeof(buffer), 1000) != HAL_OK)
    {
        assert_param(false);
    }
    LCDslaveSelect(false);
}


void sharpMemoryLCD_printTextLine(uint8_t line, const char *text, uint8_t nbChar)
{
    uint8_t buffer[Font16.Height*SCREEN_LENGTH/Font16.Width];
    // this init could be optimized later by only init the end of line + using SIMD
    for(uint16_t i = 0; i<(Font16.Height*SCREEN_LENGTH/Font16.Width); i++)
    {
        buffer[i] = 0;
    }

    if(line < (SCREEN_LENGTH/Font16.Width))
    {
        uint16_t sreenX=0;
        uint16_t XbytesPerFontChar = (Font16.Width+7)/8;
        uint16_t bytesPerFontChar = XbytesPerFontChar*Font16.Height;
        //end display at first character out of ascii (most common will be \0) or out of screen
        for(uint8_t c=0; (c<nbChar) && (' '<=text[c]) && (text[c]<='~') && (c<(SCREEN_LENGTH/Font16.Width)); c++)
        {
            // here SIMD instructions could be used to process 4 lines in one instruction
            for(uint8_t x=0; x<Font16.Width; x++)
            {
                for(uint8_t y=0; y<Font16.Height; y++)
                {
                    uint16_t currentByteInFont = bytesPerFontChar*(text[c]-' ') + XbytesPerFontChar*y + x/8;
                    // could be optimized to do several pixel at one time to go until byte alignement
                    buffer[y*SCREEN_LENGTH + sreenX/8] = (1<<(sreenX % 8) & Font16.table[currentByteInFont]);
                }
                sreenX++;
            }
        }

        LCDslaveSelect(true);
        uint8_t cmd_buffer[2] = {CMD_DATA_UPDATE, 0x00};
        // send "data update" command
        if(HAL_SPI_Transmit(&SpiHandle, cmd_buffer, sizeof(cmd_buffer), 1000) != HAL_OK)
        {
            assert_param(false);
        }
        // send the lines to update
        if(HAL_SPI_Transmit(&SpiHandle, buffer, sizeof(buffer), 1000) != HAL_OK)
        {
            assert_param(false);
        }
        // send 16 dummy bits
        if(HAL_SPI_Transmit(&SpiHandle, cmd_buffer, sizeof(cmd_buffer), 1000) != HAL_OK)
        {
            assert_param(false);
        }
        LCDslaveSelect(false);
    }
}
