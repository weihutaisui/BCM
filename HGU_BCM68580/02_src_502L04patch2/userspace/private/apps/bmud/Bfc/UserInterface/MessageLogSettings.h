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
//  Filename:       MessageLogSettings.h
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

#ifndef MESSAGELOGSETTINGS_H
#define MESSAGELOGSETTINGS_H

//********************** Include Files ***************************************

#include "typedefs.h"

#include <iostream>
using namespace std; 

//********************** Global Types ****************************************

//********************** Global Constants ************************************

//********************** Global Variables ************************************

//********************** Forward Declarations ********************************
 
#if (BCM_CONSOLE_SUPPORT || BFC_INCLUDE_CONSOLE_SUPPORT)
// I need to use a forward declaration here rather than including the .h file
// in order to avoid circular dependencies.
class BcmMsgLogSettingsCommandTable;
#endif

//********************** Class Declaration ***********************************


class BcmMessageLogSettings
{
public:

    // These are the severity levels associated with a message.
    typedef enum
    {
        // Need text describing when to use each of these.
    
        // Common severities for most purposes.
        kFatalErrorMessages =           0x00000001,
        kErrorMessages =                0x00000002,
        kWarningMessages =              0x00000004,
        kInitializationMessages =       0x00000008,
        kFunctionEntryExitMessages =    0x00000010,
        kInformationalMessages =        0x00000020,
    
        // Setting this bit will DISABLE 'always' messages.  The sense of
        // this bit is reversed for backwards compatibility with NV settings
        kAlwaysMessage =                0x80000000,
    
        kNoMessages =                   0x00000000,    // To turn them all off.
        kAllMessages =                  0xffffffff,    // To turn them all on.
    
        // These bits are left for app-specific use.  Generally, these will be used
        // in combination with kInformational so that they can easily be turned off.
        // They are really meant to be used for differentiating one source of info
        // messages from another (e.g. "got here" versus dumping buffer contents,
        // etc.).  It is up to the specific class/module to determine the meaning
        // and use of these bits.
        kAppSpecificMessages =          0x7fffffc0
    
    } SeverityBits;

    // These are the fields that can be enabled/disabled for display.  They will
    // be ORed together when passing them to the constructor.
    typedef enum
    {
        kMsTimestampField =     0x40,
        kThreadIdField =        0x20,
        kTimestampField =       0x10,
        kModuleNameField =      0x08,
        kFunctionNameField =    0x04,
        kInstanceNameField =    0x02,
        kSeverityField =        0x01,

        kAllFields =            0x3f
    } Fields;

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
    BcmMessageLogSettings(const char *pModuleName = NULL,
                          const char *pInstanceName = NULL);

    // Copy Constructor.  Initializes the state of this object to match that
    // of the instance passed in.
    //
    // This will often be used by objects in their constructors to copy the
    // "default" settings from the static instance owned by the class/module.
    //
    // Parameters:
    //      otherInstance - reference to the object to copy.
    //
    // Returns:  N/A
    //
    BcmMessageLogSettings(const BcmMessageLogSettings &otherInstance);

    // Destructor.  Frees up any memory/objects allocated, cleans up internal
    // state.
    //
    // Parameters:  N/A
    //
    // Returns:  N/A
    //
    virtual ~BcmMessageLogSettings();

    // Assignment operator.  Copies the state of the instance passed in so that
    // this object is "identical".
    //
    // Parameters:
    //      otherInstance - reference to the object to copy.
    //
    // Returns:
    //      A reference to "this" so that operator = can be chained.
    //
    BcmMessageLogSettings & operator = (const BcmMessageLogSettings &otherInstance);

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
    void SetModuleName(const char *pModuleName);

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
    void SetInstanceName(const char *pInstanceName);

    // Lets the instance name be retrieved.  Note that this can be NULL!
    //
    // Parameters:  None.
    //
    // Returns:
    //      The instance name that was previously set.
    //
    inline const char *GetInstanceName(void) const;

    // This is used by the macros to determine which of the severity codes are
    // enabled for display.
    //
    // Parameters:  None.
    //
    // Returns:
    //      The set of severity codes that are enabled.
    //
    inline uint32 GetEnabledSeverities(void) const;

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
    static void GetEnabledSeveritiesNames(uint32 severities, char *pNamesString, unsigned int maxLength);

    // Allows the enabled severities to be changed.
    //
    // Parameters:
    //      severities - the severities that are to be enabled.
    //
    // Returns:  Nothing.
    //
    inline void SetEnabledSeverities(uint32 severities);

