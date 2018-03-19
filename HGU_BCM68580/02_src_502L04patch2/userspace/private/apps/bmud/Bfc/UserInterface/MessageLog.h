//****************************************************************************
//
// Copyright (c) 2000-2013 Broadcom Corporation
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
//*****************************************************************
//  $Id$
//
//  Filename:       MessageLog.h
//  Author:         David Pullen
//  Creation Date:  Feb 24, 2000
//
//****************************************************************************
//  Description:
//      This class contains the abstraction of a message logging facility, which
//      is used by various objects to write output for the user to see.
//
//      For the most part, it looks and behaves like an ostream (e.g. cout), but
//      with a few extensions.  With message logging, there is a need to be able
//      to associate a severity code with the message (Fatal Error, Error, 
//      Warning, Informational, etc.), and to be able to selectively
//      enable/disable logging of various severities.
//
//      In addition, there is other common information (timestamp, module name,
//      instance name, etc) that can be selectively enabled/disabled, and that
//      is automatically handled for the user.
//
//      On top of that, there is a need to decouple the message generation and
//      formatting from the output device, so that message logs can go to the
//      console, a trace buffer, a disk, a network socket, etc, depending on
//      the needs of the target system.  To a certain extent, this is provided
//      in the nature of ostreams, where the storage is separated from the
//      formatting, but we may need to carry this a bit further.
//
//      There is also a desire to be able to hook this into the syslog service,
//      if provided.
//
//      Finally, we need to be able to compile this whole mess out for code and
//      memory space considerations.
//
//      The message logging subsystem generally consists of 3 components:
//          - The device to which output is to be written (console, network,
//            trace buffer, etc.).  This will tend to be a singleton, but can
//            be changed on the fly.
//          - A class that handles formatting of the optional fields of the
//            message, and whether or not the message should be written.
//          - A class- or module-static settings instance, which contains
//            defaults for each instance.
//          - A per object message log instance that contains settings (copied
//            from the static one initially), the name strings, field
//            enable/disable flags, etc.  This is what the messages are written
//            to, allowing it to determine how much, if any, work needs to be
//            done.
//
//      Message format:  Optional fields are in '[]'
//
//          [timestamp ][module/class name::][function name: ][(instance name) ][severity - ]message and parameters
//
//      Examples:
//
//          00005321 BcmOctetBuffer::Write: (Message Buffer 2) ERROR - Buffer full!
//
//          WARNING - Missing parameter; default value will be used.
//
//          Hit any key to continue...
//
//****************************************************************************

#ifndef MESSAGELOG_H
#define MESSAGELOG_H

//********************** Include Files ***************************************

#include "typedefs.h"

// This file contains the definitions of the severity code bits.
#include "MessageLogSettings.h"

#include "SystemTime.h"

#include <iostream>

//********************** Global Types ****************************************

//********************** Global Constants ************************************

// These defines turn various message logging levels off at compile time,
// removing them from the code.  It's an interesting way to get rid of some
// of the code that is needed only for initial development, but leaving the
// rest of the message logging facilities present.
//
// Set the define to 0 to disable that level, or 1 to enable it.

#if ( BCM_REDUCED_IMAGE_SIZE )
#define DEFAULT_ENABLE_VALUE    0
#else
#define DEFAULT_ENABLE_VALUE    1
#endif

#define ENABLE_FATAL_ERROR_MSG  DEFAULT_ENABLE_VALUE
#define ENABLE_ERROR_MSG        DEFAULT_ENABLE_VALUE
#define ENABLE_WARNING_MSG      DEFAULT_ENABLE_VALUE
#define ENABLE_DATA_PATH_MSG    0
#define ENABLE_INIT_MSG         DEFAULT_ENABLE_VALUE
#define ENABLE_FUNC_MSG         DEFAULT_ENABLE_VALUE
#define ENABLE_INFO_MSG         DEFAULT_ENABLE_VALUE
#define ENABLE_ALWAYS_MSG       DEFAULT_ENABLE_VALUE
#define ENABLE_APP_MSG          DEFAULT_ENABLE_VALUE

