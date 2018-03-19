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
//  Filename:       CommandParameter.cpp
//  Author:         David Pullen
//  Creation Date:  Feb 22, 2001
//
//****************************************************************************
//  Description:
//      This is the abstract base class for all types of command parameters.
//      Command parameters know how to print themselves to an ostream, and can
//      parse themselves from a string.
//
//      A console command can have 0 or more parameters, each of which can be
//      optional, and may have restrictions as to the type or range of the
//      value.  The actual data type of the value is handled by the derived
//      class.
//
//      NOTE:  All of the string pointers in the class MUST be static strings
//             that will not change or be deleted, etc., because only the string
//             pointers are copied and stored!
//
//****************************************************************************

//********************** Include Files ***************************************

// My api and definitions...
#include "CommandParameter.h"

#include "MessageLog.h"

#if (SUPPORT_INTERACTIVE_CONSOLE)
#include "ConsoleThread.h"
#endif

#include <string.h>

//********************** Local Types *****************************************

//********************** Local Constants *************************************

//********************** Local Variables *************************************

//********************** Class Method Implementations ************************


// Default/Initializing Constructor.  Initializes the state of the object,
// storing the values specified.
//
// Parameters:
//      clientId - the id that the client wants to associate with this
//                 parameter.
//      isOptional - true if the parameter is optional, false if it is
//                   required.
//      pParameterName - the name of this parameter (for printing).
//      pDefaultName - the default name string to be used if pParameterName
//                     is NULL.
//
// Returns:  N/A
//
BcmCommandParameter::BcmCommandParameter(unsigned int clientId, 
                                         bool isOptional,
                                         const char *pParameterName,
                                         const char *pDefaultName) :
    fClientId(clientId),
    fIsOptional(isOptional),
    fWasParsed(false),
    pfParameterName(pParameterName)
{
    // If the user didn't pass in a parameter name, then use the default passed
    // by the derived class.
    if (pfParameterName == NULL)
    {
        pfParameterName = pDefaultName;
    }
}


// Destructor.  Frees up any memory/objects allocated, cleans up internal
// state.
//
// Parameters:  N/A
//
// Returns:  N/A
//
BcmCommandParameter::~BcmCommandParameter()
{
    // Don't delete the name pointer, just clear it.
    pfParameterName = NULL;
}


// Tries to parse itself from the specified string, starting at the offset
// specified.  If successful, then the "was parsed" state will be true, the
// value will be as specified, and the offset parameter will be updated to
// reflect the offset of the next un-parsed character in the string.
//
// This method does some parameter checking to make sure things are ok,
// skips any whitespace at the front of the parameters, then it calls
// ParseImpl() to let the derived class to some work.  Based on the return
// code from ParseImpl(), this method will update the state.
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
bool BcmCommandParameter::Parse(const char *pParameterString, 
                                unsigned int &offset)
{
    bool result;

    // Make sure the pointer isn't NULL.
    if (pParameterString == NULL)
    {
        gLogMessageRaw 
            << "BcmCommandParameter::Parse: ERROR - "
            << "pParameterString is NULL!" 
            << endl;

        return false;
    }

    // Make sure the offset isn't past the end of the string.
    if (offset > strlen(pParameterString))
    {
        gLogMessageRaw 
            << "BcmCommandParameter::Parse: ERROR - "
            << "offset is past end of string!" 
            << endl;

        return false;
    }

    // Skip whitespace at offset.
    while ((pParameterString[offset] == ' ') ||
           (pParameterString[offset] == '\t') ||
           (pParameterString[offset] == '\r') ||
           (pParameterString[offset] == '\n'))
    {
        offset++;
    }

    // Call the derived class.  If successful, then set was-parsed to true.
    //
    // What if it returns false?  There could be a couple of reasons for this:
    //   - syntax error
    //   - value out of range
    //   - flag parameter doesn't match
    //   - flag parameter matches, but its value is missing, value out of range,
    //       or syntax error
    //
    // In all cases, I think that we don't want to call was-parsed false if this
    // attempt to parse failed, but a previous (or future) attempt succeeds.
    //
    // Required parameters are generally order-dependent, and will only be tried
    // once; initially was-parsed is false, and if they succeed, then was-parsed
    // will be true, and that parameter won't be checked again.  Otherwise,
    // there's a problem and parsing will stop with an error, so it still won't
    // be checked again.
    //
    // Optional parameters can be order-dependent or order-independent, and
    // order-independent parameters can be tried multiple times.  In fact, all
    // of them will be tried multiple times.  We don't want an optional flag
    // parameter to go from success to failure because the second time through
    // the flag didn't match.  However, if there is a parse error on a flag
    // that matches, then we might actually want this to be reported and
    // break out of parsing.
    //
    // UNFINISHED - I might want to change this API so that it returns a more
    // meaningful result.  Don't what I would do with it, though...
    result = ParseImpl(pParameterString, offset);

    if (result)
    {
        fWasParsed = true;
    }

    return result;
}


