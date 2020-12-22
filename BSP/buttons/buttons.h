#ifndef BUTTONS_H
#define BUTTONS_H

#include <stdbool.h>

typedef enum button{
    BUTTON_CENTER,
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_LEFT,
    BUTTON_RIGHT,
    NB_BUTTONS
} button_e;

/**
 * @brief Init peripherals needed for buttons
 *
 */
void buttons_init(void);

/**
 * @brief return the state of a button
 *
 * @param button the button to read
 * @return true when button is pressed
 */
bool buttons_isPressed(button_e button);


#endif
