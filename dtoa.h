/******************************************************************************
 * File Name:	dtoa.h
 * Program:		Project for Real-Time Embedded Systems Programming class
 * Author:		Robert Weber
 * Purpose:		Header file for dtoa.c file. 
 *
 *  Date	Changed by:	Changes:
 * -------	-----------	-------------------------------------------------------
 * 17Mar02	R Weber		Original file.
 * 24Feb03  R Weber     Made WriteDtoASample use a ptr, rather than a value, to
 *                      increase speed.
 ******************************************************************************/
#if !defined(DTOA_H)	/* Prevents including this file multiple times */
#define DTOA_H

/* Function Prototypes */
void InitDtoA(void);
void WriteDtoASample(unsigned int);

#endif /* DTOA_H */
