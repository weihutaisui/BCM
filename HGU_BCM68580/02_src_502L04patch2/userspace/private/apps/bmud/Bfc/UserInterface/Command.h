//****************************************************************************
//
// Copyright (c) 2000-2009 Broadcom Corporation
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
//
//  Filename:       Command.h
//  Author:         David Pullen
//  Creation Date:  Feb 15, 2000
//
//****************************************************************************
//  Description:
//      This class is the abstraction of a user command that is added to a
//      BcmCommandTable.  It groups all of the command settings, and provides
//      some helpers for manipulating things.
//
//      NOTE:  All of the string pointers in the class MUST be static strings
//             that will not change or be deleted, etc., because only the string
//             pointers are copied and stored!
//
//      Good Example 1:  This string is static/constant in the program, and will
//                       never leave any scope.  This is the preferred way.
//
//          myCommand.pfCommandName = "help";
//
//      Good Example 2:  This string is stored at file scope and thus is always
//                       valid.  Note, however, that it must not be changed
//                       after being stored.
//
//          static char gName[80];
//
//          void MyFunc(void)
//          {
//              sprintf(gName, "help");
//
//              myCommand.pfCommandName = gName;
//          }
//
//      Bad Example 1:  This string is allocated on the stack, and becomes
//                      invalid once the function exits.
//
//          void MyFunc(void)
//          {
//              char name[80];
//              sprintf(name, "help");
//
//              myCommand.pfCommandName = name;
//          }
//
//      Bad Example 2:  This string continues to exist because it is allocated
//                      on the heap, but it will never be deleted, thus causing
//                      a memory leak.
//
//          void MyFunc(void)
//          {
//              char *pName = new char[80];
//              sprintf(pName, "help");
//
//              myCommand.pfCommandName = pName;
//          }
//
//****************************************************************************

#ifndef COMMAND_H
#define COMMAND_H

//********************** Include Files ***************************************

#include "typedefs.h"

#include "CommandParameterList.h"

#include <iostream>

//********************** Global Types ****************************************

#if !(BCM_REDUCED_IMAGE_SIZE)

// DPullen - Aug 14, 2002 - remove the old-style command stuff for slim
// images as part of a code shrink effort.  This should be done for non-slim
// images as well, but there are still a few command tables that use the old
// stuff, and I don't have the time or patience to convert them over and
// test all of the commands to ensure that I haven't broken anything.

// This is the prototype for the functions that will be called when the user
// types a valid command.  It must be a "C" style function, or a static
// class method.
//
// E.g.
//      void MyCmdHandler(void *pInstanceValue, unsigned int commandId, 
//                        const char *pParameters);
//
// Note that pInstanceValue can be NULL if there are no instances registered
// with the command table.
//
// Parameters:
//      pInstanceValue - the instance value to which the command should be
//                       applied.  Instances must be registered with the
//                       command table in order for them to receive the
//                       command.
//      commandId - the id of the command that was entered (from the
//                  Command structure).
//      pParameters - the string containing any parameters that were entered
//                    along with the command.  You must not modify this
//                    string (e.g. with strtok) to parse it, and you must
//                    not free it.
//
// Returns:  Nothing.
//
typedef void (*CommandFunction) (void *pInstanceValue,
                                 unsigned int commandId,
                                 const char *pParameters);

#endif

class BcmCommand;

// This is the new type of callout that is used for commands with the command
// parameter objects.
//
// Parameters:
//      pInstanceValue - the instance value to which the command should be
//                       applied.  Instances must be registered with the
//                       command table in order for them to receive the
//                       command.
//      command - the command (with parameters) that was entered by the user.
//
// Returns:  Nothing.
//
typedef void (*CommandFunctionWithParms) (void *pInstanceValue,
                                          const BcmCommand &command);


#if (BFC_INCLUDE_CMD_HELP_TEXT)

// If the user did not specify the nocmd_help_text build option, we will revert
// to a normal const char * as the type.
typedef const char * BcmCommandRemovableHelpText;

#else

// The purpose of this class is to act as a replacement for a few "const char *"
// members in the BcmCommand class (mainly pfCommandHelp and pfExamples).  This
// class provides operators that allow it to behave (generally) like a const
// char * for all uses of those members, but it has the benefit of causing the
// actual text strings that would have been assigned to the variables to be
// compiled out.  This is only done for code shrink purposes, and is tied to
// a build option (cmd_help_text and nocmd_help_text).
class BcmCommandRemovableHelpText
{
public:

    // Dummy/default constructor and destructor.
    BcmCommandRemovableHelpText(void) {}
    ~BcmCommandRemovableHelpText() {}

    // This satisfies the typical usage of the class member - assigning a
    // pointer to the text.
    void operator = (const char *) {}

