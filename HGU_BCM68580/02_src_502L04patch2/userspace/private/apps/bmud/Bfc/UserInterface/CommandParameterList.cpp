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
//  Filename:       CommandParameterList.cpp
//  Author:         David Pullen
//  Creation Date:  Feb 27, 2001
//
//****************************************************************************
//  Description:
//      This is a helper class that holds a set of BcmCommandParameter objects.
//      It knows how to iterate over the list in the right order and to get
//      each to parse itself from a string.
//
//****************************************************************************

//********************** Include Files ***************************************

// My api and definitions...
#include "CommandParameterList.h"

#include "MessageLog.h"

//********************** Local Types *****************************************

//********************** Local Constants *************************************

//********************** Local Variables *************************************

//********************** Class Method Implementations ************************


// Default Constructor.  Initializes the state of the object...
//
// Parameters:  None.
//
// Returns:  N/A
//
BcmCommandParameterList::BcmCommandParameterList(void)
{
    // Nothing to do...
}


// Destructor.  Frees up any memory/objects allocated, cleans up internal
// state.
//
// Parameters:  N/A
//
// Returns:  N/A
//
BcmCommandParameterList::~BcmCommandParameterList()
{
    // Delete all of the parameters in my containers.
    RemoveAll();
}


// Parses the specified string of parameters, storing them in the objects
// contained in this object.  It tries the order-independent parameters
// (usually flag parameters) before doing the order-dependent parameters.
//
// Parsing starts by resetting all parameters to an un-parsed state.  Then,
// for each "word" in the parameter string, we check the order-independent
// parameters to see if it matches; if not, then we try to parse the next
// unparsed order-dependent parameter.
//
// In order for the parse to be successful, all of the required parameters
// must be parsed.  Also, any optional parameters that were present must
// be parsed correctly (without error).  Any extra un-used parameter "words"
// will be reported but won't be considered an error.
// 
// Parameters:
//      pParameterString - pointer to the string of parameters to be parsed.
//
// Returns:
//      true if parsing was successful.
//      false if there was a problem.
//
bool BcmCommandParameterList::Parse(const char *pParameterString)
{
    list<BcmCommandParameter *>::iterator orderDepIterator;
    list<BcmCommandParameter *>::iterator orderIndepIterator;
    unsigned int offset = 0;

    // Reset all parameters to an unparsed state.
    orderDepIterator = fOrderDependentParameters.begin();
    while (orderDepIterator != fOrderDependentParameters.end())
    {
        (*orderDepIterator)->ResetParsed();
        ++orderDepIterator;
    }

    orderIndepIterator = fOrderIndependentParameters.begin();
    while (orderIndepIterator != fOrderIndependentParameters.end())
    {
        (*orderIndepIterator)->ResetParsed();
        ++orderIndepIterator;
    }

    // Prime the order-dependent iterator; we only traverse this list once.
    orderDepIterator = fOrderDependentParameters.begin();

    // Continue until we reach the end of the parameter string.
    while (pParameterString[offset] != '\0')
    {
        // See if one of the order-independent parameters will claim this
        // parameter.  We need to do this each time, for each
        orderIndepIterator = fOrderIndependentParameters.begin();
        while (orderIndepIterator != fOrderIndependentParameters.end())
        {
            if ((*orderIndepIterator)->Parse(pParameterString, offset))
            {
                break;
            }

            ++orderIndepIterator;
        }

        // If it was not parsed so far, then let the next order-dependent
        // parameter try.
        if (orderIndepIterator == fOrderIndependentParameters.end())
        {
            if (orderDepIterator != fOrderDependentParameters.end())
            {
                // See if the parameter likes the string input; if not, but the
                // parameter is optional, then it may just be that the user
                // didn't enter the parameter, so we need to try the next object.
                if (((*orderDepIterator)->Parse(pParameterString, offset) == false) &&
                    ((*orderDepIterator)->IsOptional() == false))
                {
                    // If the required order-dependent parameter failed to parse
                    // it, then we can't continue.  Presumably, the parameter
                    // object already complained about the problem, so we don't
                    // need to do so ourselves.

                    return false;
                }
                
                ++orderDepIterator;
            }

            // If nobody parsed it, then warn about an unused parameter.  There
            // is a special case that we shouldn't complain about - whitespace
            // at the end of the parameters.
            else if (pParameterString[offset] != '\0')
            {
                // Skip whitespace, so we won't complain about a space at the end.
                // This belongs someplace else, but that will require re-thinking
                // parsing in general (Who should consume whitespace?).
                offset += strspn( pParameterString+offset, " \t" );

                if (pParameterString[offset] != '\0')
                {
                    gLogMessageRaw 
                        << "WARNING - Unrecognized/unused parameter(s) - '" 
                        << pParameterString + offset << "'; ignoring..." << endl;
                    
                    // We could skip this parameter and continue anyway; it seems
                    // unlikely that anything good can come of it, so I'll just
                    // break out of the loop and see if we got enough useful info
                    // to declare victory.
                    break;
                }
            }
        }
    }

    // Final check; only declare victory if all non-optional parameters were
    // parsed.
    orderDepIterator = fOrderDependentParameters.begin();
    while (orderDepIterator != fOrderDependentParameters.end())
    {
        if (((*orderDepIterator)->IsOptional() == false) &&
            ((*orderDepIterator)->WasParsed() == false))
        {
            gLogMessageRaw
                << "ERROR - required parameter (" << *(*orderDepIterator)
                << ") was not specified!" << endl;

            return false;
        }

        ++orderDepIterator;
    }

    orderIndepIterator = fOrderIndependentParameters.begin();
    while (orderIndepIterator != fOrderIndependentParameters.end())
    {
        if (((*orderIndepIterator)->IsOptional() == false) &&
            ((*orderIndepIterator)->WasParsed() == false))
        {
            gLogMessageRaw
                << "ERROR - required parameter (" << *(*orderIndepIterator)
                << ") was not specified!" << endl;

            return false;
        }

        ++orderIndepIterator;
    }

    // If we make it this far, then things must be ok.
    return true;
}


