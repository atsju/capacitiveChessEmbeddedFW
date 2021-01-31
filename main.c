#include <stm32l4xx_hal.h>

#include "LS013B7DH03.h"
#include "buttons.h"
#include "capacitive.h"
#include "led.h"
#include "SMPS.h"
#include "SEGGER_RTT.h"
#include "arm_math.h"
#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>

#define NB_ADC_MEAS_AVG_CALIB (64)
#define NB_ADC_MEAS_AVG_DETECT (8)
#define MAX_STD_DEV_VARIATION (3.0f)
#define MIN_MEAN_VARIATION  (10u)
#define MAX_MEAN_VARIATION  (100u)

static void SystemClockHSI_Config(void);
static void Error_Handler(void);
static void mainTask(void *arg);

// made global to avoid stack usage with freeRTOS
float calibRawMeas[NB_CAP_CHAN][NB_ADC_MEAS_AVG_CALIB];
float calibsMean[NB_CAP_CHAN];
float calibsStdDev[NB_CAP_CHAN];
float detectRawMeas[NB_CAP_CHAN][NB_ADC_MEAS_AVG_DETECT];
float detectMean[NB_CAP_CHAN];
float detectStdDev[NB_CAP_CHAN];

static void mainTask(void *arg)
{
    BSP_SMPS_Init(0);
    // connect VDD12 after 100ms
    BSP_SMPS_Supply_Enable(100,0);
    SystemClockHSI_Config();
    // choose internal voltage scale 2 so that external VDD12 is effectively used
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE2);

    vTaskDelay(1);
    sharpMemoryLCD_init();
    vTaskDelay(100);

    led_init();
    led_blinkTest();

    buttons_init();
    capacitive_init();
    vTaskDelay(500); //ensure all OPAMPS are ON

    SEGGER_RTT_WriteString(0, "-- Board startup --\r\n");

    sharpMemoryLCD_printTextLine(0, "calibrating", 11);
    vTaskDelay(10);

    uint16_t rawADC;


    for(uint8_t i=0; i<NB_ADC_MEAS_AVG_CALIB; i++)
    {
        for(uint8_t chan=0; chan<NB_CAP_CHAN; chan++)
        {
            capacitive_getADCvalue(chan, &rawADC);
            calibRawMeas[chan][i] = rawADC;
        }
    }

    // compute mean values and standard deviation
    for(uint8_t chan=0; chan<NB_CAP_CHAN; chan++)
    {
        arm_mean_f32(calibRawMeas[chan], NB_ADC_MEAS_AVG_CALIB, &(calibsMean[chan]));
        arm_std_f32(calibRawMeas[chan], NB_ADC_MEAS_AVG_CALIB, &(calibsStdDev[chan]));
    }





    vTaskDelay(1);

    SEGGER_RTT_WriteString(0, "calib values\r\n");
    SEGGER_RTT_WriteString(0, "1    2    3    4    5    6    7    8    a    b    c    d    e    f    g    h\r\n");
    for(uint8_t i=0; i<NB_ADC_MEAS_AVG_CALIB; i++)
    {
        for(uint8_t chan=0; chan<NB_CAP_CHAN; chan++)
        {
            SEGGER_RTT_printf(0, "%d ",(uint16_t)calibRawMeas[chan][i]);
        }
        SEGGER_RTT_WriteString(0, "\r\n");
        vTaskDelay(1);
    }
    SEGGER_RTT_WriteString(0, "mean values\r\n");
    for(uint8_t chan=0; chan<NB_CAP_CHAN; chan++)
    {
        SEGGER_RTT_printf(0, "%d ",(uint16_t)calibsMean[chan]);
    }
    SEGGER_RTT_WriteString(0, "\r\n");
    vTaskDelay(1);
    SEGGER_RTT_WriteString(0, "std dev values\r\n");
    for(uint8_t chan=0; chan<NB_CAP_CHAN; chan++)
    {
        SEGGER_RTT_printf(0, "%d ",(uint16_t)calibsStdDev[chan]);
        calibsStdDev[chan] *= MAX_STD_DEV_VARIATION;
    }
    SEGGER_RTT_WriteString(0, "\r\n");
    vTaskDelay(1);



    char printBuffer[11];

    for(uint8_t i=0; i<NB_CAP_CHAN/2; i++)
    {
        // one line and one colums value per screen line
        sprintf(printBuffer, "%1i %4i %4i", i, (uint16_t)calibsMean[i], (uint16_t)calibsMean[i+NB_CAP_CHAN/2]);
        sharpMemoryLCD_printTextLine(i, printBuffer, 11);
        vTaskDelay(10);
    }
    vTaskDelay(500);

    TaskHandle_t led_squareTaskHandle;
    xTaskCreate(led_squareTask, "led_squareTask", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &led_squareTaskHandle);

    SEGGER_RTT_WriteString(0, "std dev values real measurements\r\n");
    while(1)
    {
        for(uint8_t i=0; i<NB_ADC_MEAS_AVG_DETECT; i++)
        {
            for(uint8_t chan=0; chan<NB_CAP_CHAN; chan++)
            {
                capacitive_getADCvalue(chan, &rawADC);
                detectRawMeas[chan][i] = rawADC;
            }
        }


        // compute mean values and standard deviation
        for(uint8_t chan=0; chan<NB_CAP_CHAN; chan++)
        {
            arm_mean_f32(detectRawMeas[chan], NB_ADC_MEAS_AVG_DETECT, &(detectMean[chan]));
            arm_std_f32(detectRawMeas[chan], NB_ADC_MEAS_AVG_DETECT, &(detectStdDev[chan]));
        }

        for(uint8_t chan=0; chan<NB_CAP_CHAN; chan++)
        {
            SEGGER_RTT_printf(0, "%d ",(uint16_t)detectStdDev[chan]);
        }
        SEGGER_RTT_WriteString(0, "\r\n");


        for(uint8_t i=0; i<NB_CAP_CHAN/2; i++)
        {
            // one line and one colums value per screen line
            sprintf(printBuffer, "%1i %4i %4i", i, (int16_t)(detectMean[i]-calibsMean[i]), (int16_t)(detectMean[i+NB_CAP_CHAN/2]-calibsMean[i+NB_CAP_CHAN/2]));
            sharpMemoryLCD_printTextLine(i, printBuffer, 11);
            vTaskDelay(10);
        }

        bool allStdDevAcceptable = true;
        uint8_t nbAcceptableCol = 0;
        uint8_t nbAcceptableRow = 0;
        uint8_t detectedRow, detectedCol;
        for(uint8_t i=0; i<NB_CAP_CHAN/2; i++)
        {
            if(detectStdDev[i]>calibsStdDev[i] || detectStdDev[i+NB_CAP_CHAN/2]>calibsStdDev[i+NB_CAP_CHAN/2])
            {
                allStdDevAcceptable = false;
            }

            if((detectMean[i]-calibsMean[i]) >= MIN_MEAN_VARIATION && (detectMean[i]-calibsMean[i]) <= MAX_MEAN_VARIATION)
            {
                nbAcceptableRow++;
                detectedRow = i;
            }
            if((detectMean[i+NB_CAP_CHAN/2]-calibsMean[i+NB_CAP_CHAN/2]) >= MIN_MEAN_VARIATION && (detectMean[i+NB_CAP_CHAN/2]-calibsMean[i+NB_CAP_CHAN/2]) <= MAX_MEAN_VARIATION)
            {
                nbAcceptableCol++;
                detectedCol = i;
            }
        }
        if(nbAcceptableRow==1 && nbAcceptableCol==1 && allStdDevAcceptable)
        {
            // light up corresponding cell
            xSemaphoreTake(led_squareTaskInfo.led_STI_mutexHandle, portMAX_DELAY);
            led_squareTaskInfo.led_STI_row = detectedRow;
            led_squareTaskInfo.led_STI_col = detectedCol;
            led_squareTaskInfo.led_STI_isOn = true;
            xSemaphoreGive(led_squareTaskInfo.led_STI_mutexHandle);
            vTaskResume(led_squareTaskHandle);
        }
        else
        {
            // do not light up anything
            xSemaphoreTake(led_squareTaskInfo.led_STI_mutexHandle, portMAX_DELAY);
            led_squareTaskInfo.led_STI_isOn = false;
            xSemaphoreGive(led_squareTaskInfo.led_STI_mutexHandle);
        }

    }
}

