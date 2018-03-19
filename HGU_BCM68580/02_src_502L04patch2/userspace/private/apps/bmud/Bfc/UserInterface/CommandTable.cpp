//****************************************************************************
//
// Copyright (c) 2000-2010 Broadcom Corporation
//
// This program is the proprietary software of Broadcom Corporation and/or
// its licensors, and may only be used, duplicated, modified or distributed
// pursuant to the terms and conditions of a separate, written license
// agreement executed between you and Broadcom (an "Authorized License").
// Except as set forth in an Authorized License, Broadcom grants no license
// (express or implied), right to use, or waiver of any kind with respect to
// the Software, and Broadcom expressly reserves all rights in and to the
// Software and all intellectual property rights therein.  IF YOU HAVE NO
// AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY,
// AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE
// SOFTWARE.  
//
// Except as expressly set forth in the Authorized License,
//
// 1.     This program, including its structure, sequence and organization,
// constitutes the valuable trade secrets of Broadcom, and you shall use all
// reasonable efforts to protect the confidentiality thereof, and to use this
// information only in connection with your use of Broadcom integrated circuit
// products.
//
// 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED
// "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS
// OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
// RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL
// IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR
// A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET
// ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME
// THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
//
// 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM
// OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
// INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY
// RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM
// HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN
// EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1,
// WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY
// FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
//
//****************************************************************************
//  $Id$
//
//  Filename:       CommandTable.cpp
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

//********************** Include Files ***************************************

// My api and definitions...
#include "CommandTable.h"

#include "BoolCommandParameter.h"

#include "MutexSemaphore.h"
#include "OperatingSystemFactory.h"

#include <string.h>
#include <iomanip>
#include <assert.h>
#include <ctype.h>

//********************** Local Types *****************************************

//********************** Local Constants *************************************

//********************** Local Variables *************************************

//********************** Class Method Implementations ************************


// Initializing Constructor.  Initializes the state of the object.
//
// If pTableName or pTableCommandName is NULL or an empty string, then
// "{unnamed}" will be used.
// If pCommandPrompt is NULL or empty, then "Command> " will be used.
//
// Parameters:
//      pTableName - the string name to be displayed for this command table.
//                   This is used primarily for changing subtables.
//      pTableCommandName - the name of the "command" that will be displayed
//                          and used when the user types "cd" or "help".
//      pCommandPrompt - the command prompt to be displayed when this table
//                       is active.
//      pTableDescription - descriptive text telling what kinds of commands
//                          are in this table.
//
// Returns:  N/A
//
BcmCommandTable::BcmCommandTable(const char *pTableName, 
                                 const char *pTableCommandName,
                                 const char *pCommandPrompt,
                                 const char *pTableDescription)
{
    // Set this to NULL early on so that the code below can deal with a wacky
    // re-entrancy condition (the mutex that I create tries to access me before
    // I finish my constructor.
    pfMutex = NULL;
    
    // Store the parameters passed in, with some validation.  Set the default
    // values if the string pointers are NULL or empty.
    if ((pTableName == NULL) || (pTableName[0] == '\0'))
    {
        //gLogMessageRaw << "BcmCommandTable::BcmCommandTable:  WARNING - Table name is empty; default will be assigned." << endl;

        pTableName = "{unnamed}";
    }

    pfTableName = new char[strlen(pTableName) + 1];

    strcpy(pfTableName, pTableName);

    if ((pTableCommandName == NULL) || (pTableCommandName[0] == '\0'))
    {
        //gLogMessageRaw << "BcmCommandTable::BcmCommandTable:  WARNING - Table command name is empty; default will be assigned." << endl;

        pTableCommandName = "{unnamed}";
    }

    pfTableCommandName = new char[strlen(pTableCommandName) + 1];

    strcpy(pfTableCommandName, pTableCommandName);

    if ((pCommandPrompt == NULL) || (pCommandPrompt[0] == '\0'))
    {
        //gLogMessageRaw << "BcmCommandTable::BcmCommandTable:  WARNING - Command prompt is empty; default will be assigned." << endl;

        pCommandPrompt = "Command> ";
    }

    pfCommandPrompt = new char[strlen(pCommandPrompt) + 1];

    strcpy(pfCommandPrompt, pCommandPrompt);

    if ((pTableDescription == NULL) || (pTableDescription[0] == '\0'))
    {
        //gLogMessageRaw << "BcmCommandTable::BcmCommandTable:  WARNING - Table description is empty; default will be assigned." << endl;

        pTableDescription = "{No description}";
    }

    pfTableDescription = new char[strlen(pTableDescription) + 1];

    strcpy(pfTableDescription, pTableDescription);

    // Set the parent table pointer to NULL.  This will be filled in later by
    // the command thread.
    pfParentTable = NULL;

    // Create my mutex semaphore to protect my STL object.
    pfMutex = BcmOperatingSystemFactory::NewMutexSemaphore(pfTableName);

    if (pfMutex == NULL)
    {
        gLogMessageRaw << "BcmCommandTable::BcmCommandTable:  ERROR - Failed to create a mutex semaphore!" << endl;

        assert(0);
    }
}


// Destructor.  Frees up any memory/objects allocated, cleans up internal
// state.
//
// Parameters:  N/A
//
// Returns:  N/A
//
BcmCommandTable::~BcmCommandTable()
{
    list<Instance>::iterator myIterator;

    for (myIterator = fInstances.begin(); myIterator != fInstances.end(); ++myIterator)
    {
        delete (*myIterator).pfInstanceName;
        (*myIterator).pfInstanceName = NULL;
    }
    
    // Free memory that was allocated, and NULL the pointers.
    delete pfTableName;
    pfTableName = NULL;

    delete pfTableCommandName;
    pfTableCommandName = NULL;

    delete pfCommandPrompt;
    pfCommandPrompt = NULL;

    delete pfTableDescription;
    pfTableDescription = NULL;

    // This protects against a wacky race condition - the mutex will register
    // itself with a command table (via its message log settings), which will
    // cause it to call into the command table as we are destroying it.  This
    // prevents this from happening, since the code below looks for a NULL
    // mutex.
    BcmMutexSemaphore *pMutex = pfMutex;
    pfMutex = NULL;
    delete pMutex;

    pfParentTable = NULL;
}


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
void BcmCommandTable::CommandPromptWithParents(char *pString, 
                                               unsigned int maxLen,
                                               const char *pSeparator) const
{
    if (pString != NULL)
    {
        pString[0] = '\0';

        // Let my parent go first.
        if (pfParentTable != NULL)
        {
            pfParentTable->CommandPromptWithParents(pString, maxLen, pSeparator);

            strncat(pString, pSeparator, maxLen - 1 - strlen(pString));
        }

        strncat(pString, pfCommandPrompt, maxLen - 1 - strlen(pString));
    }
}