    // Allows the enabled fields to be queried.
    //
    // Parameters:  None.
    //
    // Returns:
    //      The mask of enabled fields.
    //
    inline uint8 GetEnabledFields(void) const;

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
    static void GetEnabledFieldsNames(uint8 fields, char *pNamesString, unsigned int maxLength);

    // Allows the enabled fields to be changed.
    //
    // Parameters:
    //      fields - the fields that are to be enabled.
    //
    // Returns:  Nothing.
    //
    inline void SetEnabledFields(uint8 fields);

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
    ostream &Print(ostream &outputStream) const;

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
    ostream &PrintFields(ostream &outputStream, uint32 severity,
                         const char *pFunctionName = NULL) const;

    // This method causes the settings instance to register itself with its
    // command table.  It is necessary to do this manually as a separate step
    // because of the interdependence between BcmCommandTable and this class,
    // this causes a deadlock in initialization.
    //
    // Parameters:  None.
    //
    // Returns:  Nothing.
    //
    void Register(void);

    // This method causes the settings instance to deregister itself from its
    // command table.
    //
    // Parameters:  None.
    //
    // Returns:  Nothing.
    //
    void Deregister(void);

    // Returns true if kInformationalMessages logging is enabled.
    //
    bool InfoMsgLogging() const
    {
        return ((fEnabledSeverities & kInformationalMessages) ? true : false);
    }

    // Accessors for the alternate ostream object.
    //
    // PR9539
    //
    ostream *AlternateOstream(uint32 severities) const;
    void AlternateOstream(ostream *pOstream);

public:

    // Accessors to set the default severities and fields.  This is usually
    // just called by the nonvol object.
    //
    // Parameters:
    //      value - the new default value to use.
    //
    // Returns:  Nothing.
    //
    static void SetDefaultSeverities(uint32 value);
    static void SetDefaultFields(uint8 value);

private:

    // Pointer to the string given to me in the constructor.  I don't delete or
    // modify this memory.
    const char *pfModuleName;

    // Pointer to my copy of the instance name.  I create and delete this memory.
    char *pfInstanceName;

    // The severity bits that are enabled.  These are used by the macros to
    // determine whether or not a give message should be displayed.
    uint32 fEnabledSeverities;

    // The optional fields that are enabled for display.
    uint8 fEnabledFields;

    // This is used to store a poniter to an alternate ostream where you want
    // the output to go.  By default, this is NULL, and output goes to the
    // ostream returned by GetMessageLogStream().
    //
    // PR9539
    ostream *pfAlternateOstream;

private:

    // These are the default values that will be used for the enabled severities
    // and fields.  Most often, these are set by the nonvol object.
    static uint32 fDefaultSeverities;
    static uint8 fDefaultFields;

public:

#if (BCM_CONSOLE_SUPPORT || BFC_INCLUDE_CONSOLE_SUPPORT)
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
    static BcmMsgLogSettingsCommandTable &GetCommandTableSingleton(void);
#endif

};


//********************** Inline Method Implementations ***********************


// Lets the instance name be retrieved.  Note that this can be NULL!
//
// Parameters:  None.
//
// Returns:
//      The instance name that was previously set.
//
inline const char *BcmMessageLogSettings::GetInstanceName(void) const
{
    return pfInstanceName;
}


// This is used by the macros to determine which of the severity codes are
// enabled for display.
//
// Parameters:  None.
//
// Returns:
//      The set of severity codes that are enabled.
//
inline uint32 BcmMessageLogSettings::GetEnabledSeverities(void) const
{
    return fEnabledSeverities;
}


// Allows the enabled severities to be changed.
//
// Parameters:
//      severities - the severities that are to be enabled.
//
// Returns:  Nothing.
//
inline void BcmMessageLogSettings::SetEnabledSeverities(uint32 severities)
{
    fEnabledSeverities = severities;
}


// Allows the enabled fields to be queried.
//
// Parameters:  None.
//
// Returns:
//      The mask of enabled fields.
//
inline uint8 BcmMessageLogSettings::GetEnabledFields(void) const
{
    return fEnabledFields;
}


// Allows the enabled fields to be changed.
//
// Parameters:
//      fields - the fields that are to be enabled.
//
// Returns:  Nothing.
//
inline void BcmMessageLogSettings::SetEnabledFields(uint8 fields)
{
    fEnabledFields = fields;
}


// Insertion operator for this class.  It allows you to do the following:
//
//    cout << instance << endl;
//
inline ostream & operator << (ostream &outputStream, const BcmMessageLogSettings &instance)
{
    return instance.Print(outputStream);
}


#endif


