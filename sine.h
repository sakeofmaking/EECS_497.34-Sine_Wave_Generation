/******************************************************************************
 * File Name:	sine.h
 * Program:		Project for Real-Time Embedded Systems Programming class
 * Author:		Robert Weber
 * Purpose:		Header file for sine.c file.
 *
 *  Date	Changed by:	Changes:
 * -------	-----------	-------------------------------------------------------
 * 30Apr02	R Weber		Initial File
 ******************************************************************************/
#if !defined(SINE_H)		/* Prevents including this file multiple times */
#define SINE_H

#include "errors.h"

/* Number of samples per period of the Output signal */
#define SAMPLES_PER_PERIOD		100

/******************************************************************************
 * Define Macros for getting the desired or actual voltage or frequency.
 ******************************************************************************/
extern volatile unsigned int FreqDesired, VoltDesired, FreqActual, VoltActual;
#define GET_FREQ_DESIRED()      FreqDesired
#define GET_VOLT_DESIRED()      VoltDesired
#define GET_FREQ_ACTUAL()       FreqActual
#define GET_VOLT_ACTUAL()       VoltActual

// Access functions for sine wave
eErrorType SetFreq(unsigned int);
eErrorType SetVolt(unsigned int);
void initSine(void);
void CalcSineValues(unsigned int);
void UpdateSignal(void);

#endif
