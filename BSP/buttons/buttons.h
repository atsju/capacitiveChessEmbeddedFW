#ifndef BUTTONS_H
#define BUTTONS_H

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

#endif