// Prints itself (mainly the parameter name, with various bits of accessory
// symbols to show optional, etc.) to the specified ostream.
//
// Parameters:
//      outstream - the ostream to be used.
//
// Returns:
//      A reference to the ostream for chaining insertion operators.
//
ostream &BcmCommandParameter::Print(ostream &outstream) const
{
    const char *pNameString = "{unnamed parm?}";

    if (pfParameterName != NULL)
    {
        pNameString = pfParameterName;
    }

    // Print a space, followed by the name, followed by any derived class
    // specific stuff.  If the parameter is optional, then enclose it in
    // "[]" chars.
    outstream << " ";

    if (fIsOptional)
    {
        outstream << "[";
    }

    outstream << pNameString;

    PrintImpl(outstream);

    if (fIsOptional)
    {
        outstream << "]";
    }

    return outstream;
}


// These methods are object cast handlers that allow a pointer to the base
// class to be cast as the specified type, and to provide safe access to
// the data in the derived class.  If the client uses these methods, then
// it is guaranteed not to crash due to an incorrect typecast, because the
// base class will catch any incorrect casts and handle them safely.
//
// There is one of these for every type of data that derived classes handle.

// Numbers, as signed/unsigned int, and float.
signed int BcmCommandParameter::AsSignedInt(void) const
{
    // Notice the strange way I have broken up all of the strings below.  They
    // are split up so that parts of the string that don't change are separated
    // from the parts that change from one function to the next.  This lets the
    // compiler collapse all of the indentical string down to a single string,
    // which saves a LOT of code size!
    gLogMessageRaw
        << "BcmCommandParameter::" << "AsSignedInt"
        << ": ERROR - This is not the correct type for this parameter!" << endl;

    return 0;
}

unsigned int BcmCommandParameter::AsUnsignedInt(void) const
{
    gLogMessageRaw
        << "BcmCommandParameter::" << "AsUnsignedInt"
        << ": ERROR - This is not the correct type for this parameter!" << endl;

    return 0;
}

float BcmCommandParameter::AsFloat(void) const
{
    gLogMessageRaw
        << "BcmCommandParameter::" << "AsFloat"
        << ": ERROR - This is not the correct type for this parameter!" << endl;

    return 0.0;
}

// MAC Address.
const BcmMacAddress &BcmCommandParameter::AsMacAddress(void) const
{
    gLogMessageRaw
        << "BcmCommandParameter::" << "AsMacAddress"
        << ": ERROR - This is not the correct type for this parameter!" << endl;

    return kAllZerosMac;
}

// IP Address.
const BcmIpAddress &BcmCommandParameter::AsIpAddress(void) const
{
    gLogMessageRaw
        << "BcmCommandParameter::" << "AsIpAddress"
        << ": ERROR - This is not the correct type for this parameter!" << endl;

    return kAllZerosIp;
}

// Bitmask.  You pass in the current value, and the derived class performs
// the operation that was specified by the user on it to yield the value
// that is returned.
uint32 BcmCommandParameter::AsBitmask(uint32 /*currentValue*/) const
{
    gLogMessageRaw
        << "BcmCommandParameter::" << "AsBitmask"
        << ": ERROR - This is not the correct type for this parameter!" << endl;

    return 0;
}

