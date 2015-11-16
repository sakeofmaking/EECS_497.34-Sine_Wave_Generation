/******************************************************************************
 * File Name:	lcd.h
 * Program:		Project for Real-Time Embedded Systems Programming class
 * Author:		Robert Weber
 * Purpose:		Header file for lcd.c file.
 ******************************************************************************/
#if !defined(LCD_H)		/* Prevents including this file multiple times */
#define LCD_H

/* Interrupt prototypes */
void InitLCD(void);
void LCDWrite(char,             // LCD character
               unsigned char);  // Position (0 - 5)

#endif /* LCD_H */
