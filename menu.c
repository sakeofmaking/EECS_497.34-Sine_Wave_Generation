/******************************************************************************
 * File Name:	menu.c
 * Program:		Project for Real-Time Embedded Systems Programming class
 * Author:		Robert Weber
 * Purpose:		Contains menu functions.
 *
 *  Date	Changed by:	Changes:
 * -------	-----------	-------------------------------------------------------
 * 17Aug03	R Weber		Copied from 68HC11 code.
 * 02Feb04  R Weber     Split help menu into two sets of writes to prevent Tx
 *                      buffer overflow without increasing Tx buffer size.
 * 29Sep05	T Lill		Editorial changes only.
 * 05Dec07	T Lill		Replaced "debug" prompt with "cmd"
 ******************************************************************************/
#include "string.h"
#include "stdlib.h"
#include "lib.h"
#include "serial.h"
#include "errors.h"
#include "menu.h"
#include "lcd.h"
#include "tempsensor.h"
#include "sine.h"
#include "dtoa.h"

#define MAX_MEM_SIZE 0x40
#define MAX_MEM_ADDR 0x4FF
#define MIN_MEM_ADDR 0x100

// Enumeration for the menuing system
typedef enum {
    TOP_MENU,
    DISPLAY_HELP_MENU1,
    DISPLAY_HELP_MENU2,
    DISPLAY_HELP_MENU3,
    DISPLAY_HELP_MENU4,
    DISPLAY_HELP_MENU5,
    DISPLAY_HELP_MENU6,
	GET_LCD_CHARACTER,
    GET_LCD_POSITION,
	SIGNAL_READ_FREQUENCY,
	SIGNAL_READ_VOLTAGE,
	WRITE_D2A,
	MEMORY_GET_ADDRESS,
	MEMORY_GET_LENGTH
} DebugMenuStateType;

typedef enum {
	READ_MEMORY,
    WRITE_MEMORY
} DebugMenuSubType;

/******************************************************************************
 * Processes keypresses received via RS-232. Implements a menuing system.
 ******************************************************************************/
static char zInputStr[MAX_IN_STR_SIZE];
static char *ptrInputStr = zInputStr;
static char LCDChar, LCDPosition;
static DebugMenuStateType MenuState = TOP_MENU;

int DisplaySamples = 0;

