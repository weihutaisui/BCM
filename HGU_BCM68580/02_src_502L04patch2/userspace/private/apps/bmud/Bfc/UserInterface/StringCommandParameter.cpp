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
//  Filename:       StringCommandParameter.cpp
//  Author:         David Pullen
//  Creation Date:  Feb 23, 2001
//
//****************************************************************************
//  Description:
//      This is the derived class for string parameters.  It handles parsing
//      a string, including some special processing (NULL means 0-length string
//      and " " can be used to delineate strings with spaces.
//
//      NOTE:  All of the string pointers in the class MUST be static strings
//             that will not change or be deleted, etc., because only the string
//             pointers are copied and stored!
//
//****************************************************************************

//********************** Include Files ***************************************

// My api and definitions...
#include "StringCommandParameter.h"

#include "MessageLog.h"

#include <string.h>

//********************** Local Types *****************************************

//********************** Local Constants *************************************

//********************** Local Variables *************************************

//********************** Class Method Implementations ************************


// Default/Initializing Constructor.  Initializes the state of the object,
// storing the values specified.
//
// If pParameterName is NULL, then "String" will be used.
//
// Parameters:
//      clientId - the id that the client wants to associate with this
//                 parameter.
//      isOptional - true if the parameter is optional, false if it is
//                   required.
//      pParameterName - the name of this parameter (for printing).
//      maxLength - the maximum length that the string can be.
//      parseToEol - for cases where the string parameter is expected to
//                   be the last or only parameter, this allows the user
//                   to type spaces in the string without enclosing them
//                   in quotes, forcing us to parse to the end of the line.
//
// Returns:  N/A
//
BcmStringCommandParameter::BcmStringCommandParameter(unsigned int clientId, 
                                                     bool isOptional,
                                                     const char *pParameterName,
                                                     unsigned int maxLength,
                                                     bool parseToEol) :
    BcmCommandParameter(clientId, isOptional, pParameterName, "String"),
    pfValue(NULL),
    fMaxLength(maxLength),
    fParseToEol(parseToEol)
{
    // Allocate memory for my string and prime it as empty.
    pfValue = new char[fMaxLength];
    pfValue[0] = '\0';
}


// Destructor.  Frees up any memory/objects allocated, cleans up internal
// state.
//
// Parameters:  N/A
//
// Returns:  N/A
//
BcmStringCommandParameter::~BcmStringCommandParameter()
{
    // Get rid of the memory.
    delete pfValue;
    pfValue = NULL;
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
ostream &BcmStringCommandParameter::PrintValue(ostream &outstream) const
{
    return outstream << "'" << pfValue << "'";
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
void BcmStringCommandParameter::PrintValue(char string[64]) const
{
    // This ensures that we don't copy too many bytes.
    memset(string, 0, 64);

    strncpy(string, pfValue, 64 - 1);
}


// These methods are object cast handlers that allow a pointer to the base
// class to be cast as the specified type, and to provide safe access to
// the data in the derived class.  If the client uses these methods, then
// it is guaranteed not to crash due to an incorrect typecast, because the
// base class will catch any incorrect casts and handle them safely.
//
// There is one of these for every type of data that derived classes handle.

// Only implement the one that I care about.
const char *BcmStringCommandParameter::AsString(void) const
{
    return pfValue;
}


// These are similar to the above methods, except that they allow a client
// to set the current value rather than getting it.  Again, these methods
// provide type safety, etc.
//
// There isn't a Set method for the flag parameter value; you must get the
// object associated with the flag parameter and call it's Set method.

// String, with \0 termination.
bool BcmStringCommandParameter::SetAsString(const char *pValue)
{
    // This ensures that we don't copy too many bytes.
    memset(pfValue, 0, fMaxLength);

    strncpy(pfValue, pValue, fMaxLength - 1);

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
bool BcmStringCommandParameter::ParseImpl(const char *pParameterString, 
                                          unsigned int &offset)
{
    const char *pStringStart, *pStringEnd;
    unsigned int stringLength;

    // Set the starting point for the string.
    pStringStart = pParameterString + offset;
    pStringEnd = pStringStart;

    // UNFINISHED - do I want to allow a mechanism for them to use " chars in
    // the string?  Right now, if " is the first char, then it will be skipped,
    // and the string will be terminated by a second " char.
    //
    // UNFINISHED - do I want to allow a mechanism for them to use NULL as a
    // real string, rather than an empty string?

    // If the string starts with ", then it must end with ".  Otherwise, it
    // ends with \0 or with a whitespace char.
    if (fParseToEol == true)
    {
        // Unconditionally seek the \0 terminator.
        while (*pStringEnd != '\0')
        {
            pStringEnd++;
        }
    }
    else if (*pStringStart == '"')
    {
        pStringStart++;
        pStringEnd++;

        while (*pStringEnd != '"')
        {
            if (*pStringEnd == '\0')
            {
                PrintError(pParameterString + offset, "Missing \" at end of string!");

                return false;
            }

            pStringEnd++;
        }
    }
    else
    {
        while ((*pStringEnd != ' ') && (*pStringEnd != '\t') &&
               (*pStringEnd != '\r') && (*pStringEnd != '\n') &&
               (*pStringEnd != '\0'))
        {
            pStringEnd++;
        }
    }

    stringLength = pStringEnd - pStringStart;

    // Make sure the string length is less than the max allowed.
    if (stringLength < fMaxLength)
    {
        // Copy from the source to my buffer, then \0 terminate.
        strncpy(pfValue, pStringStart, stringLength);
        pfValue[stringLength] = '\0';

        // If the user specified the special string "NULL" with no spaces or
        // other appendages, then truncate my string.
        if (strcmp(pfValue, "NULL") == 0)
        {
            pfValue[0] = '\0';
        }

        offset += (pStringEnd - (pParameterString + offset));

        // Skip the closing " if the string was in quotes.
        if (*pStringEnd == '"')
        {
            offset++;
        }

        return true;
    }

    // Don't print an error if this is an optional parameter.
    if (!IsOptional())
    {
        PrintError(pParameterString + offset, "Must be a string less than ", false);

        gLogMessageRaw << fMaxLength << " bytes!" << endl;
    }

    return false;
}


// Gives the derived class a chance to do some derived-class specific stuff
// during the print.
//
// Parameters:
//      outstream - the ostream to be used.
//
// Returns:
//      A reference to the ostream for chaining insertion operators.
//
ostream &BcmStringCommandParameter::PrintImpl(ostream &outstream) const
{
    // Print the max allowed characters, minus the null.
    return outstream << "{" << fMaxLength - 1 << "}";
}


