//****************************************************************************
//
//  Copyright (c) 2000  Broadcom Corporation
//  All Rights Reserved
//  No portions of this material may be reproduced in any form without the
//  written permission of:
//          Broadcom Corporation
//          16215 Alton Parkway
//          Irvine, California 92619
//  All information contained in this document is Broadcom Corporation
//  company private, proprietary, and trade secret.
//
//****************************************************************************
//  $Id$
//
//  Filename:       Utilities.cpp
//  Author:         David Pullen
//  Creation Date:  Oct 22, 2000
//
//****************************************************************************
//  Description:
//      General utility functions that are used everywhere.
//
//****************************************************************************

//********************** Include Files ***************************************

// My api and definitions...
#include "Utilities.h"

#include "MessageLog.h"

#include <string.h>

//********************** Local Types *****************************************

//********************** Local Constants *************************************

//********************** Local Variables *************************************

//********************** Class Method Implementations ************************


// This helper function converts an abstract bitmask (of up to 32 bits) into
// a string of names, where each name corresponds to a bit in the mask.  It
// starts iterating at bit 31, and continues until it has checked through bit 0
// or has filled the output string.
//
// The bitmask is passed in by the user, and can be any size up to 32 bits
// (smaller sizes will be converted up to 32 bits, padded with 0's so that they
// will be skipped).
//
// The array of bit names must be specified, and must not be NULL.  It is
// assumed that pBitNames[0] corresponds to bit 0, and so on.  If there are
// gaps in the bits (reserved, etc.), then the pBitNames index corresponding
// to that index can be NULL.  Also, you don't have to specify unused MSB names,
// e.g. if only the 3 LSBs are used, then pBitNames can have only 3 entries.
//
// If a bit in the mask is set, and there is no corresponding string in the bit
// names array, then the string {Unknown} will be used.
//
// The separator string that is specified will be used in between each name.
//
// If the bitmask is 0 (no bits set), then the string {None} will be stored.
//
// Parameters:
//      bitmask - the mask of bits to be converted into strings.
//      pString - pointer to the string into which the names should be stored.
//      maxLength - the maximum number of characters (including the \0) that
//                  can be stored in pString.
//      pBitNames - pointer to an array of strings for the bit names.
//      numberOfNames - the number of bit names in the pBitNames array.
//      pSeparatorString - the string to be placed between each name.
//
// Returns:
//      true if the string was created successfully.
//      false if the string pointer was NULL, or if the string was too short to
//          hold all of the characters (truncated).
//
bool ConvertBitmaskToString(uint32 bitmask, char *pString, unsigned int maxLength,
                            const char *pBitNames[], unsigned int numberOfNames,
                            const char *pSeparatorString)
{
    unsigned int charsAppended = 0;
    unsigned int index;
    uint32 bit;
    const char *pStringToCopy;
    bool firstOne = true;

    // Quick sanity checks.
    if (pString == NULL)
    {
        gLogMessageRaw << "Utilities::ConvertBitmaskToString:  ERROR - pString parameter is NULL!" << endl;

        return false;
    }
    
    // Prime the string with NULL terminator.
    pString[0] = '\0';
    
    if (maxLength <= 1)
    {
        gLogMessageRaw << "Utilities::ConvertBitmaskToString:  ERROR - maxLength parameter is < 1!" << endl;

        return false;
    }
    if (pBitNames == NULL)
    {
        gLogMessageRaw << "Utilities::ConvertBitmaskToString:  ERROR - pBitNames parameter is NULL!" << endl;

        return false;
    }

    // Make sure the separator string is not NULL.
    if (pSeparatorString == NULL)
    {
        pSeparatorString = "|";
    }

    // Decrement this by 1 so that we don't overflow the buffer (leave room for
    // the \0).
    maxLength--;

    // If the bitmask is empty, then treat is specially.  Even so, iterate any
    // way, so that the wrap-up handling will be done.
    if (bitmask == 0)
    {
        strncat(pString, "{None}", maxLength);
    }

    // Concatenate the names for each bit that is set.
    bit = 0x80000000;
    index = 31;
    while (bit != 0)
    {
        if (bitmask & bit)
        {
            // If this is the first bit we've come across, then just concat the
            // name; otherwise, we want to concat the separator string before
            // doing the name.
            if (firstOne)
            {
                firstOne = false;
            }
            else
            {
                // Append the string, using up to max length chars total, so we need
                // to adjust for how many we have already used.
                strncat(pString, pSeparatorString, maxLength - charsAppended);

                // Adjust the number of bytes that we have used.
                charsAppended += strlen(pSeparatorString);

                // PR6576 - it is possible that appending the separator might
                // use up all the remaining characters, in which case I need to
                // stop.  Previously, we didn't stop, and could crash because
                // the strncat below would write past the end of the buffer.
                if (charsAppended >= maxLength)
                {
                    break;
                }
            }

            // If the name for this index is within range and is not NULL, then
            // use it; otherwise, use {Unknown}.
            if ((index < numberOfNames) && (pBitNames[index] != NULL))
            {
                pStringToCopy = pBitNames[index];
            }
            else
            {
                pStringToCopy = "{Unknown}";
            }

            // Append the string, using up to max length chars total, so we need
            // to adjust for how many we have already used.
            strncat(pString, pStringToCopy, maxLength - charsAppended);

            // Adjust the number of bytes that we have used.
            charsAppended += strlen(pStringToCopy);

            // If we filled the output string, then stop iterating.
            if (charsAppended >= maxLength)
            {
                break;
            }
        }

        // Try the next bit.
        bit >>= 1;
        index--;
    }

    // If we stopped before doing all bits, then we truncated the string.
    if (bit != 0)
    {
        gLogMessageRaw << "Utilities::ConvertBitmaskToString:  WARNING - the string was truncated!" << endl;

        return false;
    }

    return true;
}


#if (__DCPLUSPLUS__ == 1) || defined(TARGETOS_vxWorks) || defined(TARGETOS_eCos)

int strnicmp(const char *str1, const char *str2, int cnt)
{
    char c1, c2;
    for (;;cnt--, str1++, str2++)
    {
        if (cnt == 0) return 0;    /* Strings were equal */

        if (isupper(*str1))
            c1 = tolower(*str1);
        else c1 = *str1;   

        if (isupper(*str2))
            c2 = tolower(*str2);
        else c2 = *str2;   

        if (c1 == c2)
        {
            if (c1 == '\0') return 0;
        }
        else
        {
            if (c1 < c2)
                return -1;
            else return 1;
        }
    }
    /*NOTREACHED*/
}

int stricmp(const char *str1, const char *str2)
{
    int str1Len, str2Len, minLen;

    // Calculate the length of both strings, and select the shorter one.
    str1Len = strlen(str1);
    str2Len = strlen(str2);

    minLen = str1Len;
    if (str2Len < minLen)
    {
        minLen = str2Len;
    }

    // Leverage the strnicmp() function to compare the strings.  Note that I
    // compare for length + 1 characters; this causes the 0 terminator to be
    // included in the comparison.  If the strings are not the same length,
    // then they can't be treated as equal.  Including the 0 terminator causes
    // this to happen.  If the strings are the same length, then the 0
    // terminator will be at the same location, so equal strings will be
    // computed as such.
    return strnicmp(str1, str2, minLen + 1);
}

#endif // end of #if (__DCPLUSPLUS__ == 1) || defined(TARGETOS_vxWorks)

