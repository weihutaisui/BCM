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
//  Filename:       IpV6Address.h
//  Author:         Sean Dunlap
//  Creation Date:  December 14, 2005
//
//****************************************************************************

#ifndef IPV6ADDRESS_H
#define IPV6ADDRESS_H

//********************** Include Files ***************************************

#include "typedefs.h"
#include "OctetBuffer.h"
#include "MacAddress.h"

//********************** Global Types ****************************************

//********************** Global Constants ************************************

//********************** Global Variables ************************************

//********************** Forward Declarations ********************************

//********************** Class Declaration ***********************************

/**
*      This class is an abstraction of an IP address, providing helper methods
*      to convert to/from various formats.  It is also highly optimized for the
*      kinds of math and comparison operations that we tend to perform.
*/
class BcmIpV6Address
{
public:

    enum
    {
        ADDR_LEN = 16    ///< length of IP address in bytes
    };

    /// Default constructor.
    ///
    BcmIpV6Address();

    /// Copy constructor.
    ///
    /// \param
    ///     rhs - the instance to be copied.
    ///
    BcmIpV6Address( const BcmIpV6Address& rhs );

    /// Initializing constructor.  Reads from the string specified to
    /// initialize the object.
    ///
    /// \param
    ///     pBuf - pointer to a buffer with the 16 bytes to use.
    ///
    BcmIpV6Address(const char *pString);

    /// Initializing constructor.  Reads 16 bytes from the buffer specified to
    /// initialize the object.
    ///
    /// \param
    ///     pBuf - pointer to a buffer with the 16 bytes to use.
    ///
	BcmIpV6Address( const uint8 * pBuf, uint8 prefixLength = 0 );

    /// Destructor.
    ///
    virtual ~BcmIpV6Address();

    /// Assignment operator.
    ///
    /// \param
    ///     rhs - the instance to be copied.
    ///
    /// \return
    ///     A reference to this object.
    ///
    BcmIpV6Address& operator=( const BcmIpV6Address& rhs );

    /// Reset data members to default values.
    ///
	void reset();

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
    bool operator<( const BcmIpV6Address& rhs ) const;

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
    bool operator<=( const BcmIpV6Address& rhs ) const;

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
    bool operator==( const BcmIpV6Address& rhs ) const;

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
    bool operator!=( const BcmIpV6Address& rhs ) const;

    /// Accessor for the byte value at the specified index.  The index must be
    /// in the range 0..3.  Anything outside of this range will cause a 0 to be
    /// returned.
    ///
    /// \param
    ///     Index - the index of the value to be returned.  Must be 0..3.
    ///
    /// \return
    ///     The value at the specified index.
    ///
    unsigned char operator[] (int Index) const;
    
public:

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
    bool WriteTo(BcmOctetBuffer &octetBuffer) const;
        
    /// This methods causes the object to read its 16 bytes from the specified
    /// location in the buffer.  This is the opposite of WriteTo().  Note that
    /// the offset parameter will be adjusted to reflect the next position in the
    /// buffer that needs to read from.
    ///
    /// Note that if the read is successful, then the offset parameter will be
    /// incremented by 16.  If the read fails, then neither the IP address nor
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
    bool ReadFrom(const BcmOctetBuffer &octetBuffer, unsigned int &offset);

    /// Print formatted object contents to specified ostream.
    ///
    /// \param
    ///      ostream& ostrm - reference to the destination output stream.
    ///
    /// \return
    ///      Reference to the destination output stream.
    ///
    ostream& Print( ostream& ostrm ) const;

    /// Allows the 16 bytes of the IP address to be set via an array of bytes,
    /// which is most often how they are stored in packets.
    ///
    /// \param
    ///      pBytes - pointer to the array of 16 bytes containing the IP address.
    ///
    void Set(const uint8 *pBytes);

    /// Allows the IP address to be set by parsing from an ASCII string format
    /// where the form is x:x:x:x:x:x:x:x, where the 'x's are the
    /// hexadecimal values of the eight 16-bit pieces of the address.
    ///
    /// Note that the numbers must be in hex, they must be separated by
    /// periods (.), and the string must be terminated by a '\0'.
    ///
    /// \param
    ///      pString - the ASCII string containing the dotted-decimal IP address.
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem (NULL string, invalid format, etc.).
    ///
    bool Set(const char *pString, unsigned int *pBytesRead = NULL);

    /// Converts the 16 IP address bytes to an ASCII string in the preferred
    /// form of representing IPV6 addresses as a text string.
    ///
    /// Note that the pString parameter must not be NULL, and it must point to a
    /// buffer of at least 40 bytes in order to hold the entire string.
    ///
    /// \param
    ///      pString - pointer to a string where the IP address should be
    ///                written.
    ///      bufferSize - size of the buffer to write to
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem (NULL string).
    ///
    bool Get(char *pString, unsigned int bufferSize) const;

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
    void Get(uint8 * pIPv6Address) const;

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
    void GetMulticastDestinationMacAddress( BcmMacAddress & macAddress ) const;

    /// AppendInterfaceToken
    ///
    /// Appenends an 8 byte interface token, which is derived from the 
    /// ethernet MAC address as described in 
    /// RFC2464 - Transmission of IPv6 Packets over Ethernet Networks
    ///
    /// \param
    ///     macAddress - mac address used to derive interface token
    ///
    bool AppendInterfaceToken( const BcmMacAddress & macAddress );

public:

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
    bool IsEqualWithPrefix( const BcmIpV6Address & prefix ) const;
    
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
    bool IsPrefixEqual ( const BcmIpV6Address & prefix ) const;

