//****************************************************************************
//
//  Copyright (c) 2000  Broadcom Corporation
//  All Rights Reserved
//  No portions of this material may be reproduced in any form without the
//  written permission of:
//          Broadcom Corporation
//          16251 Laguna Canyon Road
//          Irvine, California 92618
//  All information contained in this document is Broadcom Corporation
//  company private, proprietary, and trade secret.
//
//****************************************************************************
//  $Id$
//
//  Filename:       CommandTable.h
//  Author:         David Pullen
//  Creation Date:  Feb 11, 2000
//
//****************************************************************************
//  Description:
//      This is the container class that holds a set of related commands, with
//      some additional state.  It is used by the ConsoleThread singleton to
//      to find matching commands.
//
//      Commands are parsed here, and generally take the following format:
//
//          command [(#)] [parameters...]
//
//      "command" is the command to execute.
//      "(#)" is the optional instance to be called; is absent, the command is
//          executed for all instances.
//      "parameters" are the optional parameters for the command.
//
//      Examples:
//          help
//          help show_stats
//          show_stats (1) -all
//          ping 10.20.30.40
//          ping -n 20 -t 0 10.20.30.40
//
//****************************************************************************

#ifndef COMMANDTABLE_H
#define COMMANDTABLE_H

//********************** Include Files ***************************************

#include "typedefs.h"

#include "Command.h"

#include "MessageLog.h"

// The container used to hold things.  I chose an slist because it is efficient
// in memory use, and is fairly efficient for linear iteration.
//#include "slist.h"
// change to use list instead for portability to HP STL
#include <list>

#include <iostream>

//********************** Global Types ****************************************

//********************** Global Constants ************************************

//********************** Global Variables ************************************

//********************** Forward Declarations ********************************

class BcmMutexSemaphore;

//********************** Class Declaration ***********************************


class BcmCommandTable
{
public:

    // Initializing Constructor.  Initializes the state of the object.
    //
    // If pTableName or pTableCommandName is NULL or an empty string, then
    // "{unnamed}" will be used.
    // If pCommandPrompt is NULL or empty, then "Command> " will be used.
    //
    // Parameters:
    //      pTableName - the string name to be displayed for this command table.
    //      pTableCommandName - the name of the "command" that will be displayed
    //                          and used when the user types "cd" or "help".
    //      pCommandPrompt - the command prompt to be displayed when this table
    //                       is active.
    //      pTableDescription - descriptive text telling what kinds of commands
    //                          are in this table.
    //
    // Returns:  N/A
    //
    BcmCommandTable(const char *pTableName, const char *pTableCommandName,
                    const char *pCommandPrompt, 
                    const char *pTableDescription = NULL);

    // Destructor.  Frees up any memory/objects allocated, cleans up internal
    // state.
    //
    // Parameters:  N/A
    //
    // Returns:  N/A
    //
    virtual ~BcmCommandTable();

    // Accessors for various bits of state information.
    inline const char *Name(void);
    inline const char *FullName(void);
    inline const char *CommandPrompt(void);
    inline void SetCommandPrompt(char *);
    inline BcmCommandTable *ParentTable(void);
    inline void SetParentTable( BcmCommandTable * pParentTable );

    // Copies the full path from the parent table to this one into the specified
    // string buffer, using the table separator string specified.
    //
    // Parameters:
    //      pString - string into which the path should be placed.
    //      maxLen - the maximum number of chars that can be placed in the
    //               string, including the \0.
    //      pSeparator - the string that should be placed between each table.
    //
    // Returns:  Nothing.
    //
    void CommandPromptWithParents(char *pString, unsigned int maxLen,
                                  const char *pSeparator = "\\") const;

    // Copies the full path from the parent table to this one into the specified
    // string buffer, using the table separator string specified.  The difference
    // between this method and CommandPromptWithParents is simply that this one
    // returns the table names as opposed to the table prompts.
    //
    // Parameters:
    //      pString - string into which the path should be placed.
    //      maxLen - the maximum number of chars that can be placed in the
    //               string, including the \0.
    //      pSeparator - the string that should be placed between each table.
    //
    // Returns:  Nothing.
    //
    void NameWithParents(char *pString, unsigned int maxLen,
                             const char *pSeparator = "\\") const;

