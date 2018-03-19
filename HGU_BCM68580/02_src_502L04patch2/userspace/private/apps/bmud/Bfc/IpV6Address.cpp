//****************************************************************************
//
// Copyright (c) 2006-2010 Broadcom Corporation
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
//  Filename:       IpV6Address.cpp
//  Author:         Sean Dunlap
//  Creation Date:  January 11, 2006
//
//****************************************************************************

// Set this to 1 to enable the regression test code.
#define REGRESSION_TEST 0

//********************** Include Files ***************************************

#include "IpV6Address.h"
#include "IpAddress.h"
#include <stdio.h>
#include <iomanip>
#include <iostream>

//********************** Local Types *****************************************

//********************** Local Constants *************************************

//********************** Local Variables *************************************

//********************** Forward Declaration *********************************
static int inet_pton6(const char *src, uint8 *dst);
static int inet_pton4(const char *src, uint8 *dst);
static const char * inet_ntop6(const uint8 *src, char *dst, int size);
static const char * inet_ntop4(const uint8 *src, char *dst, int size);


//********************** Global instances ************************************

// ===============
// IPv6 Addresses
// ===============

const BcmIpV6Address kIPv6UnspecifiedIpV6Address("::");
const BcmIpV6Address kIPv6LoopbackIpV6Address("::1");

const BcmIpAddress kIPv6UnspecifiedAddress("::");
const BcmIpAddress kIPv6LoopbackAddress("::1");


// ==============
// IPv6 prefixes
// ==============

// Aggregatable Global Unicast Addresses Prefix = 001
const BcmIpV6Address kAggregateGlobalUnicastAddressPrefix("2000::/3");

// Link Local Unicast Addresses Prefix = 1111 1110 10
const BcmIpV6Address kLinkLocalUnicastAddressPrefix("fe80::/64");

// Site Local Unicast Addresses Prefix = 1111 1110 11
const BcmIpV6Address kSiteLocalUnicastAddressPrefix("fec0::/48");

// Multicast Addresses Prefix = 1111 1111
const BcmIpV6Address kMulticastAddressPrefix("ff::/8");

// Solicited Nodes Addresses Prefix = 1111 1110 11
const BcmIpV6Address kSolicitedNodeAddressPrefix("FF02:0:0:0:0:1:FF00::/104");

//IPv4-compatible IPv6 address
// |                80 bits               | 16 |      32 bits        |
// +--------------------------------------+--------------------------+
// |0000..............................0000|0000|    IPv4 address     |
// +--------------------------------------+----+---------------------+
const BcmIpV6Address kIPv4CompatibleIPv6AddressPrefix("::/96");

// IPv4-mapped IPv6 address
// |                80 bits               | 16 |      32 bits        |
// +--------------------------------------+--------------------------+
// |0000..............................0000|FFFF|    IPv4 address     |
// +--------------------------------------+----+---------------------+
const BcmIpV6Address kIPv4MappedIPv6AddressPrefix("::ffff:0.0.0.0/96");

//  All Nodes Addresses:    FF01:0:0:0:0:0:0:1
//                          FF02:0:0:0:0:0:0:1
const BcmIpV6Address kAllNodesMulticastAddress1("FF01:0:0:0:0:0:0:1");
const BcmIpV6Address kAllNodesMulticastAddress2("FF02:0:0:0:0:0:0:1");

// All Routers Addresses:   FF01:0:0:0:0:0:0:2 - node-local
//                          FF02:0:0:0:0:0:0:2
//                          FF05:0:0:0:0:0:0:2
const BcmIpV6Address kNodeLocalAllRoutersMulticastAddress("FF01:0:0:0:0:0:0:2");
const BcmIpV6Address kLinkLocalAllRoutersMulticastAddress("FF02:0:0:0:0:0:0:2");
const BcmIpV6Address kSiteLocalAllRoutersMulticastAddress("FF05:0:0:0:0:0:0:2");


//********************** Class Method Implementations ************************

BcmIpV6Address::BcmIpV6Address() 
{
    memset( fIpV6Address, 0, 16 );

    // if prefix length is zero, this is an IP address.
    // otherwise it represents a prefix. 
    fPrefixLengthBits = 0;
}

BcmIpV6Address::BcmIpV6Address(const char *pString)
{
    fPrefixLengthBits = 0;

    Set( pString );
}

/// Copy constructor.
///
/// \param
///     rhs - the instance to be copied.
///
BcmIpV6Address::BcmIpV6Address( const BcmIpV6Address& rhs )
{
    memcpy( fIpV6Address, &rhs.fIpV6Address, sizeof(fIpV6Address) );
    fPrefixLengthBits = rhs.fPrefixLengthBits;
}

/// Destructor.
///
BcmIpV6Address::~BcmIpV6Address() 
{
}


BcmIpV6Address::BcmIpV6Address( const uint8 * pBuf, uint8 prefixLength )
{
    memcpy( fIpV6Address, pBuf, sizeof(fIpV6Address) );
    fPrefixLengthBits = prefixLength;
}



/// Reset data members to default values.
///
void BcmIpV6Address::reset()
{
    memset( fIpV6Address, 0, sizeof(fIpV6Address) );
}