// These defines turn Call Tracing off at compile time, removing it from the
// code.  Call Tracing causes a message to be printed whenever a function
// is entered and exited.  We differentiate between datapath and non-datapath
// Call Tracing for performance reasons; it may be desirable to enable Call
// Tracing in control/management code, but not for the high-performance data
// path in the system.
//
// Set this to 0 to disable, or 1 to enable.
#define ENABLE_CALL_TRACING 0
#define ENABLE_DATAPATH_CALL_TRACING 0

//********************** Forward Declarations ********************************

class BcmNullMessageLog;

//********************** Global Variables ************************************

// This is the ostream instance that is used when a message is to be displayed.
// This MUST be set up by the application main!
//
// Note that the macros below have code built in to handle the case where this
// is not initialized (set to NULL), so that this is safe.
extern ostream *pgMessageLogStream;

// Controls whether or not call tracing is enabled for the whole system.  If
// enabled, then call tracing must also be enabled for the current thread for
// a call trace to be printed.
extern bool gGlobalCallTraceEnabled;

// Strings used for call tracing.  They are actually placed in the .cpp file
// so that the strings aren't replicated in every module.  Note the double
// const here; this makes both the pointer const, and the thing pointed to
// const, meaning that you can't change the string, and you can't change the
// pointer to the string.
extern const char * const pgCallTraceString;
extern const char * const pgCallTraceModuleFuncSeparator;
extern const char * const pgCallTraceEnteringString;
extern const char * const pgCallTraceExitingString;
extern const char * const pgCallTraceStateString;

// Used by the console thread trigger redisplay of the command line.  When
// a message log is written, it can obscure what the user was typing.
extern bool gMessageWritten;

// This is the one and only instance of the "null" message sink.  It is used
// by the macros above.
extern BcmNullMessageLog gNullMessageLog;

//********************** Macro Declarations **********************************

// These macros expand out to do the conditional logging work.  It accepts a
// BcmMessageLogSettings instance, the function name that we are currently
// running in, and the severity code associated with the message.  It checks to
// see if the specified severity bit(s) are enabled; if so, then it sets the
// function name in the settings instance, sends it to the ostream, and returns
// that ostream so that the following insertion operators will work.  Otherwise,
// it returns a special object that turns the following insertion operators
// into NOPs (consuming no CPU, but still correct for the compiler's sake).
//
// Note that the message is only displayed if the ALL of the severity bits are
// set to 1 in the settings object.  Thus, if your message has
// kInformationalMessages and an app specific bit set, but the settings object
// has kInformationalMessages disabled, then the message will be suppressed.
//
// Just to emphacise, if a message is suppressed (because the bits don't match),
// then very few CPU cycles are consumed (just enough to compare the bits!).
//
// The macro itself is particularly nasty, doing some wacky stuff with objects,
// but this is mostly to appease the various compilers and get them to do what I
// want.
//
// NOTE:  If you want to disable all message logging at compile time (thus
//        reducing code space), you can define DISABLE_MESSAGE_LOGGING.
//
// This is what the C function prototypes would look like:
//
//  ostream &LogMessage(BcmMessageLogSettings &settingsInstance,
//                      const char *pFunctionName, uint32 severity);
//
//  ostream &LogMessageNoFields(BcmMessageLogSettings &settingsInstance,
//                              uint32 severity);
//
// This is how you will use it; note that it looks an behaves like an ostream
// (e.g. cout), except for the special parameters that you pass it:
//
//  gLogMessage(msgSettings, "MyFunc", kErrorMessage) << "There was an error!" << endl;
//
//  gLogMessageNoFields(msgSettings, kErrorMessage) << "There was an error!" << endl;
//
// Parameters:
//      settingsInstance - a reference to a BcmMessageLogSettings instance.
//      pFunctionName - pointer to a string containing the name of the function
//                      in which the message is being generated.  This can be
//                      NULL if you don't want a function name to be displayed.
//      severity - a uint32 (or constant) containing the severity code bits
//                 that apply to this message.
//
// Returns:
//      A reference to the ostream for chaining the insertion operator.
//

