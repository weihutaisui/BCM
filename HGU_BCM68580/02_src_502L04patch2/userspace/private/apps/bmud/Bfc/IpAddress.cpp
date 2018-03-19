//****************************************************************************
//
// Copyright (c) 1999-2009 Broadcom Corporation
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

#include "MessageLog.h"

// enables printing of call stack (without a crash) when someone uses a 
// class method on an IPv6 address that only makes sense on an IPv4 address.
#define DEBUG_IP_ADDRESS_METHOD_USAGE 1

// Not available for Win32...
#if (defined(WIN32) || defined(TARGETOS_Linux) )
    #undef DEBUG_IP_ADDRESS_METHOD_USAGE
    #define DEBUG_IP_ADDRESS_METHOD_USAGE 0
#endif

#if (DEBUG_IP_ADDRESS_METHOD_USAGE)

extern "C" void MipsStackTrace( cyg_uint32 pc, cyg_uint32 sp, cyg_uint32 ra );

#endif

//********************** Local Types *****************************************

//********************** Local Constants *************************************

//********************** Local Variables *************************************

//********************** Forward Declaration *********************************

//********************** Global instances ************************************

const BcmIpAddress kAllZerosIp(0, 0, 0, 0);
const BcmIpAddress kBroadcastIp(0xff, 0xff, 0xff, 0xff);
const BcmIpAddress kIgmpAllHostsIp(0xe0, 0x00, 0x00, 0x01);
const BcmIpAddress kSoftwareLoopbackIp(127, 0, 0, 1);

//********************** Class Method Implementations ************************

/// Default constructor.
///
BcmIpAddress::BcmIpAddress()
{
    reset();
}

BcmIpAddress::BcmIpAddress(const char *pString)
{
    Set( pString );
}

/// Copy constructor.
///
/// \param
///     rhs - the instance to be copied.
///
BcmIpAddress::BcmIpAddress( const BcmIpAddress& rhs )
{
    fIsIpV6Address = rhs.fIsIpV6Address;
    fIpV4Address = rhs.fIpV4Address;
    fIpV6Address = rhs.fIpV6Address;
}

BcmIpAddress::~BcmIpAddress() 
{
}

/// Initializing constructor.  Uses the values specified to initialize
/// the object.
///
/// \param
///     ipV4Address - an IPv4 address
///
BcmIpAddress::BcmIpAddress( const BcmIpV4Address & ipV4Address )
{
    reset();
    fIsIpV6Address = false;
    fIpV4Address = ipV4Address;
}

/// Initializing constructor.  Uses the values specified to initialize
/// the object.
///
/// \param
///     ipV4Address - an IPv4 address
///
BcmIpAddress::BcmIpAddress( const BcmIpV6Address & ipV6Address )
{
    reset();
    fIsIpV6Address = true;
    fIpV6Address = ipV6Address;
}

/// Initializing constructor.  Uses the values specified to initialize
/// the object.
///
/// \param
///     ipV6Address - an IPv6 address
///
BcmIpAddress::BcmIpAddress( const byte* buf, bool isIpV6Address )
{
    reset();
    fIsIpV6Address = isIpV6Address;
    if( isIpV6Address )
    {
        fIpV6Address.Set( buf );
    }
    else
    {
        fIpV4Address.Set( buf );
    }
}

/// Initializing constructor.  Uses the values specified to initialize
/// the object.
///
/// \param
///     b0 - the first byte of the IPv4 address.
/// \param
///     b1 - the second byte of the IPv4 address.
/// \param
///     b2 - the third byte of the IPv4 address.
/// \param
///     b3 - the fourth byte of the IPv4 address.
///
BcmIpAddress::BcmIpAddress( byte b0, byte b1, byte b2, byte b3 )
{
    reset();
    fIsIpV6Address = false;
    fIpV4Address.Set( b0, b1, b2, b3 );
}

/// Initializing constructor.  Takes a sockaddr structure which knows if
/// it's an IPv4 or IPv6 address.
///
/// \param
///     saddr - pointer to a sockaddr structure
///
BcmIpAddress::BcmIpAddress( const struct sockaddr* pSockAddr )
{
  Set (pSockAddr);
}