/// Write the IpAddress to the end of specified buffer in network order.  
/// This is commonly used as part of the IP protocol header.  
///
/// \param
///      octetBuffer - reference to the destination buffer.
///
/// \retval
///      true if successful.
/// \retval
///      false if there was a problem (not enough space in the 
///          output buffer).
///
bool BcmIpV6Address::WriteTo(BcmOctetBuffer &octetBuffer) const
{ 
    return octetBuffer.AddToEnd( fIpV6Address, sizeof( fIpV6Address ) );
}
    
/// This methods causes the object to read its 16 bytes from the specified
/// location in the buffer.  This is the opposite of WriteTo().  Note that
/// the offset parameter will be adjusted to reflect the next position in the
/// buffer that needs to read from.
///
/// Note that if the read is successful, then the offset parameter will be
/// incremented by 4.  If the read fails, then neither the IP address nor
/// the offset parameter will be modified.
///
/// \param
///      octetBuffer - reference to the buffer from which the bytes should
///                    be read.
/// \param
///      offset - the offset into the buffer where the first IP address byte
///               is.
///
/// \retval
///      true if successful.
/// \retval
///      false if there was a problem (not enough bytes of data in the
///          buffer).
///
bool BcmIpV6Address::ReadFrom(const BcmOctetBuffer &octetBuffer, unsigned int &offset)
{
    return octetBuffer.Read(offset, fIpV6Address, 16 );
}

// Allows the IP address to be set by parsing from an ASCII string format
// (i.e. "FEDC:BA98:7654:3210:FEDC:BA98:7654:3210").
//
// Note that the numbers must be in hex, they must be separated in groups 
// of 4 nibbles by colons, and the string must be terminated by a '\0'.
//
// Parameters:
//      pString - the ASCII string containing the dotted-decimal IP address.
//
// Returns:
//      true if successful.
//      false if there was a problem (NULL string, invalid format, etc.).
//
bool BcmIpV6Address::Set(const char *pString, unsigned int *pBytesRead)
{
    char pStringCopy[256];
    strncpy(pStringCopy, pString, sizeof(pStringCopy));

    // Terminate the string on whitespace.
    {
        char *pWhitespace;

        pWhitespace = strchr( pStringCopy, ' ' );
        if (pWhitespace != NULL)
        {
            *pWhitespace = '\0';
        }
    
        pWhitespace = strchr( pStringCopy, '\t' );
        if (pWhitespace != NULL)
        {
            *pWhitespace = '\0';
        }
    }

    // first strip any trailing /%d in case the user specified a
    // prefix in the string
    char * pPrefix;
    pPrefix = strchr( pStringCopy, '/' );

    if( pPrefix )
    {
        // strip the prefix from the pString before calling inet_pton6, which doesn't
        // support the prefix convention
        *pPrefix++ = '\0';
        sscanf( pPrefix, "%d", &fPrefixLengthBits );
    }

    if( inet_pton6(pStringCopy, fIpV6Address ) == 0 )
    {
        if( pBytesRead )
            *pBytesRead = 0;

        return false;
    }
        
    if( pBytesRead )
        *pBytesRead = strlen( pStringCopy );

    return true;
}


// Converts the 16 IP address bytes to an ASCII string in dotted-decimal
// format.
//
// Note that the pString parameter must not be NULL, and it must point to a
// buffer of at least 16 bytes in order to hold the entire string.
//
// Parameters:
//      pString - pointer to a string where the IP address should be
//                written.
//      bufferSize - size of the buffer to write to
//
// Returns:
//      true if successful.
//      false if there was a problem (NULL string).
//
bool BcmIpV6Address::Get(char *pString, unsigned int bufferSize) const
{
    char tempBuffer[64];

    // Validate the parameter.
    if (pString == NULL)
    {
        return false;
    }

    // assume buffer has at least 64 bytes available. 
    // if you are reading this, you probably found an overrun.  sorry ;-)
    inet_ntop6( fIpV6Address, tempBuffer, sizeof(tempBuffer) );

    // if the prefix length is non-zero, then this address a prefix.
    // append the prefix length to output string along with a '/' character 
    // to reflect this. 
    if( fPrefixLengthBits != 0 )
    {
        char prefix[20];
        sprintf(prefix, "/%d", fPrefixLengthBits);
        strcat( tempBuffer, prefix );
    }

    if( strlen(tempBuffer) <= bufferSize )
    {
        strcpy( pString, tempBuffer );
        return true;
    }

    return false;
}

/// Extracts the 16 IP address bytes to a 16 byte buffer in binary form.
///
/// \param
///      pIPv6Address - pointer to 16 byte buffer to copy the address into
///
/// \retval
///      true if successful.
/// \retval
///      false if there was a problem (NULL string).
///
void BcmIpV6Address::Get(uint8 * pIPv6Address) const
{
    memcpy( pIPv6Address, fIpV6Address, 16 );
}

///  Maps a multicast destination IPv6 address to a 
///  destination MAC address per RFC 2464
///
/// \param
///      macAddress - MAC address to write to
///
/// \retval
///      true if successful.
/// \retval
///      false if there was a problem (NULL string).
///
void BcmIpV6Address::GetMulticastDestinationMacAddress( BcmMacAddress & macAddress ) const
{
 
    // An IPv6 packet with a multicast destination address DST, consisting
    // of the sixteen octets DST[1] through DST[16], is transmitted to the
    // Ethernet multicast address whose first two octets are the value 3333
    // hexadecimal and whose last four octets are the last four octets of
    // DST.    
    uint8 macAddr[6];

    macAddr[0] = 0x33;
    macAddr[1] = 0x33;
    macAddr[2] = fIpV6Address[12];
    macAddr[3] = fIpV6Address[13];
    macAddr[4] = fIpV6Address[14];
    macAddr[5] = fIpV6Address[15];

    macAddress.Set(macAddr);
}