void RunMenu(void)
{
    char cTempChar = 1;     // Set to any value other than 0
    char zOutputStr[MAX_MEM_SIZE + 3];  // Add space for newline, return and NULL
    char *ptrOutputStr;
    eErrorType error = NO_ERROR;
    static DebugMenuSubType MenuAction = READ_MEMORY;
	static unsigned int i, Address = 0, Length = 0, Value = 0;
	static unsigned int Frequency = 0, Voltage = 0;

    // Read input characters until input buffer is empty
    while ((cTempChar = SCIReadChar()) != 0)
    {   // Have another character from input buffer
        if (cTempChar == '\r')
        {   // Enter character. Process input
            *ptrInputStr = '\0';        // append Null
            ptrInputStr = zInputStr;    // Reset input string
            SCIWriteString_P(PSTR("\n\r"));     // Move cursor to next line

			// Reset displaying of A/D samples
            DisplaySamples = 0;

            // Process entry based on debug menu state
            switch(MenuState)
            {
                case TOP_MENU:
                    if (strcmp(zInputStr, "?") == 0)
                    {   // Help screen
						MenuState = DISPLAY_HELP_MENU1;
                    }

                    else if (strcmp(zInputStr, "ge") == 0)
                    {   // Display desired signal parameters
                        // Retrieve signal parameters
                        error = GetError();
                        SCIWriteString_P(PSTR("  Error = "));
                        ptrOutputStr = zOutputStr;
                        _itoa(&ptrOutputStr, error, 10);
                        SCIWriteString(zOutputStr);
                        SCIWriteString_P(PSTR("\n\r"));
                    }


                    else if (strcmp(zInputStr, "ce") == 0)
                    {
                        ClearError();
                    }

                    else if (strcmp(zInputStr, "lcd") == 0)
                    {
                        SCIWriteString_P(PSTR("  Enter character to display (0-9 or a space): "));
                        MenuState = GET_LCD_CHARACTER;
                    }

                    else if (strcmp(zInputStr, "te") == 0)
                    {   // Display temperature
                        int temperature;

                        temperature = ReadTemperature();
                        SCIWriteString_P(PSTR("  Temperature = "));
                        ptrOutputStr = zOutputStr;
                        _itoa(&ptrOutputStr, temperature, 10);
                        SCIWriteString(zOutputStr);
                        SCIWriteString_P(PSTR("\n\r"));
                    }

                    else if (strcmp(zInputStr, "dsp") == 0)
                    {   // Display desired signal parameters
                        // Retrieve signal parameters
                        Frequency = GET_FREQ_DESIRED();
                        Voltage = GET_VOLT_DESIRED();
                        
                        SCIWriteString_P(PSTR("  Desired Frequency = "));
                        ptrOutputStr = zOutputStr;
                        _itoa(&ptrOutputStr, Frequency, 10);
                        SCIWriteString(zOutputStr);
                        SCIWriteString_P(PSTR("\n\r"));

                        SCIWriteString_P(PSTR("  Desired Voltage = "));
                        ptrOutputStr = zOutputStr;
                        _itoa(&ptrOutputStr, Voltage, 10);
                        SCIWriteString(zOutputStr);
                        SCIWriteString_P(PSTR("\n\r"));

                        // Retrieve signal parameters
                        Frequency = GET_FREQ_ACTUAL();
                        Voltage = GET_VOLT_ACTUAL();
                        
                        SCIWriteString_P(PSTR("  Actual Frequency = "));
                        ptrOutputStr = zOutputStr;
                        _itoa(&ptrOutputStr, Frequency, 10);
                        SCIWriteString(zOutputStr);
                        SCIWriteString_P(PSTR("\n\r"));

                        SCIWriteString_P(PSTR("  Actual Voltage = "));
                        ptrOutputStr = zOutputStr;
                        _itoa(&ptrOutputStr, Voltage, 10);
                        SCIWriteString(zOutputStr);
                        SCIWriteString_P(PSTR("\n\r"));
                    }

                    else if (strcmp(zInputStr, "msp") == 0)
                    {   // Change desired signal parameters
                        SCIWriteString("  Enter desired voltage (100 to 500): ");
                        MenuState = SIGNAL_READ_VOLTAGE;
                    }

                    else if (strcmp(zInputStr, "wv") == 0)
                    {   // Change desired signal parameters
                        SCIWriteString("  Enter desired voltage (0 to 1023): ");
                        MenuState = WRITE_D2A;
                    }

                    else if (strcmp(zInputStr, "ds") == 0)
                    {   // Display A/D Samples
                        SCIWriteString("  Hit Enter key to terminate\n\r");
                        DisplaySamples = 1;
                    }

					else if ((strcmp(zInputStr, "rm") == 0) ||
                             (strcmp(zInputStr, "wm") == 0))
                    {   // Memory menu
                        if (strcmp(zInputStr, "rm") == 0)
                        {
                            MenuAction = READ_MEMORY;
                        }
                        else
                        {
                            MenuAction = WRITE_MEMORY;
                        }

                        // Get address
                        SCIWriteString("  Address (0x100 to 0x4FF) = ");
                        MenuState = MEMORY_GET_ADDRESS;
                    }

				   else
                    {   // No entry
                        // Back to top menu
                        MenuState = TOP_MENU;
                    }
                    break;

				case GET_LCD_CHARACTER:
                    if (zInputStr[0] != '\0')
                    {   // Just skip NULL entries
                        LCDChar = zInputStr[0];

                        // Now get position
                        SCIWriteString_P(PSTR("\n\r  Enter LCD Position(2-7): "));
                        MenuState = GET_LCD_POSITION;
                    }
                    else
                    {   // No entry
                        // Back to top menu
                        MenuState = TOP_MENU;
                    }
                    break;

                case GET_LCD_POSITION:
                    if (zInputStr[0] != '\0')
                    {   // Just skip NULL entries
                        LCDPosition = _atoi(zInputStr, 10);

                        // Now get position
                        MenuState = TOP_MENU;
                    }
                    else
                    {   // No entry
                        // Back to top menu
                        MenuState = TOP_MENU;
                    }
                    break;

                case WRITE_D2A:
                    if (zInputStr[0] != '\0')
                    {   // Just skip NULL entries
                        Voltage = _atoi(zInputStr, 10);

						if ((Voltage >= 0) && (Voltage <= 1023))
						{	// Valid voltage. Write to D/A
							WriteDtoASample(Voltage);
						}
                    }
                    // Back to top menu
                    MenuState = TOP_MENU;
                    break;

                case SIGNAL_READ_VOLTAGE:
                    if (zInputStr[0] != '\0')
                    {   // Just skip NULL entries
                        Voltage = _atoi(zInputStr, 10);

                        // Now get frequency
                        SCIWriteString_P(PSTR("\n\r  Enter desired frequency (40 to 100): "));
                        MenuState = SIGNAL_READ_FREQUENCY;
                    }
                    else
                    {   // No entry
                        // Back to top menu
                        MenuState = TOP_MENU;
                    }
                    break;

                case SIGNAL_READ_FREQUENCY:
                    if (zInputStr[0] != '\0')
                    {   // Just skip NULL entries
                        Frequency = _atoi(zInputStr, 10);

                        // Set the parameters
                        if ((error = SetFreq(Frequency)) != NO_ERROR)
                        {
                            SCIWriteString_P(PSTR("\n\r  Error setting frequency"));
                        }
                        else
                        {   // Set voltage
                            if ((error = SetVolt(Voltage)) != NO_ERROR)
                            {
                                SCIWriteString_P(PSTR("\n\r  Error setting voltage"));
                            }
                        }

                        // Back to top menu
                        MenuState = TOP_MENU;
                    }
                    else
                    {   // No entry
                        // Back to top menu
                        MenuState = TOP_MENU;
                    }
                    break;

				case MEMORY_GET_ADDRESS:
                    if (zInputStr[0] != '\0')
                    {   // Just skip NULL entries
                        Address = _atoi(zInputStr, 16);
						
						if ((Address < MIN_MEM_ADDR) ||
						    (Address > MAX_MEM_ADDR))
						{	// Address out of range
							SCIWriteString_P(PSTR("  Address out of range\n\r"));
						}
						else
						{
							if (MenuAction == READ_MEMORY)
							{
								// Now get length
								SCIWriteString_P(PSTR("  Length (in hex) = "));
							}
							else
							{
								SCIWriteString_P(PSTR("  Value (in hex) = "));
							}
							MenuState = MEMORY_GET_LENGTH;
						}
					}
					else
					{   // No entry
						// Back to top menu
						MenuState = TOP_MENU;
					}
                    break;

               case MEMORY_GET_LENGTH:
                    if (zInputStr[0] != '\0')
                    {
                        if (MenuAction == READ_MEMORY)
                        {
                            Length = _atoi(zInputStr, 16);
							
							// Limit memory to available RAM
							if ((Address + Length - 1) > MAX_MEM_ADDR)
							{
								Length = MAX_MEM_ADDR - Address + 1;
							}
							
                            if (Length == 0)
                            {   // default to 1
                                Length = 1;
                            }

                            if (Length > MAX_MEM_SIZE)
                            {
                                Length = MAX_MEM_SIZE;
                            }

                            SCIWriteString_P(PSTR("  Memory ="));

                            for (i = 0; i < Length; ++i)
                            {
                                if ((i % 16) == 0)
                                {
                                    SCIWriteString_P(PSTR("\n\r  "));

                                    // Display address
                                    ptrOutputStr = zOutputStr;
                                    _itoa(&ptrOutputStr, Address, 16);
                                    SCIWriteString(zOutputStr);
                                    SCIWriteString(">  ");
                                }

                                ptrOutputStr = zOutputStr;
                                *ptrOutputStr = *(unsigned char *)Address;
                                ++Address;

                                // Convert character to an ASCII string in Hex
                                _itoa(&ptrOutputStr, (int)zOutputStr[0], 16);

                                // Force to be 2 characters long
                                if (strlen(zOutputStr) < 2)
                                {
                                    zOutputStr[1] = zOutputStr[0];
                                    zOutputStr[0]= '0';
                                }
                                zOutputStr[2] = ' ';
                                zOutputStr[3] = '\0';
                                SCIWriteString(zOutputStr);
                            }

                            SCIWriteString_P(PSTR("\n\r"));
                        }
                        else
                        {   // Write memory
                            Value = _atoi(zInputStr, 16);

                            if (Value > 0xFF)
                            {   // invalid value
                                SCIWriteString_P(PSTR("Invalid value\n\r"));
                                MenuState = TOP_MENU;
                            }
                            else
                            {
                                SCIWriteString_P(PSTR("Write Mem\n\r"));
                                ptrOutputStr = (char *)Address;
                                *ptrOutputStr = (char)Value;

                                SCIWriteString_P(PSTR("\n\r"));
                            }
                        }

                        // Back to top menu
                        MenuState = TOP_MENU;
                    }
                    break;

				default:
                    // Erroneous state. Reset to none
                    MenuState = TOP_MENU;
                    break;
            }   // end switch

            // Reset string pointer. May have been moved during command processing
            ptrInputStr = zInputStr;

            if (MenuState == TOP_MENU)
            {
                // Display prompt
                SCIWriteString_P(PSTR("cmd> "));
            }
        }
        else
        {   // Save new character to input buffer
            if (ptrInputStr < &zInputStr[MAX_IN_STR_SIZE-2])
            {   // Buffer is not full
                *ptrInputStr = cTempChar;
                *(ptrInputStr+1) = '\0';    // Keep null in string

                // echo character
                SCIWriteString(ptrInputStr);

                ++ptrInputStr;
            }   // else, buffer is full. Ignore characters.
        }
    }   // End while. All characters processed

	if (MenuState == DISPLAY_HELP_MENU1)
	{	// Display 1st part of help menu
		SCIWriteString_P(PSTR("  Commands are:\n\r"));
		SCIWriteString_P(PSTR("  ge  - Display error code\n\r"));
		MenuState = DISPLAY_HELP_MENU2;
	}
	
	else if (MenuState == DISPLAY_HELP_MENU2)
	{	// Display 2nd part of help menu
		SCIWriteString_P(PSTR("  ce  - Clear current error\n\r"));
		SCIWriteString_P(PSTR("  lcd - Display LCD character\n\r"));
		MenuState = DISPLAY_HELP_MENU3;
	}
	
	else if (MenuState == DISPLAY_HELP_MENU3)
	{	// Display 3rd part of help menu
		SCIWriteString_P(PSTR("  te  - Display temperature\n\r"));
		SCIWriteString_P(PSTR("  wv  - Write voltage to D/A\n\r"));
		MenuState = DISPLAY_HELP_MENU4;
	}

	else if (MenuState == DISPLAY_HELP_MENU4)
	{	// Display 4th part of help menu
		SCIWriteString_P(PSTR("  dsp - Display signal parameters\n\r"));
		SCIWriteString_P(PSTR("  msp - Change desired signal parameters\n\r"));
		MenuState = DISPLAY_HELP_MENU5;
	}

	else if (MenuState == DISPLAY_HELP_MENU5)
	{	// Display 5th part of help menu
		SCIWriteString_P(PSTR("  ds  - Display A/D samples \n\r"));
		SCIWriteString_P(PSTR("  rm  - Read memory\n\r"));
		MenuState = DISPLAY_HELP_MENU6;
	}

	else if (MenuState == DISPLAY_HELP_MENU6)
	{	// Display 6th part of help menu
		SCIWriteString_P(PSTR("  wm  - Write memory\r"));
		SCIWriteString_P(PSTR("  ?   - Display this help menu\n\r"));
		MenuState = TOP_MENU;
	}
}
