//****************************************************************************
//
//  Copyright (c) 1999, 2000  Broadcom Corporation
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
//  Filename:       IpAddress.cpp
//  Author:         Cliff Danielson
//  Creation Date:  August 3, 1999
//
//****************************************************************************
//  Description:
//      This class is an abstraction of an IP address, providing helper methods
//      to convert to/from various formats.
//
//****************************************************************************


//********************** Include Files ***************************************

#include "IpAddress.h"
#include <stdio.h>
#include <iomanip>

//********************** Local Types *****************************************

//********************** Local Constants *************************************

//********************** Local Variables *************************************

//********************** Forward Declaration *********************************

//********************** Global instances ************************************

const BcmIpV4Address kIpV4AllZerosIp(0, 0, 0, 0);
const BcmIpV4Address kIpV4BroadcastIp(0xff, 0xff, 0xff, 0xff);
const BcmIpV4Address kIpV4IgmpAllHostsIp(0xe0, 0x00, 0x00, 0x01);
const BcmIpV4Address kIpV4SoftwareLoopbackIp(127, 0, 0, 1);

//********************** Class Method Implementations ************************

BcmIpV4Address::~BcmIpV4Address() 
{
}

BcmIpV4Address::BcmIpV4Address(const char *pString)
{
    Set( pString );
}

BcmIpV4Address::BcmIpV4Address( const byte* buf )
{
    fU32Rep = buf[0];
    fU32Rep <<= 8;
    fU32Rep |= buf[1];
    fU32Rep <<= 8;
    fU32Rep |= buf[2];
    fU32Rep <<= 8;
    fU32Rep |= buf[3];

}

BcmIpV4Address::BcmIpV4Address( byte b0, byte b1, byte b2, byte b3 )
{
    fU32Rep = b0;
    fU32Rep <<= 8;
    fU32Rep |= b1;
    fU32Rep <<= 8;
    fU32Rep |= b2;
    fU32Rep <<= 8;
    fU32Rep |= b3;
}


ostream& BcmIpV4Address::Print( ostream& ostrm ) const
{
    uint8 byteRep[4];

    // for dotted decimal format, left adjust decimal numbers
    // with no fill char.
    char old_fill = ostrm.fill( 0 );

    Get(byteRep[0], byteRep[1], byteRep[2], byteRep[3]);

    ostrm << dec << uint16( byteRep[ 0 ] ) << '.'
        << uint16( byteRep[ 1 ] ) << '.'
        << uint16( byteRep[ 2 ] ) << '.'
        << uint16( byteRep[ 3 ] );

    // restore entry formatting
    ostrm.fill( old_fill );
    return ostrm;
}


// Allows the 4 bytes of the IP address to be set.
//
// Parameters:
//      ipAddress1-4 - the 4 bytes of the address to be used.
//
// Returns:  Nothing.
//
void BcmIpV4Address::Set(uint8 ipAddress1, uint8 ipAddress2, uint8 ipAddress3,
                       uint8 ipAddress4)
{
    fU32Rep = ipAddress1;
    fU32Rep <<= 8;
    fU32Rep |= ipAddress2;
    fU32Rep <<= 8;
    fU32Rep |= ipAddress3;
    fU32Rep <<= 8;
    fU32Rep |= ipAddress4;
}

// Allows the IP address to be set by parsing from an ASCII string format
// (i.e. "192.168.190.220").
//
// Note that the numbers must be in decimal, they must be separated by
// periods (.), and the string must be terminated by a '\0'.
//
// Parameters:
//      pString - the ASCII string containing the dotted-decimal IP address.
//      pBytesRead - output variable (optional) into which the number of
//                   ASCII characters read will be placed.
//
// Returns:
//      true if successful.
//      false if there was a problem (NULL string, invalid format, etc.).
//
bool BcmIpV4Address::Set(const char *pString, unsigned int *pBytesRead)
{
    int ipAddress1, ipAddress2, ipAddress3, ipAddress4;
    int numScanned;
    unsigned int dummyBytesRead;

    // Validate the parameter.
    if (pString == NULL)
    {
        return false;
    }

    if (pBytesRead == NULL)
    {
        pBytesRead = &dummyBytesRead;
    }

    // Try to convert the string to 4 values.
    numScanned = sscanf(pString, "%d.%d.%d.%d%n", &ipAddress1, &ipAddress2,
                        &ipAddress3, &ipAddress4, pBytesRead);

    // Check the result, and the range of each value.
    if ((numScanned != 4) ||
        ((ipAddress1 & ~0xff) != 0) ||
        ((ipAddress2 & ~0xff) != 0) ||
        ((ipAddress3 & ~0xff) != 0) ||
        ((ipAddress4 & ~0xff) != 0))
    {
        *pBytesRead = 0;

        return false;
    }

    // Set the values.
    Set(ipAddress1, ipAddress2, ipAddress3, ipAddress4);

    return true;
}


// Allows all 4 IP address bytes to be retrieved.
//
// Parameters:
//      ipAddress1-4 - the 4 bytes of the address to be retrieved.
//
// Returns:  Nothing.
//
void BcmIpV4Address::Get(uint8 &ipAddress1, uint8 &ipAddress2, uint8 &ipAddress3,
                       uint8 &ipAddress4) const
{
    ipAddress1 = (uint8) ((fU32Rep >> 24) & 0xff);
    ipAddress2 = (uint8) ((fU32Rep >> 16) & 0xff);
    ipAddress3 = (uint8) ((fU32Rep >>  8) & 0xff);
    ipAddress4 = (uint8) (fU32Rep & 0xff);
}


// Converts the 4 IP address bytes to an ASCII string in dotted-decimal
// format.
//
// Note that the pString parameter must not be NULL, and it must point to a
// buffer of at least 16 bytes in order to hold the entire string.
//
// Parameters:
//      pString - pointer to a string where the IP address should be
//                written.
//
// Returns:
//      true if successful.
//      false if there was a problem (NULL string).
//
bool BcmIpV4Address::Get(char *pString, unsigned int bufferSize) const
{
    uint8 byteRep[4];
    
    if( bufferSize < 16 )
    {
        return false;
    }

    // Validate the parameter.
    if (pString == NULL)
    {
        return false;
    }

    Get(byteRep[0], byteRep[1], byteRep[2], byteRep[3]);

    // Convert to ASCII dotted-decimal format.
    sprintf(pString, "%d.%d.%d.%d", byteRep[0], byteRep[1], byteRep[2],
            byteRep[3]);

    return true;
}

