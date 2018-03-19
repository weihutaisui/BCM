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
//  Filename:       MessageLogSettings.cpp
//  Author:         David Pullen
//  Creation Date:  Feb 24, 2000
//
//****************************************************************************
//  Description:
//      This class contains the settings information for message logging (the
//      optional field values, the enabled message severities, etc),  plus
//      methods to format them to an ostream.
//
//      It is used by the message logging macros to determine whether or not
//      a given message should be logged (based on the severity associated
//      with it), and then to kick off the process of logging the message with
//      the optional fields.
//
//      Message format:  Optional fields are in '[]'
//
//          [timestamp ][[thread id] ][module/class name::][function name: ][(instance name) ][severity - ]message and parameters
//
//      Examples:
//
//          00005321 [Thread1] BcmOctetBuffer::Write: (Message Buffer 2) ERROR - Buffer full!
//
//          WARNING - Missing parameter; default value will be used.
//
//          Hit any key to continue...
//
//****************************************************************************

//********************** Include Files ***************************************

// My api and definitions...
#include "MessageLogSettings.h"

// For getting the timestamp.
#include "SystemTime.h"

#include "Utilities.h"

#ifndef V2_OS_ABSTRACTIONS_NOT_SUPPORTED
#include "OperatingSystemFactory.h"
#endif

#if (BCM_CONSOLE_SUPPORT)
#include "MsgLogSettingsCommandTable.h"
#endif

#include <string.h>
#include <iomanip>

//********************** Local Types *****************************************

//********************** Local Constants *************************************

//********************** Local Variables *************************************

//********************** Class Method Implementations ************************


// I must initialize these static members here.
uint32 BcmMessageLogSettings::fDefaultSeverities = kFatalErrorMessages | kErrorMessages | kWarningMessages;
uint8 BcmMessageLogSettings::fDefaultFields = kAllFields;


// Initializing Constructor.  Initializes the state of the object with the
// settings specified.
//
// This will often only be used for the static instance of the settings
// that is owned by the class or module.
//
// Note that the module name string parameter MUST be static in nature; we
// don't store a copy of it, just a pointer to it.
//
// The instance name string parameter will be copied and stored locally.
//
// Parameters:
//      pModuleName - pointer to a string containing the name of the class
//                    or module to which these settings belong.  If NULL,
//                    then this field will not be displayed.
//      pInstanceName - pointer to a string containing the name of the
//                      class/module instance to which these settings
//                      belong.  If NULL, then this field will not be
//                      displayed.
//
// Returns:  N/A
//
BcmMessageLogSettings::BcmMessageLogSettings(const char *pModuleName,
                                             const char *pInstanceName)
{
#if (BCM_CONSOLE_SUPPORT)
    // Force my command table to come into existance (if it isn't already).
    GetCommandTableSingleton();
#endif

    // Just store a pointer to this.
    pfModuleName = pModuleName;

    // I have to do a deep copy of this one.
    pfInstanceName = NULL;
    SetInstanceName(pInstanceName);

    // Use defaults; clients can override later if they want to.
    fEnabledSeverities = fDefaultSeverities;
    fEnabledFields = fDefaultFields;

    // PR9539 - initially, this needs to be NULL so that output goes to the
    // default ostream.
    pfAlternateOstream = NULL;
}


// Copy Constructor.  Initializes the state of this object to match that
// of the instance passed in.
//
// Parameters:
//      otherInstance - reference to the object to copy.
//
// Returns:  N/A
//
BcmMessageLogSettings::BcmMessageLogSettings(const BcmMessageLogSettings &otherInstance)
{

#if (BCM_CONSOLE_SUPPORT)
    // Force my command table to come into existance (if it isn't already).
    GetCommandTableSingleton();
#endif

    pfInstanceName = NULL;

    // Usually, you can just call the assignment operator to do the work.
    *this = otherInstance;
}


