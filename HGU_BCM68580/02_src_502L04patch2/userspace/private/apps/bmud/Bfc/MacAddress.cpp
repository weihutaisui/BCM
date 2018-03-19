//****************************************************************************
//
// Copyright (c) 1999-2010 Broadcom Corporation
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
//  Filename:       MacAddress.cpp
//  Author:         David Pullen
//  Creation Date:  May 3, 1999
//
//****************************************************************************
//  Description:
//      This is a utility class that encapsulates an Ethernet MAC address.  It
//      is really just an array of 6 bytes, but we do lots of stuff with them
//      (comparisons, reading from/writing to octet buffers, etc.).
//
//****************************************************************************

//********************** Include Files ***************************************

// My api and definitions...
#include "MacAddress.h"

// For memcpy() and other memory functions.
#include <string.h>
#include <stdio.h>

#include <iomanip>

//********************** Local Types *****************************************

//********************** Local Constants *************************************

//********************** Local Variables *************************************

//********************** Global instances ************************************

const BcmMacAddress kAllZerosMac(0, 0, 0, 0, 0, 0);
const BcmMacAddress kBroadcastMac(0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
const BcmMacAddress kIgmpAllHostsMac(0x01, 0x00, 0x5e, 0x00, 0x00, 0x01);

// bcastCmMacAddr - global function which returns a reference to MacAddress
//      object which represents the broadcast MAC address for all CM.
//      enum class constants do not work well for MacAddress, since
//      each address is (6) bytes long.  hence the global function.
//
BcmMacAddress& bcastCmMacAddr()
{
    static BcmMacAddress bcast_cm_mac( 0x01, 0xE0, 0x2F, 0x00, 0x00, 0x01 );
    return bcast_cm_mac;
}

// bcastCmtsMacAddr - global function which returns a reference to MacAddress
//      object which represents the broadcast MAC address for all CMTS.
//
BcmMacAddress& bcastCmtsMacAddr()
{
    static BcmMacAddress bcast_cmts_mac( 0x01, 0xE0, 0x2F, 0x00, 0x00, 0x02 );
    return bcast_cmts_mac;
}

//********************** Class Method Implementations ************************


// Default Constructor.  Initializes the state of the object...
//
// Parameters:  None.
//
// Returns:  N/A
//
BcmMacAddress::BcmMacAddress(void)
{
    // Set protected and private members to a known state.
    fMacValue1 = 0;
    fMacValue2 = 0;
}


// Initializing Constructor.  Initializes the state of the object...
//
// Parameters:  None.
//
// Returns:  N/A
//
BcmMacAddress::BcmMacAddress(uint8 macAddress1, uint8 macAddress2, 
                             uint8 macAddress3, uint8 macAddress4, 
                             uint8 macAddress5, uint8 macAddress6)
{
    // Just defer this to the helper method.
    Set(macAddress1, macAddress2, macAddress3, 
        macAddress4, macAddress5, macAddress6);
}


// Copy Constructor.  Initializes the state of this object to match that
// of the instance passed in.
//
// Parameters:
//      otherInstance - reference to the object to copy.
//
// Returns:  N/A
//
BcmMacAddress::BcmMacAddress(const BcmMacAddress &otherInstance)
{
    // Just call the assignment operator to do the work.
    *this = otherInstance;
}


// Destructor.  Frees up any memory/objects allocated, cleans up internal
// state.
//
// Parameters:  N/A
//
// Returns:  N/A
//
BcmMacAddress::~BcmMacAddress()
{
    // Nothing to do...
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
BcmMacAddress & BcmMacAddress::operator = (const BcmMacAddress &otherInstance)
{
    // Copy the other instance's data.
    fMacValue1 = otherInstance.fMacValue1;
    fMacValue2 = otherInstance.fMacValue2;

    return *this;
}


// Allows the 6-bytes MAC address to be set.
//
// Parameters:
//      macAddress1-6 - the 6 bytes of the address to be used.
//
// Returns:  Nothing.
//
void BcmMacAddress::Set(uint8 macAddress1, uint8 macAddress2, uint8 macAddress3,
                        uint8 macAddress4, uint8 macAddress5, uint8 macAddress6)
{
    // Store the 6 bytes as packed values in host order for use in comparisons.
    fMacValue1 = macAddress1;
    fMacValue1 <<= 8;
    fMacValue1 |= macAddress2;
    fMacValue1 <<= 8;
    fMacValue1 |= macAddress3;
    fMacValue1 <<= 8;
    fMacValue1 |= macAddress4;

    fMacValue2 = macAddress5;
    fMacValue2 <<= 8;
    fMacValue2 |= macAddress6;
}

// Construct a MacAddress object from a byte buffer.
//
// Parameters:
//      buf - source buffer.
//
BcmMacAddress::BcmMacAddress( const byte* buf )
{
    Set(buf);
}

// Allows the MAC address to be set by parsing from an ASCII string format
// (i.e. "01:02:03:04:05:06").
//
// Note that the numbers must be in hex, they can be separated by colons (:)
// or periods (.), and the string must be terminated by a '\0'.
//
// Parameters:
//      pString - the ASCII string containing the 6 bytes of the MAC address.
//      pBytesRead - output variable (optional) into which the number of
//                   ASCII characters read will be placed.
//
// Returns:
//      true if successful.
//      false if there was a problem (NULL string, invalid format, etc.).
//
bool BcmMacAddress::Set(const char *pString, unsigned int *pBytesRead)
{
    int numScanned;
    int values[6];
    unsigned int dummyBytesRead;

    // Do a little validation first.
    if (pString == NULL)
    {
        return false;
    }

    if (pBytesRead == NULL)
    {
        pBytesRead = &dummyBytesRead;
    }

    // Parse the bytes out.  Each address byte is required to be a hex number;
    // the bytes can be separated by either a : or a ., and there can be spaces
    // interspersed.
    numScanned = sscanf(pString, "%x%*[:.]%x%*[:.]%x%*[:.]%x%*[:.]%x%*[:.]%x%n",
                        &(values[0]), &(values[1]), &(values[2]),
                        &(values[3]), &(values[4]), &(values[5]), pBytesRead);

    // Do some post-parsing validation - make sure enough values were parsed,
    // and that each value is in the correct range.
    if (numScanned != 6)
    {
        *pBytesRead = 0;

        return false;
    }
    for (int i = 0; i < 6; i++)
    {
        if ((values[i] < 0) || (values[i] > 255))
        {
            *pBytesRead = 0;

            return false;
        }
    }

    // Vector to the common handler.
    Set(values[0], values[1], values[2], values[3], values[4], values[5]);

    return true;
}

// Does EVERYTHING that the above Set(const char *pString, unsigned int *pBytesRead) does
// But it verifies that the entered MAC Address is valid.
// It accept the following formatting of MAC Address entries:
//
// 01:23:45:67:89:ab
// 01.23.45.67.89.ab
// 01-23-45-67-89-ab
// 01 23 45 67 89 ab
//
// Note that the numbers must be in hex, they can be separated by colons (:)
// or periods (.), and the string must be terminated by a '\0'.
//
// Parameters:
//      pString - the ASCII string containing the 6 bytes of the MAC address.
//      pBytesRead - output variable (optional) into which the number of
//                   ASCII characters read will be placed.
//
// Returns:
//      true if successful.
//      false if there was a problem (NULL string, invalid format, etc.).
//
bool BcmMacAddress::VerifyAndSet(const char *pString, unsigned int *pBytesRead)
{
    int numScanned;
    int values[6];
    unsigned int dummyBytesRead;

    // Do a little validation first.
    if (pString == NULL)
    {
        return false;
    }

    if (pBytesRead == NULL)
    {
        pBytesRead = &dummyBytesRead;
    }
    for(unsigned int j = 0; j < strlen(pString); j++)
    {
        switch(j)
        {
            case 0:
            case 1:
            case 3:
            case 4:
            case 6:
            case 7:
            case 9:
            case 10:
            case 12:
            case 13:
            case 15:
            case 16:
                if( (pString[j] >= '0' && pString[j] <= '9') ||
                    (pString[j] >= 'a' && pString[j] <= 'f') ||
                    (pString[j] >= 'A' && pString[j] <= 'F')   )
                {
                    continue;
                }
                else 
                {
                    return(false);
                }
                break;
            default:
//                if( (pString[j] == '-') ||
//                    (pString[j] == '.') ||
//                    (pString[j] == ' ')   )
//                {
//                    pString[j] = ':';
//                }
                break;
        }
    }

    // Parse the bytes out.  Each address byte is required to be a hex number;
    // the bytes can be separated by either a : or a ., and there can be spaces
    // interspersed.
    numScanned = sscanf(pString, "%x%*[:.-]%x%*[:.-]%x%*[:.-]%x%*[:.-]%x%*[:.-]%x%n",
                        &(values[0]), &(values[1]), &(values[2]),
                        &(values[3]), &(values[4]), &(values[5]), pBytesRead);

    // Do some post-parsing validation - make sure enough values were parsed,
    // and that each value is in the correct range.
    if (numScanned != 6)
    {
        *pBytesRead = 0;

        return false;
    }
    for (int i = 0; i < 6; i++)
    {
        if ((values[i] < 0) || (values[i] > 255))
        {
            *pBytesRead = 0;

            return false;
        }
    }

    // Vector to the common handler.
    Set(values[0], values[1], values[2], values[3], values[4], values[5]);

    return true;
}


// Allows all 6 MAC address bytes to be retrieved.
//
// Parameters:
//      macAddress1-6 - the 6 bytes of the address to be retrieved.
//
// Returns:  Nothing.
//
void BcmMacAddress::Get(uint8 &macAddress1, uint8 &macAddress2, uint8 &macAddress3,
                        uint8 &macAddress4, uint8 &macAddress5, uint8 &macAddress6) const
{
    macAddress1 = (uint8) ((fMacValue1 >> 24) & 0xff);
    macAddress2 = (uint8) ((fMacValue1 >> 16) & 0xff);
    macAddress3 = (uint8) ((fMacValue1 >>  8) & 0xff);
    macAddress4 = (uint8) (fMacValue1 & 0xff);
    macAddress5 = (uint8) ((fMacValue2 >> 8) & 0xff);
    macAddress6 = (uint8) (fMacValue2 & 0xff);
}


// Converts the 6 MAC address bytes to an ASCII string.
//
// Note that the pString parameter must not be NULL, and it must point to a
// buffer of at least 18 bytes in order to hold the entire hex string, or
// 24 bytes to hold the entire decimal string, including the \0 string
// terminator.
//
// Parameters:
//      pString - pointer to a string where the MAC address should be
//                written.
//      numbersAsHex - tells whether to format the string as hex or decimal
//                     numbers.
//
// Returns:
//      true if successful.
//      false if there was a problem (NULL string).
//
bool BcmMacAddress::Get(char *pString, bool numbersAsHex, bool useColons) const
{
    // Do some validation first.
    if (pString == NULL)
    {
        return false;
    }

    uint8 macAddress[6];

    Get(macAddress[0], macAddress[1], macAddress[2], 
        macAddress[3], macAddress[4], macAddress[5]);

    // Always write out using the most commonly seen format (colon separated,
    // no spaces).
    if (numbersAsHex)
    {
        if(useColons)
        {
           sprintf(pString, "%02x:%02x:%02x:%02x:%02x:%02x",
                   macAddress[0], macAddress[1], macAddress[2],
                   macAddress[3], macAddress[4], macAddress[5]);
        }
        else
        {
           sprintf(pString, "%02x-%02x-%02x-%02x-%02x-%02x",
                   macAddress[0], macAddress[1], macAddress[2],
                   macAddress[3], macAddress[4], macAddress[5]);
        }
    }
    else
    {
        if(useColons)
        {
           sprintf(pString, "%d:%d:%d:%d:%d:%d",
                   (unsigned int) macAddress[0], (unsigned int) macAddress[1], 
                   (unsigned int) macAddress[2], (unsigned int) macAddress[3], 
                   (unsigned int) macAddress[4], (unsigned int) macAddress[5]);
        }
        else
        {
           sprintf(pString, "%d-%d-%d-%d-%d-%d",
                   (unsigned int) macAddress[0], (unsigned int) macAddress[1], 
                   (unsigned int) macAddress[2], (unsigned int) macAddress[3], 
                   (unsigned int) macAddress[4], (unsigned int) macAddress[5]);
        }
    }

    return true;
}


// print formatted BcmMacAddress to specified ostream.
//
// Parameters:
//      ostream& ostrm - reference to the destination output stream.
//
// Returns:
//      reference to the destination output stream.
ostream& BcmMacAddress::Print( ostream& ostrm ) const
{
    uint8 macAddress[6];

    Get(macAddress[0], macAddress[1], macAddress[2], 
        macAddress[3], macAddress[4], macAddress[5]);
    
    ostrm.fill( '0' );

    ostrm << hex 
          << setw(2) << (unsigned int) macAddress[0] << ':'
          << setw(2) << (unsigned int) macAddress[1] << ':'
          << setw(2) << (unsigned int) macAddress[2] << ':'
          << setw(2) << (unsigned int) macAddress[3] << ':'
          << setw(2) << (unsigned int) macAddress[4] << ':'
          << setw(2) << (unsigned int) macAddress[5]
          << dec;

    ostrm.fill( ' ' );

    return ostrm;
}

