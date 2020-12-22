#ifndef LED_H
#define LED_H

#include <stdbool.h>

/**
 * @brief Init needed peripherals for LED usage
 */
void led_init(void);

/**
 * @brief Light every LED one after other
 */
void led_blinkTest(void);

#endif
