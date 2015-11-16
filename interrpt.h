/******************************************************************************
 * File Name:	Interrpt.h
 * Program:		Project for Fundamentals of Embedded Systems class
 * Author:		Robert Weber
 * Purpose:		Header file for Interrupts.c file.
 *
 *  Date	Changed by:	Changes:
 * -------	-----------	-------------------------------------------------------
 * 12Aug01	R Weber		Initial File
 * 27Aug01	R Weber		Added interrupt for signal generation.
 *  2Sep01	R Weber		Added trap for unused interrupts.
 *  3Mar02  R Weber     Moved definition of vectors to here.
 ******************************************************************************/
#if !defined(INTERRPT_H)		/* Prevents including this file multiple times */
#define INTERRPT_H
#include <avr/interrupt.h>

/* Interrupt prototypes */
void ISR_InitTimer0(void);
void ISR_InitTimer1(void);

#if !defined (SLOW_SINE)
void UpdateFreqCnt(void);
#endif

#endif /* INTERRPT_H */