    // Allows a command to be added to the table.  It is added at the end of the
    // table.
    //
    // Parameters:
    //      command - the command to be added.  This is copied into the table.
    //
    // Returns:
    //      true if the command was added successfully.
    //      false if the command was not added.
    //
    bool AddCommand(BcmCommand &command);

    // Removes the specified command (based on the command id) from the table.
    //
    // Parameters:
    //      commandId - the id of the command to be removed.
    //
    // Returns:
    //      true if the command was removed.
    //      false if the command was not found.
    //
    bool RemoveCommand(unsigned int commandId);

    // Prints a list of all commands matching the pCommandName string to 
    // the console.  If instructed to do so, the method will recurse subtatbles.
    //
    // Parameters:
    //      pCommandName - the string to match
    //      recurse - true means recurse subtables, false means don't
    //
    // Returns:
    //      true if a match is found, false if not.
    //
    bool ListMatchingCommands (const char *pCommandName, bool recurse);

    // Allows you to changed the enable state of the specified command.
    //
    // Parameters:
    //      commandId - the id of the command to change.
    //      enable - set to true if the command is enabled, or false if it
    //               is disabled.
    //
    // Returns:
    //      true if the command state was changed.
    //      false if the command was not found.
    //
    bool EnableCommand(unsigned int commandId, bool enable);

    // Allows the client to ask whether or not the specified command is in this
    // table.  If so, then the command id is returned.
    //
    // Parameters:
    //      pCommandName - the string containing the command name.
    //      commandId - reference to a variable where the command id should be
    //                  stored.
    //
    // Returns:
    //      true if the command exists in this table.
    //      false if the command was not found.
    //
    bool IsValidCommand(char *pCommandName, unsigned int &commandId);

    // Helper method that returns the largest command id of all the commands
    // that are currently in the table.
    //
    // We are finding more and more that commands need to be added to a command
    // table in a piece-meal fashion in different modules (for vendor
    // extensions, etc).  Each module needs to know where to start numbering
    // their commands since they won't know the id values of other commands
    // that may have been added before them.
    //
    // Parameters:  None.
    //
    // Returns:
    //      The largest command id.
    //
    unsigned int LargestCommandId(void);

    // Returns the command string associated with the specified command id.
    // This is the string that is used when comparing against user input.
    //
    // Parameters:
    //      commandId - the id of the command.
    //
    // Returns:
    //      The command string.  This can return NULL!
    //
    const char *GetCommandString(unsigned int commandId);

    typedef enum
    {
        // This means that there was a problem with the command string (poorly
        // formatted, empty, etc.).  Further attempts to execute the command in
        // other tables will likely fail as well, so don't bother.
        kError,

        // This means that the command was not found, and should be looked for
        // in other tables.
        kNotFound,

        // This means that the command was found and processed according to its
        // state.  It may or may not have been executed, but either way, you
        // should not continue looking in other tables.
        kFound

    } ExecuteResult;

    // Searches the command table for the specified command, and calls the
    // command handler function specified for the registered instance(s) that
    // are specified.
    //
    // Parameters:
    //      pCommandString - the string containing the command (and parameters)
    //                       specified.
    //
    // Returns:
    //      One of the enum values above.
    //
    ExecuteResult ExecuteCommand(const char *pCommandString);

    // Function that can be overridden in case a specific command table
    // wants to handle an unknown command differently.
    //
    // Parameters:
    //      pCommandString - the string containing the command (and parameters)
    //                       specified.
    //
    // Returns:
    //      void.
    //
    virtual void CommandNotFound(const char *pCommandString);