/// AppendInterfaceToken
///
/// Appenends an 8 byte interface token, which is derived from the 
/// ethernet MAC address as described in 
/// RFC2464 - Transmission of IPv6 Packets over Ethernet Networks
///
/// \param
///     macAddress - mac address used to derive interface token
///
bool BcmIpV6Address::AppendInterfaceToken( const BcmMacAddress & macAddress )
{
    if( fPrefixLengthBits > 64 )
    {
        return false;
    }
    uint8 macAddr[6];

    macAddress.getAddr(macAddr);

    // The OUI of the Ethernet address (the first three octets) becomes the
    // company_id of the EUI-64 (the first three octets).  
    fIpV6Address[8] = macAddr[0];
    fIpV6Address[9] = macAddr[1];
    fIpV6Address[10] = macAddr[2];

    // The fourth and fifth octets of the EUI are set to the fixed value FFFE hexadecimal.
    fIpV6Address[11] = 0xff;
    fIpV6Address[12] = 0xfe;

    // The last three octets of the Ethernet address become the last three
    // octets of the EUI-64.
    fIpV6Address[13] = macAddr[3];
    fIpV6Address[14] = macAddr[4];
    fIpV6Address[15] = macAddr[5];    

    // The Interface Identifier is then formed from the EUI-64 by
    // complementing the "Universal/Local" (U/L) bit, which is the next-to-
    // lowest order bit of the first octet of the EUI-64.
    fIpV6Address[8] ^= 0x02;

    return true;
}


// Print formatted object contents to specified ostream.
//
// \param
//      ostream& ostrm - reference to the destination output stream.
//
// \return
//      Reference to the destination output stream.
//
// from RFC3513 section 2.2 Text Representation of Addresses
// 2.2 Text Representation of Addresses
//
//   There are three conventional forms for representing IPv6 addresses as
//   text strings:
//
//   The preferred form is x:x:x:x:x:x:x:x, where the 'x's are the
//   hexadecimal values of the eight 16-bit pieces of the address.
//   Examples:
//
//      FEDC:BA98:7654:3210:FEDC:BA98:7654:3210
//
//      1080:0:0:0:8:800:200C:417A
//
//   Note that it is not necessary to write the leading zeros in an
//   individual field, but there must be at least one numeral in every
//   field
//
//   The use of "::" indicates one or more groups of 16 bits of zeros.
//
//   An alternative form that is sometimes more convenient when dealing
//   with a mixed environment of IPv4 and IPv6 nodes is
//   x:x:x:x:x:x:d.d.d.d, where the 'x's are the hexadecimal values of
//   the six high-order 16-bit pieces of the address, and the 'd's are
//   the decimal values of the four low-order 8-bit pieces of the
//   address (standard IPv4 representation). 
//
//   The text representation of IPv6 address prefixes is similar to the
//   way IPv4 addresses prefixes are written in CIDR notation [CIDR].  An
//   IPv6 address prefix is represented by the notation:
//
//      ipv6-address/prefix-length
//
//   where
//
//      ipv6-address    is an IPv6 address in any of the notations listed
//                      in section 2.2.
//

ostream& BcmIpV6Address::Print( ostream& ostrm ) const
{
 	char printBuffer[100];

    inet_ntop6( fIpV6Address, printBuffer, sizeof( printBuffer ) );

    // append prefix if specified
    if( fPrefixLengthBits )
    {
        char prefix[10];
        sprintf(prefix, "/%d", fPrefixLengthBits);
        strcat( printBuffer, prefix );
    }

    return ostrm << printBuffer;
}

/// Assignment operator.
///
/// \param
///     rhs - the instance to be copied.
///
/// \return
///     A reference to this object.
///
BcmIpV6Address& BcmIpV6Address::operator=( const BcmIpV6Address& rhs )
{
    if( this == &rhs )
    {
        // special case: assignment to self
        return *this;
    }

    memcpy( fIpV6Address, rhs.fIpV6Address, sizeof(fIpV6Address));
    fPrefixLengthBits = rhs.fPrefixLengthBits;
    
    return *this;
}

/// Less than operator.
///
/// \param
///     rhs - the instance to be compared to.
///
/// \retval
///     true if this instance is less than the one specified.
/// \retval
///     false if not less than.
///
bool BcmIpV6Address::operator<( const BcmIpV6Address& rhs ) const
{
    unsigned int i;
    for( i=0; i<sizeof(fIpV6Address); i++ )
    {
        if( fIpV6Address[i] < rhs.fIpV6Address[i] )
            return true;
    }

    return false;
}

/// Less than or equal operator.
///
/// \param
///     rhs - the instance to be compared to.
///
/// \retval
///     true if this instance is less than or equal to the one specified.
/// \retval
///     false if not less than or equal to.
///
bool BcmIpV6Address::operator<=( const BcmIpV6Address& rhs ) const
{
    return (*this < rhs) || (*this == rhs);
}



/// Equality operator.
///
/// \param
///     rhs - the instance to be compared to.
///
/// \retval
///     true if this instance is equal to the one specified.
/// \retval
///     false if not equal.
///
bool BcmIpV6Address::operator==( const BcmIpV6Address& rhs ) const
{
    bool ret_val = true;
    
    if( memcmp( fIpV6Address, rhs.fIpV6Address, sizeof( fIpV6Address )  ) != 0 )
    {
        ret_val = false;
    }
    return ret_val;
}

