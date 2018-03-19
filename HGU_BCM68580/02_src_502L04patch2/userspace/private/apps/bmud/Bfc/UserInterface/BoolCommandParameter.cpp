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
//  Filename:       BoolCommandParameter.cpp
//  Author:         David Pullen
//  Creation Date:  Feb 23, 2001
//
//****************************************************************************
//  Description:
//      This is the derived class for boolean parameters.  It handles parsing
//      a bool, and also handles YesNo types.
//
//      NOTE:  All of the string pointers in the class MUST be static strings
//             that will not change or be deleted, etc., because only the string
//             pointers are copied and stored!
//
//****************************************************************************

//********************** Include Files ***************************************

// My api and definitions...
#include "BoolCommandParameter.h"

#include "MessageLog.h"

//********************** Local Types *****************************************

//********************** Local Constants *************************************

static const char *gDefaultName[2] =
{
    "true|false",
    "yes|no"
};

//********************** Local Variables *************************************

//********************** Class Method Implementations ************************


// Default/Initializing Constructor.  Initializes the state of the object,
// storing the values specified.
//
// If pParameterName is NULL, then "true|false" or "yes|no" will be used,
// depending on the mode.
//
// Parameters:
//      clientId - the id that the client wants to associate with this
//                 parameter.
//      isOptional - true if the parameter is optional, false if it is
//                   required.
//      pParameterName - the name of this parameter (for printing).
//      modeIsYesNo - set this to true if you want it to parse and display
//                    itself as a YesNo value rather than true/false.
//
// Returns:  N/A
//
BcmBoolCommandParameter::BcmBoolCommandParameter(unsigned int clientId, 
                                                 bool isOptional,
                                                 const char *pParameterName,
                                                 bool modeIsYesNo) :
    BcmCommandParameter(clientId, isOptional, pParameterName, gDefaultName[modeIsYesNo]),
    fValue(false),
    fModeIsYesNo(modeIsYesNo)
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
BcmBoolCommandParameter::~BcmBoolCommandParameter()
{
    // Nothing else to do.
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
ostream &BcmBoolCommandParameter::PrintValue(ostream &outstream) const
{
    if (fModeIsYesNo)
    {
        return outstream << (YesNo) fValue;
    }

    return outstream << fValue;
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
void BcmBoolCommandParameter::PrintValue(char string[64]) const
{
    const char *pString;

    if (fModeIsYesNo)
    {
        if (fValue)
        {
            pString = "yes";
        }
        else
        {
            pString = "no";
        }
    }
    else
    {
        if (fValue)
        {
            pString = "true";
        }
        else
        {
            pString = "false";
        }
    }

    strcpy(string, pString);
}


// These methods are object cast handlers that allow a pointer to the base
// class to be cast as the specified type, and to provide safe access to
// the data in the derived class.  If the client uses these methods, then
// it is guaranteed not to crash due to an incorrect typecast, because the
// base class will catch any incorrect casts and handle them safely.
//
// There is one of these for every type of data that derived classes handle.

// Only implement the one(s) that I care about.
bool BcmBoolCommandParameter::AsBool(void) const
{
    return fValue;
}

YesNo BcmBoolCommandParameter::AsYesNo(void) const
{
    // Do some minor translation work.
    return (YesNo) fValue;
}


// These are similar to the above methods, except that they allow a client
// to set the current value rather than getting it.  Again, these methods
// provide type safety, etc.
//
// There isn't a Set method for the flag parameter value; you must get the
// object associated with the flag parameter and call it's Set method.

// Bool and YesNo types.  They are both serviced by the same derived class,
// and handle things like true/false, yes/no, enable/disable, on/off, etc.
bool BcmBoolCommandParameter::SetAsBool(bool value)
{
    fValue = value;

    return true;
}

bool BcmBoolCommandParameter::SetAsYesNo(YesNo value)
{
    fValue = (bool) value;

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
bool BcmBoolCommandParameter::ParseImpl(const char *pParameterString, 
                                        unsigned int &offset)
{
    bool goodValue = true;

    // Accept 1, t/T, y/Y as "true"; 0, f/F, n/N as "false".
    switch (pParameterString[offset])
    {
        case 't':
        case 'T':
        case '1':
        case 'y':
        case 'Y':
            fValue = true;
            break;

        case 'f':
        case 'F':
        case '0':
        case 'n':
        case 'N':
            fValue = false;
            break;

        default:
            goodValue = false;
            break;
    }

    // If we found a good value, then skip the offset forward until we reach
    // a whitespace char or the end of the string.
    if (goodValue)
    {
        while ((pParameterString[offset] != '\0') &&
               (pParameterString[offset] != '\t') &&
               (pParameterString[offset] != '\r') &&
               (pParameterString[offset] != '\n') &&
               (pParameterString[offset] != ' '))
        {
            offset++;
        }

        return true;
    }

    // Don't print an error if this is an optional parameter.
    if (!IsOptional())
    {
        PrintError(pParameterString + offset);
    }

    return false;
}


// Prints the valid value that is accepted 
// This method is optional to the derived class.
//
// \param
//      string - the string into which the value should be printed.
//
void BcmBoolCommandParameter::PrintValidValue(char string[64]) const
{
    // UNFINISHED - print the token string that this corresponds to?
    
    sprintf(string, "%.63s", fModeIsYesNo?gDefaultName[1]:gDefaultName[0]);
}