    // Adds the specified command table to be added as a subtable of this
    // command table.
    //
    // Parameters:
    //      pSubtable - the command table to be added.
    //
    // Returns:
    //      true if the table was added.
    //      false if the table was not added (invalid pointer, etc.).
    //
    bool AddSubtable(BcmCommandTable *pSubtable);

    // Removes the specified subtable from this command table.
    //
    // Parameters:
    //      pSubtable - the command table to be removed.
    //
    // Returns:
    //      true if the table was removed.
    //      false if the table wasn't found.
    //
    bool RemoveSubtable(BcmCommandTable *pSubtable);

    // Allows the client to ask whether or not the specified command table is a
    // subtable of this command table.
    //
    // Parameters:
    //      pSubtableName - string containing the name to be searched for.
    //      pSubtable - reference to a subtable pointer where the subtable
    //                  address is to be returned if found.
    //
    // Returns:
    //      true if the subtable was found.
    //      false if the subtable was not found.
    //
    bool IsValidSubtable(const char *pSubtableName, BcmCommandTable *&pSubtable);

    // Allows a new object instance to register itself with the command table.
    // This is especially useful when you have, for example, a single command
    // table for controlling Packet Classifier behavior, but you can have
    // multiple instances of the Classifiers.  You may want to control only one
    // instance at a time, or you may want to control them all at the same time.
    //
    // Each instance of an object with a command table will register itself with
    // the command table using this method.  Note that this implies that there
    // is only one instance of the command table for all of the objects.
    //
    // Alternatively, in a regular C module, you may have an array of structs
    // that it controls.  There will be one table for the module, but each
    // struct in the array can be registered via this method so that they can
    // be independantly controlled.
    //
    // Parameters:
    //      pInstanceName - the string name you want to be displayed for this
    //                      instance.
    //      pInstanceValue - a value (usually the "this" pointer) associated
    //                       with this instance.  This value MUST be unique!
    //
    // Returns:
    //      true if the instance was added.
    //      false if the instance couldn't be added (or was previously added).
    //
    bool AddInstance(const char *pInstanceName, void *pInstanceValue);

    // Deregisters the specified instance from the command table (usually for
    // when the object is being destroyed).
    //
    // Parameters:
    //      pInstanceValue - the unique instance value that as passed in during
    //                       AddInstance (usually the "this" pointer).
    //
    // Returns:
    //      true if the instance was removed.
    //      false if the instance wasn't found.
    //
    bool RemoveInstance(void *pInstanceValue);

    // Writes general command format and usage information to the specified
    // ostream.
    //
    // Parameters:
    //      outputStream - the ostream to be written to.
    //
    // Returns:
    //      A reference to the ostream that was passed in (to support chaining).
    //
    ostream & PrintUsage(ostream &outputStream);

    typedef enum
    {
        // This shows only the command names, separated by a space.
        kCommandsOnly,

        // This shows the full command help text for all commands.
        kCommandsAndHelp,

        // Shows all of the instances that have registered themselves with the
        // command table.
        kInstancesOnly,

        // Shows all of the command help text and instances.
        kCommandsAndInstances,

        // Shows the command table name and descriptive text.
        kCommandTableInfo,

        // Shows the command table name and descriptive text, along with the
        // command names, separated by a space, and does the same for all of
        // the contained subtables.
        kTableInfoAndCommandsRecursive,

        // Shows the full command help text for all of the contained subtables
        kCommandsAndHelpRecursive

    } Verbosity;

    // Writes command table info to the specified ostream.  The amount of info
    // written depends on the verbosity specified.
    //
    // Parameters:
    //      outputStream - the ostream to be written to.
    //      verbosity - the amount of information to be written.
    //
    // Returns:
    //      A reference to the ostream that was passed in (to support chaining).
    //
    ostream & Print(ostream &outputStream, Verbosity verbosity = kCommandsAndInstances);