// Copies the full path from the parent table to this one into the specified
// string buffer, using the table separator string specified.  The difference
// between this method and CommandPromptWithParents is simply that this one
// returns the table names as opposed to the table prompts.  Also, it does not
// include the name of the root directory, it just refers to it as '\'.
//
// Parameters:
//      pString - string into which the path should be placed.
//      maxLen - the maximum number of chars that can be placed in the
//               string, including the \0.
//      pSeparator - the string that should be placed between each table.
//
// Returns:  Nothing.
//
void BcmCommandTable::NameWithParents(char *pString, 
                                      unsigned int maxLen,
                                      const char *pSeparator) const
{
    if (pString != NULL)
    {
        pString[0] = '\0';

        // Let my parent go first.
        if (pfParentTable != NULL)
        {
          // Exclude the root command table, because you can't cd to it
          // by name.
          if (pfParentTable->pfParentTable != NULL)
          {
            pfParentTable->NameWithParents(pString, maxLen, pSeparator);
            strncat(pString, pSeparator, maxLen - 1 - strlen(pString));
          }

          strncat(pString, pfTableCommandName, maxLen - 1 - strlen(pString));
        }
    }
}


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
bool BcmCommandTable::AddCommand(BcmCommand &command)
{
    list<BcmCommand>::iterator myIterator;
    bool result;

    // Do some quick validation.
    if (
    #if !(BCM_REDUCED_IMAGE_SIZE)
    
    // DPullen - Aug 14, 2002 - remove the old-style command stuff for slim
    // images as part of a code shrink effort.  This should be done for non-slim
    // images as well, but there are still a few command tables that use the old
    // stuff, and I don't have the time or patience to convert them over and
    // test all of the commands to ensure that I haven't broken anything.
    
        (command.fCommandFunction == NULL) &&

    #endif

        (command.fCommandFunctionWithParms == NULL))
    {
        gLogMessageRaw << "BcmCommandTable::AddCommand:  ERROR - Command function is NULL!" << endl;

        return false;
    }

    if ((command.pfCommandName == NULL) || (strlen(command.pfCommandName) == 0))
    {
        //gLogMessageRaw << "BcmCommandTable::AddCommand:  WARNING - Command name is empty!  A default will be assigned..." << endl;

        command.pfCommandName = "{unnamed}";
    }
    // NOTE:  It is now legal for the command parameters string to be NULL or empty.
    /*if ((command.pfParameters == NULL) || (strlen(command.pfParameters) == 0))
    {
        //gLogMessageRaw << "BcmCommandTable::AddCommand:  WARNING - Usage help string is empty!  A default will be assigned..." << endl;

        command.pfParameters = "{No usage help available...}";
    }*/
    if ((command.pfCommandHelp == NULL) || (strlen(command.pfCommandHelp) == 0))
    {
        //gLogMessageRaw << "BcmCommandTable::AddCommand:  WARNING - Command help string is empty!  A default will be assigned..." << endl;

        command.pfCommandHelp = "{No command help available...}";
    }

    if (pfMutex == NULL)
    {
        gLogMessageRaw << "BcmCommandTable::AddCommand:  ERROR - my mutex is NULL!" << endl;

        return false;
    }

    // Protect my list.
    pfMutex->Lock();

    // Make sure the command Id and name don't already exist.
    result  = FindCommand(command.fCommandId, myIterator);
    result |= FindCommand((char *)command.pfCommandName, myIterator);

    if (result == false)
    {
        // Insert the command where its spot was found - in alphabetical order.
        fCommands.insert(myIterator, command);

        // Set this to NULL just in case the client doesn't!  I created a copy
        // of the command that was passed in, and it took ownership of the
        // parameter list object, so it will delete it.
        command.pfCommandParameterList = NULL;

        /*gLogMessageRaw << "BcmCommandTable::AddCommand:  INFO - Added command '"
             << command.pfCommandName << "' (" << command.fCommandId
             << ") to command table '" << pfTableName << "'." << endl;*/

        // Switch the sense of result.
        result = true;
    }
    else
    {
        // Log the error.
        gLogMessageRaw << "BcmCommandTable::AddCommand:  ERROR - "
             << "Attempted to add duplicate command '"
             << command.pfCommandName << "' (" << command.fCommandId 
             << ") to command table '" << pfTableName << "'." << endl;

        // Switch the sense of result.
        result = false;
    }

    // Unlock access to my list.
    pfMutex->Unlock();

    return result;
}


// Removes the specified command (based on the command id) from the table.
//
// Parameters:
//      commandId - the id of the command to be removed.
//
// Returns:
//      true if the command was removed.
//      false if the command was not found.
//
bool BcmCommandTable::RemoveCommand(unsigned int commandId)
{
    list<BcmCommand>::iterator myIterator;
    bool result;

    if (pfMutex == NULL)
    {
        gLogMessageRaw << "BcmCommandTable::RemoveCommand:  ERROR - my mutex is NULL!" << endl;

        return false;
    }

    // Protect my list.
    pfMutex->Lock();

    // Find and remove the command from the list.
    result = FindCommand(commandId, myIterator);

    if (result == true)
    {
        /*gLogMessageRaw << "BcmCommandTable::RemoveCommand:  INFO - Removed command '"
             << (*myIterator).pfCommandName << "' (" << (*myIterator).fCommandId
             << ") from command table '" << pfTableName << "'." << endl;*/

        // Remove it from the list.
        fCommands.erase(myIterator);
    }
    else
    {
        // Log the error.
        gLogMessageRaw << "BcmCommandTable::RemoveCommand:  ERROR - "
             << "Couldn't find command id "
             << commandId << " in command table '" << pfTableName << "'." << endl;
    }

    // Unlock access to my list.
    pfMutex->Unlock();

    return result;
}