// Bool and YesNo types.  They are both serviced by the same derived class,
// and handle things like true/false, yes/no, enable/disable, on/off, etc.
bool BcmCommandParameter::AsBool(void) const
{
    gLogMessageRaw
        << "BcmCommandParameter::" << "AsBool"
        << ": ERROR - This is not the correct type for this parameter!" << endl;

    return false;
}

YesNo BcmCommandParameter::AsYesNo(void) const
{
    gLogMessageRaw
        << "BcmCommandParameter::" << "AsYesNo"
        << ": ERROR - This is not the correct type for this parameter!" << endl;

    return kNo;
}

// String, with \0 termination.
const char *BcmCommandParameter::AsString(void) const
{
    gLogMessageRaw
        << "BcmCommandParameter::" << "AsString"
        << ": ERROR - This is not the correct type for this parameter!" << endl;

    return "";
}

// Parameter associated with a flag parameter.  Note that this can return
// NULL if this isn't a flag parameter, or if there is no parameter
// associated with the flag.
const BcmCommandParameter *BcmCommandParameter::AsFlagParameter(void) const
{
    gLogMessageRaw
        << "BcmCommandParameter::" << "AsFlagParameter"
        << ": ERROR - This is not the correct type for this parameter!" << endl;

    return NULL;
}


// These are similar to the above methods, except that they allow a client
// to set the current value rather than getting it.  Again, these methods
// provide type safety, etc.
//
// There isn't a Set method for the flag parameter value; you must get the
// object associated with the flag parameter and call it's Set method.

// Numbers, as signed/unsigned int, and float.
bool BcmCommandParameter::SetAsSignedInt(signed int /*value*/)
{
    gLogMessageRaw
        << "BcmCommandParameter::" << "SetAsSignedInt"
        << ": ERROR - This is not the correct type for this parameter!" << endl;

    return false;
}

bool BcmCommandParameter::SetAsUnsignedInt(unsigned int /*value*/)
{
    gLogMessageRaw
        << "BcmCommandParameter::" << "SetAsUnsignedInt"
        << ": ERROR - This is not the correct type for this parameter!" << endl;

    return false;
}

bool BcmCommandParameter::SetAsFloat(float /*value*/)
{
    gLogMessageRaw
        << "BcmCommandParameter::" << "SetAsFloat"
        << ": ERROR - This is not the correct type for this parameter!" << endl;

    return false;
}

// MAC Address.
bool BcmCommandParameter::SetAsMacAddress(const BcmMacAddress & /*value*/)
{
    gLogMessageRaw
        << "BcmCommandParameter::" << "SetAsMacAddress"
        << ": ERROR - This is not the correct type for this parameter!" << endl;

    return false;
}

// IP Address.
bool BcmCommandParameter::SetAsIpAddress(const BcmIpAddress & /*value*/)
{
    gLogMessageRaw
        << "BcmCommandParameter::" << "SetAsIpAddress"
        << ": ERROR - This is not the correct type for this parameter!" << endl;

    return false;
}

// Bitmask.  You pass in the current value, and the derived class performs
// the operation that was specified by the user on it to yield the value
// that is returned.
bool BcmCommandParameter::SetAsBitmask(uint32 /*value*/)
{
    gLogMessageRaw
        << "BcmCommandParameter::" << "SetAsBitmask"
        << ": ERROR - This is not the correct type for this parameter!" << endl;

    return false;
}

// Bool and YesNo types.  They are both serviced by the same derived class,
// and handle things like true/false, yes/no, enable/disable, on/off, etc.
bool BcmCommandParameter::SetAsBool(bool /*value*/)
{
    gLogMessageRaw
        << "BcmCommandParameter::" << "SetAsBool"
        << ": ERROR - This is not the correct type for this parameter!" << endl;

    return false;
}

bool BcmCommandParameter::SetAsYesNo(YesNo /*value*/)
{
    gLogMessageRaw
        << "BcmCommandParameter::" << "SetAsYesNo"
        << ": ERROR - This is not the correct type for this parameter!" << endl;

    return false;
}

// String, with \0 termination.
bool BcmCommandParameter::SetAsString(const char * /*pValue*/)
{
    gLogMessageRaw
        << "BcmCommandParameter::" << "SetAsString"
        << ": ERROR - This is not the correct type for this parameter!" << endl;

    return false;
}