    // Writes the names of the registered instances to the specified ostream.
    // If an instance name is specified, then only instances whose names match
    // the specified string will be displayed; otherwise, all instances will be
    // displayed.  The name filtering is not case sensitive.
    //
    // Parameters:
    //      outputStream - the ostream to be written to.
    //      pInstanceName - a string containing the instance name on which to
    //                      filter the display.
    //
    // Returns:
    //      A reference to the ostream that was passed in (to support chaining).
    //
    ostream & PrintInstances(ostream &outputStream, const char *pInstanceName = NULL);

    // Searches for the specified command id and displays the full help text
    // associated with it.
    //
    // Parameters:
    //      outputStream - the ostream to be written to.
    //      commandId - the id of the command to be printed.
    //
    // Returns:
    //      A reference to the ostream that was passed in (to support chaining).
    //
    ostream & PrintCommand(ostream &outputStream, unsigned int commandId);

    // Same as above, but uses the command name in the search.
    //
    // Parameters:
    //      outputStream - the ostream to be written to.
    //      pCommandName - the name of the command to be printed.
    //
    // Returns:
    //      A reference to the ostream that was passed in (to support chaining).
    //
    ostream & PrintCommand(ostream &outputStream, char *pCommandName);

    typedef enum
    {
        kAbbreviated,
        kFull
    } SubtableVerbosity;

    // Writes information about the contained subtables to the specified ostream.
    //
    // Parameters:
    //      outputStream - the ostream to be written to.
    //      verbosity - the amount of information to be written.
    //
    // Returns:
    //      A reference to the ostream that was passed in (to support chaining).
    //
    ostream & PrintSubtables(ostream &outputStream, SubtableVerbosity verbosity);

private:

    // This is a helper method that searches for the specified command in the
    // list, based on the command id.
    //
    // NOTE:  The caller MUST lock the mutex before calling this method, and
    //        MUST unlock it after completing manipulations on the command!
    //
    // Parameters:
    //      commandId - the command id to search for.
    //      myIterator - the iterator to be used in the search.
    //
    // Returns:
    //      true if the command was found (and myIterator points to it).
    //      false if the command was NOT found (myIterator is invalid).
    //
    bool FindCommand(unsigned int commandId, 
                     list<BcmCommand>::iterator &myIterator);

    // Same as above, only it uses the command name for the search.
    //
    // NOTE:  The caller MUST lock the mutex before calling this method, and
    //        MUST unlock it after completing manipulations on the command!
    //
    // Parameters:
    //      pCommandString - the name of the command to be found.
    //      myIterator - the iterator to be used in the search.
    //
    // Returns:
    //      true if the command was found (and myIterator points to it).
    //      false if the command was NOT found (myIterator is invalid).
    //
    bool FindCommand(char *pCommandString, 
                     list<BcmCommand>::iterator &myIterator);

    // This is a helper method that searches for the specified subtable in the
    // list, based on the subtable address.
    //
    // NOTE:  The caller MUST lock the mutex before calling this method, and
    //        MUST unlock it after completing manipulations on the command!
    //
    // Parameters:
    //      pSubtable - the address of the subtable to search for.
    //      myIterator - the iterator to be used in the search.
    //
    // Returns:
    //      true if the subtable was found (and myIterator points to it).
    //      false if the subtable was NOT found (myIterator is invalid).
    //
    bool FindSubtable(BcmCommandTable *pSubtable,
                      list<BcmCommandTable *>::iterator &myIterator);

    class Instance
    {
    public:
        // begin functions required to be param for STL containers...
        //
        // default constructor.
        Instance()
          :
            pfInstanceName( 0 ),
            pfInstanceValue( 0 )
        {}
        //
        // copy constructor.
        Instance( const Instance& rhs )
          :
            pfInstanceName( rhs.pfInstanceName ),
            pfInstanceValue( rhs.pfInstanceValue )
        {}
        //
        // destructor.
        ~Instance() {}
        //
        // assignment operator.
        Instance& operator=( const Instance& rhs )
        {
            if( this == &rhs )
            {
                // special case: assignment to self
                return *this;
            }
            
            pfInstanceName = rhs.pfInstanceName;
            pfInstanceValue = rhs.pfInstanceValue;
            return *this;
        }