/// Initializing constructor.  Uses the values specified to initialize
/// the object.  PR 12595
///
/// \param
///     String - a representation of an IP address; either in ASCII
///              notation or raw octets
///
BcmIpAddress::BcmIpAddress(const BcmString &String, bool Ascii)
{
  Set (String, Ascii);
}

/// Assignment operator.
///
/// \param
///     rhs - the instance to be copied.
///
/// \return
///     A reference to this object.
///
BcmIpAddress& BcmIpAddress::operator=( const BcmIpAddress& rhs )
{
    if( this == &rhs )
    {
        // special case: assignment to self
        return *this;
    }

    fIsIpV6Address = rhs.fIsIpV6Address;
    fIpV4Address = rhs.fIpV4Address;
    fIpV6Address = rhs.fIpV6Address;
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
bool BcmIpAddress::operator<( const BcmIpAddress& rhs ) const
{
    bool ret_val = false;

    if( fIsIpV6Address != rhs.fIsIpV6Address)
    {
        #if (DEBUG_IP_ADDRESS_METHOD_USAGE)
        {
            gLogMessageRaw << "WARNING!  You called operator< on mismatched IPv4/IPv6 addresses!  Call stack:" << endl;

            uint32 ra, pc, sp;
            asm( "move %0, $31;"
                 "la   %1, 1f;"
                 "move %2, $29;"
                 "1:" :
                 "=r" (ra),
                 "=r" (pc), 
                 "=r" (sp) );
            MipsStackTrace( pc, sp, ra );
        }
        #endif
        
        return false;
    }

    // I am going to allow this operator to compare IPv6 and IPv6
    // addresses in case both types are in the same sorted STL list. 

    if( fIsIpV6Address )
    {
        if( fIpV6Address < rhs.fIpV6Address )
        {
            ret_val = true;
        }
    }
    else if( fIpV4Address < rhs.fIpV4Address )
    {
        ret_val = true;
    }

    return ret_val;
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
bool BcmIpAddress::operator<=( const BcmIpAddress& rhs ) const
{
    bool ret_val = false;

    if( fIsIpV6Address != rhs.fIsIpV6Address)
    {
        #if (DEBUG_IP_ADDRESS_METHOD_USAGE)
        {
            gLogMessageRaw << "WARNING!  You called operator<= on mismatched IPv4/IPv6 addresses!  Call stack:" << endl;

            uint32 ra, pc, sp;
            asm( "move %0, $31;"
                 "la   %1, 1f;"
                 "move %2, $29;"
                 "1:" :
                 "=r" (ra),
                 "=r" (pc), 
                 "=r" (sp) );
            MipsStackTrace( pc, sp, ra );
        }
        #endif
        
        return false;
    }

    // I am going to allow this operator to compare IPv6 and IPv6
    // addresses in case both types are in the same sorted STL list. 

    if( fIsIpV6Address )
    {
        if( fIpV6Address <= rhs.fIpV6Address )
        {
            ret_val = true;
        }
    }
    else if( fIpV4Address <= rhs.fIpV4Address )
    {
        ret_val = true;
    }
    return ret_val;
}

/// Assignment operator with mask.  Masks this object with another one using
/// the & operator.  This is useful for things like applying subnet masks.
///
/// \param
///     rhs - the instance to be used as a mask value.
///
/// \return
///     A reference to this object.
///
BcmIpAddress & BcmIpAddress::operator&= (const BcmIpAddress &rhs)
{
    // make sure we are dealing with an IPv4 addresses. 
    // if not this is a programming error and we will aseert. 
    if (fIsIpV6Address || rhs.fIsIpV6Address)
    {
        #if (DEBUG_IP_ADDRESS_METHOD_USAGE)
        {
            gLogMessageRaw << "WARNING!  You called operator&= on IPv6 address " << *this << "   Call stack:" << endl;

            uint32 ra, pc, sp;
            asm( "move %0, $31;"
                 "la   %1, 1f;"
                 "move %2, $29;"
                 "1:" :
                 "=r" (ra),
                 "=r" (pc), 
                 "=r" (sp) );
            MipsStackTrace( pc, sp, ra );
        }
        #endif
        
        return *this;
    }
    
    // Apply the mask 
    fIpV4Address &= rhs.fIpV4Address;

    return *this;
}

/// Assignment operator with mask.  Masks this object with another one using
/// the | operator.  This is useful for things like applying subnet masks.
///
/// \param
///     rhs - the instance to be used as a mask value.
///
/// \return
///     A reference to this object.
///
BcmIpAddress & BcmIpAddress::operator|= (const BcmIpAddress &rhs)
{
    // make sure we are dealing with an IPv4 addresses. 
    // if not this is a programming error and we will aseert. 
    if (fIsIpV6Address || rhs.fIsIpV6Address)
    {
        #if (DEBUG_IP_ADDRESS_METHOD_USAGE)
        {
            gLogMessageRaw << "WARNING!  You called operator|= on IPv6 address " << *this << "   Call stack:" << endl;

            uint32 ra, pc, sp;
            asm( "move %0, $31;"
                 "la   %1, 1f;"
                 "move %2, $29;"
                 "1:" :
                 "=r" (ra),
                 "=r" (pc), 
                 "=r" (sp) );
            MipsStackTrace( pc, sp, ra );
        }
        #endif
        
        return *this;
    }

    fIpV4Address |= rhs.fIpV4Address;
    return *this;
}

/// Accessor for the byte value at the specified index.  The index must be
/// in the range 0..3 or 0..15.  Anything outside of this range will cause a 0 to be
/// returned.
///
/// \param
///     Index - the index of the value to be returned.  Must be 0..3 or 0..15.
///
/// \return
///     The value at the specified index.
///
unsigned char BcmIpAddress::operator[] (int Index) const
{
    if (fIsIpV6Address)
    {
        return fIpV6Address[Index];
    }
    else
    {
        return fIpV4Address[Index];
    }
}

BcmIpAddress &BcmIpAddress::operator += (const BcmIpAddress &value)
{
    if( value.IsIpV6Address() || fIsIpV6Address )
    {
        gLogMessageRaw << "WARNING:  You called += BcmIpAddress operator for IPv6 address" << endl;
    }
    else 
    {
        fIpV4Address += value.GetIpV4Address();
    }
    return *this;
}

BcmIpAddress &BcmIpAddress::operator += (uint32 value)
{
    if( fIsIpV6Address )
    {
        gLogMessageRaw << "WARNING:  You called += uint32 operator for IPv6 address" << endl;
    }
    else 
    {
        fIpV4Address += value;
    }

    return *this;
}

BcmIpAddress &BcmIpAddress::operator ++ ()
{
    if( fIsIpV6Address )
    {
        gLogMessageRaw << "WARNING:  You called ++ operator for IPv6 address" << endl;
    }
    else
    {
        fIpV4Address ++;
    }

    return *this;
}

BcmIpAddress &BcmIpAddress::operator ++ (int)
{
    if( fIsIpV6Address )
    {
        gLogMessageRaw << "WARNING:  You called ++ int operator for IPv6 address" << endl;
    }
    else
    {
        fIpV4Address++;
    }

    return *this;
}

BcmIpAddress &BcmIpAddress::operator -= (const BcmIpAddress &value)
{
    if( value.IsIpV6Address() || fIsIpV6Address )
    {
        gLogMessageRaw << "WARNING:  You called -+ BcmIpAddress operator for IPv6 address" << endl;
    }
    else
    {
        fIpV4Address -= value.GetIpV4Address();
    }

    return *this;
}

BcmIpAddress &BcmIpAddress::operator -= (uint32 value)
{
    if( fIsIpV6Address )
    {
        gLogMessageRaw << "WARNING:  You called -+ uint32 operator for IPv6 address" << endl;
    }
    else
    {
        fIpV4Address -= value;
    }

    return *this;
}

BcmIpAddress &BcmIpAddress::operator -- ()
{
    if( fIsIpV6Address )
    {
        gLogMessageRaw << "WARNING:  You called -- operator for IPv6 address" << endl;
    }
    else
    {
        fIpV4Address --;
    }

    return *this;
}

BcmIpAddress &BcmIpAddress::operator -- (int)
{
    if( fIsIpV6Address )
    {
        gLogMessageRaw << "WARNING:  You called -- int operator for IPv6 address" << endl;
    }
    else
    {
        fIpV4Address --;
    }

    return *this;
}

/// This is similar to operator ==, except that it allows the user to apply
/// a mask before performing the comparison.  The mask is temporarily applied
/// to this instance.
///
/// \note  The mask is NOT applied to the otherInstance object before the
///        comparison!  In general, the other instance will tend to be a
///        pre-computed value that is used over and over, so you will want
///        to apply the mask to it once.
///
/// \param
///      otherInstance - the other instance to compare against.
/// \param
///      mask - the mask to be applied before comparison.
///
/// \retval
///      1 if the comparison (after applying the mask) succeeded.
/// \retval
///      0 if the comparison failed.
///
int BcmIpAddress::IsEqualWithMask(const BcmIpAddress &otherInstance,
                                  const BcmIpAddress &mask) const
{
    // make sure we are dealing exclusively with IPv4 addresses. 
    // if not this is a programming error and we will aseert. 
    if (fIsIpV6Address || otherInstance.fIsIpV6Address || mask.fIsIpV6Address )
    {
        #if (DEBUG_IP_ADDRESS_METHOD_USAGE)
        {
            gLogMessageRaw << "WARNING!  You called IsEqualWithMask on IPv6 address " << *this << "   Call stack:" << endl;

            uint32 ra, pc, sp;
            asm( "move %0, $31;"
                 "la   %1, 1f;"
                 "move %2, $29;"
                 "1:" :
                 "=r" (ra),
                 "=r" (pc), 
                 "=r" (sp) );
            MipsStackTrace( pc, sp, ra );
        }
        #endif
        
        return 0;
    }
    return fIpV4Address.IsEqualWithMask( otherInstance.fIpV4Address, mask.fIpV4Address );
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
bool BcmIpAddress::WriteTo(BcmOctetBuffer &octetBuffer, bool encodeLength) const
{
    // PR 12595:  if we are told to do so, we will write the number of octets
    // to the buffer so that whoever reads the buffer can deduce whether this 
    // is an IPv4 (4 octets) or IPv6 (16 octets) address.
    
    if( fIsIpV6Address )
    {
        if ( encodeLength )
        {
            unsigned char Length = 16;
            octetBuffer.NetworkAddToEnd(Length);
        }
          
        return fIpV6Address.WriteTo( octetBuffer );
    }
    else
    {
        if ( encodeLength )
        {
            unsigned char Length = 4;
            octetBuffer.NetworkAddToEnd(Length);
        }
    
        return fIpV4Address.WriteTo( octetBuffer );
    }
}

/// This methods causes the object to read its 4 bytes from the specified
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
bool BcmIpAddress::ReadFrom(const BcmOctetBuffer &octetBuffer, unsigned int &offset, bool decodeLength)
{
    // PR 12595:  if we are told to do so, we will read the number of octets
    // from the buffer and deduce from that whether this is an IPv4 (4 octets)
    // or IPv6 (16 octets) address.
    if ( decodeLength )
    {
        unsigned char Length;
        
        octetBuffer.NetworkRead (offset, Length);
        
        if (Length == 16)
            fIsIpV6Address = true;
        else if (Length == 4)
            fIsIpV6Address = false;
    }

    if( fIsIpV6Address )
    {
        return fIpV6Address.ReadFrom( octetBuffer, offset );
    }
    else
    {
        return fIpV4Address.ReadFrom( octetBuffer, offset );
    }
}

ostream& BcmIpAddress::Print( ostream& ostrm ) const
{
    if( fIsIpV6Address )
    {
        return fIpV6Address.Print( ostrm );
    }
    else
    {
        return fIpV4Address.Print( ostrm );
    }   
}


// Allows the 4 bytes of the IP address to be set.
//
// Parameters:
//      ipAddress1-4 - the 4 bytes of the address to be used.
//
// Returns:  Nothing.
//
void BcmIpAddress::Set(uint8 ipAddress1, uint8 ipAddress2, uint8 ipAddress3,
                       uint8 ipAddress4)
{
    reset();
    fIsIpV6Address = false;
    fIpV4Address.Set( ipAddress1, ipAddress2, ipAddress3, ipAddress4 );
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
bool BcmIpAddress::Set(const char *pString, unsigned int *pBytesRead)
{
    reset();

    // first try to parse it as an IPv4 address.  If that fails, 
    // try parsing it as an IPV6 address.  If both methods fail, 
    // return false. 
    if( fIpV4Address.Set( pString, pBytesRead ) == false )
    {
        if( fIpV6Address.Set( pString, pBytesRead ) == false )
        {
            return false;
        }
        else
        {
            fIsIpV6Address = true;
        }
    }  
    return true;
}

/// Allows the IP address to be set from a BcmString object, either by
/// parsing from ASCII or by using the raw octets.  In raw mode, the
/// decision to convert to IPv4 or IPv6 will be made based on the length
/// of the supplied string.  PR 12595
///
/// In ASCII mode, the same formatting rules apply as in the method
///
/// bool Set(const char *pString, unsigned int *pBytesRead = NULL);
///
/// \param
///      String - the string containing the IP address.
/// \param
///      Ascii - indicates ASCII parse or raw octet mode.
/// \param
///      pBytesRead - output variable (optional) into which the number of
///                   characters read will be placed.
///
/// \retval
///      true if successful.
/// \retval
///      false if there was a problem (NULL string, invalid format, etc.).
///
bool BcmIpAddress::Set(const BcmString &String, bool Ascii, unsigned int *pBytesRead)
{
    if (Ascii)
        return Set (String.c_str(), pBytesRead);
        
    if (String.length() == 0)
        *this = kIPv6UnspecifiedAddress;
    else
        Set ((byte *)String.data(), (String.length() == 16));
        
    return true;
}

// Allows all 4 IP address bytes to be retrieved.
//
// Parameters:
//      ipAddress1-4 - the 4 bytes of the address to be retrieved.
//
// Returns:  Nothing.
//
void BcmIpAddress::Get(uint8 &ipAddress1, uint8 &ipAddress2, uint8 &ipAddress3,
                       uint8 &ipAddress4) const
{
    // only valid for IPv4 address types.  programming error otherwise. 
    if (fIsIpV6Address)
    {
        #if (DEBUG_IP_ADDRESS_METHOD_USAGE)
        {
            gLogMessageRaw << "WARNING!  You called Get(uint8&,uint8&,uint8&,uint8&) on IPv6 address " << *this << "   Call stack:" << endl;

            uint32 ra, pc, sp;
            asm( "move %0, $31;"
                 "la   %1, 1f;"
                 "move %2, $29;"
                 "1:" :
                 "=r" (ra),
                 "=r" (pc), 
                 "=r" (sp) );
            MipsStackTrace( pc, sp, ra );
        }
        #endif
        
        return;
    }
    fIpV4Address.Get( ipAddress1, ipAddress2, ipAddress3, ipAddress4 );
}


// Converts the IP address bytes to an ASCII string in dotted-decimal
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
bool BcmIpAddress::Get(char *pString, unsigned int bufferSize) const
{
    char tempBuffer[100];
    bool result = false;
    if( fIsIpV6Address )
    {
        result = fIpV6Address.Get( tempBuffer, sizeof( tempBuffer) );
    }
    else
    {
        result = fIpV4Address.Get( tempBuffer, sizeof( tempBuffer) );
    }  

    if( result == true )
    {
        if( strlen( tempBuffer ) <= bufferSize )
        {
            strcpy( pString, tempBuffer );
        }
        else
        {
            #if (DEBUG_IP_ADDRESS_METHOD_USAGE)
            {
                gLogMessageRaw << "ERROR: BcmIpAddress::  Get buffer size too small to hold IPv6 address " << *this << endl;
                uint32 ra, pc, sp;
                asm( "move %0, $31;"
                     "la   %1, 1f;"
                     "move %2, $29;"
                     "1:" :
                     "=r" (ra),
                     "=r" (pc), 
                     "=r" (sp) );
                MipsStackTrace( pc, sp, ra );
            }
            #endif

            pString[0] = '\0';
            result = false;
        }
    }

    return result;
}

void BcmIpAddress::Set(uint32 ipAddress)
{
    fIsIpV6Address = false;
    fIpV4Address.Set( ipAddress );
}

void BcmIpAddress::Set(const struct sockaddr *pSockAddr)
{
    reset();

    #if !defined(WIN32)
    {
        if( pSockAddr->sa_family == AF_INET6 )
        {
            // Cast our sockaddr as an IPv6 sockaddr
            const struct sockaddr_in6 *pSockAddr6 = (const struct sockaddr_in6 *)pSockAddr;

            fIsIpV6Address = true;
            fIpV6Address.Set( pSockAddr6->sin6_addr.s6_addr );
        }
        else
        {
            const struct sockaddr_in *pSockAddr4 = (const struct sockaddr_in *)pSockAddr;

            fIsIpV6Address = false;
            fIpV4Address.NetworkSet( pSockAddr4->sin_addr.s_addr );
        }
    }
    #endif
}

/// Allows the IP address to be set as an InetAddress structure.
/// By default, the type and length of the InetAddress must be consistent.
/// Optionally, a mismatch may be allowed in which case the method will
/// adjust the string to match the type.
///
/// \param
///      Address - the InetAddress from with the IP address should be stored.
/// \param
///      EnforceTypeLengthConsistency - controls whether type/length 
///      mismatch will be allowed.
/// \param
///      TypeTakesPriority - if EnforceLegthConsistency is false, this is
///      ignored.  Otherwise, this param determines whether the object
///      will be assigned according to the InetAddress Type (thus possibly
///      adjusting length to match) or the length of the InetAddress
///      Address field (this possibly adjusting the type to match).
///
/// \retval
///      true if successful.
/// \retval
///      false if there was a problem (enforced mismatch, etc.).
///  
bool BcmIpAddress::Set (const InetAddress &ipAddress, bool EnforceTypeLengthConsistency, bool TypeTakesPriority)
{
    // Enforce consistency if told to do so.  Any mismatch will result in an
    // error being returned and no change being made to the value.
    if (EnforceTypeLengthConsistency == true)
    {
        if ((ipAddress.Type == kInetAddressUnknown)
        &&  (ipAddress.Address.length() != kInetAddressLengthUnknown))
            return false;
            
        if ((ipAddress.Type == kInetAddressIPv4)
        &&  (ipAddress.Address.length() != kInetAddressLengthIPv4))
            return false;
            
        if ((ipAddress.Type == kInetAddressIPv6)
        &&  (ipAddress.Address.length() != kInetAddressLengthIPv6))
            return false;
    }
    
    //gLogMessageRaw << "InetAddress type=" << ipAddress.Type << ", Addr=" << ipAddress.Address << endl;
    
    // Assign string to IP address.  Since we might have mismatches allowed,
    // we'll potentially truncate or pad the string if necessary.  If the
    // type & string are consistent then this becomes a no-op.
    
    unsigned char pOctets[] = {0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0};
    int Length = ipAddress.Address.length();
    InetAddressType Type = ipAddress.Type;

    if (EnforceTypeLengthConsistency == false)
    {
      /* Truncate or pad length to match type */
      
      if (TypeTakesPriority)
      {    
        if (ipAddress.Type == kInetAddressUnknown)
        {
          // Unknown has length 0 so no need to check addr length.
          Length = kInetAddressLengthUnknown;
        }
            
        else if (ipAddress.Type == kInetAddressIPv4)
        {
          if (ipAddress.Address.length() < kInetAddressLengthIPv4)
            Length = ipAddress.Address.length();
          else
            Length = kInetAddressLengthIPv4;
        }
            
        else if (ipAddress.Type == kInetAddressIPv6)
        {
          if (ipAddress.Address.length() < kInetAddressLengthIPv6)
            Length = ipAddress.Address.length();
          else
            Length = kInetAddressLengthIPv6;
        }
      }
      
      /* Assign type based on length */
      
      else
      {
        if (ipAddress.Address.length() == kInetAddressLengthIPv4)
        {
          Type = kInetAddressIPv4;
        }
            
        else if (ipAddress.Address.length() == kInetAddressLengthIPv6)
        {
          Type = kInetAddressIPv6;
        }
        
        // Anything other than 4 or 16 bytes gets called 'unknown'
        else
        {
          Length = kInetAddressLengthUnknown;
          Type = kInetAddressUnknown;
        }
      }
    }
    
    memcpy (pOctets, ipAddress.Address.data(), Length);
    
    //gLogMessageRaw << "Copying " << Length << " octets:" << BcmString(pOctets, Length) << endl;
    
    Set (pOctets, Type == kInetAddressIPv6);
    //gLogMessageRaw << "IP = " << *this << endl;
    
    return true;
}

void BcmIpAddress::Get (InetAddress &ipAddress)
{
  if ((*this == kIPv6UnspecifiedAddress)
  ||  (*this == kAllZerosIp))
    ipAddress.Type = kInetAddressUnknown;
  else if (IsIpV6Address() == true)
    ipAddress.Type = kInetAddressIPv6;
  else
    ipAddress.Type = kInetAddressIPv4;

  if ((*this != kIPv6UnspecifiedAddress)
  &&  (*this != kAllZerosIp))
    ipAddress.Address = *this;
  else
    ipAddress.Address = "";
}

void BcmIpAddress::NetworkSet(uint32 ipAddress)
{
    reset();

    fIsIpV6Address = false;
    fIpV4Address.NetworkSet(ipAddress);
}

void BcmIpAddress::Get(uint32 &ipAddress) const
{
    // method only valid for 4 byte IPv4 address
    if (fIsIpV6Address)
    {
        #if (DEBUG_IP_ADDRESS_METHOD_USAGE)
        {
            gLogMessageRaw << "WARNING!  You called Get(uint32&) on IPv6 address " << *this << "   Call stack:" << endl;

            uint32 ra, pc, sp;
            asm( "move %0, $31;"
                 "la   %1, 1f;"
                 "move %2, $29;"
                 "1:" :
                 "=r" (ra),
                 "=r" (pc), 
                 "=r" (sp) );
            MipsStackTrace( pc, sp, ra );
        }
        #endif
        
        return;
    }

    fIpV4Address.Get(ipAddress);
}

/// Extracts the 4 or 16 byte IP address bytes to a 4 or 16 byte buffer in binary form.
///
/// \param
///      isIpV6Address - pointer to 16 byte buffer to copy the address into
///      bufferSize - size of buffer to write into (as a sanity check)
///
/// \retval
///      true if successful.
/// \retval
///      false if the buffer was not big enough
///
bool BcmIpAddress::Get( uint8 * pIPAddress, unsigned int bufferSize ) const
{
    if( fIsIpV6Address )
    {
        if( bufferSize < 16 )
        {
            #if (DEBUG_IP_ADDRESS_METHOD_USAGE)
            {
                gLogMessageRaw << "WARNING!  You called Get( uint8 * pIPAddress, unsigned int bufferSize ) on IPv6 address with bufferSize = " << bufferSize << "   Call stack:" << endl;

                uint32 ra, pc, sp;
                asm( "move %0, $31;"
                     "la   %1, 1f;"
                     "move %2, $29;"
                     "1:" :
                     "=r" (ra),
                     "=r" (pc), 
                     "=r" (sp) );
                MipsStackTrace( pc, sp, ra );
            }
            #endif
            return false;
        }

        fIpV6Address.Get( pIPAddress );
    }
    else
    {
        if( bufferSize < 4 )
        {
            return false;
        }

        fIpV4Address.Get( pIPAddress[0], pIPAddress[1], pIPAddress[2], pIPAddress[3] );
    }

    return true;
}

void BcmIpAddress::NetworkGet(uint32 &ipAddress) const
{
    // method only valid for 4 byte IPv4 address
    if (fIsIpV6Address)
    {
        #if (DEBUG_IP_ADDRESS_METHOD_USAGE)
        {
            gLogMessageRaw << "WARNING!  You called NetworkGet(uint32 &) on IPv6 address " << *this << "   Call stack:" << endl;

            uint32 ra, pc, sp;
            asm( "move %0, $31;"
                 "la   %1, 1f;"
                 "move %2, $29;"
                 "1:" :
                 "=r" (ra),
                 "=r" (pc), 
                 "=r" (sp) );
            MipsStackTrace( pc, sp, ra );
        }
        #endif
        
        return;
    }
    fIpV4Address.NetworkGet(ipAddress);
}

uint32 BcmIpAddress::NetworkGet( void ) const
{
    // method only valid for 4 byte IPv4 address
    if (fIsIpV6Address)
    {
        #if (DEBUG_IP_ADDRESS_METHOD_USAGE)
        {
            gLogMessageRaw << "WARNING!  You called uint32 NetworkGet() on IPv6 address " << *this << "   Call stack:" << endl;

            uint32 ra, pc, sp;
            asm( "move %0, $31;"
                 "la   %1, 1f;"
                 "move %2, $29;"
                 "1:" :
                 "=r" (ra),
                 "=r" (pc), 
                 "=r" (sp) );
            MipsStackTrace( pc, sp, ra );
        }
        #endif  
        
        return 0;
    }    
    
    return fIpV4Address.NetworkGet();
}

bool BcmIpAddress::IsClassA(void) const
{
    if (fIsIpV6Address)
    {
        #if (DEBUG_IP_ADDRESS_METHOD_USAGE)
        {
            gLogMessageRaw << "WARNING!  You called IsClassA() on IPv6 address " << *this << "   Call stack:" << endl;

            uint32 ra, pc, sp;
            asm( "move %0, $31;"
                 "la   %1, 1f;"
                 "move %2, $29;"
                 "1:" :
                 "=r" (ra),
                 "=r" (pc), 
                 "=r" (sp) );
            MipsStackTrace( pc, sp, ra );
        }
        #endif
        
        return false;
    }    

    return fIpV4Address.IsClassA();
}

bool BcmIpAddress::IsClassB(void) const
{
    if (fIsIpV6Address)
    {
        #if (DEBUG_IP_ADDRESS_METHOD_USAGE)
        {
            gLogMessageRaw << "WARNING!  You called IsClassB() on IPv6 address " << *this << "   Call stack:" << endl;

            uint32 ra, pc, sp;
            asm( "move %0, $31;"
                 "la   %1, 1f;"
                 "move %2, $29;"
                 "1:" :
                 "=r" (ra),
                 "=r" (pc), 
                 "=r" (sp) );
            MipsStackTrace( pc, sp, ra );
        }
        #endif
        
        return false;
    }    
    return fIpV4Address.IsClassB();
}

bool BcmIpAddress::IsClassC(void) const
{
    if (fIsIpV6Address)
    {
        #if (DEBUG_IP_ADDRESS_METHOD_USAGE)
        {
            gLogMessageRaw << "WARNING!  You called IsClassC() on IPv6 address " << *this << "   Call stack:" << endl;

            uint32 ra, pc, sp;
            asm( "move %0, $31;"
                 "la   %1, 1f;"
                 "move %2, $29;"
                 "1:" :
                 "=r" (ra),
                 "=r" (pc), 
                 "=r" (sp) );
            MipsStackTrace( pc, sp, ra );
        }
        #endif
        
        return false;
    }        
    return fIpV4Address.IsClassC();
}


BcmIpAddress BcmIpAddress::DefaultSubnetMask(void) const
{
    if (fIsIpV6Address)
    {
        #if (DEBUG_IP_ADDRESS_METHOD_USAGE)
        {
            gLogMessageRaw << "WARNING!  You called DefaultSubnetMask() on IPv6 address " << *this << "   Call stack:" << endl;

            uint32 ra, pc, sp;
            asm( "move %0, $31;"
                 "la   %1, 1f;"
                 "move %2, $29;"
                 "1:" :
                 "=r" (ra),
                 "=r" (pc), 
                 "=r" (sp) );
            MipsStackTrace( pc, sp, ra );
        }
        #endif
    }  

    BcmIpV4Address defaultSubnetMask = fIpV4Address.DefaultSubnetMask();
    return BcmIpAddress( defaultSubnetMask );
}