// PR13462 - superslim nixes all debug output.
#if (BFC_CONFIG_SUPERSLIM)
#define DISABLE_MESSAGE_LOGGING 1
#endif

#ifndef DISABLE_MESSAGE_LOGGING

// This one displays the optional fields that have been enabled.
//
// PR9539 - rewrote the macro to simplify the formatting (which yields a small
// amount of code shrink and is more compatible across compilers), and also to
// support per message log settings instance re-direction of output to an
// alternate ostream.
#define gLogMessage(settingsInstance, pFunctionName, severity) \
    if ((settingsInstance).AlternateOstream(severity) == NULL) ; else (settingsInstance).PrintFields(*(settingsInstance).AlternateOstream(severity), severity, pFunctionName)
    
// This one doesn't display the optional fields, just the message that follows.
//
// PR9539 - rewrote the macro to simplify the formatting (which yields a small
// amount of code shrink and is more compatible across compilers), and also to
// support per message log settings instance re-direction of output to an
// alternate ostream.
#define gLogMessageNoFields(settingsInstance, severity) \
    if ((settingsInstance).AlternateOstream(severity) == NULL) ; else *(settingsInstance).AlternateOstream(severity)

// This one allows you to display a message without having a settings instance.
// It is useful in static methods of classes, etc.  Note that there is no
// suppression of error levels.
#define gLogMessageRaw \
    (pgMessageLogStream == NULL) ? (ostream &) gNullMessageLog : *pgMessageLogStream

#else

// Enable these if you want to permanently disable message logging at compile
// time.
#define gLogMessage(settingsInstance, pFunctionName, severity) gNullMessageLog

#define gLogMessageNoFields(settingsInstance, severity) gNullMessageLog

#define gLogMessageRaw ((ostream &) gNullMessageLog)

#endif

// These are all short-cuts for the common severities.  Here are some examples:
//
//  gErrorMsg(msgSettings, "MyFunc") << "There was an error!" << endl;
//  gInfoMsg(msgSettings, "OtherFunc") << "Got here." << endl;
//  gAlwaysMsg(msgSettings, "YourFunc") << "This message always prints!" << endl;
//
// For app-specific severity codes, you will need to call gLogMessage yourself,
// giving it (kInformationalMessages | kMyAppBit) as the severity parameter.
//
#if (ENABLE_FATAL_ERROR_MSG == 1)
    #define gFatalErrorMsg(settingsInstance, pFunctionName)     gLogMessage(settingsInstance, pFunctionName, BcmMessageLogSettings::kFatalErrorMessages)
    #define gFatalErrorMsgNoFields(settingsInstance)            gLogMessageNoFields(settingsInstance, BcmMessageLogSettings::kFatalErrorMessages)
#else
    #define gFatalErrorMsg(settingsInstance, pFunctionName)     gNullMessageLog
    #define gFatalErrorMsgNoFields(settingsInstance)            gNullMessageLog
#endif

#if (ENABLE_ERROR_MSG == 1)
    #define gErrorMsg(settingsInstance, pFunctionName)          gLogMessage(settingsInstance, pFunctionName, BcmMessageLogSettings::kErrorMessages)
    #define gErrorMsgNoFields(settingsInstance)                 gLogMessageNoFields(settingsInstance, BcmMessageLogSettings::kErrorMessages)
#else
    #define gErrorMsg(settingsInstance, pFunctionName)          gNullMessageLog
    #define gErrorMsgNoFields(settingsInstance)                 gNullMessageLog
#endif

#if (ENABLE_WARNING_MSG == 1)
    #define gWarningMsg(settingsInstance, pFunctionName)        gLogMessage(settingsInstance, pFunctionName, BcmMessageLogSettings::kWarningMessages)
    #define gWarningMsgNoFields(settingsInstance)               gLogMessageNoFields(settingsInstance, BcmMessageLogSettings::kWarningMessages)
