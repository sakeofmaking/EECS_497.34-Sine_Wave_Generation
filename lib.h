/******************************************************************************
 * File Name:	Lib.h
 * Program:		Project for Real-Time Embedded Systems Programming class
 * Author:		Robert Weber
 * Purpose:		Header file for lib.c file.
 *
 *  Date	Changed by:	Changes:
 * -------	-----------	-------------------------------------------------------
 * 29Sep05	T Lill		Added more clock speeds.  Added macros to replace 
 *						deprecated cbi, sbi functions. 
 * 05Dec07	T Lill		Deleted OSC_FREQ, use F_CPU from makefile instead.
 *						Removed BYTE and WORD definitions.  Updated _itoa and 
 *						_atoi to allow negative numbers.  Added warning about 
 *						SET_BIT and optimization.
 ******************************************************************************/
#if !defined(LIB_H)		/* Prevents including this file multiple times */
#define LIB_H

#include <avr/io.h>

#define STRLEN 20			/* Maximum string length for conversions */

/* Define memory mapping for commonly used registers */

// Define Port B uses
#define HEARTBEAT_LED_BIT				0
// Port bit to use for chip select
#define D2A_CS_BIT						4
#define ERROR_LED_BIT					5
#define TIMING_BIT						6

/* Memory types */
typedef unsigned char *MEMPTR;

typedef enum {
    FALSE = 0,
    TRUE = 1
} eBooleanType;

/* Macros replacing deprecated functions. */
#define CLEAR_BIT(port, bit) port = (port & (~(1 << bit)))
/*
 * WARNING:  SET_BIT may not work properly if the register
 *		is an I/O PINx register.  It works in this case if
 *		optimization is set to -Os, but it fails if the
 *		optimization is set to -O0.  It has not been tested
 *		for other optimization levels.
 */
#define SET_BIT(port, bit) port = (port | (1 << bit))
// Macro to toggle bits. 
#define tbi(sfr, bit) (_SFR_BYTE(sfr) ^= _BV(bit))

/* Function Prototypes */
void _itoa(char **,			// Buffer to store converted string into
			int,			// Value to convert
			int);			// Radix (only 10 or 16 allowed)
			
int _atoi(char *, 			// String to convert
			int);			// Radix (only 10 or 16 allowed)

#endif /* LIB_H */