/// Inequality operator.
///
/// \param
///     rhs - the instance to be compared to.
///
/// \retval
///     true if this instance is not equal to the one specified.
/// \retval
///     false if equal.
///
bool BcmIpV6Address::operator!=( const BcmIpV6Address& rhs ) const
{
    return !(*this==rhs);
}

/// Accessor for the byte value at the specified index.  The index must be
/// in the range 0..16.  Anything outside of this range will cause a 0 to be
/// returned.
///
/// \param
///     Index - the index of the value to be returned.  Must be 0..16.
///
/// \return
///     The value at the specified index.
///
unsigned char BcmIpV6Address::operator[] (int Index) const
{
  if ((Index < 0) || (Index > 15))
    return 0;
    
  return fIpV6Address[Index];
}


void BcmIpV6Address::Set(const uint8 *pBytes)
{
    // do a simple memcpy
    memcpy( fIpV6Address, pBytes, 16 );
}

/// Returns true if the prefix portion of the two addresses are equal.
///
/// \param
///      pIPv6Address - the other instance to compare the format prefix to
/// \param
///      prefixLengthBits - the length of the prefix in bits
/// \retval
///      true if the format prefix is equal
/// \retval
///      false if the format prefix is NOT equal
///
bool BcmIpV6Address::IsEqualWithPrefix( const BcmIpV6Address & prefix ) const
{
    unsigned int wholeBytesToCompare = prefix.fPrefixLengthBits / 8;
    unsigned int remainingBitsToCompare = prefix.fPrefixLengthBits %8;

    // special case where the prefix specified was not actually a prefix
    if( prefix.fPrefixLengthBits == 0 )
    {
        wholeBytesToCompare = 16;
    }

    if( wholeBytesToCompare && (memcmp( fIpV6Address, prefix.fIpV6Address, wholeBytesToCompare ) != 0) )
    {
        return false;
    }

    if (wholeBytesToCompare < 16)
    {
        // calculate mask for remaining bits
        unsigned char mask[] = { 0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe };

        if( (fIpV6Address[ wholeBytesToCompare ] & mask[remainingBitsToCompare]) !=
            (prefix.fIpV6Address[ wholeBytesToCompare ] & mask[remainingBitsToCompare]) )
        {
            return false;
        }

        if ((prefix.fIpV6Address[ wholeBytesToCompare ] & (~mask[remainingBitsToCompare])) != 0)
        {
            return false;
        }

        wholeBytesToCompare++;
        while (wholeBytesToCompare < 16)
        {
            if (prefix.fIpV6Address[wholeBytesToCompare] != 0)
            {
                return false;
            }
            wholeBytesToCompare++;
        }
    }

    return true;
}

/// Returns true if the prefix portion of the two addresses are equal.
///
/// \param
///      pIPv6Address - the other instance to compare the format prefix to
/// \param
///      prefixLengthBits - the length of the prefix in bits
/// \retval
///      true if the format prefix is equal
/// \retval
///      false if the format prefix is NOT equal
///
bool BcmIpV6Address::IsPrefixEqual( const BcmIpV6Address & prefix ) const
{
    unsigned int wholeBytesToCompare = prefix.fPrefixLengthBits / 8;
    unsigned int remainingBitsToCompare = prefix.fPrefixLengthBits %8;

    // special case where the prefix specified was not actually a prefix
    if( prefix.fPrefixLengthBits == 0 )
    {
        wholeBytesToCompare = 16;
    }

    if( wholeBytesToCompare && (memcmp( fIpV6Address, prefix.fIpV6Address, wholeBytesToCompare ) != 0) )
    {
        return false;
    }

    // calculate mask for remaining bits
    unsigned char mask[] = { 0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe };

    if( (fIpV6Address[ wholeBytesToCompare ] & mask[remainingBitsToCompare]) !=
        (prefix.fIpV6Address[ wholeBytesToCompare ] & mask[remainingBitsToCompare]) )
    {
        return false;
    }

    return true;
}

/// Returns the number of bits in this prefix
///
/// \retval
///      number of bits in the prefix
///
unsigned int BcmIpV6Address::PrefixLengthBits() const
{
    return fPrefixLengthBits;
}

/// Sets the number of bits in this prefix
///
///
void BcmIpV6Address::PrefixLengthBits( unsigned int prefixLen )
{
    fPrefixLengthBits = prefixLen;
}

/// IsUnspecifiedAddress() -  Returns true if the IP address is the 
///                             unspecified address.
///
/// 2.5.2 The Unspecified Address
/// The address 0:0:0:0:0:0:0:0 is called the unspecified address.  It
/// must never be assigned to any node.  It indicates the absence of an
/// address.  One example of its use is in the Source Address field of
/// any IPv6 packets sent by an initializing host before it has learned
/// its own address.
/// The unspecified address must not be used as the destination address
/// of IPv6 packets or in IPv6 Routing Headers.
///
/// \retval
///      true if this is the unspecified address.
/// \retval
///      false if this is NOT the unspecified address.
///
bool BcmIpV6Address::IsUnspecifiedAddress() const
{
    return ( *this == kIPv6UnspecifiedIpV6Address );
}