// Little utility function used in ListMatchingCommands, below, to
// make a string all lower case (for case-insensitive comparisons).
// Drag that there isn't a standard "C" function for this...
int StrLower (const char *pSrc, char *pDst, int MaxLen)
{
  int Loop;
  
  memset (pDst, 0, MaxLen);
  
  for (Loop = 0; Loop < (int) strlen(pSrc); Loop++)
  {
    if (Loop == MaxLen)
      break;
      
    pDst[Loop] = tolower(pSrc[Loop]);
  }
  
  return Loop;
}


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
bool BcmCommandTable::ListMatchingCommands (const char *pCommandName, bool recurse)
{
    list<BcmCommand>::iterator commandIterator;
    char commandNameLwr[64], matchLwr[64], path[128], separator[] = "/";
    int numScanned, commandId;
    bool result = false;

    // Parse the command name.
    numScanned = sscanf(pCommandName, "%s", commandNameLwr);

    if (numScanned == 0)
    {
        return false;
    }

    if (pfMutex == NULL)
    {
        gLogMessageRaw << "BcmCommandTable::ListMatchingCommands:  ERROR - my mutex is NULL!" << endl;

        return false;
    }
    
    StrLower (pCommandName, commandNameLwr, 64);

    // Protect my list.
    pfMutex->Lock();

    // First, find any matching commands in my own table
    if (!fCommands.empty())
    {
        for (commandIterator = fCommands.begin(); commandIterator != fCommands.end(); ++commandIterator)
        {
            //if (strncmp((*commandIterator)->pfCommandName, pCommandName, strlen(pCommandName)) == 0)
            StrLower ((*commandIterator).pfCommandName, matchLwr, 64);
            if (strstr (matchLwr, commandNameLwr))
            {
                result = true;

                commandId = (*commandIterator).fCommandId;
                NameWithParents (path, sizeof(path)-1, separator);
                if (strlen (path) != 0)
                    gLogMessageRaw << separator << path << separator << GetCommandString (commandId) << endl;
                else
                    gLogMessageRaw << separator << GetCommandString (commandId) << endl;
            }
        }
    }

    // Then, recurse through any subtables if instructed to do so.
    if (recurse == true)
    {
        list<BcmCommandTable *>::iterator subtableIterator;

        if (!fSubtables.empty())
        {
            for (subtableIterator = fSubtables.begin(); subtableIterator != fSubtables.end(); ++subtableIterator)
            {
                if ((*subtableIterator)->ListMatchingCommands(pCommandName, recurse) == true)
                    result = true;
            }
        }
    }

    // Unlock access to my list.
    pfMutex->Unlock();

    return result;
}


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
bool BcmCommandTable::EnableCommand(unsigned int commandId, bool enable)
{
    list<BcmCommand>::iterator myIterator;
    bool result;
    
    if (pfMutex == NULL)
    {
        gLogMessageRaw << "BcmCommandTable::EnableCommand:  ERROR - my mutex is NULL!" << endl;

        return false;
    }

    // Protect my list.
    pfMutex->Lock();

    // Find and remove the command from the list.
    result = FindCommand(commandId, myIterator);

    if (result == true)
    {
        // Change the state.
        (*myIterator).fIsEnabled = enable;

        /*gLogMessageRaw << "BcmCommandTable::EnableCommand:  INFO - Set enabled state of command '"
             << (*myIterator).pfCommandName << "' (" << (*myIterator).fCommandId
             << ") to " << enable << "." << endl;*/
    }
    else
    {
        // Log the error.
        gLogMessageRaw << "BcmCommandTable::EnableCommand:  ERROR - "
             << "Couldn't find command id "
             << commandId << " in command table '" << pfTableName << "'." << endl;
    }

    // Unlock access to my list.
    pfMutex->Unlock();

    return result;
}


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
bool BcmCommandTable::IsValidCommand(char *pCommandName, unsigned int &commandId)
{
    list<BcmCommand>::iterator myIterator;
    char commandName[128];
    int numScanned;
    bool result;

    // Parse the command name.
    numScanned = sscanf(pCommandName, "%s", commandName);

    if (numScanned == 0)
    {
        return false;
    }

    if (pfMutex == NULL)
    {
        gLogMessageRaw << "BcmCommandTable::IsValidCommand:  ERROR - my mutex is NULL!" << endl;

        return false;
    }

    // Protect my list.
    pfMutex->Lock();

    // Find the command in the list.
    result = FindCommand(commandName, myIterator);

    if (result == true)
    {
        commandId = (*myIterator).fCommandId;
    }

    // Unlock access to my list.
    pfMutex->Unlock();

    return result;
}


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
unsigned int BcmCommandTable::LargestCommandId(void)
{
    list<BcmCommand>::iterator myIterator;
    unsigned int largestCommand = 0;

    if (pfMutex == NULL)
    {
        gLogMessageRaw << "BcmCommandTable::LargestCommandId:  ERROR - my mutex is NULL!" << endl;

        return 0;
    }

    // Protect my list.
    pfMutex->Lock();

    // Iterate over the list, storing the command id that is largest.
    myIterator = fCommands.begin();
    while (myIterator != fCommands.end())
    {
        if (largestCommand < (*myIterator).fCommandId)
        {
            largestCommand = (*myIterator).fCommandId;
        }

        ++myIterator;
    }

    // Unlock access to my list.
    pfMutex->Unlock();

    return largestCommand;
}