#if (SUPPORT_INTERACTIVE_CONSOLE)
// This method causes the user to be prompted to enter a value (who's type
// depends on the derived class).  The user will be presented the specified
// prompt string, which will be modified to contain the current value of the
// parameter.  You can set the current value via one of the Set*() methods
// above.
//
// The console will collect input until the user hits enter, and the derived
// class will be told to parse the value.  This will repeat until parsing is
// successful.
//
// Parameters:
//      pPromptString - the string to present to the user.
//
// Returns:  Nothing.
//
void BcmCommandParameter::PromptUser(const char *pPromptString)
{
    char parameter[64];
    char validValue[64];
    char promptString[512];
    BcmConsoleThread *pConsoleThread = BcmConsoleThread::GetSingletonInstance();
    bool result;
    bool wasOptional;
    unsigned int offset;

    if (pConsoleThread == NULL)
    {
        gLogMessageRaw
            << "BcmCommandParameter::PromptUser: ERROR - There is no console thread singleton!  Can't get input..." << endl;

        return;
    }

    // Set the default prompt string if one was not given.
    if (pPromptString == NULL)
    {
        pPromptString = "Enter value: ";
    }

    // Get the derived class to print its current value to our string.
    PrintValue(parameter);
    memset(validValue, 0, sizeof(validValue));
    PrintValidValue(validValue);

    // Build the prompt string with the current value.
    sprintf(promptString, "%.377s (%.64s) [%.64s] ", pPromptString, validValue, parameter);

    // Force the parameter to be mandatory; this gets it to complain about
    // parsing errors.
    wasOptional = fIsOptional;
    fIsOptional = false;

    do
    {
        // Get input from the user.
        pConsoleThread->GetCommandString(promptString, parameter, sizeof(parameter));

        // Special case; if the user just hit enter, then use the current value;
        // don't try to parse (since this will always fail).
        if (parameter[0] == '\0')
        {
            break;
        }

        // Call the routine to parse and potentially store it.
        offset = 0;
        result = Parse(parameter, offset);

    } while (result == false);

    // Restore the original optional state of the parameter.
    fIsOptional = wasOptional;
}
#endif

// Gives the derived class a chance to do some derived-class specific stuff
// during the print.
//
// Parameters:
//      outstream - the ostream to be used.
//
// Returns:
//      A reference to the ostream for chaining insertion operators.
//
ostream &BcmCommandParameter::PrintImpl(ostream &outstream) const
{
    // By default, don't do anything.
    return outstream;
}


// This is a helper method for printing error information.  It prints a
// standard error message, including the basic parameter object info and
// the string token that caused the problem (passed in as a parameter), as
// well as an optional error string that the derived class can provide.
//
// In some cases, the derived class' error string has variable data, so
// you can tell this method not to insert a newline, allowing you to append
// your class-specific data and append your own newline.
//
// Parameters:
//      pParameterString - pointer to the string containing the parameter
//                         that caused the problem.  The pointer must be
//                         set to the correct offset.
//      pErrorString - pointer to derived-class specific error message.  If
//                     NULL, then nothing is printed.
//      insertNewline - set this to false if you want to append your own
//                      error text and insert your own newline.
//
// Returns:  Nothing.
//
void BcmCommandParameter::PrintError(const char *pParameterString,
                                     const char *pErrorString,
                                     bool insertNewline) const
{
    unsigned int index = 0;

    gLogMessageRaw
        << "ERROR:  Invalid value for parameter" << *this << "!  '";

    while ((pParameterString[index] != ' ') &&
           (pParameterString[index] != '\t') &&
           (pParameterString[index] != '\0'))
    {
        gLogMessageRaw << pParameterString[index];

        index++;
    }

    if (pErrorString == NULL)
    {
        pErrorString = "";
    }

    gLogMessageRaw << "'  " << pErrorString;
    
    if (insertNewline)
    {
        gLogMessageRaw << endl;
    }
}

// Prints the valid value that is accepted 
// This method is optional to the derived class.
//
// \param
//      string - the string into which the value should be printed.
//
void BcmCommandParameter::PrintValidValue(char string[64]) const
{
}
