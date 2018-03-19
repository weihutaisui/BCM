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
//  $Id$
//
//  Filename:       IpAddress.h
//  Author:         Sean Dunlap
//  Creation Date:  February 3, 2006
//
//      This is a wrapper class for BcmIpV4Address and BcmIpV6Address. 
//   
//
//****************************************************************************

#ifndef IPADDRESS_H
#define IPADDRESS_H

//********************** Include Files ***************************************

#include "typedefs.h"
#include "OctetBuffer.h"
#include "BcmString.h"

#include "IpV4Address.h"
#include "IpV6Address.h"

#include <iostream>

#if defined(WIN32)
    #include <windows.h>    // for sockaddr, sockaddr_in, & sockaddr_in6
#else
    #include <sys/socket.h> // for sockaddr
    #include <netdb.h>      // for sockaddr_in & sockaddr_in6
#endif

//********************** Global Types ****************************************

// For use by SNMP InetAddressType / InetAddress pairs mostly

typedef enum 
{
  kInetAddressUnknown = 0,
  kInetAddressIPv4 = 1,
  kInetAddressIPv6 = 2
} InetAddressType;

typedef enum 
{
  kInetAddressLengthUnknown = 0,
  kInetAddressLengthIPv4 = 4,
  kInetAddressLengthIPv6 = 16
} InetAddressLength;

typedef struct _InetAddress
{
  InetAddressType Type;
  BcmString Address;
} InetAddress;

//********************** Global Constants ************************************

//********************** Global Variables ************************************

//********************** Forward Declarations ********************************

//********************** Class Declaration ***********************************


/**
*      This class is an abstraction of an IP address, providing helper methods
*      to convert to/from various formats.  It is also highly optimized for the
*      kinds of math and comparison operations that we tend to perform.
*/
class BcmIpAddress
{
public:

    /// Default constructor.
    ///
    BcmIpAddress();

    /// Copy constructor.
    ///
    /// \param
    ///     rhs - the instance to be copied.
    ///
    BcmIpAddress( const BcmIpAddress& rhs );

public:

    /// Initializing constructor.  Uses the values specified to initialize
    /// the object.
    ///
    /// \param
    ///     ipV4Address - an IPv4 address
    ///
    BcmIpAddress(const char *pString);

    /// Initializing constructor.  Uses the values specified to initialize
    /// the object.
    ///
    /// \param
    ///     ipV4Address - an IPv4 address
    ///
	BcmIpAddress( const BcmIpV4Address & ipV4Address );

    /// Initializing constructor.  Uses the values specified to initialize
    /// the object.
    ///
    /// \param
    ///     ipV6Address - an IPv6 address
    ///
	BcmIpAddress( const BcmIpV6Address & ipV6Address );
    
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
	BcmIpAddress( byte b0, byte b1, byte b2, byte b3 );

    /// Initializing constructor.  Reads 4 (or 16) bytes from the buffer specified to
    /// initialize the object.
    ///
    /// \param
    ///     buf - pointer to a buffer with the 4 (or 16) bytes to use.
    ///
	BcmIpAddress( const byte* buf, bool isIpV6Address = false );
    
    /// Initializing constructor.  Takes a sockaddr structure which knows if
    /// it's an IPv4 or IPv6 address.
    ///
    /// \param
    ///     saddr - pointer to a sockaddr structure
    ///
	BcmIpAddress( const struct sockaddr* pSockAddr );
    
    /// Initializing constructor.  Uses the values specified to initialize
    /// the object. PR 12595
    ///
    /// \param
    ///     String - a representation of an IP address; either in ASCII
    ///              notation or raw octets
    ///
    BcmIpAddress(const BcmString &String, bool Ascii = false);

    /// Destructor.
    ///
    ~BcmIpAddress();

    /// Assignment operator.
    ///
    /// \param
    ///     rhs - the instance to be copied.
    ///
    /// \return
    ///     A reference to this object.
    ///
    BcmIpAddress& operator=( const BcmIpAddress& rhs );

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
    bool operator<( const BcmIpAddress& rhs ) const;

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
    bool operator<=( const BcmIpAddress& rhs ) const;

	/// Equality operator.
    ///
    /// PR12290 - inlined this method.
    ///
    /// \param
    ///     rhs - the instance to be compared to.
    ///
    /// \retval
    ///     true if this instance is equal to the one specified.
    /// \retval
    ///     false if not equal.
    ///
    inline bool operator==( const BcmIpAddress& rhs ) const;

	/// Inequality operator.
    ///
    /// PR12290 - inlined this method.
    ///
    /// \param
    ///     rhs - the instance to be compared to.
    ///
    /// \retval
    ///     true if this instance is not equal to the one specified.
    /// \retval
    ///     false if equal.
    ///
    inline bool operator!=( const BcmIpAddress& rhs ) const;