// Returns the command string associated with the specified command id.
// This is the string that is used when comparing against user input.
//
// Parameters:
//      commandId - the id of the command.
//
// Returns:
//      The command string.  This can return NULL!
//
const char *BcmCommandTable::GetCommandString(unsigned int commandId)
{
    list<BcmCommand>::iterator myIterator;
    bool result;
    const char *pCommandString = NULL;
    
    if (pfMutex == NULL)
    {
        gLogMessageRaw << "BcmCommandTable::GetCommandString:  ERROR - my mutex is NULL!" << endl;

        return NULL;
    }

    // Protect my list.
    pfMutex->Lock();

    // Find the command in the list.
    result = FindCommand(commandId, myIterator);

    if (result == true)
    {
        // Get the string.
        pCommandString = (*myIterator).pfCommandName;
    }
    else
    {
        // Log the error.
        gLogMessageRaw << "BcmCommandTable::GetCommandString:  ERROR - "
             << "Couldn't find command id "
             << commandId << " in command table '" << pfTableName << "'." << endl;
    }

    // Unlock access to my list.
    pfMutex->Unlock();

    return pCommandString;
}


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
BcmCommandTable::ExecuteResult BcmCommandTable::ExecuteCommand(const char *pCommandString)
{
    list<BcmCommand>::iterator myIterator;
    bool result;
    char commandName[128], instances[128];
    const char *pParameters;
    int numScanned, parameterOffset;
    ExecuteResult returnValue = kFound;
    
    // Do some quick validation.
    if (pCommandString == NULL)
    {
        gLogMessageRaw << "BcmCommandTable::ExecuteCommand:  ERROR - Command string is NULL!" << endl;

        return kError;
    }

    // Parse the command name, and consume any white space following it.  After
    // this, parameterOffset will contain the offset into the string of the
    // parameters.
    numScanned = sscanf(pCommandString, "%s%n %n", commandName, &parameterOffset,
                        &parameterOffset);

    if (numScanned < 1)
    {
        //gLogMessageRaw << "BcmCommandTable::ExecuteCommand:  INFO - No command specified!" << endl;

        return kError;
    }

    // For some reason, the previous sscanf won't consume any trailing
    // whitespace at the end of the string.  It will only consume it if there is
    // a non-whitespace char, so we must manually skip any trailing whitespace.
    while ((pCommandString[parameterOffset] != '\0') &&
           (isspace(pCommandString[parameterOffset])))
    {
        parameterOffset++;
    }

    memset(instances, 0, sizeof(instances));

    // If the first byte of the parameters is a '(', then this begins the list
    // of instances to which the command should apply.  The command's parameters
    // start after the ')'.  Otherwise, the parameters start where the offset
    // indicates.
    if (pCommandString[parameterOffset] == '(')
    {
        // Skip the '('.
        parameterOffset++;

        // Find the ')', if there is one.
        pParameters = strchr(pCommandString + parameterOffset, ')');
        if (pParameters != NULL)
        {
            // Copy the characters between the '(' and ')' into the instances
            // string.
            strncpy(instances, pCommandString + parameterOffset, 
                    pParameters - (pCommandString + parameterOffset));

            // Skip the ')'.
            pParameters++;
        }
        else
        {
            // The command string is poorly formatted.
            gLogMessageRaw << "ERROR - Missing ')' in the instance list." << endl;

            return kError;
        }
    }
    else
    {
        pParameters = pCommandString + parameterOffset;
    }

    if (pfMutex == NULL)
    {
        gLogMessageRaw << "BcmCommandTable::ExecuteCommand:  ERROR - my mutex is NULL!" << endl;

        return kError;
    }

    // Protect my list.
    pfMutex->Lock();

    // Find the command in the list.
    result = FindCommand(commandName, myIterator);

    if (result == true)
    {
        /*gLogMessageRaw << "BcmCommandTable::ExecuteCommand:  INFO - Selected command '"
             << (*myIterator).pfCommandName << "' (" << (*myIterator).fCommandId
             << ") in command table '" << pfTableName << "'." << endl;*/

        if ((*myIterator).fIsEnabled == false)
        {
            gLogMessageRaw << "Command '" << (*myIterator).pfCommandName
                 << "' is disabled!  Can't execute it..." << endl;
        }
        else if (!fInstances.empty())
        {
            unsigned int instanceNumberBegin, instanceNumberEnd, instanceNumber;
            char *pInstances = instances;
            unsigned int instanceOffset;
            bool firstTime = true;

            // If this is a new-style command (with parameter objects), then
            // let the objects parse the parameter string.
            if ((*myIterator).pfCommandParameterList != NULL)
            {
                bool parseResult;

                parseResult = (*myIterator).pfCommandParameterList->Parse(pParameters);

                if (!parseResult)
                {
                    // Display help for the command.
                    gLogMessageRaw << "\n" << (*myIterator);

                    // Don't bother to continue; parsing the parameters failed.
                    pfMutex->Unlock();

                    return kError;
                }
            }
            // Catch the case where a new-style command doesn't take any parameters.
            else if ((*myIterator).fCommandFunctionWithParms != NULL)
            {
                // If there were any parameters specified, report a warning.
                //
                // PR9395 - skip leading white space (strspn doesn't do what
                // we expect it to in all cases).
                //pParameters += strspn( " \t", pParameters );
                while ((*pParameters == ' ') || (*pParameters == '\t'))
                {
                    pParameters++;
                }
                if (*pParameters != '\0')
                {
                    gLogMessageRaw 
                        << "WARNING - Unrecognized/unused parameter(s) - '" 
                        << pParameters << "'; ignoring..." << endl;
                }
            }

            // By default, if no instances are specified, we do the whole range.
            instanceNumberBegin = 0;
            instanceNumberEnd = fInstances.size() - 1;

            do
            {
                // Try to parse the begin range value.
                instanceOffset = 0;
                numScanned = sscanf(pInstances, "%u%n %n",
                                    &instanceNumberBegin, &instanceOffset,
                                    &instanceOffset);

                // Skip this in the string.
                pInstances += instanceOffset;

                // If nothing was specified, and this is not the first time
                // around, then bail out.  If this is the first time around,
                // then we do all instances.
                if ((firstTime == false) && (numScanned < 1))
                {
                    break;
                }
                else if ((firstTime == true) && (numScanned < 1) &&
                         (fInstances.size() > 1))
                {
#if (SUPPORT_INTERACTIVE_CONSOLE)
                    // Otherwise, if this is the first time and nothing was
                    // specified, then this command will be applied to all
                    // registered instances.  This may be accidental, so we'll
                    // ask the user if he/she really wants to do this.
                    BcmBoolCommandParameter yesPleaseContinue(0, false, NULL, true);

                    gLogMessageRaw << "\nWARNING:  This will be applied to all " 
                                   << fInstances.size() << " registered instances!\n";

                    // Get input from the user.
                    yesPleaseContinue.PromptUser("Do you really want to do this?");

                    // If they answered no, then abort.
                    if (yesPleaseContinue.AsBool() == false)
                    {
                        break;
                    }
#else
                    break;
#endif
                }

                firstTime = false;

                if (numScanned > 0)
                {
                    // See if there is a range delimiter specified.  If so, then
                    // parse the end value.  Otherwise, set the end value to the
                    // begin value.
                    instanceNumberEnd = instanceNumberBegin;
                    if (*pInstances == '-')
                    {
                        // Skip the delimiter.
                        pInstances++;

                        // Try to parse the end range value.
                        instanceOffset = 0;
                        numScanned = sscanf(pInstances, "%u%n %n",
                                            &instanceNumberEnd, &instanceOffset,
                                            &instanceOffset);

                        // Skip this in the string.
                        pInstances += instanceOffset;

                        if (numScanned < 1)
                        {
                            // Allow an open-ended range to be specified.
                            instanceNumberEnd = fInstances.size() - 1;
                        }
                    }
                }

                // Now skip any value separators in the string to get to the 
                // next value.
                while ((*pInstances == ' ') || (*pInstances == '\t') ||
                       (*pInstances == ',') || (*pInstances == ';'))
                {
                    pInstances++;
                }

                // Verify that the begin is less than the end value.
                if (instanceNumberBegin > instanceNumberEnd)
                {
                    gLogMessageRaw << "ERROR - invalid instance range specified!  End value ("
                         << instanceNumberEnd << ") is less than begin value ("
                         << instanceNumberBegin << ")!" << endl;

                    break;
                }

                // Verify that both the begin and end are valid.
                if ((instanceNumberBegin < fInstances.size()) &&
                    (instanceNumberEnd < fInstances.size()))
                {
                    list<Instance>::iterator instanceIterator;

                    // Goto the first instance specified.
                    instanceIterator = fInstances.begin();
                    for (instanceNumber = 0; instanceNumber < instanceNumberBegin; instanceNumber++)
                    {
                        ++instanceIterator;
                    }

                    // Apply the command to all of the instances in the range.
                    for (instanceNumber = instanceNumberBegin; instanceNumber <= instanceNumberEnd; instanceNumber++)
                    {
                        /*gLogMessageRaw << "BcmCommandTable::ExecuteCommand:  INFO - Calling command handler for instance "
                             << instanceNumber << " ('" << (*instanceIterator).pfInstanceName
                             << "')..." << endl;*/

                        // only print instance number if there is more than 1 registered instance
                        if( fInstances.size() > 1 )
                        {
                            gLogMessageRaw << "\nInstance:  "
                                 << (*instanceIterator).pfInstanceName << " ("
                                 << (*instanceIterator).pfInstanceValue << ")\n" << endl;
                        }

                        #if !(BCM_REDUCED_IMAGE_SIZE)
    
                        // DPullen - Aug 14, 2002 - remove the old-style command
                        // stuff for slim images as part of a code shrink effort.
                        // This should be done for non-slim images as well, but
                        // there are still a few command tables that use the old
                        // stuff, and I don't have the time or patience to
                        // convert them over and test all of the commands to
                        // ensure that I haven't broken anything.
    
                        if ((*myIterator).fCommandFunction != NULL)
                        {
                            (*myIterator).fCommandFunction((*instanceIterator).pfInstanceValue,
                                                            (*myIterator).fCommandId,
                                                            pParameters);
                        }
                        else
                        
                        #endif
                        
                        if ((*myIterator).fCommandFunctionWithParms != NULL)
                        {
                            (*myIterator).fCommandFunctionWithParms((*instanceIterator).pfInstanceValue,
                                                                     (*myIterator));
                        }

                        ++instanceIterator;
                    }
                }
                else
                {
                    gLogMessageRaw << "ERROR - invalid instance range specified ("
                         << instanceNumberBegin << "-" << instanceNumberEnd << ")!  Range must be a between 0.."
                         << fInstances.size() - 1 << ", inclusive." << endl;

                    break;
                }

            } while (1);
        }
        else
        {
            gLogMessageRaw << "WARNING - no instances registered; can't execute the command!" << endl;
        }
    }
    else
    {
        // Log it.
        /*gLogMessageRaw << "BcmCommandTable::ExecuteCommand:  INFO - Couldn't find command '"
             << commandName << "' in command table '" << pfTableName << "'." << endl;*/

        returnValue = kNotFound;
    }
    
    // Unlock access to my list.
    pfMutex->Unlock();

    return returnValue;
}



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
void BcmCommandTable::CommandNotFound(const char *pCommandString)
{
    gLogMessageRaw << "Error - what Unknown command:  '"
        << pCommandString << "'" << endl;
}

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
bool BcmCommandTable::AddSubtable(BcmCommandTable *pSubtable)
{
    list<BcmCommandTable *>::iterator myIterator;
    bool result;

    if (pSubtable == NULL)
    {
        gLogMessageRaw << "BcmCommandTable::AddSubtable:  ERROR - The specified subtable is NULL!" << endl;

        return false;
    }

    if (pfMutex == NULL)
    {
        gLogMessageRaw << "BcmCommandTable::AddSubtable:  ERROR - my mutex is NULL!" << endl;

        return false;
    }

    // Protect my list.
    pfMutex->Lock();

    // Make sure this subtable isn't already in there.
    result = FindSubtable(pSubtable, myIterator);

    if (result == false)
    {
        // Link it to me.
        pSubtable->pfParentTable = this;

        // Insert the table in the position found - in alphabetical order.
        fSubtables.insert(myIterator, pSubtable);

        // Reverse the sense of the result.
        result = true;
    }
    else
    {
        gLogMessageRaw << "BcmCommandTable::AddSubtable:  ERROR - Attempted to add subtable " << 
            pSubtable->Name() << " that already exists!" << endl;

        // Reverse the sense of the result.
        result = false;
    }

    // Unlock access to my list.
    pfMutex->Unlock();

    return result;
}


