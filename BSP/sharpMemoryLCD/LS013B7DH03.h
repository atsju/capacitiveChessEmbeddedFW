
#ifndef LS013B7DH03
#define LS013B7DH03

#include <stdint.h>

/**
 * @brief init needed peripherals and clear screen
 * Set timer to output extcomin signal (EXTMODE=H)
 */
void sharpMemoryLCD_init(void);

/**
 * @brief Clear screen, put all pixels white
 */
void sharpMemoryLCD_clearScreen(void);

/**
 * @brief Print text to screen on given line
 * 16 pixels per line, each char is 11 pixel long giving 11 char per line.
 * Whole line is erased when printing even if string is shorter.
 * Other line stay untouched.
 *
 * @param line line number from 0 to 7
 * @param text pointer to the string to print
 * @param nbChar length of the string to display
 */
void sharpMemoryLCD_printTextLine(uint8_t line, const char *text, uint8_t nbChar);


#endif