int main(void)
{
    HAL_Init();

    led_squareTaskInfo.led_STI_mutexHandle = xSemaphoreCreateMutex();
    // wait for mutex infinite time
    //xSemaphoreTake(led_squareTaskInfo.led_STI_mutexHandle, portMAX_DELAY);
    //led_squareTaskInfo.led_STI_row = 5;
    //led_squareTaskInfo.led_STI_col = 2;
    //led_squareTaskInfo.led_STI_isOn = true;
    //xSemaphoreGive(led_squareTaskInfo.led_STI_mutexHandle);

    xTaskCreate(mainTask, "mainTask", configMINIMAL_STACK_SIZE*8, NULL, tskIDLE_PRIORITY + 1, NULL);
    vTaskStartScheduler();

    while(1);
    return 0;
}



/**
  * @brief  Switch the PLL source from MSI  to HSI, and select the PLL as SYSCLK
  *         source.
  *         The system Clock is configured as follows :
  *            System Clock source            = PLL (HSI)
  *            SYSCLK(Hz)                     = 120000000
  *            HCLK(Hz)                       = 120000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            MSI Frequency(Hz)              = 4000000
  *            HSI Frequency(Hz)              = 16000000
  *            PLLM                           = 2
  *            PLLN                           = 30
  *            PLLP                           = 7
  *            PLLQ                           = 4
  *            PLLR                           = 2
  *            Flash Latency(WS)              = 4
  * @param  None
  * @retval None
  */