// Removes the specified subtable from this command table.
//
// Parameters:
//      pSubtable - the command table to be removed.
//
// Returns:
//      true if the table was removed.
//      false if the table wasn't found.
//
bool BcmCommandTable::RemoveSubtable(BcmCommandTable *pSubtable)
{
    list<BcmCommandTable *>::iterator myIterator;
    bool result;

    if (pfMutex == NULL)
    {
        gLogMessageRaw << "BcmCommandTable::RemoveSubtable:  ERROR - my mutex is NULL!" << endl;

        return false;
    }

    // Protect my list.
    pfMutex->Lock();

    // Make sure this subtable isn't already in there.
    result = FindSubtable(pSubtable, myIterator);

    if (result == true)
    {
        // Unlink it from me.
        (*myIterator)->pfParentTable = NULL;

        // Remove it from the list.
        fSubtables.erase(myIterator);
    }
    else
    {
        // Log the error.
        gLogMessageRaw << "BcmCommandTable::RemoveSubtable:  ERROR - Specified subtable doesn't exist; can't remove it!" << endl;
    }

    // Unlock access to my list.
    pfMutex->Unlock();

    return result;
}


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
bool BcmCommandTable::IsValidSubtable(const char *pSubtableName, 
                                      BcmCommandTable *&pSubtable)
{
    list<BcmCommandTable *>::iterator myIterator;
    bool result;
    char nameParameter[128];
    int numScanned;

    if (pSubtableName == NULL)
    {
        gLogMessageRaw << "BcmCommandTable::IsValidSubtable:  ERROR - Subtable name string is NULL!" << endl;

        return false;
    }

    // Parse the first parameter.
    numScanned = sscanf(pSubtableName, "%s", nameParameter);
    if (numScanned == 1)
    {
        // If that worked, then see if there is a '[' as the first character.
        // If so, then eliminate it from the string.
        if (nameParameter[0] == '[')
        {
            int length;

            strncpy(nameParameter, nameParameter + 1, strlen(nameParameter));

            // Now see if the last character is a ']'.  If so, then eliminate it
            // as well.
            length = strlen(nameParameter);
            if ((length > 0) &&
                (nameParameter[length - 1] == ']'))
            {
                nameParameter[length - 1] = '\0';
            }
        }
    }

    if ((numScanned == 0) || (strlen(nameParameter) == 0))
    {
        // This isn't an error, it just doesn't match any entries.
        return false;
    }

    if (pfMutex == NULL)
    {
        gLogMessageRaw << "BcmCommandTable::IsValidSubtable:  ERROR - my mutex is NULL!" << endl;

        return false;
    }

    // Protect my list.
    pfMutex->Lock();

    // I need to look for the subtable based on its name.
    result = false;
    for (myIterator = fSubtables.begin(); myIterator != fSubtables.end(); ++myIterator)
    {
        // Compare the name to the string specified.
        if (strncmp((*myIterator)->Name(), nameParameter, strlen(nameParameter)) == 0)
        {
            result = true;
            pSubtable = *myIterator;

            break;
        }
    }

    // Unlock access to my list.
    pfMutex->Unlock();

    return result;
}


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
bool BcmCommandTable::AddInstance(const char *pInstanceName, void *pInstanceValue)
{
    list<Instance>::iterator myIterator;
    bool result;

    // Do some quick validation.
    if ((pInstanceName == NULL) || (strlen(pInstanceName) == 0))
    {
        //gLogMessageRaw << "BcmCommandTable::AddInstance:  WARNING - Instance name is empty; assigning default name." << endl;

        pInstanceName = "{unnamed}";
    }

    // Protect my list.  I will allow this one to run even if the mutex is NULL,
    // because the mutex itself will try to access me during its construction!
    if (pfMutex != NULL)
    {
        pfMutex->Lock();
    }

    // Make sure the instance doesn't already exist.
    result = FindInstance(pInstanceValue, myIterator);

    if (result == false)
    {
        Instance instance;
    
        // Set up the structure.
        instance.pfInstanceName = new char[strlen(pInstanceName) + 1];
        instance.pfInstanceValue = pInstanceValue;

        strcpy(instance.pfInstanceName, pInstanceName);

        // Just add it to the end of the table.
        fInstances.insert(fInstances.end(), instance);

        /*gLogMessageRaw << "BcmCommandTable::AddInstance:  INFO - Added instance '"
             << pInstanceName << "' (" << pInstanceValue << ") to command table '"
             << pfTableName << "'." << endl;*/

        // Switch the sense of result.
        result = true;
    }
    else
    {
        // Log the error.
        gLogMessageRaw << "BcmCommandTable::AddInstance:  ERROR - "
             << "Attempted to add duplicate instance '"
             << pInstanceName << "' (" << pInstanceValue
             << ") to command table '" << pfTableName << "'." << endl;

        // Switch the sense of result.
        result = false;
    }

    // Unlock access to my list.
    if (pfMutex != NULL)
    {
        pfMutex->Unlock();
    }

    return result;
}


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
bool BcmCommandTable::RemoveInstance(void *pInstanceValue)
{
    list<Instance>::iterator myIterator;
    bool result;

    // Silently exit this one if the mutex is NULL.  This happens when my
    // mutex is being destroyed (in my destructor!), so I need to handle this
    // gracefully.
    if (pfMutex == NULL)
    {
        return false;
    }

    // Protect my list.
    pfMutex->Lock();

    // Find and remove the command from the list.
    result = FindInstance(pInstanceValue, myIterator);
    
    if (result == true)
    {
        /*gLogMessageRaw << "BcmCommandTable::RemoveInstance:  INFO - Removed instance '"
             << (*myIterator).pfInstanceName << "' ("
             << (*myIterator).pfInstanceValue << ") from command table '"
             << pfTableName << "'." << endl;*/

        // Free the instance name memory.
        delete (*myIterator).pfInstanceName;
        (*myIterator).pfInstanceName = NULL;

        // Remove it from the list.
        fInstances.erase(myIterator);
    }
    else
    {
        // Log the error.
        gLogMessageRaw << "BcmCommandTable::RemoveInstance:  ERROR - "
             << "Couldn't find instance ("
             << pInstanceValue << ") in command table '" << pfTableName << "'." << endl;
    }

    // Unlock access to my list.
    pfMutex->Unlock();

    return result;
}