/// IsLoopbackAddress()
///
/// Returns true if the IP address is the the loopback address.
/// 2.5.3 The Loopback Address
/// The unicast address 0:0:0:0:0:0:0:1 is called the loopback address.
/// It may be used by a node to send an IPv6 packet to itself.  It may
/// never be assigned to any physical interface.  It may be thought of as
/// being associated with a virtual interface (e.g., the loopback
/// interface).
///
/// \retval
///      true if this is the loopback address.
/// \retval
///      false if this is NOT the loopback address.
///
bool BcmIpV6Address::IsLoopbackAddress() const
{
    return ( *this == kIPv6LoopbackIpV6Address );
}

/// IsIpV6AddressWithEmbeddedIPv4Address()
///
/// Returns true if Ipv6 address with embedded IPv4 address
/// 2.5.4 IPv6 Addresses with Embedded IPv4 Addresses
/// The IPv6 transition mechanisms [TRAN] include a technique for hosts
/// and routers to dynamically tunnel IPv6 packets over IPv4 routing
/// infrastructure.  IPv6 nodes that utilize this technique are assigned
/// special IPv6 unicast addresses that carry an IPv4 address in the low-
/// order 32-bits.  This type of address is termed an "IPv4-compatible
/// IPv6 address" and has the format:
///
/// |                80 bits               | 16 |      32 bits        |
/// +--------------------------------------+--------------------------+
/// |0000..............................0000|0000|    IPv4 address     |
/// +--------------------------------------+----+---------------------+
///
/// A second type of IPv6 address which holds an embedded IPv4 address is
/// also defined.  This address is used to represent the addresses of
/// IPv4-only nodes (those that *do not* support IPv6) as IPv6 addresses.
/// This type of address is termed an "IPv4-mapped IPv6 address" and has
/// the format:
///
/// |                80 bits               | 16 |      32 bits        |
/// +--------------------------------------+--------------------------+
/// |0000..............................0000|FFFF|    IPv4 address     |
/// +--------------------------------------+----+---------------------+
///
/// \retval
///      true if this IPv6 address has an embedded IPv4 address.
/// \retval
///      false if this IPv6 address does NOT has an embedded IPv4 address.
bool BcmIpV6Address::IsIpV6AddressWithEmbeddedIPv4Address() const
{
    if( IsEqualWithPrefix( kIPv4CompatibleIPv6AddressPrefix ) ||
        IsEqualWithPrefix( kIPv4MappedIPv6AddressPrefix ) )
    {
        return true;
    }

    return false;
}

/// GetEmbeddedIpV4Address() - returns the embedded IPv4 Address
///
/// For IPv6 addresses with embedded IPv4 addresses, returns
/// the embedded IPv4 address
///
/// \retval
///      true if this is an IPv6 address with an embedded IPv4 address.
/// \retval
///      false if this is NOT an IPv6 address with an embedded IPv4 address.
///
//bool BcmIpV6Address::GetEmbeddedIpV4Address( BcmIpAddress & ipv4Address ) const
//{
//    if( IsIpV6AddressWithEmbeddedIPv4Address() == false )
//    {
//        return false;
//    }
//
//    ipv4Address.Set( fIpV6Address[12], fIpV6Address[13], fIpV6Address[14], fIpV6Address[15] );
//    return true;
//}

/// IsLinkLocalAddress()
///
/// Link-Local and Site-Local.  The Link-Local is for use on a single
/// link and the Site-Local is for use in a single site.  Link-Local
/// addresses have the following format:
///
/// |   10     |
/// |  bits    |        54 bits          |          64 bits           |
/// +----------+-------------------------+----------------------------+
/// |1111111010|           0             |       interface ID         |
/// +----------+-------------------------+----------------------------+
///
/// Link-Local addresses are designed to be used for addressing on a
/// single link for purposes such as auto-address configuration, neighbor
/// discovery, or when no routers are present.
///
/// Routers must not forward any packets with link-local source or
/// destination addresses to other links.
bool BcmIpV6Address::IsLinkLocalAddress( void ) const
{
    if( IsEqualWithPrefix( kLinkLocalUnicastAddressPrefix ) )
        return true;

    return false;
}

/// Helper method to correct the representation of link-local IPv6 addresses
/// used internally by certain (ahem!) operating systems.  The spec for the 
/// construction of the link-local IP address requires that bits 10..63 
/// (zero based) must be all clear.  However, some OS's internally use this 
/// reserved space for flags, etc.  So for link-local addresses which have  
/// been retrieved from the OS directly (eg router table, etc) we may need 
/// to clear bits 10..63.  Which means all of octets [2..7] must be zeros 
/// and octet [1] must be & 0xE0.  To wit:
///
/// The link local prefix is fe80::/10.   
/// |   10     |
/// |  bits    |        54 bits          |          64 bits           |
/// +----------+-------------------------+----------------------------+
/// |1111111010|           0             |       interface ID         |
/// +----------+-------------------------+----------------------------+
///
/// Q: IPv6 ping from other OSes does not seem to work.
/// A: Are you using link-local address for that? (fe80::x) If
///    so, be sure to clear the 2nd 16-bit field to 0.  KAME kernels
///    use those bits internally, and some older versions of ifconfig show
///    the value, but the value MUST NOT appear on the wire.
/// 
/// If ifconfig command shows that your KAME host has the
/// following address:
/// 
/// fe80:1::x:y:z:u
/// 
/// the address the host actually has is
/// 
/// fe80::x:y:z:u
///
bool BcmIpV6Address::FixUpLinkLocalAddress( void )
{
    // Start by comparing against only the first 10 bits of the link-local 
    // unicast prefix.
    BcmIpV6Address Prefix = kLinkLocalUnicastAddressPrefix;
    Prefix.PrefixLengthBits(10);

    // Return 'false' and do nothing if the first 10 bits don't look like a 
    // link-local address.
    if (IsPrefixEqual(Prefix) == false)
      return false;

    // Likewise, return 'false' and do nothing if the address is already a
    // valid link-local address.
    if (IsLinkLocalAddress() == true)
      return false;

    // If we made it to here then we need to fix up this address.
    unsigned char pAddr[16];
    
    Get(pAddr);
    pAddr[1] &= 0xE0;
    pAddr[2] = pAddr[3] = pAddr[4] = pAddr[5] = pAddr[6] = pAddr[7] = 0;
    Set(pAddr);
    
    return true;
}