// Destructor.  Frees up any memory/objects allocated, cleans up internal
// state.
//
// Parameters:  N/A
//
// Returns:  N/A
//
BcmMessageLogSettings::~BcmMessageLogSettings()
{
    fEnabledSeverities = 0;

    // Delete the instance name memory that I allocated.
    delete pfInstanceName;
    pfInstanceName = NULL;

    // Just clear the other.
    pfModuleName = NULL;

    // PR9539 - I don't delete the ostream, just clear my pointer.
    pfAlternateOstream = NULL;
}


// Assignment operator.  Copies the state of the instance passed in so that
// this object is "identical".
//
// Parameters:
//      otherInstance - reference to the object to copy.
//
// Returns:
//      A reference to "this" so that operator = can be chained.
//
BcmMessageLogSettings & BcmMessageLogSettings::operator = (const BcmMessageLogSettings &otherInstance)
{
    // Copy protected and private members of otherInstance to my members.
    pfModuleName = otherInstance.pfModuleName;

    // Do a deep copy of this.
    SetInstanceName(otherInstance.pfInstanceName);

    // Just copy these.
    fEnabledSeverities = otherInstance.fEnabledSeverities;
    fEnabledFields = otherInstance.fEnabledFields;

    // PR9539
    pfAlternateOstream = otherInstance.pfAlternateOstream;

    return *this;
}


// Allows an object to set the module/class name in its message settings
// object.  This may be necessary to override the value passed in by the
// object's base class so that the most derived class' name is reflected.
//
// Note that the module name string parameter MUST be static in nature; we
// don't store a copy of it, just a pointer to it.
//
// If NULL, then the module/class name will not be displayed.
//
// Parameters:
//      pModuleName - pointer to a string containing the name of the class
//                    or module to which these settings belong.  If NULL,
//                    then this field will not be displayed.
//
// Returns:  Nothing.
//
void BcmMessageLogSettings::SetModuleName(const char *pModuleName)
{
    // Just store the pointer.
    pfModuleName = pModuleName;
}


// Allows an object to set the instance name in its message settings
// object.
//
// If NULL, then the instance name will not be displayed.
//
// Parameters:
//      pInstanceName - pointer to a string containing the instance name to
//                      be displayed.
//
// Returns:  Nothing.
//
void BcmMessageLogSettings::SetInstanceName(const char *pInstanceName)
{
    // Delete any existing memory.
    delete pfInstanceName;

    // If the specified one is not NULL, then create memory and copy it.
    if (pInstanceName != NULL)
    {
        pfInstanceName = new char[strlen(pInstanceName) + 1];

        strcpy(pfInstanceName, pInstanceName);
    }
    else
    {
        pfInstanceName = NULL;
    }
}


// Returns the enabled severities bitmask, translated into a string for
// display purposes.
//
// Parameters:
//      severities - the severities bits to be converted.
//      pNamesString - pointer to the string into which the names should
//                     be copied.
//      maxLength - the max number of chars (including the \0) that can be
//                  placed in the string.
//
// Returns:  Nothing.
//
void BcmMessageLogSettings::GetEnabledSeveritiesNames(uint32 severities, 
                                                      char *pNamesString, 
                                                      unsigned int maxLength)
{
    const char *pNames[] =
    {
        "Fatal Errors",
        "Errors",
        "Warnings",
        "Initialization",
        "Function Entry/Exit",
        "Informational",
        "App1",
        "App2",
        "App3",
        "App4",
        "App5",
        "App6",
        "App7",
        "App8",
        "App9",
        "App10",
        "App11",
        "App12",
        "App13",
        "App14",
        "App15",
        "App16",
        "App17",
        "App18",
        "App19",
        "App20",
        "App21",
        "App22",
        "App23",
        "App24",
        "App25",
        "Disable Always Messages"
    };

    ConvertBitmaskToString(severities, pNamesString, maxLength,
                           pNames, sizeof(pNames) / sizeof(char *), " | ");
}


