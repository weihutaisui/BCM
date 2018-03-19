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
//  Filename:       StringSetCommandParameter.h
//  Author:         David Pullen
//  Creation Date:  Feb 23, 2001
//
//****************************************************************************

#ifndef STRINGSETCOMMANDPARAMETER_H
#define STRINGSETCOMMANDPARAMETER_H

//********************** Include Files ***************************************

// My base class.
#include "CommandParameter.h"

//********************** Global Types ****************************************

//********************** Global Constants ************************************

//********************** Global Variables ************************************

//********************** Class Declaration ***********************************


/** \ingroup BfcConsole
*
*      This is the derived class for string parameters which have a limited
*      set of values.  An example of this is a string parameter that can take
*      a value of "phs", "flow" and "cls", but nothing else.  It handles
*      parsing a string and making sure that it matches one of the values
*      specified.  It does not support using " chars to allow spaces or NULL
*      to mean a 0-length string.  It turns the string entered by the user into
*      a 0-based index, kind of like an enum.
*
*      The parser will allow the user to enter a partial string to match a
*      token; e.g. if the token string is "phs|flow|cls", then "phs" "ph" and
*      "p" will all match token 0.  However, "PHS" will not match, because the
*      matching is case-sensitive.
*
*      This implies that you must be careful in the order of the tokens.  If
*      the token string is "phs|flow|cls|flag", then "fl" will always match
*      token 1, and never token 3.
*
*      Note that the token string must not have spaces in it (because we won't
*      parse across a space in the input string).
*
*      \note  All of the string pointers in the class MUST be static strings
*             that will not change or be deleted, etc., because only the string
*             pointers are stored!
*/
class BcmStringSetCommandParameter : public BcmCommandParameter
{
public:

    /// Initializing Constructor.  Initializes the state of the object,
    /// storing the values specified.
    ///
    /// The values (tokens) that can be used for this parameter are specified in
    /// a string, where each token is separated by a | character.  The tokens
    /// will be used as the display name.  An example token string:
    ///
    ///      "phs|flow|cls"
    ///
    /// \param
    ///      pStringTokens - pointer to the string containing the valid values
    ///                      (tokens) that can be used.  Each token must be
    ///                      separated by a '|'.
    /// \param
    ///      clientId - the id that the client wants to associate with this
    ///                 parameter.
    /// \param
    ///      isOptional - true if the parameter is optional, false if it is
    ///                   required.
    ///
    BcmStringSetCommandParameter(const char *pStringTokens,
                                 unsigned int clientId = 0, 
                                 bool isOptional = false);

    /// Destructor.  Frees up any memory/objects allocated, cleans up internal
    /// state.
    ///
    virtual ~BcmStringSetCommandParameter();

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

    /// Prints the valid value that is accepted 
    /// This method is optional to the derived class.
    ///
    /// \param
    ///      string - the string into which the value should be printed.
    ///
    virtual void PrintValidValue(char string[64]) const;

public:

    /// Accessor for the value, using the type-specific method from the base
    /// class.  We only provide the ones that are relevant for our object type.
    ///
    /// \return
    ///     The value that was parsed or set.
    ///
    virtual unsigned int AsUnsignedInt(void) const;

public:

    /// Stores the specified value.
    ///
    /// \param
    ///     value - the value to be stored.
    ///
    /// \retval
    ///     true if the value was stored successfully.
    /// \retval
    ///     false if the value was not stored successfully.
    ///
    virtual bool SetAsUnsignedInt(unsigned int value);

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

private:

    /// The string of tokens specified by the user.
    const char *pfTokens;

    /// The 0-based index of the token that was parsed.
    unsigned int fTokenIndex;

private:

    /// Default Constructor.  Not supported.
    BcmStringSetCommandParameter(void);

    /// Copy Constructor.  Not supported.
    BcmStringSetCommandParameter(const BcmStringSetCommandParameter &otherInstance);

    /// Assignment operator.  Not supported.
    BcmStringSetCommandParameter & operator = (const BcmStringSetCommandParameter &otherInstance);

};


//********************** Inline Method Implementations ***********************


/// Insertion operator for this class.  It allows you to do the following:
///
///    cout << commandParameter << endl;
///
inline ostream & operator << (ostream &outputStream, BcmStringSetCommandParameter &commandParameter)
{
    return commandParameter.Print(outputStream);
}


#endif


