/******************************************************************************
 * File Name:	tempsensor.c
 * Program:		TempSensor
 * Author:		R. Weber + T. Lill
 * Purpose:		Demonstrate reading the A/D converter and the use of a Taylor
 *				series approximation, without floating point arithmetic.
 *
 *  Date	Changed by:	Changes:
 * -------	-----------	-------------------------------------------------------
 * 12-03-03	T. Lill		Updated for Fall '03.
 * 02-23-05 T. Lill		Removed unused constant.  Cleaned up function headers.
 * 05-25-06 T  Lill		Read A to D register in one operation.
 * 11-01-10 T. Lill		Converted program to ATmega2560
 ******************************************************************************/

/********************************* Includes ***********************************/
#include <avr/io.h>
#include "errors.h"
#include "tempsensor.h"

#include "lib.h"
#include "serial.h"

/******************************************************************************
 * constants
 *****************************************************************************/
#define T_AMB   ((int long)298)
#define BETA    ((int long)3380)
#define T_ZERO  ((int long)273)
#define TEMP1   ((BETA<<8)/T_AMB)
#define TEMP2   (TEMP1 - (1<<9))

/******************************************************************************
 * Initialization for A/D, which is needed by temperature sensor.
 *****************************************************************************/
void InitAtoD(void)
{
	/*------------------ Set ADMUX values -------------------------- 
	 * Bit(s)   7: REFS1  = 0  Choose AVCC reference voltage
	 *          6: REFS0  = 1.
	 *          5: ADLAR  = 0  Result is right-adjusted
	 *          4: MUX4   = 0  Select ADC1 for conversion
	 *          3: MUX3   = 0  
	 *          2: MUX2   = 0  
     *          1: MUX1   = 0  
	 *          0: MUX0   = 1
	 */	 
	ADMUX = _BV(REFS0) | _BV(MUX0);

	/*------------------ Set ADCSRB values -------------------------- 
	 * Bit(s)   7: Unused = 0
	 *          6: ACME   = 0  No analog multiplexing
	 *          5: Unused = 0  
	 *          4: Unused = 0  
	 *          3  MUX5   = 0
	 *          2: ADTS2  = 0  Free running mode on A/D conversion.
     *          1: ADTS1  = 0 
	 *          0: ADTS0  = 0
	 */	 
	ADCSRB = 0;

	/*------------------ Set DIDR0 and DIDR2 values -----------------
	 * Bit(s) 7-0:  ADCxD = 1  Disable all digital input buffers.
	 */	 
	DIDR0 = 0xFF;
	DIDR2 = 0xFF;

    /*------------------ Set ADCSRA values -------------------------- 
	 * Bit(s)   7:  ADEN  = 1  Enable the ADC
	 *          6:  ADSC  = 1  Start a conversion 
	 *          5: ADATE  = 1  Enable auto triggerring
	 *          4:  ADIF  = 1  Clear any pending interrupt
	 *          3:  ADIE  = 0  Don't enable interrupt
	 *          2: ADPS2  = 1  Divide 8 MHz clock by 64 to get  
     *          1: ADPS1  = 1  125 kHz.
	 *          0: ADPS0  = 0
	 */	 
	ADCSRA = _BV(ADEN) | _BV(ADSC)  | _BV(ADATE) | _BV(ADIF) 
					   | _BV(ADPS2) | _BV(ADPS1);
}

/******************************************************************************
 * Read the A/D converter, and calculate the temperature.
 *
 * ASSIGNMENT:  fill in this function to:
 *	(1) Read the Analog-to-Digital Converter
 *	(2) Calculate the temperature, in degrees Celsius, using the Taylor's
 *		series approximation discussed in class.
 *	(3) Return the temperature as an integer value from this function.
 *
 *  ___________________________________________________________________________
 *  Explanation of solution:
 *    ln (x) = 2 ((x-1)/(x+1))    i.e, first term of Taylor's series
 *           = 2 [(Vadc - (Vref - Vadc))/(Vadc + (Vref - Vadc))]
 *		     = 4(Vadc / Vref) - 2
 *		     = 4[(ADC*Vref/1024)/Vref]-2
 *		     = (ADC/256) - 2
 *  Inserting this term in the complete temperature formula would yield:
 *	  Temperature = (BETA * 256)/[(ADC - (2*256) + (BETA*256/T_AMB)] - T_ZERO
 *                = (BETA * 256)/(ADC + TEMP2) - T_ZERO
 *                = ((BETA << 2)/((ADC + TEMP2) >> 6 )) - T_ZERO;
 *
 *       where,TEMP2 = [-(2*256) + (BETA*256/T_AMB)]
 *
 *       Also note that multiplication of 256 is split into left shift of 2 
 *       in numerator and right shift of 6 in denominator for scaling purposes.
 *    
 ******************************************************************************/
int ReadTemperature(void)
{
    int long Temperature = 0;

	// calculation explanation is shown above
	Temperature = ( ( BETA << 2 ) / (( ADC  + TEMP2 ) >> 6 ) ) - T_ZERO;

   return (int)Temperature;
}