/// IsSiteLocalAddress()
/// 
/// Site-Local addresses are designed to be used for addressing inside of
/// a site without the need for a global prefix.
/// 
/// Routers must not forward any packets with site-local source or
/// destination addresses outside of the site.
///
/// Site-Local addresses have the following format:
///
/// |   10     |
/// |  bits    |   38 bits   |  16 bits  |         64 bits            |
/// +----------+-------------+-----------+----------------------------+
/// |1111111011|    0        | subnet ID |       interface ID         |
/// +----------+-------------+-----------+----------------------------+
///
bool BcmIpV6Address::IsSiteLocalAddress( void ) const
{
    if( IsEqualWithPrefix( kSiteLocalUnicastAddressPrefix ) )
        return true;

    return false;
}

/// IsAllNodesMulticastAddress()
///
/// Unicast addresses are distinguished from multicast addresses by the
/// value of the high-order octet of the addresses: a value of FF
/// (11111111) identifies an address as a multicast address; 
///
/// \retval
///      true if this is a multicast address.
/// \retval
///      false if this is NOT a multicast address
///
bool BcmIpV6Address::IsMulticastAddress() const
{
    if( IsEqualWithPrefix( kMulticastAddressPrefix ) )
        return true;

    return false;
}

/// IsAllNodesMulticastAddress()
///
/// Returns true if the IP address is the the link-local scope address to reach all nodes.
///   FF02::1
///
/// \retval
///      true if this is the all-nodes multicast address.
/// \retval
///      false if this is NOT the all-nodes multicast address
///
bool BcmIpV6Address::IsAllNodesMulticastAddress() const
{
    if( *this == kAllNodesMulticastAddress1 ||
        *this == kAllNodesMulticastAddress2 )
    {
        return true;
    }

    return false;
}

/// IsAllRoutersMulticastAddress()
///
/// Returns true if the IP address is the the link-local scope address to reach all nodes.
///   FF02::2
///
/// \retval
///      true if this is the all-router multicast address.
/// \retval
///      false if this is NOT the all-routers multicast address
///
bool BcmIpV6Address::IsAllRoutersMulticastAddress() const
{
    if( *this == kNodeLocalAllRoutersMulticastAddress ||
        *this == kLinkLocalAllRoutersMulticastAddress ||
        *this == kSiteLocalAllRoutersMulticastAddress )
    {
        return true;
    }

    return false;
}

/// GetSolicitedNodeMulticastAddress()
///
///    Solicited-Node Address:  FF02:0:0:0:0:1:FFXX:XXXX
///
/// The above multicast address is computed as a function of a node's
/// unicast and anycast addresses.  The solicited-node multicast address
/// is formed by taking the low-order 24 bits of the address (unicast or
/// anycast) and appending those bits to the prefix
/// FF02:0:0:0:0:1:FF00::/104
///
/// \retval
///      none
///
void BcmIpV6Address::GetSolicitedNodeMulticastAddress( BcmIpV6Address & solicitedNodeMulticastAddress ) const
{
    // copy the 104 bit (13 byte) prefix
    memcpy( solicitedNodeMulticastAddress.fIpV6Address, 
            kSolicitedNodeAddressPrefix.fIpV6Address, 
            104/8
          );
          
    // now append the low-order 24 bits of *this* address to the prefix
    solicitedNodeMulticastAddress.fIpV6Address[13] = fIpV6Address[13];
    solicitedNodeMulticastAddress.fIpV6Address[14] = fIpV6Address[14];
    solicitedNodeMulticastAddress.fIpV6Address[15] = fIpV6Address[15];
}