	/// Assignment operator with mask.  Masks this object with another one using
    /// the & operator.  This is useful for things like applying subnet masks.
    ///
    /// \param
    ///     rhs - the instance to be used as a mask value.
    ///
    /// \return
    ///     A reference to this object.
    ///
    BcmIpAddress &operator&= (const BcmIpAddress &rhs);
    
	/// Assignment operator with mask.  Masks this object with another one using
    /// the | operator.  This is useful for things like applying subnet masks.
    ///
    /// \param
    ///     rhs - the instance to be used as a mask value.
    ///
    /// \return
    ///     A reference to this object.
    ///
    BcmIpAddress &operator|= (const BcmIpAddress &rhs);
    
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
    unsigned char operator[] (int Index) const;

    //@{
    /// Accumulation/addition operators, including postfix and prefix increment.
    /// uint32 should be sufficient for built-in types.
    ///
    /// \param
    ///     value - the value to be accumulated.
    ///
    /// \return
    ///     A reference to this object.
    ///
    BcmIpAddress &operator += (const BcmIpAddress &value);
    BcmIpAddress &operator += (uint32 value);
    BcmIpAddress &operator ++ ();
    BcmIpAddress &operator ++ (int);
    //@}

    //@{
    /// Subtraction operators, including postfix and prefix decrement.  uint32
    /// should be sufficient for built-in types.
    ///
    /// \param
    ///     value - the value to be subtracted.
    ///
    /// \return
    ///     A reference to this object.
    ///
    BcmIpAddress &operator -= (const BcmIpAddress &value);
    BcmIpAddress &operator -= (uint32 value);
    BcmIpAddress &operator -- ();
    BcmIpAddress &operator -- (int);

    //@}
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
    int IsEqualWithMask(const BcmIpAddress &otherInstance,
                        const BcmIpAddress &mask) const;
                        
    /// Accessor for an application that needs to know whether the contained
    /// IP address is an IPv4 or IPv6 address.
    ///
    /// \retval
    ///     true if this is an IPv6 addrress
    /// \retval
    ///     false is this is an IPv4 address
    ///
    inline bool IsIpV6Address( void ) const
    {
        return fIsIpV6Address;
    }

    /// Accessor that allows the IPv4/v6 mode to be selected.  This is usually
    /// followed by a call to ReadFrom().
    ///
    /// \param
    ///     isIpV6 - true to select IPv6 mode, false to select IPv4 mode.
    ///
    inline void SetIpV6Mode(bool isIpV6)
    {
        fIsIpV6Address = isIpV6;
    }

    /// Reset data members to default values.
    ///
    /// PR12290 - inlined this method.
    ///
	inline void reset();

