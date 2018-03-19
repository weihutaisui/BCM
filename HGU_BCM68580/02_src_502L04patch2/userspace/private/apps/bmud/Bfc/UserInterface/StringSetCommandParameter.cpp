//****************************************************************************
//
//  Copyright (c) 2001  Broadcom Corporation
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
//  Filename:       StringSetCommandParameter.cpp
//  Author:         David Pullen
//  Creation Date:  Feb 23, 2001
//
//****************************************************************************
//  Description:
//      This is the derived class for string parameters which have a limited
//      set of values.  An example of this is a string parameter that can take
//      a value of "phs", "flow" and "cls", but nothing else.  It handles
//      parsing a string and making sure that it matches one of the values
//      specified.  It does not support using " chars to allow spaces or NULL
//      to mean a 0-length string.  It turns the string entered by the user into
//      a 0-based index, kind of like an enum.
//
//      The parser will allow the user to enter a partial string to match a
//      token; e.g. if the token string is "phs|flow|cls", then "phs" "ph" and
//      "p" will all match token 0.  However, "PHS" will not match, because the
//      matching is case-sensitive.
//
//      This implies that you must be careful in the order of the tokens.  If
//      the token string is "phs|flow|cls|flag", then "fl" will always match
//      token 1, and never token 3.
//
//      Note that the token string must not have spaces in it (because we won't
//      parse across a space in the input string).
//
//      NOTE:  All of the string pointers in the class MUST be static strings
//             that will not change or be deleted, etc., because only the string
//             pointers are copied and stored!
//
//****************************************************************************

//********************** Include Files ***************************************

// My api and definitions...
#include "StringSetCommandParameter.h"

#include "MessageLog.h"

#include <string.h>

//********************** Local Types *****************************************

//********************** Local Constants *************************************

//********************** Local Variables *************************************

//********************** Class Method Implementations ************************


// Initializing Constructor.  Initializes the state of the object,
// storing the values specified.
//
// The values (tokens) that can be used for this parameter are specified in
// a string, where each token is separated by a | character.  The tokens
// will be used as the display name.  An example token string:
//
//      "phs|flow|cls"
//
// Parameters:
//      pStringTokens - pointer to the string containing the valid values
//                      (tokens) that can be used.  Each token must be
//                      separated by a space.
//      clientId - the id that the client wants to associate with this
//                 parameter.
//      isOptional - true if the parameter is optional, false if it is
//                   required.
//
// Returns:  N/A
//
BcmStringSetCommandParameter::BcmStringSetCommandParameter(const char *pStringTokens,
                                                           unsigned int clientId,
                                                           bool isOptional) :
    BcmCommandParameter(clientId, isOptional, pStringTokens, NULL),
    pfTokens(pStringTokens),
    fTokenIndex(0)
{
    // Nothing else to do.
}


// Destructor.  Frees up any memory/objects allocated, cleans up internal
// state.
//
// Parameters:  N/A
//
// Returns:  N/A
//
BcmStringSetCommandParameter::~BcmStringSetCommandParameter()
{
    // Just set this to NULL; I don't delete it.
    pfTokens = NULL;
}


// Prints the value that is currently stored (regardless of whether or not
// parsing was successful) to the specified ostream.
//
// This method must be implemented by the derived class.
//
// Parameters:
//      outstream - the ostream to be used.
//
// Returns:
//      A reference to the ostream for chaining insertion operators.
//
ostream &BcmStringSetCommandParameter::PrintValue(ostream &outstream) const
{
    unsigned int tokenIndex;
    unsigned int i = 0;
    const char *pTokens;
    char tokenString[64];

    pTokens = pfTokens;
    tokenIndex = fTokenIndex;
 
    while ((tokenIndex != 0) && (*pTokens != '\0'))
    {
        if (*pTokens == '|')
        {
            tokenIndex--;
    }
        pTokens++;
    }

    while ((*pTokens != '|') && (*pTokens != '\0') && (i < 63))
    {
       tokenString[i] = *pTokens;
       i++;
       pTokens++;
    }

    tokenString[i] = '\0';

    return outstream << pfTokens << " (" << tokenString << ")";
}