// lifted from: http://cvs.opensolaris.org/source/xref/on/usr/src/lib/libnsl/nss/inet_pton.c
#define IN6ADDRSZ 16
#define INADDRSZ 4
#define INT16SZ 2
static int inet_pton6(const char *src, uint8 *dst)
{
    static const char xdigits_l[] = "0123456789abcdef",
	xdigits_u[] = "0123456789ABCDEF";
 	uint8 tmp[IN6ADDRSZ], *tp, *endp, *colonp;
 	const char *xdigits, *curtok;
 	int ch, saw_xdigit;
 	unsigned int val;
 
 	(void) memset((tp = tmp), '\0', IN6ADDRSZ);
 	endp = tp + IN6ADDRSZ;
 	colonp = NULL;
 	/* Leading :: requires some special handling. */
 	if (*src == ':')
 		if (*++src != ':')
 			return (0);

 	curtok = src;
 	saw_xdigit = 0;
    val = 0;
    while ((ch = *src++) != '\0') {
        const char *pch;
        if ((pch = strchr((xdigits = xdigits_l), ch)) == NULL)
    	    pch = strchr((xdigits = xdigits_u), ch);
        if (pch != NULL) {
            val <<= 4;
			val |= (pch - xdigits);
    		if (val > 0xffff)
    			return (0);
    		saw_xdigit = 1;
    		continue;
    	}
    	if (ch == ':') {
    		curtok = src;
    		if (!saw_xdigit) {
    			if (colonp)
    				return (0);
    			colonp = tp;
    			continue;
    		} else if (*src == '\0') {
    			return (0);
    		}
    		if (tp + INT16SZ > endp)
    			return (0);
    		*tp++ = (uint8)(val >> 8) & 0xff;
    		*tp++ = (uint8)val & 0xff;
    		saw_xdigit = 0;
    		val = 0;
    		continue;
    	}
    	if (ch == '.' && ((tp + INADDRSZ) <= endp) &&
    	    inet_pton4(curtok, tp) > 0) {
    		tp += INADDRSZ;
    		saw_xdigit = 0;
    		break;	/* '\0' was seen by inet_pton4(). */
    	}
    	return (0);
    }
    if (saw_xdigit) {
    	if (tp + INT16SZ > endp)
    		return (0);
    	*tp++ = (uint8)(val >> 8) & 0xff;
    	*tp++ = (uint8)val & 0xff;
    }
    if (colonp != NULL) {
    	/*
    	 * Since some memmove()'s erroneously fail to handle
    	 * overlapping regions, we'll do the shift by hand.
    	 */
    	const int n = tp - colonp;
    	int i;
    	if (tp == endp)
        	return (0);
    	for (i = 1; i <= n; i++) {
    		endp[- i] = colonp[n - i];
    		colonp[n - i] = 0;
    	}
    	tp = endp;
    }
    if (tp != endp)
    	return (0);

    (void) memcpy(dst, tmp, IN6ADDRSZ);

    return (1);
}


static int
inet_pton4(const char *src, uint8 *dst)
{
	static const char digits[] = "0123456789";
	int saw_digit, octets, ch;
	uint8 tmp[INADDRSZ], *tp;

	saw_digit = 0;
	octets = 0;
	*(tp = tmp) = 0;
	while ((ch = *src++) != '\0') {
		const char *pch;

		if ((pch = strchr(digits, ch)) != NULL) {
			uint32 newx = *tp * 10 + (pch - digits);

 			if (newx > 255)
 				return (0);
			*tp = newx;
			if (!saw_digit) {
				if (++octets > 4)
					return (0);
			saw_digit = 1;
    	}
    } else if (ch == '.' && saw_digit) {
			if (octets == 4)
				return (0);
			*++tp = 0;
			saw_digit = 0;
	} else
		return (0);
 	}
 	if (octets < 4)
		return (0);

	(void) memcpy(dst, tmp, INADDRSZ);
	return (1);
}

static const char *
inet_ntop4(const uint8 *src, char *dst, int size)
{
	static const char fmt[] = "%u.%u.%u.%u";
	char tmp[sizeof ("255.255.255.255")];

	if (sprintf(tmp, fmt, src[0], src[1], src[2], src[3]) > size) {
		return (NULL);
	}
	(void) strcpy(dst, tmp);
	return (dst);
}


static const char *
inet_ntop6(const uint8 *src, char *dst, int size)
{
 	/*
 	 * Note that int32_t and int16_t need only be "at least" large enough
 	 * to contain a value of the specified size.  On some systems, like
 	 * Crays, there is no such thing as an integer variable with 16 bits.
 	 * Keep this in mind if you think this function should have been coded
 	 * to use pointer overlays.  All the world's not a VAX.
 	 */
 	char tmp[sizeof ("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")], *tp;
 	struct { int base, len; } best, cur;
	unsigned int words[IN6ADDRSZ / INT16SZ];
 	int i;
 
 	/*
 	 * Preprocess:
 	 *	Copy the input (bytewise) array into a wordwise array.
 	 *	Find the longest run of 0x00's in src[] for :: shorthanding.
 	 */
 	(void) memset(words, '\0', sizeof (words));
 	for (i = 0; i < IN6ADDRSZ; i++)
 		words[i / 2] |= (src[i] << ((1 - (i % 2)) << 3));
	best.base = -1;
	best.len = 0;
 	cur.base = -1;
	cur.len = 0;
 	for (i = 0; i < (IN6ADDRSZ / INT16SZ); i++) {
 		if (words[i] == 0) {
 			if (cur.base == -1)
 				cur.base = i, cur.len = 1;
			else
 				cur.len++;
 		} else {
 			if (cur.base != -1) {
 				if (best.base == -1 || cur.len > best.len)
 					best = cur;
 				cur.base = -1;
 			}
 		}
 	}
 	if (cur.base != -1) {
 		if (best.base == -1 || cur.len > best.len)
 			best = cur;
 	}
	if (best.base != -1 && best.len < 2)
 		best.base = -1;
 
 	/*
 	 * Format the result.
	 */
 	tp = tmp;
 	for (i = 0; i < (IN6ADDRSZ / INT16SZ); i++) {
		/* Are we inside the best run of 0x00's? */
 		if (best.base != -1 && i >= best.base &&
 		    i < (best.base + best.len)) {
 			if (i == best.base)
 				*tp++ = ':';
 			continue;
 		}
 		/* Are we following an initial run of 0x00s or any real hex? */
 		if (i != 0)
 			*tp++ = ':';
 		/* Is this address an encapsulated IPv4? */
 		if (i == 6 && best.base == 0 &&
 		    (best.len == 6 || (best.len == 5 && words[5] == 0xffff))) {
 			if (!inet_ntop4(src+12, tp, sizeof (tmp) - (tp - tmp)))
 				return (NULL);
 			tp += strlen(tp);
 			break;
 		}
 		tp += sprintf(tp, "%x", words[i]);
 	}
 	/* Was it a trailing run of 0x00's? */
 	if (best.base != -1 && (best.base + best.len) == (IN6ADDRSZ / INT16SZ))
 		*tp++ = ':';
 	*tp++ = '\0';
 
 	/*
 	 * Check for overflow, copy, and we're done.
 	 */
    if ((int)(tp - tmp) > size) {
		return (NULL);
	}
	(void) strcpy(dst, tmp);
	return (dst);
}


