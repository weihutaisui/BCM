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
//  Filename:       FlagCommandParameter.cpp
//  Author:         David Pullen
//  Creation Date:  Feb 23, 2001
//
//****************************************************************************
//  Description:
//      This is the derived class for flag parameters.  A flag parameter
//      consists of a flag separator (usually '-'), a flag character, and 0 or
//      1 flag value parameters, which can be of any type.  Examples of this
//      are:
//              -a
//              -s 1
//              -n5
//              -I 192.168.190.1
//              -2                  NOTE:  Be careful to avoid confusion with
//                                         the integer -2; avoid using numbers
//                                         as the flag character, even though
//                                         it is allowed.
//
//      Currently the flag character is limited to a single character, and the
//      separator is limited to the '-' character.  The flag character is case
//      sensitive (n is not the same as N).
//
//      You can configure the flag and its value paramter such that the flag
//      is optional, but if present, then the value must be present, or you can
//      make the flag requires and the value optional, etc.
//
//      NOTE:  All of the string pointers in the class MUST be static strings
//             that will not change or be deleted, etc., because only the string
//             pointers are copied and stored!
//
//****************************************************************************

//********************** Include Files ***************************************

// My api and definitions...
#include "FlagCommandParameter.h"

#include "MessageLog.h"

//********************** Local Types *****************************************

//********************** Local Constants *************************************

//********************** Local Variables *************************************

//********************** Class Method Implementations ************************


// Default/Initializing Constructor.  Initializes the state of the object,
// storing the values specified.
//
// The flag character will be used as the name of the parameter.
//
// Note:  If you specify a pValue object, then this object will take
//        ownership of the object and will delete it when this object is
//        deleted.
//
// Parameters:
//      flagCharacter - the character to be used for the flag.
//      clientId - the id that the client wants to associate with this
//                 parameter.
//      isOptional - true if the parameter is optional, false if it is
//                   required.
//      pValue - Pointer to the parameter object to be used for the value.
//
// Returns:  N/A
//
BcmFlagCommandParameter::BcmFlagCommandParameter(char flagCharacter,
                                                 unsigned int clientId, 
                                                 bool isOptional,
                                                 BcmCommandParameter *pValue) :
    BcmCommandParameter(clientId, isOptional, "", ""),
    fFlagCharacter(flagCharacter),
    pfValue(pValue)
{
    // Make sure the user didn't specify a stupid loop by giving me a pointer
    // to myself.
    if (pfValue == this)
    {
        gLogMessageRaw
            << "BcmFlagCommandParameter::BcmFlagCommandParameter: ERROR - "
            << "pValue == this; client specified a parameter loop!  Discarding parameter value!" << endl;

        pfValue = NULL;
    }
}


// Destructor.  Frees up any memory/objects allocated, cleans up internal
// state.
//
// NOTE:  If an object was passed in to the constructor for the pValue
//        parameter, then it will be deleted by the destructor!
//
// Parameters:  N/A
//
// Returns:  N/A
//
BcmFlagCommandParameter::~BcmFlagCommandParameter()
{
    // Delete the value parameter that was given to me.
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
ostream &BcmFlagCommandParameter::PrintValue(ostream &outstream) const
{
    const char *pNotString = "";

    if (WasParsed() == false)
    {
        pNotString = " NOT";
    }

    outstream << "was" << pNotString << " parsed ";

    if (pfValue != NULL)
    {
        pfValue->PrintValue(outstream);
    }

    return outstream;
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
void BcmFlagCommandParameter::PrintValue(char string[64]) const
{
    // This method is non-sensical for this derived class.  We provide this
    // dummy empty implementation only to satisfy the compiler.
    string[0] = '\0';
}


// These methods are object cast handlers that allow a pointer to the base
// class to be cast as the specified type, and to provide safe access to
// the data in the derived class.  If the client uses these methods, then
// it is guaranteed not to crash due to an incorrect typecast, because the
// base class will catch any incorrect casts and handle them safely.
//
// There is one of these for every type of data that derived classes handle.

// Only implement the one that I care about.  NOTE:  This can return NULL!
const BcmCommandParameter *BcmFlagCommandParameter::AsFlagParameter(void) const
{
    return pfValue;
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
bool BcmFlagCommandParameter::ParseImpl(const char *pParameterString, 
                                        unsigned int &offset)
{
    // See if the first 2 chars match the flag separator and character that we
    // are expecting.
    if ((pParameterString[offset] == '-') &&
        (pParameterString[offset + 1] == fFlagCharacter))
    {
        unsigned int newOffset;

        // Store this off in case my value fails.
        newOffset = offset + 2;

        // If I have a value parameter, then try to let it parse itself.  If
        // that fails, then declare failure.  Otherwise, declare success and
        // update the offset.
        if ((pfValue != NULL) &&
            (pfValue->Parse(pParameterString, newOffset) == false))
        {
            PrintError(pParameterString + offset);

            return false;
        }

        offset = newOffset;

        return true;
    }

    // If this isn't a flag, or doesn't match my flag character, then this is
    // a failure, but I don't print anything.
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
ostream &BcmFlagCommandParameter::PrintImpl(ostream &outstream) const
{
    // Print myself as a flag, then do my value if there is one.
    outstream << "-" << fFlagCharacter;

    if (pfValue != NULL)
    {
        outstream << *pfValue;
    }

    return outstream;
}


