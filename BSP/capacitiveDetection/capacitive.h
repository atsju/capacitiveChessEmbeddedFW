#ifndef CAPACITIVE_H
#define CAPACITIVE_H

#include <stdint.h>
#include <stdbool.h>

/** 8 lines plus 8 columns */
#define NB_CAP_CHAN (16)

/**
 * @brief Init peripherals required for capacitivement measurements
 *
 * @return true if init went fine
 */
bool capacitive_init(void);

/**
 * @brief A first debug/development function to make tests
 *
 * @param capChannel the channel where to read capacity
 * @param ADCrawMeas pointer to output the raw ADC value for a group of cells
 * @return true if measurement went fine
 */
bool capacitive_getADCvalue(uint8_t capChannel, uint16_t *ADCrawMeas);


#endif