// Adds a new order-dependent parameter object to the list.  The parameters
// are stored and parsed in the order that they are added.
//
// The parameter object specified will be destroyed by this object; you must
// not destroy it yourself, and you must not add it to any other
// BcmCommandParameterList object.
//
// NOTE:  All parameters added to this object should have a unique client
//        id value assigned to them; otherwise, Find() will only return the
//        first parameter object with the specified id!
//
// Parameters:
//      pParameter - pointer to the object to be added.
//
// Returns:  Nothing.
//
void BcmCommandParameterList::AddOrderDependent(BcmCommandParameter *pParameter)
{
    if (pParameter != NULL)
    {
        // Check to see if something like this parameter is already in my list,
        // and warn the user about it.  This is a debug aid for the programmer.
        if (Find(pParameter->ClientId()))
        {
            gLogMessageRaw 
                << "WARNING:  Parameter with id " << pParameter->ClientId()
                << " already exists!" << endl;
        }

        fOrderDependentParameters.push_back(pParameter);
    }
}


// Adds a new order-independent parameter object to the list.  This will
// usually be a flag parameter, since other types of parameters often
// don't have a way to delineate themselves.
//
// The parameter object specified will be destroyed by this object; you must
// not destroy it yourself, and you must not add it to any other
// BcmCommandParameterList object.
//
// NOTE:  All parameters added to this object should have a unique client
//        id value assigned to them; otherwise, Find() will only return the
//        first parameter object with the specified id!
//
// Parameters:
//      pParameter - pointer to the object to be added.
//
// Returns:  Nothing.
//
void BcmCommandParameterList::AddOrderIndependent(BcmCommandParameter *pParameter)
{
    if (pParameter != NULL)
    {
        // Check to see if something like this parameter is already in my list,
        // and warn the user about it.  This is a debug aid for the programmer.
        if (Find(pParameter->ClientId()))
        {
            gLogMessageRaw 
                << "WARNING:  Parameter with id " << pParameter->ClientId()
                << " already exists!" << endl;
        }

        fOrderIndependentParameters.push_back(pParameter);
    }
}


// Returns a pointer to the parameter object with the specified id.  Because
// this method uses the client id in the parameter objects as the key in the
// search, you should assign each parameter object a unique id.
//
// Parameters:
//      clientId - the id of the parameter object to find.
//
// Returns:
//      A pointer to the object with the specified id, or NULL if not found.
//
const BcmCommandParameter *BcmCommandParameterList::Find(unsigned int clientId)
{
    list<BcmCommandParameter *>::iterator myIterator;

    // Try the order-dependent list first.
    myIterator = fOrderDependentParameters.begin();
    while (myIterator != fOrderDependentParameters.end())
    {
        if ((*myIterator)->ClientId() == clientId)
        {
            return *myIterator;
        }

        ++myIterator;
    }

    // Now try the order-independent list.
    myIterator = fOrderIndependentParameters.begin();
    while (myIterator != fOrderIndependentParameters.end())
    {
        if ((*myIterator)->ClientId() == clientId)
        {
            return *myIterator;
        }

        ++myIterator;
    }

    // If all else fails, return NULL.
    return NULL;
}


// Removes the parameter object with the specified id from the list and
// deletes it.
//
// Parameters:
//      clientId - the id of the parameter object to delete.
//
// Returns:
//      true if the parameter was found and deleted.
//      false if the parameter was not found.
//
bool BcmCommandParameterList::Remove(unsigned int clientId)
{
    list<BcmCommandParameter *>::iterator myIterator;

    // Try the order-dependent list first.
    myIterator = fOrderDependentParameters.begin();
    while (myIterator != fOrderDependentParameters.end())
    {
        if ((*myIterator)->ClientId() == clientId)
        {
            BcmCommandParameter *pParameter = *myIterator;

            fOrderDependentParameters.erase(myIterator);

            delete pParameter;

            return true;
        }

        ++myIterator;
    }

    // Now try the order-independent list.
    myIterator = fOrderIndependentParameters.begin();
    while (myIterator != fOrderIndependentParameters.end())
    {
        if ((*myIterator)->ClientId() == clientId)
        {
            BcmCommandParameter *pParameter = *myIterator;

            fOrderIndependentParameters.erase(myIterator);

            delete pParameter;

            return true;
        }

        ++myIterator;
    }

    // If all else fails, return false.
    return false;
}


// Removes and deletes all parameter objects from all lists.
//
// Parameters:  None.
//
// Returns:  Nothing.
//
void BcmCommandParameterList::RemoveAll(void)
{
    // Do the order-dependent list first.
    while (!fOrderDependentParameters.empty())
    {
        Remove(fOrderDependentParameters.front()->ClientId());
    }

    // Now do the order-independent list.
    while (!fOrderIndependentParameters.empty())
    {
        Remove(fOrderIndependentParameters.front()->ClientId());
    }
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
ostream &BcmCommandParameterList::Print(ostream &outstream) const
{
    list<BcmCommandParameter *>::const_iterator myIterator;

    // Print the order-independent list.
    myIterator = fOrderIndependentParameters.begin();
    while (myIterator != fOrderIndependentParameters.end())
    {
        outstream << *(*myIterator);

        ++myIterator;
    }

    // Print the order-dependent list.
    myIterator = fOrderDependentParameters.begin();
    while (myIterator != fOrderDependentParameters.end())
    {
        outstream << *(*myIterator);

        ++myIterator;
    }

    return outstream;
}