#if (REGRESSION_TEST)

//#include "whatever.h"

#endif

bool BcmIpV6Address::Regress(ostream &outStream)
{
    #if (!REGRESSION_TEST)
    {
        // Regression tests compiled out, return false.
        return false;
    }
    #else
    {
        bool result = true;
        unsigned int testNumber = 0;

        outStream << "BcmIpV6Address::Regress() - Beginning regression tests...\n" << endl;

        // ----------------------- THE DEVIL IS IN THE DETAILS ---------------
        //
        // Detailed regression tests to cover internal state and basic low-level
        // operation of the class.
        //
        // The debug output strings serve as comments for the code.

        // UNFINISHED - this needs to be fleshed out.

        // ----------------------- TYPICAL USAGE SCENARIOS -------------------
        //
        // This section shows the typical uses of the class, and serves as
        // good example code for others to copy and use.
        //
        // If you use this class in a way that is not represented here, please
        // add your own test(s).  This will ensure that your use of this class
        // is supported and works.

        // ----------------------- THE EXTERMINATOR --------------------------
        //
        // Regression tests for bugs found and fixed after the initial code
        // release.  There will be debug output containing the PR number for
        // the bug (if one was filed), the nature of the bug, etc.

        testNumber++;
        outStream << "Test " << testNumber << ":  PR21939 - IsEqualWithPrefix is implemented incorrectly for DOCSIS classification..." << endl;

        {
            BcmIpV6Address pktIpAddress, classifierIpAddress;

            {
                outStream << "\tOld ATP values, should not match..." << endl;

                pktIpAddress.Set("2001::200:00ff:fe00:001f");
                pktIpAddress.PrefixLengthBits(64);

                classifierIpAddress.Set("2001::200:00ff:fe00:001a");
                classifierIpAddress.PrefixLengthBits(124);

                if (pktIpAddress.IsEqualWithPrefix(classifierIpAddress) == true)
                {
                    outStream << "\tERROR - failed to detect mismatch!" << endl;
                    result = false;
                }
            }

            {
                outStream << "\tUpdated ATP values, should match..." << endl;

                classifierIpAddress.Set("2001::200:00ff:fe00:0010");
                classifierIpAddress.PrefixLengthBits(124);

                if (pktIpAddress.IsEqualWithPrefix(classifierIpAddress) == false)
                {
                    outStream << "\tERROR - failed to detect match!" << endl;
                    result = false;
                }
            }

            {
                unsigned int prefixLengthBits;
                uint8 pktIpAddressValue[16] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

                // Set this one byte longer so that we can handle prefix length
                // 128.
                uint8 classifierIpAddressValue[17] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00};

                outStream << "\tExhaustive comparison of all prefix lengths..." << endl;

                pktIpAddress.Set(pktIpAddressValue);

                for (prefixLengthBits = 128; prefixLengthBits > 0; prefixLengthBits--)
                {
                    if (prefixLengthBits < 128)
                    {
                        classifierIpAddress.Set(classifierIpAddressValue);
                        classifierIpAddress.PrefixLengthBits(prefixLengthBits);

                        if (pktIpAddress.IsEqualWithPrefix(classifierIpAddress) == true)
                        {
                            outStream << "\tERROR - failed to detect mis-match with prefix length " << prefixLengthBits << " bits!" << endl;
                            result = false;
                        }
                    }

                    // Clear a bit (note that the first time around (prefix
                    // length 128), this has no effect.
                    classifierIpAddressValue[prefixLengthBits / 8] <<= 1;

                    classifierIpAddress.Set(classifierIpAddressValue);
                    classifierIpAddress.PrefixLengthBits(prefixLengthBits);

                    if (pktIpAddress.IsEqualWithPrefix(classifierIpAddress) == false)
                    {
                        outStream << "\tERROR - failed to detect match with prefix length " << prefixLengthBits << " bits!" << endl;
                        result = false;
                    }
                }
            }
        }

        // ----------------------- END OF REGRESSION TESTS -------------------

        outStream << "\nBcmIpV6Address::Regress() - Finished regression tests...";
        if (result)
        {
            outStream << "all tests passed.";
        }
        else
        {
            outStream << "one or more tests FAILED!";
        }
        outStream << endl << endl;

        return result;
    }
    #endif
}