#else
    #define gWarningMsg(settingsInstance, pFunctionName)        gNullMessageLog
    #define gWarningMsgNoFields(settingsInstance)               gNullMessageLog
#endif

#if (ENABLE_INIT_MSG == 1)
    #define gInitMsg(settingsInstance, pFunctionName)           gLogMessage(settingsInstance, pFunctionName, BcmMessageLogSettings::kInitializationMessages)
    #define gInitMsgNoFields(settingsInstance)                  gLogMessageNoFields(settingsInstance, BcmMessageLogSettings::kInitializationMessages)
#else
    #define gInitMsg(settingsInstance, pFunctionName)           gNullMessageLog
    #define gInitMsgNoFields(settingsInstance)                  gNullMessageLog
#endif

// These message logs report via Informational Messages but they are placed in-line
// into the datapath of the code and need to be pulled out for performance reasons
// even with fat images unless we need to debug them.
#if (ENABLE_DATA_PATH_MSG == 1)
    #define gDataPathAppMsg(settingsInstance, pFunctionName, appSeverity)  gLogMessage(settingsInstance, pFunctionName, (BcmMessageLogSettings::kInformationalMessages | appSeverity))
    #define gDataPathMsg(settingsInstance, pFunctionName)                  gLogMessage(settingsInstance, pFunctionName, BcmMessageLogSettings::kInformationalMessages)
    #define gDataPathMsgNoFields(settingsInstance)                         gLogMessageNoFields(settingsInstance, BcmMessageLogSettings::kInformationalMessages)
#else
    #define gDataPathAppMsg(settingsInstance, pFunctionName, appSeverity)  gNullMessageLog
    #define gDataPathMsg(settingsInstance, pFunctionName)                  gNullMessageLog
    #define gDataPathMsgNoFields(settingsInstance)                         gNullMessageLog
#endif

#if (ENABLE_FUNC_MSG == 1)
    #define gFuncEntryExitMsg(settingsInstance, pFunctionName)  gLogMessage(settingsInstance, pFunctionName, BcmMessageLogSettings::kFunctionEntryExitMessages)
    #define gFuncEntryExitMsgNoFields(settingsInstance)         gLogMessageNoFields(settingsInstance, BcmMessageLogSettings::kFunctionEntryExitMessages)
#else
    #define gFuncEntryExitMsg(settingsInstance, pFunctionName)  gNullMessageLog
    #define gFuncEntryExitMsgNoFields(settingsInstance)         gNullMessageLog
#endif

#if (ENABLE_INFO_MSG == 1)
    #define gInfoMsg(settingsInstance, pFunctionName)           gLogMessage(settingsInstance, pFunctionName, BcmMessageLogSettings::kInformationalMessages)
    #define gInfoMsgNoFields(settingsInstance)                  gLogMessageNoFields(settingsInstance, BcmMessageLogSettings::kInformationalMessages)

#else
    #define gInfoMsg(settingsInstance, pFunctionName)           gNullMessageLog
    #define gInfoMsgNoFields(settingsInstance)                  gNullMessageLog
#endif

#define gNoSlimInfoMsg(settingsInstance, pFunctionName)         gLogMessage(settingsInstance, pFunctionName, BcmMessageLogSettings::kInformationalMessages)
#define gNoSlimInfoMsgNoFields(settingsInstance)                gLogMessageNoFields(settingsInstance, BcmMessageLogSettings::kInformationalMessages)

#if (ENABLE_ALWAYS_MSG == 1)
    #define gAlwaysMsg(settingsInstance, pFunctionName)         gLogMessage(settingsInstance, pFunctionName, BcmMessageLogSettings::kAlwaysMessage)
    #define gAlwaysMsgNoFields(settingsInstance)                gLogMessageNoFields(settingsInstance, BcmMessageLogSettings::kAlwaysMessage)
