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
//  Filename:       FloatCommandParameter.cpp
//  Author:         David Pullen
//  Creation Date:  Feb 22, 2001
//
//****************************************************************************
//  Description:
//      This is the derived class for floating point parameters.  It handles
//      parsing a float, and limiting it to an optional range.
//
//      NOTE:  All of the string pointers in the class MUST be static strings
//             that will not change or be deleted, etc., because only the string
//             pointers are copied and stored!
//
//****************************************************************************

//********************** Include Files ***************************************

// My api and definitions...
#include "FloatCommandParameter.h"

#include "MessageLog.h"

#include <stdio.h>

//********************** Local Types *****************************************

//********************** Local Constants *************************************

//********************** Local Variables *************************************

//********************** Class Method Implementations ************************


// Default/Initializing Constructor.  Initializes the state of the object,
// storing the values specified.
//
// If pParameterName is NULL, then "Float" will be used.
//
// Parameters:
//      clientId - the id that the client wants to associate with this
//                 parameter.
//      isOptional - true if the parameter is optional, false if it is
//                   required.
//      pParameterName - the name of this parameter (for printing).
//      minValue - the minimum value that will be allowed.
//      maxValue - the maximum value that will be allowed.
//
// Returns:  N/A
//
BcmFloatCommandParameter::BcmFloatCommandParameter(unsigned int clientId, 
                                                   bool isOptional,
                                                   const char *pParameterName,
                                                   float minValue,
                                                   float maxValue) :
    BcmCommandParameter(clientId, isOptional, pParameterName, "Float"),
    fValue(0.0),
    fMinValue(minValue),
    fMaxValue(maxValue)
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
BcmFloatCommandParameter::~BcmFloatCommandParameter()
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
ostream &BcmFloatCommandParameter::PrintValue(ostream &outstream) const
{
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
void BcmFloatCommandParameter::PrintValue(char string[64]) const
{
    sprintf(string, "%f", fValue);
}


// These methods are object cast handlers that allow a pointer to the base
// class to be cast as the specified type, and to provide safe access to
// the data in the derived class.  If the client uses these methods, then
// it is guaranteed not to crash due to an incorrect typecast, because the
// base class will catch any incorrect casts and handle them safely.
//
// There is one of these for every type of data that derived classes handle.

// Only implement the one that I care about.
float BcmFloatCommandParameter::AsFloat(void) const
{
    return fValue;
}


// These are similar to the above methods, except that they allow a client
// to set the current value rather than getting it.  Again, these methods
// provide type safety, etc.
//
// There isn't a Set method for the flag parameter value; you must get the
// object associated with the flag parameter and call it's Set method.

// Numbers, as signed/unsigned int, and float.
bool BcmFloatCommandParameter::SetAsFloat(float value)
{
    fValue = value;

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
bool BcmFloatCommandParameter::ParseImpl(const char *pParameterString, 
                                         unsigned int &offset)
{
    int numScanned, newOffset;
    float newValue;

    // Try to parse the value.  numScanned will be 1 if a value was successfully
    // parsed, -1 if there was no parameter at all, or 0 if there was a
    // non-numerical parameter.
    numScanned = sscanf(pParameterString + offset, "%f%n", &newValue, &newOffset);

    if ((numScanned == 1) &&
        (newValue >= fMinValue) && (newValue <= fMaxValue))
    {
        fValue = newValue;
        offset += newOffset;

        return true;
    }

    // Don't print an error if this is an optional parameter.
    if (!IsOptional())
    {
        PrintError(pParameterString + offset, "Must be a floating point number ", false);

        gLogMessageRaw << fMinValue << ".." << fMaxValue << "!" << endl;
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
ostream &BcmFloatCommandParameter::PrintImpl(ostream &outstream) const
{
    // If the range was limited, then print the range.
    if ((fMinValue > -FLT_MAX) ||
        (fMaxValue < FLT_MAX))
    {
        outstream << "{" << fMinValue << ".." << fMaxValue << "}";
    }

    return outstream;
}


// Prints the valid value that is accepted 
// This method is optional to the derived class.
//
// \param
//      string - the string into which the value should be printed.
//
void BcmFloatCommandParameter::PrintValidValue(char string[64]) const
{
   sprintf(string, "%f..%f", fMinValue, fMaxValue); 
}
