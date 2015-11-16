/******************************************************************************
 * File Name:	errors.c
 * Program:		Project for Fundamentals of Embedded Systems class
 * Author:		Robert Weber
 * Purpose:		Contains error-handling functions
 *
 *  Date	 By:		Changes:
 * --------- ----------	-------------------------------------------------------
 * 21 Jul 02 R Weber	Initial file
 * 18 Feb 04 T Lill		modified for Winter 04 session
 * 09 May 05 T Lill		Removed deprecated functions.
 ******************************************************************************/
#include "lib.h"
#include "errors.h"

eErrorType SystemError = NO_ERROR;

/*****************************************************************************
 * Error handler
 *****************************************************************************/
void ReportError(eErrorType iError)
{
	// Only allow 1 error to be logged at one time
	if (SystemError == NO_ERROR)
	{    /* Record error and set error LED */
		SystemError = iError;
		CLEAR_BIT(PORTB, ERROR_LED_BIT);
	}
}

void ClearError(void)
{
	/* Record error and set error LED */
	SystemError = NO_ERROR;
	SET_BIT(PORTB, ERROR_LED_BIT);
}

eErrorType GetError(void)
{
	/* Record error and set error LED */
	return SystemError;
}