// Returns the enabled fields bitmask, translated into a string for display
// purposes.
//
// Parameters:
//      fields - the fields bits to be converted.
//      pNamesString - pointer to the string into which the names should
//                     be copied.
//      maxLength - the max number of chars (including the \0) that can be
//                  placed in the string.
//
// Returns:  Nothing.
//
void BcmMessageLogSettings::GetEnabledFieldsNames(uint8 fields, 
                                                  char *pNamesString, 
                                                  unsigned int maxLength)
{
    const char *pNames[] =
    {
        "Severity",
        "Instance Name",
        "Function/Method Name",
        "Module/Class Name",
        "Timestamps",
        "Thread Id",
        "Timestamps (Milliseconds)"
    };

    ConvertBitmaskToString(fields, pNamesString, maxLength,
                           pNames, sizeof(pNames) / sizeof(char *), " | ");
}


// This one prints the settings to the ostream, rather than the optional
// fields.  It is used primarily by the command table to show which bits are
// set and cleared, and what is in the string pointers.  It is NOT used
// for the message logging itself, which causes the optional fields to be
// evaluated and conditionally printed.
//
// Parameters:
//      outputStream - the ostream to be written to.
//
// Returns:
//      A reference to the ostream to support chaining of the insertion
//      operator.
//
ostream &BcmMessageLogSettings::Print(ostream &outputStream) const
{
    const char *pModuleName = pfModuleName;
    const char *pInstanceName = pfInstanceName;
    char names[256];

    if (pModuleName == NULL)
    {
        pModuleName = "{none}";
    }
    if (pInstanceName == NULL)
    {
        pInstanceName = "{none}";
    }

    outputStream << "Message Log Settings:\n\n"
                 << "\tModule Name:              " << pModuleName << "\n"
                 << "\tInstance Name:            " << pInstanceName << "\n\n";

    GetEnabledSeveritiesNames(fEnabledSeverities, names, sizeof(names));

    outputStream << "\tEnabled Severity Bits:    " << inHex(fEnabledSeverities) << " (" << names << ")\n";

    GetEnabledFieldsNames(fEnabledFields, names, sizeof(names));

    outputStream << "\tEnabled Fields:           " << inHex(fEnabledFields) << " (" << names << ")\n";

    return outputStream << endl;
}


// Causes the enabled optional fields to be displayed.  This assumes that
// the macro has decided that the message should be displayed.  It may not
// print anything if no fields are enabled.
//
// The function name string can be NULL, in which case it will not be
// printed.
//
// Parameters:
//      outputStream - the ostream to be written to.
//      severity - the severity of this message.
//      pFunctionName - the name of the function or method from which the
//                      message is being generated.
//
// Returns:
//      A reference to the ostream to support chaining of the insertion
//      operator.
//
ostream &BcmMessageLogSettings::PrintFields(ostream &outputStream, 
                                            uint32 severity, 
                                            const char *pFunctionName) const
{
    static const char *gSeverityNames[] =
    {
        "FATAL",
        "ERROR",
        "WARNING",
        "INIT",
        "FUNC",
        "INFO"
    };

    // Conditionally output each field.
    if (fEnabledFields & kMsTimestampField)
    {
        char oldFill = outputStream.fill();

        outputStream.fill('0');
        // Display this in hex.
        outputStream << "0x" << hex << setw(8) << SystemTimeMS() << dec << ' ';
        outputStream.fill(oldFill);
    }
    if (fEnabledFields & kTimestampField)
    {
        // PR11826 Change TimeStampPrint to Print the Time of Day **NOTE: kTimestampField Must be enabled in the Log Settings.
        time_t currentTime;
        tm *gmTime;

        time(&currentTime);
        gmTime = gmtime(&currentTime);

        outputStream << "[" << setfill('0') << setw(2) << gmTime->tm_hour << ":" << setw(2) << gmTime->tm_min << ":" << setw(2) << gmTime->tm_sec << " "
                     << setw(2) << (gmTime->tm_mon + 1) << "/" << setw(2) << gmTime->tm_mday << "/" << setw(4) << (gmTime->tm_year + 1900) << "] " << setfill(' ');
    }
    if (fEnabledFields & kThreadIdField)
    {
#ifndef V2_OS_ABSTRACTIONS_NOT_SUPPORTED
        MessageLogPrintThreadInfo(outputStream);
#endif
    }
    if ((fEnabledFields & kModuleNameField) && (pfModuleName != NULL))
    {
        outputStream << pfModuleName << "::";
    }
    if ((fEnabledFields & kFunctionNameField) && (pFunctionName != NULL))
    {
        outputStream << pFunctionName << ":  ";
    }
    if ((fEnabledFields & kInstanceNameField) && (pfInstanceName != NULL))
    {
        outputStream << '(' << pfInstanceName << ") ";
    }
    if ((fEnabledFields & kSeverityField) && (severity != kAlwaysMessage))
    {
        unsigned int index;

        // If there are any app-specific bits, then handle it first.
        if ((severity & (uint32) kAppSpecificMessages) != 0)
        {
            outputStream << "APP ";
        }

        // Scan the bits one at a time, looking for active ones.  In general,
        // there will only be one, but there can be more, so I'll print them
        // all.
        for (index = 0; index < 6; index++)
        {
            if (severity & 0x01)
            {
                outputStream << gSeverityNames[index] << ' ';
            }

            severity >>= 1;
        }


        outputStream << "- ";
    }

    return outputStream;
}


