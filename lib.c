/******************************************************************************
 * File Name:	Lib.c
 * Program:		Project for Real-Time Embedded Systems Programming class
 * Author:		Robert Weber
 * Purpose:		Contains library-like functions.
 *
 *  Date	  By:		Changes:
 * ---------  ---------	-------------------------------------------------------
 * 05 Dec 07  T Lill	Updated _itoa and _atoi to allow negative numbers.
******************************************************************************/
#include "string.h"
#include "lib.h"

/******************************************************************************
 * Converts the passed integer to its equivalent ASCII string, using the
 * specified base. The allowable base is only 10 or 16.
 ******************************************************************************/
void _itoa(char **buf, int i, int base)
{
    char *s;
    int rem;
    char rev[STRLEN+1];

    if (i == 0)
    { /* Converting a 0, so we just generate the return string */
        (*buf)[0] = '0';
        (*buf)[1] = '\0';   /* RJW: Null-terminate string */
        return;
    }
	/*
	 * If the number to be converted is negative, we'll just treat it as
	 * positive, add a minus sign to the output string, then perform the
	 * conversion,
	 */
	if (i < 0)
	{
		i = -i;
		(*buf)[0] = '-';
		++(*buf);
	} 

    /* Set "rev" array to 0 */
    memset((MEMPTR)rev,0,STRLEN+1);

    /* Initialize first location in "reverse" array to 0 */
    rev[STRLEN] = 0;
    s = &rev[STRLEN];     /* Will access reverse array by pointer "s" */
  
    /* This loop divides the integer by the base. The remainder of that
     * division indicates the "character" for that position in the string.
     * This is converted to an ASCII character and saved in "rev".
     *
     * The quotient is the term for the next division.
     *
     * End result is a string that consists of ASCII version of the input
     * number. */
    while (i)     /* Continue until i == NULL */
    {
        rem = i % base;     /* Get character for this position in string */
        if (rem < 10)
        {
            *--s = rem + '0';           /* Convert to ASCII */
        }
        else if (base == 16)
        {
            *--s = "abcdef"[rem - 10];  /* Convert to ASCII */
        }
        i /= base;
    }

    /* Copy string in "rev" to string that will be returned.
     * Append a NULL. */
    while (*s)
    {
        (*buf)[0] = *s++;
        ++(*buf);
    }
    (*buf)[0] = '\0';
}

/******************************************************************************
 * Converts the passed string to an integer. It does not verify that the 
 * contents of the string are valid integer characters.
 *
 * For some reason, I cannot use "base" in the calculation for power. Hence,
 * I use an "if" statement to check if it's "10" or "16". Hence, only those
 * values for base can be used. (Note: This is an issue with an old compiler
 * on 68HC11. Not verified to still be a problem here).
 ******************************************************************************/
int _atoi(char *buf, int base)
{
	int StrVal = 0;
	int StrLength;
	int i;
    int power = 1;
	eBooleanType negNum = FALSE;
	char dummy;

    StrLength = strlen(buf);
	/*
	 * First, test the leading character for a minus sign.  If present,
	 * strip it off but set a flag so we will remember to return a 
	 * negative result.
	 */
	if (*buf == '-')
	{
		dummy = *buf++;		// this avoids a (spurious) compiler warning
		negNum = TRUE;
		StrLength--;
	}

    // Calculate power of base for first digit
    for (i = 0; i < StrLength - 1; ++i)
    {
        if (base == 10)
        {
            power *= 10;
        }

        if (base == 16)
        {
            power *= 16;
        }
    }

    for (i = 0; i < StrLength; ++i)
	{
		if ((*buf >= 'a') && (*buf <= 'f'))
		{   // lower-case ascii
			StrVal += ((*buf - 'a') + 10) * power;
		}
		else if ((*buf >= 'A') && (*buf <= 'F'))
		{   // upper-case ascii
			StrVal += ((*buf - 'A') + 10) * power;
		}
        else
        {
		    StrVal += (*buf - '0') * power;
        }

        // Move to next character. Reduce power by base
		++buf;
        power /= base;
	}

    return negNum ? -StrVal : StrVal;
}
