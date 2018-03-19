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
//  Filename:       FlagCommandParameter.h
//  Author:         David Pullen
//  Creation Date:  Feb 23, 2001
//
//****************************************************************************

#ifndef FLAGCOMMANDPARAMETER_H
#define FLAGCOMMANDPARAMETER_H

//********************** Include Files ***************************************

// My base class.
#include "CommandParameter.h"

//********************** Global Types ****************************************

//********************** Global Constants ************************************

//********************** Global Variables ************************************

//********************** Class Declaration ***********************************


/** \ingroup BfcConsole
*
*      This is the derived class for flag parameters.  A flag parameter
*      consists of a flag separator (usually '-'), a flag character, and 0 or
*      1 flag value parameters, which can be of any type.  Examples of this
*      are:
* \verbatim
    -a
    -s 1
    -n5
    -I 192.168.190.1
    -2
* \endverbatim
*
*      \note  Be careful to avoid confusion with the integer -2; avoid using
*             numbers as the flag character, even though it is allowed.
*
*      Currently the flag character is limited to a single character, and the
*      separator is always the '-' character.  The flag character is case
*      sensitive ('n' is not the same as 'N').
*
*      You can configure the flag and its value paramter such that the flag
*      is optional, but if present, then the value must be present, or you can
*      make the flag required and the value optional, etc.
*
*      \note  All of the string pointers in the class MUST be static strings
*             that will not change or be deleted, etc., because only the string
*             pointers are stored!
*/
class BcmFlagCommandParameter : public BcmCommandParameter
{
public:

    /// Default/Initializing Constructor.  Initializes the state of the object,
    /// storing the values specified.
    ///
    /// The flag character will be used as the name of the parameter.
    ///
    /// \note  If you specify a pValue object, then this object will take
    ///        ownership of the object and will delete it when this object is
    ///        deleted.
    ///
    /// \param
    ///      flagCharacter - the character to be used for the flag.
    /// \param
    ///      clientId - the id that the client wants to associate with this
    ///                 parameter.
    /// \param
    ///      isOptional - true if the parameter is optional, false if it is
    ///                   required.
    /// \param
    ///      pValue - Pointer to the parameter object to be used for the value.
    ///
    BcmFlagCommandParameter(char flagCharacter,
                            unsigned int clientId = 0, 
                            bool isOptional = true,
                            BcmCommandParameter *pValue = NULL);

    /// Destructor.  Frees up any memory/objects allocated, cleans up internal
    /// state.
    ///
    /// \note  If an object was passed in to the constructor for the pValue
    ///        parameter, it will be deleted by the destructor!
    ///
    virtual ~BcmFlagCommandParameter();

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
    virtual ostream &PrintValue(ostream &outstream) const;

    /// Prints the value that is currently stored (regardless of whether or not
    /// parsing was successful) to the specified string.
    ///
    /// This method must be implemented by the derived class.
    ///
    /// \param
    ///      string - the string into which the value should be printed.
    ///
    virtual void PrintValue(char string[64]) const;

public:

    /// Accessor for the value, using the type-specific method from the base
    /// class.  We only provide the ones that are relevant for our object type.
    ///
    /// \note  This can return NULL!
    ///
    /// \return
    ///     The command parameter object that was associated with the flag in
    ///     the constructor.  Query it for the value that was specified along
    ///     with the flag.
    ///
    virtual const BcmCommandParameter *AsFlagParameter(void) const;

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
    virtual bool ParseImpl(const char *pParameterString, unsigned int &offset);

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

private:

    /// The flag character.
    char fFlagCharacter;

    /// The optional flag value that can be parsed.
    BcmCommandParameter *pfValue;

private:

    /// Copy Constructor.  Not supported.
    BcmFlagCommandParameter(const BcmFlagCommandParameter &otherInstance);

    /// Assignment operator.  Not supported.
    BcmFlagCommandParameter & operator = (const BcmFlagCommandParameter &otherInstance);

};


//********************** Inline Method Implementations ***********************


/// Insertion operator for this class.  It allows you to do the following:
///
///    cout << commandParameter << endl;
///
inline ostream & operator << (ostream &outputStream, const BcmFlagCommandParameter &commandParameter)
{
    return commandParameter.Print(outputStream);
}


#endif