    /// Write the IpAddress to the end of specified buffer in network order.  
    /// This is commonly used as part of the IP protocol header.  
    ///
    /// \param
    ///      octetBuffer - reference to the destination buffer.
    /// \param
    ///      encodeLength - if set to true, we will write the length of the IP
    ///                     address to the buffer before the actual octets
    ///                     which represent the address.  This would be used
    ///                     in applications where we need to be able to 
    ///                     distinguish IPv4 from IPv6 addresses.  PR 12595
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem (not enough space in the 
    ///          output buffer).
    ///
    bool WriteTo(BcmOctetBuffer &octetBuffer, bool encodeLength = false) const;
        
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
    /// \param
    ///      decodeLength - if set to true, we will read the length of the IP
    ///                     address from the buffer before the actual octets
    ///                     which represent the address.  This would be used
    ///                     in applications where we need to be able to 
    ///                     distinguish IPv4 from IPv6 addresses.  PR 12595
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem (not enough bytes of data in the
    ///          buffer).
    ///
    bool ReadFrom(const BcmOctetBuffer &octetBuffer, unsigned int &offset, bool decodeLength = false);

    /// Print formatted object contents to specified ostream.
    ///
    /// \param
    ///      ostream& ostrm - reference to the destination output stream.
    ///
    /// \return
    ///      Reference to the destination output stream.
    ///
    ostream& Print( ostream& ostrm ) const;

    /// Allows the 4 bytes of the IP address to be set.
    ///
    /// \param
    ///     ipAddress1 - the first byte of the IP address.
    /// \param
    ///     ipAddress2 - the second byte of the IP address.
    /// \param
    ///     ipAddress3 - the third byte of the IP address.
    /// \param
    ///     ipAddress4 - the fourth byte of the IP address.
    ///
    void Set(uint8 ipAddress1, uint8 ipAddress2, 
             uint8 ipAddress3, uint8 ipAddress4);

    /// Allows the 4 (or 16) bytes of the IP address to be set via an array of bytes,
    /// which is most often how they are stored in packets.  The array is assumed
    /// to be in network byte order, and will be converted to host endian order.
    ///
    /// PR12290 - inlined this method.
    ///
    /// \param
    ///      pBytes - pointer to the array of 4 bytes containing the IP address.
    ///
    inline void Set(uint8 *pBytes, bool isIpV6Address = false);
    
    /// Allows the IP address to be set from a sockaddr structure, which knows
    /// whether it's an IPv4 or IPv6 address.
    ///
    /// \param
    ///      pSockAddr - pointer to a sockaddr structure
    ///
    void Set(const struct sockaddr *pSockAddr);

    /// Allows the IP address to be set by parsing from an ASCII string format
    /// (i.e. "192.168.190.220").
    ///
    /// Note that the numbers must be in decimal, they must be separated by
    /// periods (.), and the string must be terminated by a '\0'.
    ///
    /// \param
    ///      pString - the ASCII string containing the dotted-decimal IP address.
    /// \param
    ///      pBytesRead - output variable (optional) into which the number of
    ///                   ASCII characters read will be placed.
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem (NULL string, invalid format, etc.).
    ///
    bool Set(const char *pString, unsigned int *pBytesRead = NULL);
    
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
    bool Set(const BcmString &String, bool Ascii = false, unsigned int *pBytesRead = NULL);

    /// Allows the IP address to be set as a uint32 (all 4 bytes packed together).
    /// Note that the bytes are in the native processor (host) order.
    ///
    /// \param
    ///      ipAddress - the uint32 from with the IP address should be stored.
    ///  
    void Set(uint32 ipAddress);
    
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
    bool Set (const InetAddress &ipAddress, bool EnforceTypeLengthConsistency = true, bool TypeTakesPriority = true);
    void Get (InetAddress &ipAddress);

    /// Same as above, but it converts the IP address from network order to host
    /// order first.
    ///
    /// \param
    ///      ipAddress - the uint32 from with the IP address should be stored.
    ///  
    void NetworkSet(uint32 ipAddress);

    /// Allows all 4 IP address bytes to be retrieved.
    ///
    /// \param
    ///     ipAddress1 - the first byte of the IP address.
    /// \param
    ///     ipAddress2 - the second byte of the IP address.
    /// \param
    ///     ipAddress3 - the third byte of the IP address.
    /// \param
    ///     ipAddress4 - the fourth byte of the IP address.
    ///
    void Get(uint8 &ipAddress1, uint8 &ipAddress2, uint8 &ipAddress3,
             uint8 &ipAddress4) const;

    /// Converts the 4 IP address bytes to an ASCII string in dotted-decimal
    /// format.
    ///
    /// Note that the pString parameter must not be NULL, and it must point to a
    /// buffer of at least 16 bytes in order to hold the entire string.
    ///
    /// \param
    ///      pString - pointer to a string where the IP address should be
    ///                written.
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem (NULL string).
    ///
    bool Get(char *pString, unsigned int bufferSize) const;

    //@{
    /// Allows the IP address to be extracted as a uint32 (all 4 bytes packed
    /// together).  Note that the bytes are in the native processor (host) order.
    ///
    /// \param
    ///      ipAddress - reference to a uint32 into which the IP address should
    ///                  be written.
    ///
    /// \return
    ///     The uint32 IP address value.
    ///
    void Get(uint32 &ipAddress) const;
	uint32 Get( void ) const;
    //@}

    /// Extracts the 4 or 16 byte IP address bytes to a 16 byte buffer in binary form.
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
    bool Get(uint8 * pIPAddress, unsigned int bufferSize) const;
    
    //@{
    /// Allows the IP address to be extracted as a sockaddr structure.
    ///
    /// PR12290 - inlined this method.
    ///
    /// \param
    ///      pSockAddr - a pointer to the sockaddr structure to be filled in.
    ///
    inline void Get(struct sockaddr *pSockAddr) const;
    //@}

    //@{
    /// This extracts the IP address into a uint32 in network order.
    ///
    /// \param
    ///      ipAddress - reference to a uint32 into which the IP address should
    ///                  be written.
    ///
    /// \return
    ///     The uint32 IP address value.
    ///
    void NetworkGet(uint32 &ipAddress) const;
    uint32 NetworkGet() const;
    //@}

    //@{
    /// Allows the client to query the address class for the currently stored IP
    /// address, per RFC 791.
    ///
    /// \retval
    ///      true if the address class matches
    /// \retval
    ///     false otherwise
    ///
    bool IsClassA(void) const;
    bool IsClassB(void) const;
    bool IsClassC(void) const;
    //@}

    /// Returns the default subnet mask that should be used for the currently
    /// stored IP address, per RFC 791.
    ///
    /// \return
    ///      The default subnet mask for the IP address class.
    ///
    BcmIpAddress DefaultSubnetMask(void) const;

    /// Accessor for the contained BcmIpV6Address
    ///
    /// \return
    ///     The contained IPv6 address
    ///
    inline const BcmIpV6Address & GetIpV6Address( void ) const;

    /// Accessor for the contained BcmIpV4Address
    ///
    /// \return
    ///     The contained IPv4 address
    ///
    inline const BcmIpV4Address & GetIpV4Address( void ) const;

    /// Accessor for the contained BcmIpV6Address (allowing modification of
    /// that object).
    ///
    /// Added for PR13079.
    /// 
    /// \return
    ///     The contained IPv6 address
    ///
    inline BcmIpV6Address & GetIpV6AddressMod( void );

    /// Accessor for the contained BcmIpV4Address (allowing modification of
    /// that object).
    ///
    /// Added for PR13079.
    /// 
    /// \return
    ///     The contained IPv4 address
    ///
    inline BcmIpV4Address & GetIpV4AddressMod( void );

protected:

    // flags whether this is an IPV6 address or IPV4 address
    bool fIsIpV6Address;

    // If this is an IPV4 address, it is contained in this field
    BcmIpV4Address fIpV4Address;

    // If this is an IPV6 address, it is contained in this field
    BcmIpV6Address fIpV6Address;
};


//********************** Global instances ************************************

/// Special global object who's value is "0.0.0.0".  This is useful
/// for comparisons, and eliminates the need to create a temporary one.
extern const BcmIpAddress kAllZerosIp;

/// Special global object who's value is "255.255.255.255".  This is useful
/// for comparisons, and eliminates the need to create a temporary one.
extern const BcmIpAddress kBroadcastIp;

/// Special global object who's value is "127.0.0.1".  This is useful
/// for comparisons, and eliminates the need to create a temporary one.
extern const BcmIpAddress kSoftwareLoopbackIp;

/// The address 0:0:0:0:0:0:0:0 is called the unspecified address.
extern const BcmIpAddress kIPv6UnspecifiedAddress;

/// The address 0:0:0:0:0:0:0:0 is the loopback address.
extern const BcmIpAddress kIPv6LoopbackAddress;

//********************** Inline Method Implementations ***********************

/// Allows you to do the following:
///
///  cout << ipAddress << endl;
///
inline ostream& operator<<( ostream& ostrm, const BcmIpAddress& rhs )
{
    return rhs.Print( ostrm );
}


inline bool BcmIpAddress::operator==( const BcmIpAddress& rhs ) const
{
    if( fIsIpV6Address != rhs.fIsIpV6Address)
    {
        #if (0) //DEBUG_IP_ADDRESS_METHOD_USAGE)
        {
            gLogMessageRaw << "WARNING!  You called operator== on mismatched IPv4/IPv6 addresses!  Call stack:" << endl;

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

    if (fIsIpV6Address)
    {
        if (fIpV6Address == rhs.fIpV6Address)
        {
            return true;
        }
    }
    else
    {
        if (fIpV4Address == rhs.fIpV4Address)
        {
            return true;
        }
    }

    return false;
}


inline bool BcmIpAddress::operator!=( const BcmIpAddress& rhs ) const
{
    return !(*this == rhs);
}


inline const BcmIpV6Address & BcmIpAddress::GetIpV6Address( void ) const
{
    return fIpV6Address;
}


inline const BcmIpV4Address & BcmIpAddress::GetIpV4Address( void ) const
{
    return fIpV4Address;
}


inline BcmIpV6Address & BcmIpAddress::GetIpV6AddressMod( void )
{
    return fIpV6Address;
}


inline BcmIpV4Address & BcmIpAddress::GetIpV4AddressMod( void )
{
    return fIpV4Address;
}


inline void BcmIpAddress::reset()
{
    fIsIpV6Address = false;
    fIpV4Address.reset();
    fIpV6Address.reset();
}


inline void BcmIpAddress::Set(uint8 *pBytes, bool isIpV6Address)
{
    if( isIpV6Address )
    {
        fIsIpV6Address = true;
        fIpV6Address.Set( pBytes );
    }
    else
    {
        fIsIpV6Address = false;
        fIpV4Address.Set( pBytes );
    }
}


inline uint32 BcmIpAddress::Get( void ) const
{
    if (fIsIpV6Address)
    {
        #if (DEBUG_IP_ADDRESS_METHOD_USAGE)
        {
            gLogMessageRaw << "WARNING!  You called uint32 Get() on IPv6 address " << *this << "   Call stack:" << endl;

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
    // method only valid for 4 byte IPv4 address
    return fIpV4Address.Get();
}


#endif