// Prints the value that is currently stored (regardless of whether or not
// parsing was successful) to the specified string.
//
// This method must be implemented by the derived class.
//
// Parameters:
//      outstream - the ostream to be used.
//
// Returns:  Nothing.
//
void BcmStringSetCommandParameter::PrintValue(char string[64]) const
{
    unsigned int tokenIndex;
    unsigned int i = 0;
    const char *pTokens;
    char tokenString[64];

    pTokens = pfTokens;
    tokenIndex = fTokenIndex;

    while ((tokenIndex != 0) && (*pTokens != '\0'))
    {
        if (*pTokens == '|')
        {
            tokenIndex--;
        }
        pTokens++;
    }

    while ((*pTokens != '|') && (*pTokens != '\0') && (i < 63))
    {
       tokenString[i] = *pTokens;
       i++;
       pTokens++;
    }

    tokenString[i] = '\0';

    strcpy(string,tokenString);

}


// These methods are object cast handlers that allow a pointer to the base
// class to be cast as the specified type, and to provide safe access to
// the data in the derived class.  If the client uses these methods, then
// it is guaranteed not to crash due to an incorrect typecast, because the
// base class will catch any incorrect casts and handle them safely.
//
// There is one of these for every type of data that derived classes handle.

// Only implement the one that I care about.
unsigned int BcmStringSetCommandParameter::AsUnsignedInt(void) const
{
    return fTokenIndex;
}


// These are similar to the above methods, except that they allow a client
// to set the current value rather than getting it.  Again, these methods
// provide type safety, etc.
//
// There isn't a Set method for the flag parameter value; you must get the
// object associated with the flag parameter and call it's Set method.

// Numbers, as signed/unsigned int, and float.
bool BcmStringSetCommandParameter::SetAsUnsignedInt(unsigned int value)
{
    // UNFINISHED - do bounds checking?  Maybe we should use the SetAsString()
    // method instead?
    fTokenIndex = value;

    return true;
}


// This must be implemented by the derived class.  It is a "template method"
// that allows the derived class to do the real work of parsing the string
// and updating its state.
//
// The derived class can assume that the pParameterString pointer is not
// NULL.
//
// Parameters:
//      pParameterString - pointer to the string containing the parameters
//                         to be parsed.  It will not be modified.
//      offset - the offset into the string where parsing should begin.
//
// Returns:
//      true if the parameter was successfully parsed.
//      false if the parameter was not successfully parsed.
//
bool BcmStringSetCommandParameter::ParseImpl(const char *pParameterString, 
                                             unsigned int &offset)
{
    unsigned int tokenIndex, bytesConsumed;
    const char *pTokens, *pString;
    bool matchFound;

    // Set the starting point for the tokens.
    pTokens = pfTokens;
    tokenIndex = 0;

    // Keep going until we reach the end of the tokens.
    while (*pTokens != '\0')
    {
        // Start at the beginning of the input string.
        pString = pParameterString + offset;
        bytesConsumed = 0;

        matchFound = false;

        // Keep going until we reach the end of the current token.
        while (1)
        {
            // If we have reached the end of the input string, then we haven't
            // found a mismatch, so this must be a match.
            if ((*pString == ' ') || (*pString == '\t') ||
                (*pString == '\0'))
            {
                matchFound = true;

                break;
            }

            // If the current char doesn't match the current token char, then
            // we have a known mismatch, so I need to skip ahead to then end of
            // the token.
            if (*pString != *pTokens)
            {
                while ((*pTokens != '|') && (*pTokens != '\0'))
                {
                    pTokens++;
                }

                break;
            }

            // Otherwise, good match so far, so try the next char.
            pString++;
            pTokens++;
            bytesConsumed++;
        }

        // If we found a good match, then use it and break out.  Note that a
        // match must consist of one or more characters.
        if (matchFound)
        {
            if (bytesConsumed > 0)
            {
                fTokenIndex = tokenIndex;
                offset += bytesConsumed;

                return true;
            }

            break;
        }

        // Start on the next token.
        tokenIndex++;

        // Skip the separator if we're on one.
        if (*pTokens == '|')
        {
            pTokens++;
        }
    }

    // Don't print an error if this is an optional parameter.
    if (!IsOptional())
    {
        PrintError(pParameterString + offset, "Must match one of the tokens!");
    }

    return false;
}

// Prints the valid value that is accepted 
// This method is optional to the derived class.
//
// \param
//      string - the string into which the value should be printed.
//
void BcmStringSetCommandParameter::PrintValidValue(char string[64]) const
{
    // UNFINISHED - print the token string that this corresponds to?
    sprintf(string, "%.63s", pfTokens);
}



