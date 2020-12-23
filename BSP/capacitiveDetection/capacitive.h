#ifndef CAPACITIVE_H
#define CAPACITIVE_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Init peripherals required for capacitivement measurements
 *
 * @return true if init went fine
 */
bool capacitive_init(void);

/**
 * @brief A first debug/development function to make tests
 *
 * @param ADCrawMeas pointer to output the raw ADC value for a group of cells
 * @return true if measurement went fine
 */
bool capacitive_getADCvalue(uint16_t *ADCrawMeas);


#endif
