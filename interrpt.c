/******************************************************************************
 * File Name:	Interrpt.c
 * Program:		Temperature program for Fundamentals of Embedded Systems
 * Author:		Robert Weber
 * Purpose:		This file contains all interrupt functions implemented in C.
 *
 *  Date		Changed by	Changes
 * ---------	-----------	-------------------------------------------------------
 * 21 Jul 02	R Weber		Original file.
 * 10 Feb 04	T Lill		Removed unused variable in SIG_OUTPUT_COMPARE_0 ISR.  
 *							Modified for class project.
 * 17 Nov 04	T Lill		Editorial changes only.
 * 03 May 05	T Lill		Removed deprecated functions.
 * 05 Apr 06	T Lill		Replaced obsolete signal.h header with interrupt.h,
 *							replaced INTERRUPT macro with SIGNAL.
 * 25 May 06	T Lill		Replaced SIGNAL with ISR.
 * 31 Oct 07 	T Lill		Deleted OSC_FREQ symbol, replace with F_CPU in makefile.
 * 17 Oct 10	T Lill		Modified program for ATmega2560 processor.
 ******************************************************************************/

/********************************* Includes ***********************************/
#include <avr/io.h>
#include <avr/interrupt.h>

#include "interrpt.h"
#include "heartbeat.h"
#include "errors.h"
#include "lib.h"
#include "menu.h"
#include "serial.h"

// Define execution times for interrupts
/* Update rate of Main Timer in seconds */
#define TIMER0_SCALER			1024

// "Medium" thread time set to every 25 mSecs.
#define TIMER0_TIME             0.025
/* The following calculates how many counts are needed to achieve the desired
 * rate. There is no checking on the resulting value, which must be less than
 * 255. 
 * For F_CPU = 8 MHz, this value works out to be 195, which is OK. 
 */
#define TIMER0_CNT				((TIMER0_TIME * F_CPU)/TIMER0_SCALER)

/* Define times for the various medium-thread tasks, based on how often
 * the thread's interrupt occurs. */
#define HEARTBEAT_TIME          0.5     /* seconds between toggling of LED */
#define MENU_TIME               0.1     /* seconds between running menu */
#define MAX_MEDIUM_THREAD_TIME  5       /* max # of mSecs for any task */

extern void UpdateSignal();
extern int FreqDesired;

/******************************************************************************
 * global variables
 *****************************************************************************/

/******************************************************************************
 * Function prototypes
 *****************************************************************************/

/******************************************************************************
 * Initialization routines for used interrupts
 *****************************************************************************/
void ISR_InitTimer0()
{
	/*------------------ Set TCCR0A values --------------------------
	 * Bit 7: COM0A1 = 0  Normal port operation. No Output Compare
	 *     6: COM0A0 = 0
	 *     5: COM0B1 = 0  
	 *     4: COM0B0 = 0
	 *     3: unused = 0  
	 *     2: unused = 0
	 *     1: WGM01  = 1  Sets Waveform Generation mode to CTC
	 *     0: WGM00  = 0
	 */
	TCCR0A  = _BV(WGM01);

	/*------------------ Set TCCR0B values --------------------------
	 * Bit 7: FOCOA  = 0  Disable Force Output Compare
	 *     6: FOCOB  = 0
	 *     5: unused = 0
	 *     4: unused = 0
	 *     3: WGM02  = 0  Sets Waveform Generation mode to CTC
	 *     2: CS02   = 1  Sets prescaler to 1024, I/O clock
	 *     1: CS01   = 0
	 *     0: CS00   = 1
	 */
	TCCR0B = _BV(CS02) | _BV(CS00);
   
	// Load Compare values for timer 0
	OCR0A = TIMER0_CNT;
	OCR0B = 0;		// not using this feature

	/*----------------- Set TIMSK0 values -------------------------
	 * Bit(s) 7-3: Unused = 0
	 *          2: OCIE0B = 0  Disable timer compare interrupt B
	 *          1: OCIE0A = 1  Enable timer compare interrupt A
	 *          0: TOIE0  = 0  Disable overflow interrupt
     */
	TIMSK0 = _BV(OCIE0A);

	// Optional: Initialize timer to 0
	TCNT0 = 0;
}

#if !defined (SLOW_SINE)
void ISR_InitTimer1()
{
	// Set timer 1 to CTC mode
	// Set prescaler to 1
	TCCR1B  = _BV(WGM12) | _BV(CS10);
   
	// Initialize compare register for freq desired
	OCR1A = F_CPU / (FreqDesired * 100);

	// Enable overflow interrupt A
	TIMSK1 = _BV(OCIE1A);

	// Initialize timer 1 to 0
	TCNT1 = 0;

}
#endif /* !SLOW_SINE */

/******************************************************************************
 * Interrupt handlers
 *****************************************************************************/
/* 
 * This is the ISR that handles Timer 0 Compare interrupts.
 */
ISR(TIMER0_COMPA_vect)
{
	static unsigned int uiMedThreadCount = 1;		
	static eBooleanType bMedThreadInProgress = FALSE;

	// Check for overrun. 
	if (bMedThreadInProgress == TRUE)
	{	/* Haven't finished previous time through
		 * Set error, and don't do any more tasks. Just return. */
		ReportError(MEDIUM_TASK_OVERRUN);
	}
	else
	{   // No overrun. Continue tasks
		bMedThreadInProgress = TRUE;

	/**************************************************************************
	 * Update medium-thread main timer
	 **************************************************************************/
		if (uiMedThreadCount >= (unsigned int)
			(MAX_MEDIUM_THREAD_TIME/TIMER0_TIME))
		{	/* Reset the interrupt counter */
			uiMedThreadCount = 1;
		}
		else
		{
			++uiMedThreadCount;
		}

	/**************************************************************************
	 * Call the Medium thread tasks, when it's time
	 **************************************************************************/    
       if ((uiMedThreadCount % (unsigned int) 
             (HEARTBEAT_TIME/TIMER0_TIME)) == 0)
        {
            // Toggle the heartbeat LED
            heartbeat();
        }

		// using the formula to calculate the number of counts needed to get the desired interrupt rate:
	    // ( SignalPeriod (1/F) / Timer1 Update Rate ( ) / Samples Per Period )
  
       if ((uiMedThreadCount % (unsigned int)( ( TIMER0_CNT * TIMER0_SCALER / FreqDesired ) / 100 )) == 0)
        {
			UpdateSignal();
        }
        
        if ((uiMedThreadCount % (unsigned int) 
             (MENU_TIME/TIMER0_TIME)) == 0)
        {
            // Toggle the heartbeat LED
            RunMenu();
        }
        // Clear in-progress flag
        bMedThreadInProgress = FALSE;
    }   // End of medium thread tasks
}

#if !defined (SLOW_SINE)

ISR(TIMER1_COMPA_vect)
{
	// Triggers when output compare = OCR1A
	UpdateSignal();
}

void UpdateFreqCnt(){
	OCR1A = F_CPU / (FreqDesired * 100);
}

#endif /* !SLOW_SINE */

/* This handler takes care of all unused interrupts
 */
ISR(__vector_default)
{
	ReportError(UNUSED_INTERRUPT);
}