#else
    #define gAlwaysMsg(settingsInstance, pFunctionName)         gNullMessageLog
    #define gAlwaysMsgNoFields(settingsInstance)                gNullMessageLog
#endif

// These macros are identical in use and control to gAlwaysMsg macros, except
// that they are not compiled out for slim builds.
#define gNoSlimAlwaysMsg(settingsInstance, pFunctionName)         gLogMessage(settingsInstance, pFunctionName, BcmMessageLogSettings::kAlwaysMessage)
#define gNoSlimAlwaysMsgNoFields(settingsInstance)                gLogMessageNoFields(settingsInstance, BcmMessageLogSettings::kAlwaysMessage)

#if (ENABLE_APP_MSG == 1)
    #define gAppMsg(settingsInstance, pFunctionName, appSeverity)  gLogMessage(settingsInstance, pFunctionName, (BcmMessageLogSettings::kInformationalMessages | appSeverity))
    #define gAppMsgNoFields(settingsInstance, appSeverity)         gLogMessageNoFields(settingsInstance, (BcmMessageLogSettings::kInformationalMessages | appSeverity))
#else    
    #define gAppMsg(settingsInstance, pFunctionName, appSeverity)  gNullMessageLog
    #define gAppMsgNoFields(settingsInstance, appSeverity)         gNullMessageLog
#endif


// Macros for Call Tracing.  These are not ostream-like, they are more like a C
// function call.  This is what the prototypes would look like:
//
// void CallTrace(const char *pModuleClassName, const char *pFunctionName);
//
// template <class StateInfo>
// void CallTraceState(const char *pModuleClassName, const char *pFunctionName,
//                     const StateInfo &stateInfo);
//
// pModuleClassName should be the name of the file or class.
// pFunctionName should be the name of the function.
// stateInfo is any variable or object that can be inserted to an ostream.
//
// Here is how you will use them:
//
// void BcmMyObject::MyFunction(void)
// {
//     // This causes the module and function name to be printed when entering
//     // and exiting the function.
//     CallTrace("BcmMyObject" , "MyFunction");
//
//     // This also causes the module and function name to be printed when
//     // entering and exiting the function, and also prints the state variable
//     // or object when entering the function.  Unfortunately, there is no
//     // clean way to make it print on exit too.
//     CallTraceState("BcmMyObject" , "MyFunction", fMyState);
//
//     // Other code here...
// }
//
#if ENABLE_CALL_TRACING
    #define CallTrace(pModuleClassName, pFunctionName) \
        BcmCallTrace __callTraceInstance(pModuleClassName, pFunctionName)
        
    #define CallTraceState(pModuleClassName, pFunctionName, stateInfo) \
        BcmCallTrace __callTraceInstance(pModuleClassName, pFunctionName); \
        if (gGlobalCallTraceEnabled && CallTraceEnabledForCurrentThread()) \
            gLogMessageRaw << pgCallTraceStateString << stateInfo << endl
#else
    // If not enabled, these are empty macros, generating no code.
    #define CallTrace(pModuleClassName, pFunctionName)
    #define CallTraceState(pModuleClassName, pFunctionName, stateInfo)
#endif    

#if ENABLE_DATAPATH_CALL_TRACING
    // If enabled, these are mapped to the standard CallTrace macros.
    #define CallTraceDatapath(pModuleClassName, pFunctionName) CallTrace(pModuleClassName, pFunctionName)
    #define CallTraceStateDatapath(pModuleClassName, pFunctionName, stateInfo) CallTraceState(pModuleClassName, pFunctionName, stateInfo)
#else
    // If not enabled, these are empty macros, generating no code.
    #define CallTraceDatapath(pModuleClassName, pFunctionName)
    #define CallTraceStateDatapath(pModuleClassName, pFunctionName, stateInfo)
#endif

//********************** Function Declarations *******************************

