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
//  Filename:       CommandParameterList.h
//  Author:         David Pullen
//  Creation Date:  Feb 26, 2001
//
//****************************************************************************

#ifndef COMMANDPARAMETERLIST_H
#define COMMANDPARAMETERLIST_H

//********************** Include Files ***************************************

#include "CommandParameter.h"

// STL Container for the parameter objects.
#include <list>

#include <iostream>

//********************** Global Types ****************************************

//********************** Global Constants ************************************

//********************** Global Variables ************************************

//********************** Forward Declarations ********************************

//********************** Class Declaration ***********************************


/** \ingroup BfcConsole
*
*      This is a helper class that holds a set of BcmCommandParameter objects.
*      It knows how to iterate over the list in the right order and to get
*      each to parse itself from a string.
*/
class BcmCommandParameterList
{
public:

    /// Default Constructor.  Initializes the state of the object...
    ///
    BcmCommandParameterList(void);

    /// Destructor.  Frees up any memory/objects allocated, cleans up internal
    /// state.
    ///
    virtual ~BcmCommandParameterList();

    /// Parses the specified string of parameters, storing them in the objects
    /// contained in this object.  It tries the order-independent parameters
    /// (usually flag parameters) before doing the order-dependent parameters.
    ///
    /// Parsing starts by resetting all parameters to an un-parsed state.  Then,
    /// for each "word" in the parameter string, we check the order-independent
    /// parameters to see if it matches; if not, then we try to parse the next
    /// unparsed order-dependent parameter.
    ///
    /// In order for the parse to be successful, all of the required parameters
    /// must be parsed.  Also, any optional parameters that were present must
    /// be parsed correctly (without error).  Any extra un-used parameter "words"
    /// will be reported but won't be considered an error.
    /// 
    /// \param
    ///      pParameterString - pointer to the string of parameters to be parsed.
    ///
    /// \retval
    ///      true if parsing was successful.
    /// \retval
    ///      false if there was a problem.
    ///
    bool Parse(const char *pParameterString);

    /// Adds a new order-dependent parameter object to the list.  The parameters
    /// are stored and parsed in the order that they are added.
    ///
    /// The parameter object specified will be destroyed by this object; you must
    /// not destroy it yourself, and you must not add it to any other
    /// BcmCommandParameterList object.
    ///
    /// \note  All parameters added to this object should have a unique client
    ///        id value assigned to them; otherwise, Find() will only return the
    ///        first parameter object with the specified id!
    ///
    /// \param
    ///      pParameter - pointer to the object to be added.
    ///
    void AddOrderDependent(BcmCommandParameter *pParameter);

    /// Adds a new order-independent parameter object to the list.  This will
    /// usually be a flag parameter, since other types of parameters often
    /// don't have a way to delineate themselves.
    ///
    /// The parameter object specified will be destroyed by this object; you must
    /// not destroy it yourself, and you must not add it to any other
    /// BcmCommandParameterList object.
    ///
    /// \note  All parameters added to this object should have a unique client
    ///        id value assigned to them; otherwise, Find() will only return the
    ///        first parameter object with the specified id!
    ///
    /// \param
    ///      pParameter - pointer to the object to be added.
    ///
    void AddOrderIndependent(BcmCommandParameter *pParameter);

    /// Returns a pointer to the parameter object with the specified id.  Because
    /// this method uses the client id in the parameter objects as the key in the
    /// search, you should assign each parameter object a unique id.
    ///
    /// \param
    ///      clientId - the id of the parameter object to find.
    ///
    /// \return
    ///      A pointer to the object with the specified id, or NULL if not found.
    ///
    const BcmCommandParameter *Find(unsigned int clientId);

    /// Removes the parameter object with the specified id from the list and
    /// deletes it.
    ///
    /// \param
    ///      clientId - the id of the parameter object to delete.
    ///
    /// \retval
    ///      true if the parameter was found and deleted.
    /// \retval
    ///      false if the parameter was not found.
    ///
    bool Remove(unsigned int clientId);

    /// Removes and deletes all parameter objects from all lists.
    ///
    void RemoveAll(void);

    /// Prints itself (mainly the objects it contains) to the specified ostream.
    ///
    /// \param
    ///      outstream - the ostream to be used.
    ///
    /// \return
    ///      A reference to the ostream for chaining insertion operators.
    ///
    ostream &Print(ostream &outstream) const;

private:

    /// An STL object to store the order-dependent parameter objects.
    list<BcmCommandParameter *> fOrderDependentParameters;

    /// An STL object to store the order-independent parameter objects.
    list<BcmCommandParameter *> fOrderIndependentParameters;

private:

    /// Copy Constructor.  Not supported.
    BcmCommandParameterList(const BcmCommandParameterList &otherInstance);

    /// Assignment operator.  Not supported.
    BcmCommandParameterList & operator = (const BcmCommandParameterList &otherInstance);

};


//********************** Inline Method Implementations ***********************


/// Insertion operator for this class.  It allows you to do the following:
///
///    cout << commandParameter << endl;
///
inline ostream & operator << (ostream &outputStream, const BcmCommandParameterList &commandParameter)
{
    return commandParameter.Print(outputStream);
}


#endif