static void SystemClockHSI_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  /* -1- Select MSI as system clock source to allow modification of the PLL configuration */
  RCC_ClkInitStruct.ClockType       = RCC_CLOCKTYPE_SYSCLK;
  RCC_ClkInitStruct.SYSCLKSource    = RCC_SYSCLKSOURCE_MSI;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* -2- Enable voltage range 1 boost mode for frequency above 80 Mhz */
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);
  __HAL_RCC_PWR_CLK_DISABLE();

  /* -3- Enable HSI Oscillator, select it as PLL source and finally activate the PLL */
  RCC_OscInitStruct.OscillatorType       = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState             = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue  = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState         = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource        = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM             = 2;
  RCC_OscInitStruct.PLL.PLLN             = 30;
  RCC_OscInitStruct.PLL.PLLP             = 7;
  RCC_OscInitStruct.PLL.PLLQ             = 4;
  RCC_OscInitStruct.PLL.PLLR             = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* -4- To avoid undershoot due to maximum frequency, select PLL as system clock source */
  /* with AHB prescaler divider 2 as first step */
  RCC_ClkInitStruct.ClockType       = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource    = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider  = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider  = RCC_HCLK_DIV1;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* -5- AHB prescaler divider at 1 as second step */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* -6- Optional: Disable MSI Oscillator */
  RCC_OscInitStruct.OscillatorType  = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState        = RCC_MSI_OFF;
  RCC_OscInitStruct.PLL.PLLState    = RCC_PLL_NONE;  /* No update on PLL */
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
}


static void Error_Handler(void)
{
    while(1)
    {
        ;
    }
}

void assert_failed(uint8_t *file, uint32_t line)
{
    sharpMemoryLCD_printTextLine(0,"  ASSERT", 8);
    // TODO print file and line
    while(1)
    {
        ;
    }
}