// Print the specified buffer contents to specified output stream in ASCII hex
// format.
//
// Parameters:
//      ostrm - the ostream to print to.
//      pBuffer - pointer to the buffer of data to print.
//      length - the number of bytes of data to print.
//
// Returns:
//      A reference to the ostream.
//
ostream &PrintBufferHex(ostream &ostrm, const byte *pBuffer, unsigned int length);

// Returns a reference to the current message logging ostream.  This has a
// side effect of setting the flag that tells the console to redisply the
// current command line.
//
// Parameters:  None.
//
// Returns:
//      A reference to the current message logging ostream.
//
inline ostream &GetMessageLogStream(void);

#if ENABLE_CALL_TRACING

// Checks to see whether or not call tracing is enabled for the current thread.
// This is implemented in a OS-specific manner.
//
// Parameters:  None.
//
// Returns:
//      true if call tracing is enabled for the current thread.
//      false if call tracing is disabled for the current thread.
//
bool CallTraceEnabledForCurrentThread(void);

#endif

// This method prints OS-specific thread name info to the specified ostream
// for the message logging facilities.
//
// Parameters:
//      outputStream - the ostream to be written to.
//
// Returns:
//      A reference to the ostream to support chaining of the insertion
//      operator.
//
ostream &MessageLogPrintThreadInfo(ostream &outputStream);

//********************** Class Declarations **********************************


// The purpose of this class is to serve as a dummy "sink" for the insertion
// operators that follow the macros listed above.  If the macro decides that
// the message should not be displayed, then the rest of the message is inserted
// to this class.
//
// Because all of the methods of this class are inline, and they do nothing,
// the compilers are smart enough not to generate any code for them.  Thus,
// no CPU cycles are consumed if the message is to be dropped!
//
class BcmNullMessageLog
{
public:
    
    // Default Constructor.  Initializes the state of the object...
    //
    // Parameters:  None.
    //
    // Returns:  N/A
    //
    BcmNullMessageLog(void) {}
    
    // Copy Constructor.  Initializes the state of this object to match that
    // of the instance passed in.
    //
    // Parameters:
    //      otherInstance - reference to the object to copy.
    //
    // Returns:  N/A
    //
    BcmNullMessageLog(const BcmNullMessageLog &) {}

    // This satisfies the twisted requirements of the ternary (? :) operator
    // that is used in the macros above.  The ternary operator requires that
    // both results (to the left and right of the :) be of the same type, or
    // that there be a conversion operator to transform one to the other.
    //
    // Since one of the results is an ostream, I need to be able to convert
    // myself to an ostream.  Note that no code is actually generated for this,
    // or for any of the other methods in this class.  It is just required in
    // order for the compiler to be happy.
    //
    // Parameters:
    //      ostream - type to convert myself to.
    //
    // Returns:  N/A
    //
    BcmNullMessageLog(const ostream &) {}
    
    // Destructor.  Frees up any memory/objects allocated, cleans up internal
    // state.
    //
    // Parameters:  N/A
    //
    // Returns:  N/A
    //
    ~BcmNullMessageLog() {}

    // Assignment operator.  Copies the state of the instance passed in so that
    // this object is "identical".
    //
    // Parameters:
    //      otherInstance - reference to the object to copy.
    //
    // Returns:
    //      A reference to "this" so that operator = can be chained.
    //
    BcmNullMessageLog & operator = (const BcmNullMessageLog &) {return *this;}

};


#if ENABLE_CALL_TRACING

// This class is used for Call Tracing.  When the client uses the CallTrace()
// macro, it causes this object to be created on the stack.  If call tracing is
// enabled, then the module and function name will be printed.  When the
// function exits, then another printout will occur.
//
class BcmCallTrace
{
public:

    // Initializing constructor.
    //
    // Parameters:
    //      pModuleClassName - the string containing the module or class name
    //                         to be displayed.  If NULL, then the compiler
    //                         macro __FILE__ will be used.
    //      pFunctionName - the string containing the function name to be
    //                      displayed.  If NULL, then the compiler macro
    //                      __LINE__ will be used.
    //
    // Returns:  N/A
    //
    BcmCallTrace(const char *pModuleClassName, const char *pFunctionName)
    {
        // Store these for use in the destructor.
        pfModuleClassName = pModuleClassName;
        pfFunctionName = pFunctionName;

        Print(true);
    }

