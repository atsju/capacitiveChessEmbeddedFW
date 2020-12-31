#ifndef SMPS_H
#define SMPS_H

#include <stdint.h>

#define SMPS_OK     0
#define SMPS_KO     1

uint32_t BSP_SMPS_Init(uint32_t VoltageRange);
uint32_t BSP_SMPS_DeInit(void);
uint32_t BSP_SMPS_Enable(uint32_t Delay, uint32_t Power_Good_Check);
uint32_t BSP_SMPS_Disable(void);
uint32_t BSP_SMPS_Supply_Enable(uint32_t Delay, uint32_t Power_Good_Check);
uint32_t BSP_SMPS_Supply_Disable(void);

#endif