        //
        // less than operator.
        bool operator<( const Instance& rhs ) const
        {
           // actually junk result
            return (pfInstanceName < rhs.pfInstanceName);
        }
      
        //
        // equality operator.
        bool operator==( const Instance& rhs ) const
        {
            if ((pfInstanceName == rhs.pfInstanceName) && 
                (pfInstanceValue == rhs.pfInstanceValue))
            {
                return true;
            }

            return false;
        }
       // end functions required to be param for STL containers...
        
        // The name given by this instance during registration.
        char *pfInstanceName;

        // Some client-assigned value to be associated with the instance.
        // Generally, this will be the "this" pointer, but it could also be
        // something else, as long as it's unique.
        void *pfInstanceValue;

    };

    // This is a helper method that searches for the specified instance in the
    // list, based on the instance value.
    //
    // NOTE:  The caller MUST lock the mutex before calling this method, and
    //        MUST unlock it after completing manipulations on the command!
    //
    // Parameters:
    //      pInstanceValue - the instance value to search for.
    //      myIterator - the iterator to be used in the search.
    //
    // Returns:
    //      true if the instance was found (and myIterator points to it).
    //      false if the instance was NOT found (myIterator is invalid).
    //
    bool FindInstance(void *pInstanceValue, 
                      list<Instance>::iterator &myIterator);

protected:

    // The name of this command table.
    char *pfTableName;

    // The display name of the table, used for "help" and "cd" commands.
    char *pfTableCommandName;

    // The prompt to be displayed when this command table is active.
    char *pfCommandPrompt;

    // Descriptive text telling what commands are in this table.
    char *pfTableDescription;

    // The array of commands present in this table.  Note that the order of
    // the commands in the table is important; when it does a string search,
    // the first match is selected.
    list<BcmCommand> fCommands;

    // If I am a subtable, then this refers to my parent.
    BcmCommandTable *pfParentTable;

    // The set of subtables that have been associated with this table.
    list<BcmCommandTable *> fSubtables;

    // The set of object instances that have registered themselves with this
    // table.
    list<Instance> fInstances;

    // I need this to protect accesses to my STL objects against multiple
    // threads.
    BcmMutexSemaphore *pfMutex;

private:

    // Default Constructor.  Not supported.
    //
    // Parameters:  None.
    //
    // Returns:  N/A
    //
    BcmCommandTable(void);

    // Copy Constructor.  Not supported.
    //
    // Parameters:
    //      otherInstance - reference to the object to copy.
    //
    // Returns:  N/A
    //
    BcmCommandTable(const BcmCommandTable &otherInstance);

    // Assignment operator.  Not supported.
    //
    // Parameters:
    //      otherInstance - reference to the object to copy.
    //
    // Returns:
    //      A reference to "this" so that operator = can be chained.
    //
    BcmCommandTable & operator = (const BcmCommandTable &otherInstance);

};


//********************** Inline Method Implementations ***********************


// Accessors for various bits of state information.
inline const char *BcmCommandTable::Name(void)
{ 
    return pfTableCommandName; 
}

inline const char *BcmCommandTable::FullName(void)
{
    return pfTableName;
}

inline const char *BcmCommandTable::CommandPrompt(void) 
{ 
    return pfCommandPrompt; 
}

inline void BcmCommandTable::SetCommandPrompt(char * pCommandPrompt) 
{ 
    pfCommandPrompt = pCommandPrompt; 
}


inline BcmCommandTable *BcmCommandTable::ParentTable(void) 
{ 
    return pfParentTable; 
}

inline void BcmCommandTable::SetParentTable(BcmCommandTable * pParentTable) 
{ 
    pfParentTable = pParentTable; 
}


// Insertion operator for this class.  It allows you to do the following:
//
//    cout << commandTable << endl;
//
inline ostream & operator << (ostream &outputStream, BcmCommandTable &commandTable)
{
    return commandTable.Print(outputStream);
}


#endif


