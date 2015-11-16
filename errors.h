/******************************************************************************
 * File Name:	errors.h
 * Program:		Project for Fundamentals of Embedded Systems class
 * Author:		Robert Weber
 * Purpose:		Contains enumeration for error values. 
 *
 *  Date	Changed by:	Changes:
 * -------	-----------	-------------------------------------------------------
 * 16Mar02	R Weber		Original file.
 * 18Feb04	T Lill		modified for Winter 04 session
 * 18May08	T Lill		deleted redundant error symbol.
 ******************************************************************************/
#if !defined(ERRORS_H)		/* Prevents including this file multiple times */
#define ERRORS_H

typedef enum
{
    NO_ERROR = 0,

    // Overrun faults
    SLOW_TASK_OVERRUN,      // 1
    MEDIUM_TASK_OVERRUN,
    FAST_TASK_OVERRUN,

    // SCI faults
    SCI_RX_BUFFER_OVERFLOW, // 4
    SCI_TX_BUFFER_OVERFLOW,
	SCI_RX_FRAME,
	SCI_RX_DATA_OVERRUN,
	SCI_RX_PARITY,

    /* SPI Faults */
    SPI_WRITE_COLLISION,    // 10
    SPI_MODE_FAULT,
    SPI_PREV_TX_INCOMPLETE,

    // LCD faults
    LCD_INVALID_CHAR,       // 13
    LCD_INVALID_POS,
    
    /* Unused Interrupts */
    UNUSED_INTERRUPT,

	// Parameter errors
    INVALID_PARAMETER,
    PARAMETER_OUT_OF_RANGE
} eErrorType;

/* Function Prototypes */
void ReportError(eErrorType);
void ClearError(void);
eErrorType GetError(void);

#endif /* ERRORS_H */
