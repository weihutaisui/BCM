//****************************************************************************
//
//  Copyright (c) 2001-2003  Broadcom Corporation
//  All Rights Reserved
//  No portions of this material may be reproduced in any form without the
//  written permission of:
//          Broadcom Corporation
//          16215 Alton Parkway
//          Irvine, California 92618
//  All information contained in this document is Broadcom Corporation
//  company private, proprietary, and trade secret.
//
//****************************************************************************
//  $Id$
//
//  Filename:       CommandParameter.h
//  Author:         David Pullen
//  Creation Date:  Feb 22, 2001
//
//****************************************************************************

#ifndef COMMANDPARAMETER_H
#define COMMANDPARAMETER_H

//********************** Include Files ***************************************

#include "MacAddress.h"
#include "IpAddress.h"

#include <iostream>

//********************** Global Types ****************************************

//********************** Global Constants ************************************

//********************** Global Variables ************************************

//********************** Class Declaration ***********************************


/** \ingroup BfcConsole
*
*      This is the abstract base class for all types of command parameters.
*      Command parameters know how to print themselves to an ostream, and can
*      parse themselves from a string.
*
*      A console command can have 0 or more parameters, each of which can be
*      optional, and may have restrictions as to the type or range of the
*      value.  The actual data type of the value is handled by the derived
*      class.
*
*      \note  All of the string pointers in the class MUST be static strings
*             that will not change or be deleted, etc., because only the string
*             pointers are stored!
*/
class BcmCommandParameter
{
public:

    /// Default/Initializing Constructor.  Initializes the state of the object,
    /// storing the values specified.
    ///
    /// \param
    ///      clientId - the id that the client wants to associate with this
    ///                 parameter.
    /// \param
    ///      isOptional - true if the parameter is optional, false if it is
    ///                   required.
    /// \param
    ///      pParameterName - the name of this parameter (for printing).
    /// \param
    ///      pDefaultName - the default name string to be used if pParameterName
    ///                     is NULL.
    ///
    BcmCommandParameter(unsigned int clientId = 0, bool isOptional = false,
                        const char *pParameterName = NULL,
                        const char *pDefaultName = NULL);

    /// Destructor.  Frees up any memory/objects allocated, cleans up internal
    /// state.
    ///
    virtual ~BcmCommandParameter();

    /// Returns the client id that was passed in to the constructor.
    ///
    /// \return
    ///      The client id.
    ///
    inline unsigned int ClientId(void) const;

    /// Resets the "was parsed" state.  This is generally called just before
    /// parsing a new command string.
    ///
    inline void ResetParsed(void);

    /// Tries to parse itself from the specified string, starting at the offset
    /// specified.  If successful, then the "was parsed" state will be true, the
    /// value will be as specified, and the offset parameter will be updated to
    /// reflect the offset of the next un-parsed character in the string.
    ///
    /// This method does some parameter checking to make sure things are ok,
    /// skips any whitespace at the front of the parameters, then it calls
    /// ParseImpl() to let the derived class to some work.  Based on the return
    /// code from ParseImpl(), this method will update the state.
    ///
    /// \param
    ///      pParameterString - pointer to the string containing the parameters
    ///                         to be parsed.  It will not be modified.
    /// \param
    ///      offset - the offset into the string where parsing should begin.
    ///
    /// \retval
    ///      true if the parameter was successfully parsed.
    /// \retval
    ///      false if the parameter was not successfully parsed.
    ///
    bool Parse(const char *pParameterString, unsigned int &offset);

    /// Tells whether or not the parameter is optional.
    ///
    /// \retval
    ///      true if the parameter is optional.
    /// \retval
    ///      false if the parameter is not optional.
    ///
    inline bool IsOptional(void) const;

    /// Tells whether or not the parameter was parsed since ResetParsed() was
    /// last called.  If this returns false, then it means that either the
    /// parameter was not present, or that a parameter was present but was
    /// not parsed because it was not valid (incorrect format/type, out of
    /// range, etc.).
    ///
    /// \retval
    ///      true if the parameter was parsed.
    /// \retval
    ///      false if the parameter was not parsed.
    ///
    inline bool WasParsed(void) const;