// Writes general command format and usage information to the specified
// ostream.
//
// Parameters:
//      outputStream - the ostream to be written to.
//
// Returns:
//      A reference to the ostream that was passed in (to support chaining).
//
ostream & BcmCommandTable::PrintUsage(ostream &outputStream)
{
    return outputStream << 

"General command format:  cmd [(#)] [parms...]\n\n"

"'cmd' is the command to be executed.  Type 'help' to get a list of available\n"
"commands.  Note that the parser can handle partial command names (e.g. 'he'\n"
"for 'help').  It always selects the first good match that it finds.\n\n"

"'(#)' is the optional set or range of instance numbers.  If specified, then\n"
"the command is only executed for those instances.  If absent, then the command\n"
"is executed for all registered instances.  The instance numbers are delimited\n"
"by one of a space, comma, or semicolon (e.g. '(0, 1; 2 3)').  A range is\n"
"delimited by a dash (e.g. '(0-3)').  You can specify an open-ended range that\n"
"goes to the end of the instances (e.g. '(3-)'), and you can specify mixes of\n"
"values (e.g. '(0 - 3, 5, 7-)').\n\n"

"'parms' are the optional parameters for the command.  See the help information\n"
"for the command for details on the parameters.\n\n"

"Note that all strings and parameters are case-sensitive, so 'help' is not the\n"
"same as 'Help', and '-i' is not the same as '-I'.\n\n"

"While entering a command, typing a '?' will display the help for that command,\n"
"then will restore the line you were entering so you can finish the command.\n\n"

"Each command can have 0 or more parameters, and there are different types of\n"
"parameters (each with their own parsing syntax):\n"
"  Boolean - Accepts 1/t/T/y/Y as 'true'; 0/f/F/n/N as 'false'.\n"
"  Flag - A '-' followed by a single character; may also be followed by one\n"
"    command parameter (e.g. '-s', '-i IpAddress', '-n Unsigned Int').\n"
"  Float - An IEEE single-precision (32-bit) floating point number (0, 7.5, 0.3,\n"
"    1.3e-5).  The valid range for the value will be shown after the name as\n"
"    {min..max} if it is limited.\n"
"  IpAddress - A dotted decimal IP address, such as '192.168.100.1'.\n"
"  MacAddress - A colon-delimited hex MAC address, such as '00:10:18:de:ad:01'.\n"
"  SignedInt - A signed integer value (32-bit), in hex or decimal.  The valid\n"
"    range for the value will be shown after the name as {min..max}.\n"
"  UnsignedInt - An unsigned integer value (32-bit), in hex or decimal.  The\n"
"    valid range for the value will be shown after the name as {min..max}.\n"
"  String - A string value, delimited by whitespace.  If you need to enter a\n"
"    string with spaces, enclose it with quotes (e.g. \"String with spaces\").\n"
"    If you need to enter an empty string, use \"\" or the word NULL as the\n"
"    value.  The max length of the string is shown after the name as {length}.\n"
"  StringSet - An enum-like entry method.  Accepts one of the string values\n"
"    listed, which is translated to a number 0..n-1, where n is the number of\n"
"    strings.\n"
"  Bitmask - Allows you to enter a bitmask value in decimal or hex.  A bitmask\n"
"    may only allow some bits to be specified; these are shown in help text as\n"
"    {0xNN} after the parameter name.  If nothing is shown, all 32 bits are\n"
"    valid.  Additionally, you can specify one of these special characters\n"
"    before the value to modify the existing value:\n"
"      '=' - store the value ('=0x20' -> value  = 0x20)\n"
"      '&' - bitwise AND     ('&0x20' -> value &= 0x20)\n"
"      '|' - bitwise OR      ('|0x20' -> value |= 0x20)\n"
"      '~' - invert/AND      ('~0x20' -> value &= ~0x20)"
<< endl;

}


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
ostream & BcmCommandTable::Print(ostream &outputStream, Verbosity verbosity)
{
    if (pfMutex == NULL)
    {
        gLogMessageRaw << "BcmCommandTable::Print:  ERROR - my mutex is NULL!" << endl;

        return outputStream;
    }

    // Protect my list.
    pfMutex->Lock();

    // The output format depends on the verbosity that was asked for.  Note that
    // the order of these is important!
    if ((verbosity == kCommandTableInfo) || 
        (verbosity == kTableInfoAndCommandsRecursive))
    {
        outputStream << "Command Table:  " << pfTableName << " ("
                     << pfTableCommandName << ")\n\n";

        outputStream << "Description:\n\n" << pfTableDescription << endl;
    }

    if (verbosity == kTableInfoAndCommandsRecursive)
    {
        outputStream << "\nAvailable Commands:\n\n";
    }

    if ((verbosity == kCommandsOnly) ||
        (verbosity == kTableInfoAndCommandsRecursive))
    {
        list<BcmCommand>::iterator myIterator;
        unsigned int column, length;
        int columnwidth;
        char paddedstring[81];

        // This lists the command names, arranged in 5 columns.
        column = 0;
        for (myIterator = fCommands.begin(); myIterator != fCommands.end(); ++myIterator)
        {
            // Only display if not hidden.
            if ((*myIterator).fIsHidden == false)
            {
                // If we're too close to the right edge (80 columns, leaving room
                // for a space), then wrap to the next line before printing.
                length = strlen((*myIterator).pfCommandName);
                if ((*myIterator).fIsEnabled == false)
                {
                    length += 2;
                }
                columnwidth = (((length+16)/16)*16)-1;
                column += columnwidth;
                if ((column) > 80)
                {
                    outputStream << "\n";
                    column = columnwidth;
                }

                // If the command is disabled, then surround it by '{' and '}'.
                if ((*myIterator).fIsEnabled == false)
                {
                    sprintf(paddedstring, "{%s}%*s", (*myIterator).pfCommandName, columnwidth-length, "");
                }
                else
                {
                    sprintf(paddedstring, "%-*s", columnwidth, (*myIterator).pfCommandName);
                }
                outputStream << paddedstring;
                if (column < 79)
                {
                    outputStream << " ";
                }
                column++;
            }
        }

        // Start a fresh line after the last command is printed.
        if (column != 0)
        {
            outputStream << "\n";
        }

        outputStream << "----\n";
    }

    if (verbosity == kCommandsAndHelpRecursive)
    {
        list<BcmCommand>::iterator myIterator;
        char path[128], separator[] = "/";

        // These list the full command help.
        for (myIterator = fCommands.begin(); myIterator != fCommands.end(); ++myIterator)
        {
            NameWithParents (path, sizeof(path)-1, separator);
            outputStream << "TABLE:  ";
            if (path[0] != '\0')
            {
                outputStream << path;
            }
            else
            {
                outputStream << "built-in";
            }
            outputStream << "\n\n";

            outputStream << (*myIterator);

            outputStream << "\n" << setfill('-') << setw(80) << "-\n\n" << setfill(' ');
        }
    }

    if (verbosity == kTableInfoAndCommandsRecursive)
    {
        if (!fSubtables.empty())
        {
            outputStream << "\nAvailable sub-command tables:\n\n";

            PrintSubtables(outputStream, kAbbreviated);
        }

        outputStream << "\n" << setfill('-') << setw(80) << "-\n\n" << setfill(' ');
    }

    if ((verbosity == kTableInfoAndCommandsRecursive) || (verbosity == kCommandsAndHelpRecursive))
    {
        list<BcmCommandTable *>::iterator myIterator;
    
        if (!fSubtables.empty())
        {
            // Get each of my contained subtables to print their information
            for (myIterator = fSubtables.begin(); myIterator != fSubtables.end(); ++myIterator)
            {
                (*myIterator)->Print(outputStream, verbosity);
            }
        }
    }

    if ((verbosity == kCommandsAndHelp) || (verbosity == kCommandsAndInstances))
    {
        list<BcmCommand>::iterator myIterator;

        // These list the full command help.
        for (myIterator = fCommands.begin(); myIterator != fCommands.end(); ++myIterator)
        {
            // Only display if not hidden.
            if ((*myIterator).fIsHidden == false)
            {
                outputStream << (*myIterator);

                outputStream << "\n" << setfill('-') << setw(79) << "-\n\n" << setfill(' ');
            }
        }
    }

    if ((verbosity == kInstancesOnly) || (verbosity == kCommandsAndInstances))
    {
        list<Instance>::iterator myIterator;
        unsigned int instanceNumber;

        // Show all of the instances that have registered.
        outputStream << "Registered Instances:\n\n";

        instanceNumber = 0;
        for (myIterator = fInstances.begin(); myIterator != fInstances.end(); ++myIterator)
        {
            outputStream << instanceNumber << ")  " 
                         << (*myIterator).pfInstanceName << "  ("
                         << (*myIterator).pfInstanceValue << ")\n";

            instanceNumber++;
        }
    }

    // Unlock access to my list.
    pfMutex->Unlock();

    return outputStream;
}


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
ostream & BcmCommandTable::PrintInstances(ostream &outputStream, const char *pInstanceName)
{
    list<Instance>::iterator myIterator;
    unsigned int instanceNumber;
    char instanceLwr[64], matchLwr[64];

    if (pfMutex == NULL)
    {
        gLogMessageRaw << "BcmCommandTable::PrintInstances:  ERROR - my mutex is NULL!" << endl;

        return outputStream;
    }

    // Set up the default instance name.
    if (pInstanceName == NULL)
    {
        pInstanceName = "";
    }

    // Convert name to match to all lower case
    StrLower (pInstanceName, matchLwr, 64);

    // Protect my list.
    pfMutex->Lock();

    // Show all of the instances that have registered.
    outputStream << "Registered Instances:\n\n";

    instanceNumber = 0;
    for (myIterator = fInstances.begin(); myIterator != fInstances.end(); ++myIterator)
    {
        // Convert instance name to all lower case
        StrLower ((*myIterator).pfInstanceName, instanceLwr, 64);

        // Only display the instance name if the strings match.
        if (strstr(instanceLwr, matchLwr) != NULL)
        {
            outputStream << instanceNumber << ")  " 
                         << (*myIterator).pfInstanceName << "  ("
                         << (*myIterator).pfInstanceValue << ")\n";
        }

        instanceNumber++;
    }
    
    // Unlock access to my list.
    pfMutex->Unlock();

    return outputStream;
}


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
ostream & BcmCommandTable::PrintCommand(ostream &outputStream, 
                                        unsigned int commandId)
{
    list<BcmCommand>::iterator myIterator;

    if (pfMutex == NULL)
    {
        gLogMessageRaw << "BcmCommandTable::PrintCommand:  ERROR - my mutex is NULL!" << endl;

        return outputStream;
    }

    // Protect my list.
    pfMutex->Lock();

    // Find the specified command, then print it.
    if (FindCommand(commandId, myIterator) == true)
    {
        outputStream << (*myIterator);
    }

    // Unlock access to my list.
    pfMutex->Unlock();

    return outputStream;
}


