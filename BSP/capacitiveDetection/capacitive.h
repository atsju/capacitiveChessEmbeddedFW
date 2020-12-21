#ifndef CAPACITIVE_H
#define CAPACITIVE_H

#include <stdint.h>

/**
 * @brief Init peripherals required for capacitivement measurements
 */
void capacitive_init(void);

/**
 * @brief A first debug/development function to make tests
 *
 * @return uint16_t the raw ADC value for a group of cells
 */
uint16_t capacitive_getADCvalue(void);

#endif
