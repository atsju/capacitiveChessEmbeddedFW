#ifndef SMPS_H
#define SMPS_H

#include <stdint.h>

#define SMPS_OK     0
#define SMPS_KO     1


/**
  * @brief  Initialize the external SMPS component
  * @param  VoltageRange: Select operating SMPS supply
  *           @arg DCDC_AND_BOARD_DEPENDENT
  * @note   VoltageRange is not used with all boards. When not
  *         used, resort to PWR_REGULATOR_VOLTAGE_SCALE2 by default.
  * @retval SMPS status
  */
uint32_t BSP_SMPS_Init(uint32_t VoltageRange);

/**
  * @brief  DeInitialize the external SMPS component
  * @note   Low power consumption GPIO settings
  * @retval SMPS status
  */
uint32_t BSP_SMPS_DeInit(void);

/**
  * @brief  Enable the external SMPS component
  * @param  Delay: delay in ms after enable
  * @param  Power_Good_Check: Enable Power good check
  * @note   Power_Good_Check is not used with all external
  *         SMPS components
  * @retval SMPS status
  *           @arg SMPS_OK: SMPS ENABLE OK
  *           @arg SMPS_KO: POWER GOOD CHECK FAILS
  */
uint32_t BSP_SMPS_Enable(uint32_t Delay, uint32_t Power_Good_Check);

/**
  * @brief  Disable the external SMPS component
  * @note   SMPS SWITCH should be disabled first !
  * @retval SMPS status
  *           @arg SMPS_OK: SMPS DISABLE OK - DONE
  *           @arg SMPS_KO: POWER GOOD CHECK FAILS
  *
  */
uint32_t BSP_SMPS_Disable(void);

/**
  * @brief  Enable the external SMPS SWITCH component
  * @param  Delay: delay in ms before SMPS SWITCH ENABLE
  * @param  Power_Good_Check: Enable Power good check
  * @note   Power_Good_Check is not used with all boards
  * @retval SMPS status
  *           @arg SMPS_OK: SMPS ENABLE OK
  *           @arg SMPS_KO: POWER GOOD CHECK FAILS
  */
uint32_t BSP_SMPS_Supply_Enable(uint32_t Delay, uint32_t Power_Good_Check);

/**
  * @brief  Disable the external SMPS SWITCH component
  * @retval SMPS status
  *           @arg SMPS_OK: SMPS SWITCH DISABLE OK
  */
uint32_t BSP_SMPS_Supply_Disable(void);

#endif