// Same as above, but uses the command name in the search.
//
// Parameters:
//      outputStream - the ostream to be written to.
//      pCommandName - the name of the command to be printed.
//
// Returns:
//      A reference to the ostream that was passed in (to support chaining).
//
ostream & BcmCommandTable::PrintCommand(ostream &outputStream, 
                                        char *pCommandName)
{
    list<BcmCommand>::iterator myIterator;

    if (pfMutex == NULL)
    {
        gLogMessageRaw << "BcmCommandTable::PrintCommand:  ERROR - my mutex is NULL!" << endl;

        return outputStream;
    }

    // Protect my list.
    pfMutex->Lock();

    // Find the specified command, then print it.
    if (FindCommand(pCommandName, myIterator) == true)
    {
        outputStream << (*myIterator);
    }

    // Unlock access to my list.
    pfMutex->Unlock();

    return outputStream;
}


// Writes information about the contained subtables to the specified ostream.
//
// Parameters:
//      outputStream - the ostream to be written to.
//      verbosity - the amount of information to be written.
//
// Returns:
//      A reference to the ostream that was passed in (to support chaining).
//
ostream & BcmCommandTable::PrintSubtables(ostream &outputStream, 
                                          SubtableVerbosity verbosity)
{
    if (pfMutex == NULL)
    {
        gLogMessageRaw << "BcmCommandTable::PrintSubtables:  ERROR - my mutex is NULL!" << endl;

        return outputStream;
    }

    // Protect my list.
    pfMutex->Lock();

    if (verbosity == kAbbreviated)
    {
        list<BcmCommandTable *>::iterator myIterator;
        unsigned int column, length;

        // Just print the subtable names, separated by a space.
        column = 0;
        for (myIterator = fSubtables.begin(); myIterator != fSubtables.end(); ++myIterator)
        {
            // If we're too close to the right edge (80 columns, leaving room
            // for a space and []), then wrap to the next line before printing.
            length = strlen((*myIterator)->Name()) + 3;
            column += length;
            if (column > 78)
            {
                outputStream << "\n";

                column = length;
            }

            outputStream << "[" << (*myIterator)->Name() << "] ";
        }

        // Start a fresh line after the last subtable is printed.
        if (column != 0)
        {
            outputStream << "\n";
        }
    }
    else
    {
        list<BcmCommandTable *>::iterator myIterator;
    
        // Get each of my contained subtables to print their information, then
        // put out an extra blank line.
        for (myIterator = fSubtables.begin(); myIterator != fSubtables.end(); ++myIterator)
        {
            (*myIterator)->Print(outputStream, kCommandTableInfo);

            outputStream << "\n" << setfill('-') << setw(79) << "-\n\n" << setfill(' ');
        }
    }

    // Unlock access to my list.
    pfMutex->Unlock();

    return outputStream;
}


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
bool BcmCommandTable::FindCommand(unsigned int commandId,
                                  list<BcmCommand>::iterator &myIterator)
{
    // Find the command in the list.
    if (!fCommands.empty())
    {
        for (myIterator = fCommands.begin(); myIterator != fCommands.end(); ++myIterator)
        {
            if ((*myIterator).fCommandId == commandId)
            {
                return true;
            }
        }
    }

    // Not found.
    return false;
}


