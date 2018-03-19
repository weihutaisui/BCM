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
//
//  Filename:       Command.cpp
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

//********************** Include Files ***************************************

// My api and definitions...
#include "Command.h"

//********************** Local Types *****************************************

//********************** Global Constants ************************************

//********************** Local Constants *************************************

//********************** Local Variables *************************************

static const char *pgDefaultName = "{unnamed}";
static const char *pgDefaultExamples = "No examples available...";
static const char *pgDefaultHelp = "No command help available...";

//********************** Forward Function definitions ************************

void vfnPrintTab (ostream &outputStream, const char* pString);
int ifnGetBreak (const char* sString, int indent, int iDelimiterLength, int maxWidth[]);

//********************** Class Method Implementations ************************


// Default Constructor.  Initializes the state of the object...
//
// Parameters:  None.
//
// Returns:  N/A
//
BcmCommand::BcmCommand(void)
{
    // Set protected and private members to a known state.
    Reset();
}


// Copy Constructor.  Initializes the state of this object to match that
// of the instance passed in.
//
// Parameters:
//      otherInstance - reference to the object to copy.
//
// Returns:  N/A
//
BcmCommand::BcmCommand(const BcmCommand &otherInstance)
{
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
BcmCommand::~BcmCommand()
{
    // Clean up the protected and private members; delete memory/objects, and
    // set pointers to NULL.
    fIsEnabled = false;
    pfCommandName = NULL;
    pfCommandHelp = NULL;
    pfExamples = NULL;

    #if !(BCM_REDUCED_IMAGE_SIZE)
    
    // DPullen - Aug 14, 2002 - remove the old-style command stuff for slim
    // images as part of a code shrink effort.  This should be done for non-slim
    // images as well, but there are still a few command tables that use the old
    // stuff, and I don't have the time or patience to convert them over and
    // test all of the commands to ensure that I haven't broken anything.
    
    pfParameters = NULL;
    fCommandFunction = NULL;
    
    #endif
    
    delete pfCommandParameterList;
    pfCommandParameterList = NULL;

    fCommandFunctionWithParms = NULL;
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
BcmCommand & BcmCommand::operator = (const BcmCommand &otherInstance)
{
    // Copy protected and private members of otherInstance to my members.
    fIsEnabled = otherInstance.fIsEnabled;
    fIsHidden = otherInstance.fIsHidden;
    fCommandId = otherInstance.fCommandId;
    pfCommandName = otherInstance.pfCommandName;
    pfCommandHelp = otherInstance.pfCommandHelp;
    pfExamples = otherInstance.pfExamples;

    #if !(BCM_REDUCED_IMAGE_SIZE)
    
    // DPullen - Aug 14, 2002 - remove the old-style command stuff for slim
    // images as part of a code shrink effort.  This should be done for non-slim
    // images as well, but there are still a few command tables that use the old
    // stuff, and I don't have the time or patience to convert them over and
    // test all of the commands to ensure that I haven't broken anything.
    
    pfParameters = otherInstance.pfParameters;
    fCommandFunction = otherInstance.fCommandFunction;
    
    #endif

    pfCommandParameterList = otherInstance.pfCommandParameterList;
    fCommandFunctionWithParms = otherInstance.fCommandFunctionWithParms;

    return *this;
}


// This method resets the command settings to a known state.  After calling
// this method, at a minimum you must set the command id, the command name,
// and command functions fields to good values before the command can be
// used.
//
// Parameters:  None.
//
// Returns:  Nothing.
//
void BcmCommand::Reset(void)
{
    fIsEnabled = true;
    fIsHidden = false;
    fCommandId = 0;

    pfCommandName = pgDefaultName;

    pfCommandHelp = NULL;
    pfExamples = NULL;

    #if !(BCM_REDUCED_IMAGE_SIZE)
    
    // DPullen - Aug 14, 2002 - remove the old-style command stuff for slim
    // images as part of a code shrink effort.  This should be done for non-slim
    // images as well, but there are still a few command tables that use the old
    // stuff, and I don't have the time or patience to convert them over and
    // test all of the commands to ensure that I haven't broken anything.
    
    pfParameters = NULL;
    fCommandFunction = NULL;
    
    #endif

    pfCommandParameterList = NULL;
    fCommandFunctionWithParms = NULL;
}


// Prints the full command help text to the specified ostream.
//
// Parameters:
//      outputStream - the ostream to be written to.
//
// Returns:
//      A reference to the ostream that was passed in (to support chaining).
//
ostream & BcmCommand::Print(ostream &outputStream) const
{
    const char *pCommandName;

    outputStream << "COMMAND:  ";

    if (pfCommandName != NULL)
    {
        pCommandName = pfCommandName;
    }
    else
    {
        pCommandName = pgDefaultName;
    }

    outputStream << pCommandName;

    if (fIsEnabled == false)
    {
        outputStream << " {disabled}";
    }

    if (fIsHidden == true)
    {
        outputStream << " {hidden}";
    }

    outputStream << "\n\nUSAGE:  " << pCommandName << " ";
     
    #if !(BCM_REDUCED_IMAGE_SIZE)
    
    // DPullen - Aug 14, 2002 - remove the old-style command stuff for slim
    // images as part of a code shrink effort.  This should be done for non-slim
    // images as well, but there are still a few command tables that use the old
    // stuff, and I don't have the time or patience to convert them over and
    // test all of the commands to ensure that I haven't broken anything.
    
    if (pfParameters != NULL)
    {
        vfnPrintTab (outputStream, pfParameters);
    }
    else
    
    #endif
    
    if (pfCommandParameterList != NULL)
    {
        outputStream << *pfCommandParameterList;
    }

    outputStream << "\n\nDESCRIPTION:\n";
     
    if (pfCommandHelp != NULL)
    {
        vfnPrintTab (outputStream, pfCommandHelp);
    }
    else
    {
        outputStream << pgDefaultHelp;
    }

    outputStream << "\n\nEXAMPLES:\n";

    if (pfExamples != NULL)
    {
        vfnPrintTab (outputStream, pfExamples);
    }
    else
    {
        vfnPrintTab (outputStream, pgDefaultExamples);
    }

    return outputStream << endl;
}


// Print pString with tabs expanded into spaces.
//
// This routine is used to format and print CLI documentation.
// Formatting is governed by included LF and TAB characters
// contained within the strings to be printed.
// LF characters are used to force a line break.
// TAB characters are used to align text into columns.
// Tab stops are calculated so that everything lines up (like a table).
// Line breaks occur only after spaces, unless there are no spaces.
// Line breaks occur before 78 characters (constant in ifnGetBreak).
//
// Parameters:
//     outputStream - stream on which to print text
//     pString      - tab-delimited text
//
// Returns: N/A
//
void vfnPrintTab (ostream &outputStream, const char* pString)
{
    int         i, j;
    int         curTabCount;            // Tab count on current line.
    int         columnWidth;            // Character count since last tab.
    int         tabCount;               // Max number of tabs in a line.
    bool        indenting;              // Indicates a tab is present in the current line.
    int         currentColumn;          // Output column position.
    int         iHangingIndent;         // Hanging indent column.
    int         iDelimiterLength;       // Length of delimiter string.
    int         breakPos;               // Column where the line should break.
    int         maxWidth[10];           // Column width for a given tab count.
    const char* sDelimiter = "  -- ";   // Delimiter string (inserted in place of tab).
    const char* sFirstDelimiter = "     "; // Indent for leading tab.
    const char* p;


    // Initialize array of column widths between tabs.
    for (i=0; i<10; i++)
    {
        maxWidth[i] = 0;
    }

    // Note: sDelimiter and sFirstDelimiter must be the same length!
    iDelimiterLength = strlen (sDelimiter);

    // Calculate the column widths.  A column's width is the number of chars between tabs.
    curTabCount = 0;
    tabCount = 0;
    columnWidth = 0;
    p = pString;
    while (*p != '\0')
    {
        // Found a tab.
        if (*p == '\t')
        {
            // Save the max column width.
            if (columnWidth > maxWidth[curTabCount])
            {
                maxWidth[curTabCount] = columnWidth;
            }
            // Count this tab and reset column width.
            curTabCount++;
            columnWidth = 0;
            // Save max tab count.
            if (curTabCount > tabCount)
            {
                tabCount = curTabCount;
            }
        }
        // Found a newline.  Reset counts.
        else if (*p == '\n')
        {
            curTabCount = 0;
            columnWidth = 0;
        }
        // Printable character, so increment column width.
        else
        {
            columnWidth++;
        }
        // Examine next character in string.
        p++;
    }

    // Calculate hanging indent.  This is the sum of column delimiters plus the max
    // column widths.
    // First add indent amount for all tabs.
    iHangingIndent = iDelimiterLength * tabCount;
    // Add in the column widths for all but the last column.
    for (i=0; i<tabCount; i++)
    {
        iHangingIndent += maxWidth[i];
    }

    // Print pString.
    curTabCount = 0;
    columnWidth = 0;
    currentColumn = 0;
    indenting = false;
    p = pString;

    // Find the column at which this line should break.
    breakPos = ifnGetBreak (p, 0, iDelimiterLength, maxWidth);
    while (*p != '\0')
    {
        // If we found a tab, print delimiter.
        if (*p == '\t')
        {
            // Pad out column and adjust the current character count.
            for (j = columnWidth; j<maxWidth[curTabCount]; j++)
            {
                outputStream << ' ';
                currentColumn++;
            }
            // Add tab delimiter string.  If we're at the beginning of a line,
            // output the first delimiter (spaces).
            if (currentColumn == 0)
            {
                outputStream << sFirstDelimiter;
            }
            // Not at the beginning of line, so output normal delimiter.
            else
            {
                outputStream << sDelimiter;
            }
            // Add delimiter length to current position.
            currentColumn += iDelimiterLength;
            // Count the tab we passed
            curTabCount++;
            columnWidth = 0;
            indenting = true;
        }
        // If we hit the end of line...
        else if (*p == '\n')
        {
            // Calculate next line break column.
            breakPos = ifnGetBreak (p+1, 0, iDelimiterLength, maxWidth);
            outputStream << '\n';

            // Reset counts.
            curTabCount = 0;
            columnWidth = 0;
            currentColumn = 0;
            indenting = false;
        }
        // If it's a printable character...
        else
        {
            // If we already reached our line break point...
            if (currentColumn >= breakPos)
            {
                // Break here.
                outputStream << '\n';
                // Since there is a break here, we should eat up multiple spaces
                // that would indent the next line.
                while (*p == ' ')
                {
                    p++;
                }
                // Space over for hanging indent.
                if (indenting)
                {
                    // Calculate next break point.
                    breakPos = ifnGetBreak (p, iHangingIndent - 1, iDelimiterLength, maxWidth);
                    for (currentColumn=0; currentColumn<iHangingIndent; currentColumn++)
                    {
                        outputStream << ' ';
                    }
                }
                // No tabs found in this line, so we start again at column 1.
                else
                {
                    // Calculate next break point.
                    breakPos = ifnGetBreak (p, 0, iDelimiterLength, maxWidth);
                    currentColumn = 1;
                }
            }
            // Nothing special happened, so increment column.
            else
            {
                currentColumn++;
            }
            // Print character..
            outputStream << *p;
            columnWidth++;
        }
        // Examine next character in string.
        p++;
    }
}

// Find the column where the next line break should occur.  If indent > 0
// assume that many leading spaces will be inserted.
//
// Parameters:
//     sString          - string with embedded tabs
//     indent           - amount of leading indent
//     iDelimiterLength - length of string inserted in place of tabs
//     maxWidth[]       - array of column widths between tabs
//
// Returns:
//     The output column where a line break should occur.
//
int ifnGetBreak (const char* sString, int indent, int iDelimiterLength, int maxWidth[])
{
    int         tabCount;       // Number of tabs in line
    int         currentColumn;  // Current output column
    int         lastTab;        // Position of most recent tab stop
    int         breakPos;       // Column where this line should break
    const char* p;
    const int   maxLength = 78; // Maximum line length, before wrapping

    tabCount = 0;
    currentColumn = indent;
    breakPos = 0;
    lastTab = 0;
    p = sString;
    while (*p != '\0')
    {
        currentColumn++;
        // If we found a tab...
        if (*p == '\t')
        {
            // Adjust current position based on tab.
            currentColumn = lastTab + maxWidth[tabCount] + iDelimiterLength;
            tabCount++;
            lastTab = currentColumn;
        }
        // If we hit the end of line...
        else if (*p == '\n')
        {
            // Line break at line feed.
            breakPos = currentColumn;
            break;
        }
        // If it's a space, save the position.
        else if (*p == ' ')
        {
            // Set potential line break after space character.
            breakPos = currentColumn;
        }
        // If we would be over our max line length, exit loop.
        if (currentColumn >= maxLength)
        {
            break;
        }
        p++;
    }

    // Break at current character if no spaces were found and we didn't
    // reach an end-of-line character.
    if ((breakPos == 0) || (*p == '\0'))
    {
        breakPos = currentColumn;
    }

    return(breakPos);
}