// This method causes the settings instance to register itself with its
// command table.  It is necessary to do this manually as a separate step
// because of the interdependence between BcmCommandTable and this class,
// this causes a deadlock in initialization.
//
// Parameters:  None.
//
// Returns:  Nothing.
//
void BcmMessageLogSettings::Register(void)
{
#if (BCM_CONSOLE_SUPPORT)
    GetCommandTableSingleton().AddInstance(pfInstanceName, this);
#endif
}


// This method causes the settings instance to deregister itself from its
// command table.
//
// Parameters:  None.
//
// Returns:  Nothing.
//
void BcmMessageLogSettings::Deregister(void)
{
#if (BCM_CONSOLE_SUPPORT)
    GetCommandTableSingleton().RemoveInstance(this);
#endif
}


// PR9539
void BcmMessageLogSettings::AlternateOstream(ostream *pOstream)
{
    pfAlternateOstream = pOstream;
}


// PR9539
ostream *BcmMessageLogSettings::AlternateOstream(uint32 severities) const
{
    // Special case: the sense of the 'always' bit is reversed.  So toggle that
    // bit in my enabled severities before doing mask & compare.
    uint32 mySeverities = fEnabledSeverities ^ BcmMessageLogSettings::kAlwaysMessage;

    // If the specified severities are not enabled, suppress output by
    // returning NULL.
    if ((mySeverities & severities) != severities)
    {
        return NULL;
    }

    // If the client has not defined an alternate ostream, return the default
    // message logging stream (which may also be NULL).
    if (pfAlternateOstream == NULL)
    {
        return &(GetMessageLogStream());
    }

    return pfAlternateOstream;
}


// Accessors to set the default severities and fields.  This is usually
// just called by the nonvol object.
//
// Parameters:
//      value - the new default value to use.
//
// Returns:  Nothing.
//
void BcmMessageLogSettings::SetDefaultSeverities(uint32 value)
{
    fDefaultSeverities = value;
}


void BcmMessageLogSettings::SetDefaultFields(uint8 value)
{
    fDefaultFields = value;
}


#if (BCM_CONSOLE_SUPPORT)
// This static method allows an object (including myself) to get access to
// the singleton command table for controlling objects of my type.  This
// helps avoid order of initialization problems by ensuring that the
// object is created and initialized on-demand.
//
// Parameters:  None.
//
// Returns:
//      A reference to the command table singleton for my class.
//
BcmMsgLogSettingsCommandTable &BcmMessageLogSettings::GetCommandTableSingleton(void)
{
    // This is the singleton instance.
    static BcmMsgLogSettingsCommandTable gCommandTable;

    return gCommandTable;
}
#endif

