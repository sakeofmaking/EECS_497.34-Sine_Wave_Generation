/******************************************************************************
 * File Name:	sine.c
 * Program:		Project for Real-Time Embedded Systems Programming class
 * Author:		Robert Weber
 * Purpose:		Contains sine wave functions.
 *
 *  Date	Changed by:	Changes:
 * -------	-----------	-------------------------------------------------------
 * 30Apr02	R Weber		Initial file
 ******************************************************************************/

#include "sine.h"
#include "lib.h"
#include "dtoa.h"
#include "interrpt.h"
#if defined (SLOW_SINE)
#include "serial.h"
#endif

#if defined(DEBUG_D2A)
#include "serial.h"
#endif

#define MAX_VOLTAGE            500  // 5.0 Volts
#define MIN_VOLTAGE            100  // 1.0 Volts
#define VOLTAGE_INCREMENT       10  // 0.1 Volt increments

#define MAX_FREQUENCY          100  // 100 Hz
#define MIN_FREQUENCY           40  //  40 Hz
#define FREQUENCY_INCREMENT      5  //   5 Hz increments

/* Define constants */
#define VOLTAGE_1V_SCALE		100			/* = 1V */
#define VOLTAGE_10THV_SCALE		 10			/* = 1/10 V */
#define VOLTAGE_HALFV_SCALE		 50			/* = 1/2 V */

// # of samples in the lookup tables
#define SAMPLE_TABLE_SIZE		(SAMPLES_PER_PERIOD/2 + 1)

// SAMPLES_PER_PERIOD = 100 
/***************************** Type Definitions *******************************/


/**************************** Data Declarations *******************************/
volatile unsigned int FreqDesired = 40;
volatile unsigned int VoltDesired = 100;
volatile unsigned int FreqActual = 0;
volatile unsigned int VoltActual = 0;
extern int DisplaySamples;
/* Table to hold values for sine wave, based on full scale range.
 * Note that "const" variables can be modified once for initialization. */
static const unsigned int VoltageLookup [SAMPLE_TABLE_SIZE] = {
	1023, 1022, 1019, 1014, 1007,  998,  987,  974,  960,  943,
	 925,  906,  884,  862,  838,  812,  786,  758,  729,  700,
	 670,  639,  607,  576,  544,  512,  479,  447,  416,  384,
	 353,  323,  294,  265,  237,  211,  185,  161,  139,  117,
	  98,   80,   63,   49,   36,   25,   16,    9,    4,    1,  0};

/* This array is used for the calculated values for the output sine wave.
 * Each time a new voltage is input, the new values are calculated in the
 * available array and it is then used for output. */
static unsigned int VoltageScaled[SAMPLE_TABLE_SIZE][2];

/* Since we change this in one thread, and use it in another, we want to
 * make sure the compiler always rereads the value. */
static volatile unsigned int ucActiveVoltArray = 0;


/*************************** Function Prototypes ******************************/



/************************ Function Implementations ****************************/

/******************************************************************************
 * This function initializes the values for the sine wave.
 ******************************************************************************/
void initSine(void)
{
    // Generate new values to output for sine wave
    CalcSineValues(VoltDesired);
	
	// Initialize D/A for sine wave output
	InitDtoA();
}

/******************************************************************************
 * Set desired frequency
 ******************************************************************************/
eErrorType SetFreq(unsigned int Freq)
{
    eErrorType ReturnVal = NO_ERROR;

    if ((Freq < MIN_FREQUENCY) || (Freq > MAX_FREQUENCY))
    {   // Frequency is out of range
        ReturnVal = PARAMETER_OUT_OF_RANGE;
    }
    
    else if (Freq % FREQUENCY_INCREMENT != 0)
    {   // Invalid value
        ReturnVal = INVALID_PARAMETER;
    }

    if (ReturnVal == NO_ERROR)
    {   // No problems found with value
        FreqDesired = Freq;

#if !defined (SLOW_SINE)
        // Report new frequency to Sine Wave interrupt
		UpdateFreqCnt();
#endif
    }

    return ReturnVal;
}

/******************************************************************************
 * Set desired voltage
 ******************************************************************************/
eErrorType SetVolt(unsigned int Volt)
{
    eErrorType ReturnVal = NO_ERROR;

    if ((Volt < MIN_VOLTAGE) || (Volt > MAX_VOLTAGE))
    {   // Voltage is out of range
        ReturnVal = PARAMETER_OUT_OF_RANGE;
    }
    
    else if (Volt % VOLTAGE_INCREMENT != 0)
    {   // Invalid value
        ReturnVal = INVALID_PARAMETER;
    }

    if (ReturnVal == NO_ERROR)
    {   // No problems found with value
        VoltDesired = Volt;

        // Generate new values to output for sine wave
        CalcSineValues(VoltDesired);
}

    return ReturnVal;
}


/******************************************************************************
 * This function calculates a new set of values for a sine wave. It "populates"
 * the unused column of the VoltageScaled array with the new values, and then
 * switches that column to be the active one.
 ******************************************************************************/
void CalcSineValues ( unsigned int NewVoltage )
{

	/* Assignment
	 * Using the "unused" array, populate it with values for the scaled
	 * voltage. Remember to scale the calculation so that you don't lose
	 * resolution, and you don't have overflow.
	 *
	 * Lastly, switch ucActiveVoltArray to point to this new array of
	 * values.
	 */

	int i;

	if(ucActiveVoltArray == 0){
	 	ucActiveVoltArray = 1;
	} else{
		ucActiveVoltArray = 0;
	}

	for(i = 0; i < SAMPLE_TABLE_SIZE; i++){
		VoltageScaled[i][ucActiveVoltArray] = (VoltageLookup[i] * (NewVoltage / 10)) / 50;
	}

	VoltActual = NewVoltage;


} // End of CalcSineValues
 
/******************************************************************************
 * This file outputs the next sine wave value.
 *
 * Previous sine wave value is output first to reduce any errors in variance of
 * execution time of this function.
 * 
 * Since we want to minimize the time in this function, no error checking or
 * other "niceties" are done.
 ******************************************************************************/
void UpdateSignal( )
{
	static unsigned char ucVoltageIndex = 0;
	static unsigned int DACValue = 0;
	/* This variable specifies whether we're increasing (1) or decreasing the
	 * voltage value. */
	static unsigned char Direction = 1;

#if defined (SLOW_SINE)
    char *pDebugStr;
    char DebugStr[10];
#endif

	/* Assignment
	 * Get the new value to output and put it in DACValue.
	 */

	if(ucVoltageIndex == 0){
		Direction = 1;
	} else if(ucVoltageIndex == (SAMPLE_TABLE_SIZE - 1)){
		Direction = 0;
	}

	if(Direction == 1){
		ucVoltageIndex++;
	} else{
		ucVoltageIndex--;
	}

	DACValue = VoltageScaled[ucVoltageIndex][ucActiveVoltArray];

	WriteDtoASample(DACValue);


if ( DisplaySamples )
	{
	#if defined (SLOW_SINE)
		SCIWriteString("Sample  = ");
    	pDebugStr = DebugStr;
		_itoa(&pDebugStr, DACValue, 10);
		SCIWriteString(DebugStr);
    	SCIWriteString("\r\n");
	#endif
	}

}  // End of UpdateSignal