    /// Returns the number of bits in this prefix
    ///
    /// \retval
    ///      number of bits in the prefix
    ///
    unsigned int PrefixLengthBits() const;

    /// Sets the number of bits in this prefix
    ///
    ///
    void PrefixLengthBits( unsigned int prefixLen );

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
    bool IsUnspecifiedAddress() const;

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
    bool IsLoopbackAddress() const;

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
    bool IsIpV6AddressWithEmbeddedIPv4Address() const;

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
    //bool GetEmbeddedIpV4Address( BcmIpAddress & ipv4Address ) const;

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
    bool IsLinkLocalAddress( void ) const;

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
    bool FixUpLinkLocalAddress( void );

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
    bool IsSiteLocalAddress( void ) const;

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
    bool IsMulticastAddress() const;

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
    bool IsAllNodesMulticastAddress() const;

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
    bool IsAllRoutersMulticastAddress() const;

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
    void GetSolicitedNodeMulticastAddress( BcmIpV6Address & solicitedNodeMulticastAddress ) const;

protected:

    /// IPv6 address as an array of 16 bytes
    uint8 fIpV6Address[16];

    // the prefix length a decimal value specifying how many of the
    // leftmost contiguous bits of the address comprise
    // the prefix.
    unsigned int fPrefixLengthBits;

public:

    /// Regression tests for this class.  These are all of the tests that I
    /// used for validation when developing the class, examples of typical
    /// usage scenarios, and whatever additional tests that were added when
    /// bugs were found and fixed.
    ///
    /// The body of this method is normally compiled out; edit the .cpp file
    /// and enable the REGRESSION_TEST define to build it in to your image.
    ///
    /// This code also serves as a pretty comprehensive set of examples showing
    /// how this class can/should be used.  Any usage scenario not shown here
    /// is not supported, and may not work if this class changes.
    ///
    /// \note If you don't see your usage scenario in the regression suite,
    ///       go ahead and add it!  This serves a number of purposes:
    ///         - It shows the original designer/implementer how the class is
    ///           being used, which can lead to design improvements (new helper
    ///           methods that make things easier, etc).
    ///         - It provides more examples to other programmers that can be
    ///           copied and reused, rather than re-inventing the wheel.
    ///         - It ensures that code changes to this class won't break your
    ///           usage scenario.  Code changes are valid only if all
    ///           regression tests pass; if your usage scenario isn't
    ///           represented in the tests, then we have no way of knowing
    ///           whether or not the change will break your use.  This will
    ///           reduce the amount of debugging that you have to do, which is
    ///           always a good thing.
    ///
    /// \param
    ///     outStream - the ostream that errors and other messages should be
    ///                 printed to.
    ///
    /// \retval
    ///     true if the regression tests all passed.
    /// \retval
    ///     false if one or more regression tests failed.
    ///
    static bool Regress(ostream &outStream);

};

//********************** Global instances ************************************


// Unspecified Ipv6 Address = ::
extern const BcmIpV6Address kIPv6UnspecifiedIpV6Address;

// Aggregatable Global Unicast Addresses Prefix = 001
extern const BcmIpV6Address kAggregateGlobalUnicastAddressPrefix;

// Link Local Unicast Addresses Prefix = 1111 1110 10
extern const BcmIpV6Address kLinkLocalUnicastAddressPrefix;

// Site Local Unicast Addresses Prefix = 1111 1110 11
extern const BcmIpV6Address kSiteLocalUnicastAddressPrefix;

// Multicast Addresses Prefix = 1111 1111
extern const BcmIpV6Address kMulticastAddressPrefix;

// Solicited Nodes Addresses Prefix = 1111 1110 11
extern const BcmIpV6Address kSolicitedNodeAddressPrefix;

//IPv4-compatible IPv6 address
// |                80 bits               | 16 |      32 bits        |
// +--------------------------------------+--------------------------+
// |0000..............................0000|0000|    IPv4 address     |
// +--------------------------------------+----+---------------------+
extern const BcmIpV6Address kIPv4CompatibleIPv6AddressPrefix;

// IPv4-mapped IPv6 address
// |                80 bits               | 16 |      32 bits        |
// +--------------------------------------+--------------------------+
// |0000..............................0000|FFFF|    IPv4 address     |
// +--------------------------------------+----+---------------------+
extern const BcmIpV6Address kIPv4MappedIPv6AddressPrefix;

//  All Nodes Addresses:    FF01:0:0:0:0:0:0:1
//                          FF02:0:0:0:0:0:0:1
extern const BcmIpV6Address kAllNodesMulticastAddress1;
extern const BcmIpV6Address kAllNodesMulticastAddress2;

// All Routers Addresses:   FF01:0:0:0:0:0:0:2
//                          FF02:0:0:0:0:0:0:2
//                          FF05:0:0:0:0:0:0:2
extern const BcmIpV6Address kNodeLocalAllRoutersMulticastAddress;
extern const BcmIpV6Address kLinkLocalAllRoutersMulticastAddress;
extern const BcmIpV6Address kSiteLocalAllRoutersMulticastAddress;

//********************** Inline Method Implementations ***********************
inline ostream& operator<<( ostream& ostrm, const BcmIpV6Address& rhs )
{
    return rhs.Print( ostrm );
}


#endif

