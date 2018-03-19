//****************************************************************************
//
//  Copyright (c) 2000  Broadcom Corporation
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
//  Filename:       MessageLog.cpp
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

//********************** Include Files ***************************************

// My api and definitions...
#include "MessageLog.h"

#include <iomanip>

//********************** Local Types *****************************************

//********************** Local Constants *************************************

//********************** Local Variables *************************************

#if ENABLE_CALL_TRACING
/*static bool *pgCurrentThreadCallTraceEnable = NULL;
static bool gThreadCallTraceEnable[KC_NTASK] = {false};
static uint32 gThreadCallTraceName[KC_NTASK] = {0};*/
#endif

//********************** Global Variables ************************************

// By default, use NULL as the ostream, which is what the old V2 code did.
// This may be overridden below.
#define DEFAULT_OSTREAM NULL

// This is a new definition with the BFC system, and is specified as part of the
// BFC Message Logging component configuration parameters.  The old V2 system
// didn't set this, and we want to use the old-school behavior (NULL) in that
// case.
#ifdef BFC_CONFIG_MESSAGE_LOGGING_OSTREAM_DEFAULT

    // There are two primary conditions to consider with the new BFC approach;
    // first is where the default is supposed to be NULL.  If that config
    // parameter is 0, then we need to assign the default that is to be used.
    #if (BFC_CONFIG_MESSAGE_LOGGING_OSTREAM_DEFAULT_NULL == 0)

        // The build parameters have selected a different ostream to be used.
        // We need to undefine the symbol, and redefine it as the ostream that
        // is specified.  Note that pgMessageLogStream is a pointer, so we need
        // to take the address of the ostream that is specified.
        #undef  DEFAULT_OSTREAM
        #define DEFAULT_OSTREAM (&BFC_CONFIG_MESSAGE_LOGGING_OSTREAM_DEFAULT)

    #endif

#endif

// Set the default ostream that will be used for message logging.  This is
// normally specified at build time.
ostream *pgMessageLogStream = DEFAULT_OSTREAM;


// By default, set the global call trace enable state to "off"; this is what
// the old V2 code did.  This may be overridden below.
#define DEFAULT_CALLTRACE_GLOBAL_ENABLE false

#if (BFC_CONFIG_CALLTRACE_GLOBAL_ENABLE_DEFAULT)
    #undef  DEFAULT_CALLTRACE_GLOBAL_ENABLE
    #define DEFAULT_CALLTRACE_GLOBAL_ENABLE true
#endif

// The user can override this at compile time
// By default, call tracing is disabled.
bool gGlobalCallTraceEnabled = DEFAULT_CALLTRACE_GLOBAL_ENABLE;

#if ENABLE_CALL_TRACING

// Strings used for call tracing.  They are actually placed in the .cpp file
// so that the strings aren't replicated in every module.
const char * const pgCallTraceString = " CALLTRACE ";
const char * const pgCallTraceModuleFuncSeparator = "::";
const char * const pgCallTraceEnteringString = " --->>";
const char * const pgCallTraceExitingString = " <<---";
const char * const pgCallTraceStateString = "CALLTRACE State:  ";

#endif

// By default, set this to false.
bool gMessageWritten = false;

// I need to define the instance of this class so that it can be used by the
// macros in the .h file.
BcmNullMessageLog gNullMessageLog;

//********************** Class Method Implementations ************************


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
ostream &PrintBufferHex(ostream &ostrm, const byte *pBuffer, unsigned int length)
{
    // Convert binary data in buffer to hex formatted ascii and output to
    // console.
    if ((pBuffer != NULL) && (length > 0))
    {
        // store the old fill character. 
        char old_fill = ostrm.fill();
        //
        // set fill char to '0' and specify hex output format.
        ostrm << setfill( '0' ) << hex;

        // state var which tracks when a newline is printed.
        bool just_printed_newline = false;

        // for each byte in the buffer...
        for (unsigned int i = 0; i < length; i++)
        {
            // newline has not just been output.
            just_printed_newline = false;

            // retrieve next byte, output as (2) digit hex number followed
            // by (1) space
            ostrm << setw( 2 ) << (unsigned int) (pBuffer[i]) << " ";

            if (!((i + 1) % 4))
            {
                // every 4th byte...
                if (!((i + 1) % 16))
                {
                    // every 16th byte go to a new line and note it.
                    ostrm << "\n";
                    just_printed_newline = true;
                }
                else
                {
                    // every 4th byte, skip (2) more spaces to provide visual
                    // cues.
                    ostrm << "  ";
                }
            }
        }

        if (just_printed_newline == false)
        {
            // did NOT just print a newline, so do it now.
            ostrm << "\n";
        }

        // restore the old fill character and specify decimal output format.
        ostrm << setfill( old_fill ) << dec;
    }

    return ostrm;
}


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
bool CallTraceEnabledForCurrentThread(void)
{
    // UNFINISED

    // Look up the call trace enable variable for the current thread, and return
    // it.
    return true;
}

#endif


#if defined(PSOS)
#include <psos.h>
#elif defined(TARGETOS_vxWorks)
#include <taskLib.h>
#elif defined(WIN32)
#include <Windows.h>
#elif defined(TARGETOS_eCos)
#include <cyg/kernel/kapi.h>
static char *GetThreadName( cyg_handle_t Thread )
{
    cyg_thread_info info;

    if (cyg_thread_get_info( Thread, 0, &info ))
    {
        return info.name;
    }
    else
    {
        return "no thread name";
    }
}
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
ostream &MessageLogPrintThreadInfo(ostream &outputStream)
{
    outputStream << '[';

    #if defined(PSOS)
    
    // This structure is rather large, and can blow the stack for tasks with
    // small stacks.  Thus, we will make it static, putting it in the global
    // space.  This is subject to problems with reentrancy, but we're not
    // really too worried about that in this case.
    static tinfo taskInfo;
    int rc;
    uint32 taskId;
    
    // Query the task id for the running task.
    t_ident(NULL, 0, &taskId);

    // Query info about the current task.
    rc = t_info(taskId, &taskInfo);

    if (rc == 0)
    {
        char taskName[6];
        
        // The name returned in not NULL terminated; copy it to a NULL
        // terminated string for printing.
        taskName[0] = '\'';
        memcpy(taskName + 1, taskInfo.name, 4);
        taskName[5] = '\0';

        outputStream << taskName << "' id=" << (void *) taskId;
    }
    else
    {
        outputStream << "no thread info";
    }
    
    #elif defined(TARGETOS_vxWorks)

    char * curTaskName;
    int    curTaskId = taskIdSelf();

    curTaskName = taskName(curTaskId);

    if ( curTaskName != NULL )
    {
        outputStream << "\'" << curTaskName << "' id=" << (void *) curTaskId;
    }
    else
    {
        outputStream << "no thread info";
    }

    #elif defined(TARGETOS_eCos)

    outputStream << GetThreadName(cyg_thread_self());

    #elif defined(WIN32)
    
    DWORD threadId = GetCurrentThreadId();

    outputStream << "id=" << (void *) threadId;

    #elif defined(__linux__)

    outputStream << "id=" << (void *) pthread_self();

    #else

    outputStream << "no thread info";

    #endif

   
    
    return outputStream << "] ";
}