    /// Tells whether or not the parameter state is "happy".  This is generally
    /// called after finishing the parsing of a command string; if the parameter
    /// is optional, then this returns true; if the parameter is not optional,
    /// then it returns true only if something was successfully parsed.
    ///
    /// \retval
    ///      true if the state after parsing is ok.
    /// \retval
    ///      false if the state after parsing is not ok.
    ///
    inline bool StateIsOk(void) const;

    /// Prints itself (mainly the parameter name, with various bits of accessory
    /// symbols to show optional, etc.) to the specified ostream.
    ///
    /// \param
    ///      outstream - the ostream to be used.
    ///
    /// \return
    ///      A reference to the ostream for chaining insertion operators.
    ///
    ostream &Print(ostream &outstream) const;

    /// Prints the value that is currently stored (regardless of whether or not
    /// parsing was successful) to the specified ostream.
    ///
    /// This method must be implemented by the derived class.
    ///
    /// \param
    ///      outstream - the ostream to be used.
    ///
    /// \return
    ///      A reference to the ostream for chaining insertion operators.
    ///
    virtual ostream &PrintValue(ostream &outstream) const = 0;

    /// Prints the value that is currently stored (regardless of whether or not
    /// parsing was successful) to the specified string.
    ///
    /// This method must be implemented by the derived class.
    ///
    /// \param
    ///      string - the string into which the value should be printed.
    ///
    virtual void PrintValue(char string[64]) const = 0;
    
    /// Prints the valid value that is accepted 
    /// This method is optional to the derived class.
    ///
    /// \param
    ///      string - the string into which the value should be printed.
    ///
    virtual void PrintValidValue(char string[64]) const;

public:

    //@{
    /// These methods are type-cast handlers that allow a pointer to the base
    /// class to be cast as the specified type, and to provide safe access to
    /// the data in the derived class.  If the client uses these methods, then
    /// it is guaranteed not to crash due to an incorrect typecast, because the
    /// base class will catch any incorrect casts and handle them safely.
    ///
    /// There is one of these for every type of data that derived classes handle.
    ///
    /// See the derived class implementations of these for details on how they
    /// work.
    ///
    /// \return
    ///     A value appropriate for the type, but otherwise meaningless.  An
    ///     error message is printed if these methods are called in the base
    ///     class, since this means that the derived class didn't implement
    ///     them, and the object is being used incorrectly by the client.
    ///
    virtual signed int AsSignedInt(void) const;
    virtual unsigned int AsUnsignedInt(void) const;
    virtual float AsFloat(void) const;
    virtual const BcmMacAddress &AsMacAddress(void) const;
    virtual const BcmIpAddress &AsIpAddress(void) const;
    virtual uint32 AsBitmask(uint32 currentValue = 0) const;
    virtual bool AsBool(void) const;
    virtual YesNo AsYesNo(void) const;
    virtual const char *AsString(void) const;
    virtual const BcmCommandParameter *AsFlagParameter(void) const;
    //@}

public:

    //@{
    /// These are similar to the AsXxx methods, except that they allow a client
    /// to set the current value rather than getting it.  Again, these methods
    /// provide type safety for accessing the value stored in the derived
    /// class.
    ///
    /// There is no Set method for the flag parameter value; you must get the
    /// object associated with the flag parameter and call it's Set method.
    ///
    /// See the derived class implementations of these for details on how they
    /// work.
    ///
    /// \param
    ///     value - the value to be set/stored.
    ///
    /// \retval
    ///     false always; if the base class is called, an error message is
    ///     printed and it returns false.  This means that the object is being
    ///     used incorrectly by the client.
    ///
    virtual bool SetAsSignedInt(signed int value);
    virtual bool SetAsUnsignedInt(unsigned int value);
    virtual bool SetAsFloat(float value);
    virtual bool SetAsMacAddress(const BcmMacAddress &value);
    virtual bool SetAsIpAddress(const BcmIpAddress &value);
    virtual bool SetAsBitmask(uint32 value);
    virtual bool SetAsBool(bool value);
    virtual bool SetAsYesNo(YesNo value);
    virtual bool SetAsString(const char *pValue);
    //@}

#if (SUPPORT_INTERACTIVE_CONSOLE)
public:

