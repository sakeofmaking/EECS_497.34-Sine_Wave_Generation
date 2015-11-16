/******************************************************************************
 * File Name:	Main.c
 * Program:		"Hello World" program for Real-Time Embedded Systems Programming
 * Author:		Robert Weber
 * Purpose:		Main file. Contains the Main function and other executive
 *				functions. It's the "master" of the program.
 *
 *  Date	Changed by:	Changes:
 * -------	-----------	-------------------------------------------------------
 * 24Apr02  R Weber		Original file.
 * 06Oct05	T Lill		Removed deprecated functions
 * 05Dec07	T Lill		Updated heartbeat function.  Added missing initialization
 *						for SPI
 ******************************************************************************/
#include "lib.h"
#include "interrpt.h"
#include "heartbeat.h"
#include "serial.h"
#include "lcd.h"
#include "tempsensor.h"
#include "sine.h"
#include "dtoa.h"

/************************* Function Prototypes ******************************/
int main(void);


/*********************** Function Implementations ******************************/

/*
 * Since all we really want to do is toggle an LED on and off, there's no
 * point in keeping track of its state.  Just initialize the heartbeat LED,
 * then write 1 to PINB which toggles the bit.
 */
void heartbeat(void)
{
	asm volatile (" sbi	0x03, 0 ");	// see warning in lib.h about SET_BIT
}

/*****************************************************************************
 * Main function.
 * 
 * Initializes hardware and software. Then enters the endless foreground loop.
 * 
 *****************************************************************************/
int main(void) 
{
    // Disable interrupts
    cli();

	// Set port B as an output and turn off all LEDs
	DDRB = 0xFF;
	PORTB = 0xFF;

    /* Initialize the Timer 0 */
    ISR_InitTimer0();
#if !defined (SLOW_SINE)
	ISR_InitTimer1();
#endif

	// Initialize serial I/O
	SCIInitialize();

   	// Initialize SPI port
	InitDtoA();

	// Initialize the sine wave
	initSine();
	
    /* Enable interrupts. Do as last initialization, so interrupts are
     * not initiated until all of initialization is complete. */
   sei();

   for (; ; )		/* Foreground loops forever */
   {   // Do slow tasks here
      
   }   /* end of endless loop */

   return 0;
}
