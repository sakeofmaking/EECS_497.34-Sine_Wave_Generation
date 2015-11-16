/******************************************************************************
 * File Name:	dtoa.c
 * Program:		Project for Real-Time Embedded Systems Programming class
 * Author:		Robert Weber
 * Purpose:		Contains functions to retrieve analog values from the D/A.
 *
 *  Date	Changed by:	Changes:
 * -------	-----------	-------------------------------------------------------
 * 07Mar02	R Weber		Original file
 * 24Feb03  R Weber     Updated to increase speed.
 * 08Mar04  R Weber     Updated for Atmega169
 * 06Oct05	T Lill		Removed deprecated functions
 ******************************************************************************/
#include "lib.h"
#include "errors.h"
#include "dtoa.h"
#include "serial.h"

// Number of bytes to send on each SPI transmission
#define SPI_NUM_BYTES               2

/******************************************************************************
 * This function initializes the A/D converter
 ******************************************************************************/
void InitDtoA(void)
{
    /* Set SPI Control register, with:
     *   SPIE:  0 - SPI Interrupt disabled
     *   SPE:   1 - SPI Enabled
     *   DORD:  0 - Data order is MS bit first
     *   MSTR:  1 - CPU is the master
     *   CPOL:  0 - SCLK line is low when SPI is idling
	 *			Note: The HW on the STK502 inverts this signal, so we set this
	 *				  bit high, and therefore, get a low clock.
     *   CPHA:  0 - Data sampled on clocks rising edge
     *   SPR1:	0 - 
	 *   SPR0:  1 - Sck frequency = Fosc/8
	 *              when SPI2X is set to 1
	 */
	SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR0);
	
	/* Set SPI2X on SPSR to finish setting SCK frequency 
	 * Note that since this only writable bit in this register is the SPI2X
	 * bit, We can just write. */
	SPSR = _BV(SPI2X);
	
	/* Set Port B, pin 4 to be an output. This is actually already done in 
	 * main.c, but I like to do it again here in case, sometime down the road,
	 * it's not setup in main. Keep your configuration close to where it's needed.
	 */
	SET_BIT(DDRB, D2A_CS_BIT);
	
	/* And set PB4 high, so D/A is not selected */
	SET_BIT(PORTB, D2A_CS_BIT); 
	
	/* Set D/A to 0 initially */
	WriteDtoASample(0);
}

/******************************************************************************
 * This function writes data to the D/A.
 *
 * Our interface is 12 bits. To match the format of our D/A, we need
 * to do a 16-bit write, with the following format, Most-Significant bits first:
 * 4 MS bits: dummy bits. Don't care.
 * 10 next MS bits: The value we want to write.
 * 2 LS bits: extra bits. Don't care.
 *
 * Note: There are 2 errors that can be detected in our SPI system.
 *       Mode Fault:        This indicates two devices have tried to be a master
 *                          at the same time. This is determined to have
 *                          happened when we're a Master and the Slave Select
 *                          (SS) line is externally  driven low. Since only we
 *                          drive the PD5_SS pin, this should never happen.
 *       Write Collision:   If we try to write a sample into the data register
 *                          before the last one has been transmitted, we'll see
 *                          this error. To prevent this, we must always check
 *                          that the data register is empty before writing to
 *                          it.
 ******************************************************************************/
/* This union is used so we can access the individual bytes of the passed-in
 * integer. The endianness of the processor will affect the order of the MSB
 * and LSB. */
typedef union
{
	unsigned int WholeInt;
	struct
	{
		unsigned int LSB : 8;
		unsigned int MSB : 8;
	}Bytes;
} SampleType;

void WriteDtoASample(unsigned int Value)
{
	SampleType Sample;
    unsigned char ucSPIStatus;

	// Load output value into union
	Sample.WholeInt = Value << 2;

    /* Select D/A */
	CLEAR_BIT(PORTB, D2A_CS_BIT); 

    /* Read status register to see if any errors are set, and to clear any 
     * pending issues that prevent writing to the SPI. We do this before we
     * enable the SPI, so that if a Mode Fault exists, it will be cleared
     * when we write to the SPCR register. */
    ucSPIStatus = SPSR;

	/* Check for errors by reading the status register */
    if ((SPSR & _BV(WCOL)) != 0) 
    {   /* Write collision error. Since we're not writing to the SPI Data
         * register while a trasmission is in progress, we should never see
         * this. */
        ReportError(SPI_WRITE_COLLISION);
    }

	/* Write value to D/A. */
    SPDR = Sample.Bytes.MSB;

    /* Wait for first byte to be transmitted */
	/* Note: We really should never perform a wait like this in a non-preemptible
	 * scheduler, since if the Tx is never complete, we'll stay here forever.
	 * Better to use the SPI Interrupt, or to time out at some point. */
    while ((SPSR & _BV(SPIF)) == 0);

	/* Check for errors */
    if ((SPSR & _BV(WCOL)) != 0) 
    {
        ReportError(SPI_WRITE_COLLISION);
    }

    /* Now write 2nd byte */
    SPDR = Sample.Bytes.LSB;

    /* Again, wait for transmission to be complete before disabling SPI */
    while ((SPSR & _BV(SPIF)) == 0);

    /* Unselect D/A */
	SET_BIT(PORTB, D2A_CS_BIT);
}