    // Destructor.
    //
    // Parameters:  N/A
    //
    // Returns:  N/A
    //
    ~BcmCallTrace()
    {
        Print(false);
    }

    // Does the work of printing the call trace information.
    //
    // Parameters:
    //      entering - true if entering (constructor), false if exiting
    //                 (destructor).
    //
    // Returns:  Nothing.
    //
    inline void Print(bool entering)
    {
        // If enabled, then print the call trace.  Example:
        //
        // 0x1234 CALLTRACE [ThreadId] BcmMyObject::MyFunction --->>
        // 0x1234 CALLTRACE [ThreadId] BcmMyObject::MyFunction <<---
        //
        if (gGlobalCallTraceEnabled && CallTraceEnabledForCurrentThread())
        {
            gLogMessageRaw 
                << (void *) SystemTimeMS() << pgCallTraceString;

            if (pgMessageLogStream != NULL)
            {
                MessageLogPrintThreadInfo(*pgMessageLogStream);
            }
            
            if (pfModuleClassName == NULL)
            {
                gLogMessageRaw << __FILE__;
            }
            else
            {
                gLogMessageRaw << pfModuleClassName;
            }

            gLogMessageRaw << pgCallTraceModuleFuncSeparator;

            if (pfFunctionName == NULL)
            {
                gLogMessageRaw << __LINE__;
            }
            else
            {
                gLogMessageRaw << pfFunctionName;
            }

            if (entering)
            {
                gLogMessageRaw << pgCallTraceEnteringString;
            }
            else
            {
                gLogMessageRaw << pgCallTraceExitingString;
            }

            gLogMessageRaw << endl;
        
            // Force the command line to be redisplayed.
            gMessageWritten = true;
        }
    }

private:

    // Storage for the pointers passed in to the constructor.  Used in the
    // destructor.
    const char *pfModuleClassName;
    const char *pfFunctionName;

private:

    // Various standard methods that are not supported.
    BcmCallTrace(void);
    BcmCallTrace(const BcmCallTrace &);
    BcmCallTrace & operator = (const BcmCallTrace &);

};

#endif

//********************** Inline Method Implementations ***********************

// This is the universal insertion operator for the "null" message sink.  It
// accepts all possible parameter types because it is a template.  Also, because
// it's implementation doesn't do anything, the compilers are smart enough not
// to generate any code!  Its sole purpose in life is to make the compiler
// happy with the code syntax.
//
template <class AType>
inline BcmNullMessageLog & operator << (BcmNullMessageLog &null, const AType &) 
{
    return null;
}

#if defined(__GNUC__) || defined(WIN32)
// need this for constant text messages
inline BcmNullMessageLog & operator << (BcmNullMessageLog &null, char []) 
{
    return null;
}

// need this for endl, flush, and other manipulators
inline BcmNullMessageLog & operator << (BcmNullMessageLog &null, 
                                        ostream& (*)(ostream& ))
{
    return null;
}
#endif

#ifndef WIN32
// This one is similar to the insertion operator above, but it handles some of
// the manipulators that can't be handles via an const object reference.
inline BcmNullMessageLog & operator << (BcmNullMessageLog &null, 
                                        BcmNullMessageLog & (*)(BcmNullMessageLog &))
{
    return null;
}

// These are companions for the above insertion operator to resolve the oct,
// hex, and dec manipulators for the null message log.
inline BcmNullMessageLog &oct(BcmNullMessageLog &null) {return null;}
inline BcmNullMessageLog &dec(BcmNullMessageLog &null) {return null;}
inline BcmNullMessageLog &hex(BcmNullMessageLog &null) {return null;}
#endif


inline ostream &GetMessageLogStream(void)
{
    gMessageWritten = true;

    return *pgMessageLogStream;
}


#endif