// Same as above, only it uses the command name for the search.  This expects
// the table to be kept in alphabetical order.
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
//      false if the command was NOT found (myIterator points to the command
//          with name greater than pCommandString, or the end of the table)
//
bool BcmCommandTable::FindCommand(char *pCommandString, 
                                  list<BcmCommand>::iterator &myIterator)
{
    int result;

    // Set myIterator so it is always valid.
    myIterator = fCommands.begin();

    if ((pCommandString == NULL) || (strlen(pCommandString) == 0))
    {
        return false;
    }

    // Find the command in the list.
    if (!fCommands.empty())
    {
        for (myIterator = fCommands.begin(); myIterator != fCommands.end(); ++myIterator)
        {
            result = strncmp((*myIterator).pfCommandName, pCommandString, strlen(pCommandString));
            // Return true if the command was found.
            if (result == 0)
            {
                return true;
            }
            // Return the current position if we found a command name > pCommandString.
            if (result > 0 )
            {
                break;
            }
        }
    }

    // Not found.
    return false;
}


// This is a helper method that searches for the specified subtable in the
// list, based on the subtable address.  It expects the table to be kept in
// alphabetical order.
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
//      false if the subtable was NOT found (myIterator points to the table with
//          name greater than the subtable, or the end)
//
bool BcmCommandTable::FindSubtable(BcmCommandTable *pSubtable,
                                   list<BcmCommandTable *>::iterator &myIterator)
{
    int result;

    // Set myIterator so it is always valid.
    myIterator = fSubtables.begin();

    if (!fSubtables.empty())
    {
        for (myIterator = fSubtables.begin(); myIterator != fSubtables.end(); ++myIterator)
        {
            if ((*myIterator) == pSubtable)
            {
                return true;
            }
            result = strcmp((*myIterator)->pfTableCommandName,pSubtable->pfTableCommandName);
            if (result > 0)
            {
                return false;
            }
        }
    }

    // Not found.
    return false;
}


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
bool BcmCommandTable::FindInstance(void *pInstanceValue, 
                                   list<Instance>::iterator &myIterator)
{
    // Find the instance in the list.
    if (!fInstances.empty())
    {
        for (myIterator = fInstances.begin(); myIterator != fInstances.end(); ++myIterator)
        {
            if ((*myIterator).pfInstanceValue == pInstanceValue)
            {
                return true;
            }
        }
    }

    // Not found.
    return false;
}