    /// This method causes the user to be prompted to enter a value (who's type
    /// depends on the derived class).  The user will be presented the specified
    /// prompt string, which will be modified to contain the current value of the
    /// parameter.  You can set the current value via one of the Set*() methods
    /// above.
    ///
    /// The console will collect input until the user hits enter, and the derived
    /// class will be told to parse the value.  This will repeat until parsing is
    /// successful.
    ///
    /// \param
    ///      pPromptString - the string to present to the user.
    ///
    void PromptUser(const char *pPromptString = NULL);
#endif

protected:

    /// This must be implemented by the derived class.  It is a "template method"
    /// that allows the derived class to do the real work of parsing the string
    /// and updating its state.
    ///
    /// The derived class can assume that the pParameterString pointer is not
    /// NULL.
    ///
    /// \param
    ///      pParameterString - pointer to the string containing the parameters
    ///                         to be parsed.  It will not be modified.
    /// \param
    ///      offset - the offset into the string where parsing should begin.
    ///
    /// \retval
    ///      true if the parameter was successfully parsed.
    /// \retval
    ///      false if the parameter was not successfully parsed.
    ///
    virtual bool ParseImpl(const char *pParameterString, unsigned int &offset) = 0;

    /// Gives the derived class a chance to do some derived-class specific stuff
    /// during the print.
    ///
    /// \param
    ///      outstream - the ostream to be used.
    ///
    /// \return
    ///      A reference to the ostream for chaining insertion operators.
    ///
    virtual ostream &PrintImpl(ostream &outstream) const;

    /// This is a helper method for printing error information.  It prints a
    /// standard error message, including the basic parameter object info and
    /// the string token that caused the problem (passed in as a parameter), as
    /// well as an optional error string that the derived class can provide.
    ///
    /// In some cases, the derived class' error string has variable data, so
    /// you can tell this method not to insert a newline, allowing you to append
    /// your class-specific data and append your own newline.
    ///
    /// \param
    ///      pParameterString - pointer to the string containing the parameter
    ///                         that caused the problem.  The pointer must be
    ///                         set to the correct offset.
    /// \param
    ///      pErrorString - pointer to derived-class specific error message.  If
    ///                     NULL, then nothing is printed.
    /// \param
    ///      insertNewline - set this to false if you want to append your own
    ///                      error text and insert your own newline.
    ///
    void PrintError(const char *pParameterString,
                    const char *pErrorString = NULL, 
                    bool insertNewline = true) const;

private:

    /// Unique id assigned to us by the client.  For use by it to identify the
    /// parameter from a list of pointers to the base class.
    unsigned int fClientId;

    /// Tells whether or not the parameter is optional.
    bool fIsOptional;

    /// Tells whether or not the parameter was parsed.  This will generally be
    /// reset when a new string is being parsed, and checked at the end to see
    bool fWasParsed;

    /// The display name of this parameter.  Generally the derived class will
    /// assign this, but the client may want to override it.
    const char *pfParameterName;

private:

    /// Copy Constructor.  Not supported.
    BcmCommandParameter(const BcmCommandParameter &otherInstance);

    /// Assignment operator.  Not supported.
    BcmCommandParameter & operator = (const BcmCommandParameter &otherInstance);

};


//********************** Inline Method Implementations ***********************


/// Insertion operator for this class.  It allows you to do the following:
///
///    cout << commandParameter << endl;
///
inline ostream & operator << (ostream &outputStream, const BcmCommandParameter &commandParameter)
{
    return commandParameter.Print(outputStream);
}


inline unsigned int BcmCommandParameter::ClientId(void) const
{
    return fClientId;
}


inline void BcmCommandParameter::ResetParsed(void)
{
    fWasParsed = false;
}


inline bool BcmCommandParameter::IsOptional(void) const
{
    return fIsOptional;
}


inline bool BcmCommandParameter::WasParsed(void) const
{
    return fWasParsed;
}


inline bool BcmCommandParameter::StateIsOk(void) const
{
    if (fIsOptional == false)
    {
        return fWasParsed;
    }

    return true;
}


#endif


