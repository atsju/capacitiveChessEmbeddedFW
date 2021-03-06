
#ifndef LS013B7DH03_H
#define LS013B7DH03_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief init needed peripherals and clear screen
 * Set timer to output extcomin signal (EXTMODE=H)
 */
bool sharpMemoryLCD_init(void);

/**
 * @brief Clear screen, put all pixels white
 */
bool sharpMemoryLCD_clearScreen(void);

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
bool sharpMemoryLCD_printTextLine(uint8_t line, const char *text, uint8_t nbChar);


#endif