    // This satisfies a comparison done in the .cpp file.
    inline bool operator != (void *) const { return true; }

    // Conversion operator to get this object to look like a string.
    inline operator const char * () const { return "{compiled out}\n"; }
};

#endif

//********************** Global Constants ************************************

//********************** Global Variables ************************************

//********************** Class Declaration ***********************************


class BcmCommand
{
public:

    // Default Constructor.  Initializes the state of the object...
    //
    // Parameters:  None.
    //
    // Returns:  N/A
    //
    BcmCommand(void);

    // Copy Constructor.  Initializes the state of this object to match that
    // of the instance passed in.
    //
    // Parameters:
    //      otherInstance - reference to the object to copy.
    //
    // Returns:  N/A
    //
    BcmCommand(const BcmCommand &otherInstance);

    // Destructor.  Frees up any memory/objects allocated, cleans up internal
    // state.
    //
    // Parameters:  N/A
    //
    // Returns:  N/A
    //
    virtual ~BcmCommand();

    // Assignment operator.  Copies the state of the instance passed in so that
    // this object is "identical".
    //
    // Parameters:
    //      otherInstance - reference to the object to copy.
    //
    // Returns:
    //      A reference to "this" so that operator = can be chained.
    //
    BcmCommand & operator = (const BcmCommand &otherInstance);

    // Other operators required to be contained in an STL object.  Comparisons
    // are done based on the command id.
    inline bool operator == (const BcmCommand &otherInstance);
    inline bool operator < (const BcmCommand &otherInstance);

    // This method resets the command settings to a known state.  After calling
    // this method, at a minimum you must set the command id, the command name,
    // and command functions fields to good values before the command can be
    // used.
    //
    // Parameters:  None.
    //
    // Returns:  Nothing.
    //
    void Reset(void);

    // Prints the full command help text to the specified ostream.
    //
    // Parameters:
    //      outputStream - the ostream to be written to.
    //
    // Returns:
    //      A reference to the ostream that was passed in (to support chaining).
    //
    ostream & Print(ostream &outputStream) const;

public:

    // These members are made public so that I don't have to provide accessors
    // for them.

    // Tells whether or not the command is currently allowed to be used.  If
    // not, then it will not be allowed to be entered, and will be displayed
    // with braces ([]).
    //
    // This is useful for modal operation where a command is allowed only if
    // some condition is true.
    bool fIsEnabled;

    // Allows a command to exist that doesn't show up in any listings or help
    // unless you specifically ask for it.  The command is still executable,
    // though.
    bool fIsHidden;

    // Numerical handle associated with the command.  This is used primarily
    // from application code to identify a particular command in a method call,
    // rather than using the string name.  This will generally be some enum
    // value assigned by the client, and MUST be unique for every command in
    // the table.
    unsigned int fCommandId;

    // The full name of the command.  This is what will be used when trying
    // to match the user's command string.  This will not be modified or deleted.
    const char *pfCommandName;

    // Strings to be displayed when the user requests help for this command.
    // The usage help generally shows the set of parameters that the command
    // accepts (using typical Unix regular expression format); the examples
    // show some samples of the command with various parameters; the
    // command help is generally text describing what the command does, any
    // constraints on its use, etc.  These will not be modified or deleted.
    BcmCommandRemovableHelpText pfCommandHelp;
    BcmCommandRemovableHelpText pfExamples;

    #if !(BCM_REDUCED_IMAGE_SIZE)
    
    // DPullen - Aug 14, 2002 - remove the old-style command stuff for slim
    // images as part of a code shrink effort.  This should be done for non-slim
    // images as well, but there are still a few command tables that use the old
    // stuff, and I don't have the time or patience to convert them over and
    // test all of the commands to ensure that I haven't broken anything.
    
    // This is the function that is called when the command is executed.
    // This will often be a static class method.
    const char *pfParameters;
    CommandFunction fCommandFunction;

    #endif

    // The new way to specify a parameter list.  The objects in this list know
    // how to print and parse themselves.
    BcmCommandParameterList *pfCommandParameterList;

    // This is the alternative form of the callout.
    CommandFunctionWithParms fCommandFunctionWithParms;

protected:

private:

};


//********************** Inline Method Implementations ***********************


// Other operators required to be contained in an STL object.  Comparisons
// are done based on the command id.
inline bool BcmCommand::operator == (const BcmCommand &otherInstance)
{
    return (fCommandId == otherInstance.fCommandId);
}

inline bool BcmCommand::operator < (const BcmCommand &otherInstance)
{
    return (fCommandId < otherInstance.fCommandId);
}


// Insertion operator for this class.  It allows you to do the following:
//
//    cout << command << endl;
//
inline ostream & operator << (ostream &outputStream, const BcmCommand &command)
{
    return command.Print(outputStream);
}


#endif


