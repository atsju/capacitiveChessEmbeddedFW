
#ifndef LS013B7DH03
#define LS013B7DH03

/**
 * @brief init needed peripherals and clear screen
 * Set timer to output extcomin signal (EXTMODE=H)
 */
void sharpMemoryLCD_init(void);

/**
 * @brief Clear screen, put all pixels white
 */
void sharpMemoryLCD_clearScreen(void);

#endif
