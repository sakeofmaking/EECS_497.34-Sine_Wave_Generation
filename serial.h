/******************************************************************************
 * File Name:	serial.h
 * Program:		Project for Fundamentals of Embedded Systems class
 * Author:		Robert Weber
 * Purpose:		Contains external definitions for serial.c file. 
 *
 *  Date	Changed by:	Changes:
 * -------	-----------	-------------------------------------------------------
 * 18Feb02	R Weber		Original file.
 *  2Mar02  R Weber     Moved error codes to here.
 * 22May06	T Lill		Changed size of output string.
 ******************************************************************************/
#if !defined(SERIAL_H)		/* Prevents including this file multiple times */
#define SERIAL_H

#include <avr/pgmspace.h>

/* Maximum size of I/O strings */
#define MAX_OUT_STR_SIZE                250
#define MAX_IN_STR_SIZE                 10

/* Function Prototypes */
void SCIInitialize(void);
int  SCIWriteString(char *);
char SCIReadChar(void);
int  SCIWriteString_P(PGM_P Str_P);

#endif /* SERIAL_H */
