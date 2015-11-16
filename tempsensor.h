/******************************************************************************
 * File Name:	tempsense.h
 * Program:		Project for Fundamentals class
 * Author:		Robert Weber
 * Purpose:		Header file for tempsense.c file.
 *
 *  Date		Changed by:	Changes:
 * ---------	-----------	----------------------------------------------------
 * 01 Nov 10	T Lill		Converted program to ATmega2560
 ******************************************************************************/
#if !defined(TEMPSENSOR_H)		/* Prevents including this file multiple times */
#define TEMPSENSOR_H

/* Interrupt prototypes */
void InitAtoD(void);
int ReadTemperature(void);

#endif /* TEMPSENSOR_H */
