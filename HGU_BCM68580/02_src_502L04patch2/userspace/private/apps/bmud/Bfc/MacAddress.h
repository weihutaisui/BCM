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
//  Filename:       MacAddress.h
//  Author:         David Pullen
//  Creation Date:  May 3, 1999
//
//****************************************************************************

#ifndef MACADDRESS_H
#define MACADDRESS_H

//********************** Include Files ***************************************

#include "typedefs.h"

// Wrapper/utility class for a memory buffer.
#include "OctetBuffer.h"

#include <iostream>
#include <iomanip>

//********************** Global Types ****************************************

//********************** Global Constants ************************************

//********************** Global Variables ************************************

//********************** Class Declaration ***********************************


/**
*      This is a utility class that encapsulates an Ethernet MAC address.  It
*      is really just an array of 6 bytes, but we do lots of stuff with them
*      (comparisons, reading from/writing to octet buffers, etc.).
*
*      This class has been highly optimized for the masking and comparison
*      operations that we tend to perform as a networking device.
*
*      The BcmMacAddress class below is the result of merging two similar
*      classes, BcmMacAddress and MacAddress, each of which represented a MAC
*      address.  Each class was widely used, so completely cleaning up the
*      unified class would have meant changing the many files that used
*      the classes.  Instead, the unified class has some mildly quirky 
*      appearance to minimize the number of file changes needed.  The
*      quirks are noted below.
*/
class BcmMacAddress
{
public:

    /// Default Constructor.  Initializes the state of the object to an
    /// equivalent of the mac address 00:00:00:00:00:00.
    ///
    BcmMacAddress(void);

    /// Initializing Constructor.  Initializes the state of the object with
    /// the values specified.
    ///
    /// \param
    ///     macAddress1 - the first byte of the MAC address value.
    /// \param
    ///     macAddress2 - the second byte of the MAC address value.
    /// \param
    ///     macAddress3 - the third byte of the MAC address value.
    /// \param
    ///     macAddress4 - the fourth byte of the MAC address value.
    /// \param
    ///     macAddress5 - the fifth byte of the MAC address value.
    /// \param
    ///     macAddress6 - the sixth byte of the MAC address value.
    ///
    BcmMacAddress(uint8 macAddress1, uint8 macAddress2, uint8 macAddress3,
                  uint8 macAddress4, uint8 macAddress5, uint8 macAddress6);

    /// Copy Constructor.  Initializes the state of this object to match that
    /// of the instance passed in.
    ///
    /// \param
    ///      otherInstance - reference to the object to copy.
    ///
    BcmMacAddress(const BcmMacAddress &otherInstance);

    /// Initializing Constructor.  Initializes the state of the object from
    /// the byte array specified.  The array must contain at least 6 bytes.
    ///
    /// \note  This is an artifact of the defunct MacAddress class (although it
    ///        also used to exist in BcmMacAddress)
    ///
    /// \param
    ///     buf - pointer to buffer holding init values
    ///
	BcmMacAddress(const byte* buf);
    
    /// Destructor.  Frees up any memory/objects allocated, cleans up internal
    /// state.
    ///
    ~BcmMacAddress();

    /// Assignment operator.  Copies the state of the instance passed in so that
    /// this object is "identical".
    ///
    /// \param
    ///      otherInstance - reference to the object to copy.
    ///
    /// \return
    ///      A reference to "this" so that operator = can be chained.
    ///
    BcmMacAddress & operator = (const BcmMacAddress &otherInstance);

    /// Allows the 6-byte MAC address to be set.
    ///
    /// \param
    ///     macAddress1 - the first byte of the MAC address value.
    /// \param
    ///     macAddress2 - the second byte of the MAC address value.
    /// \param
    ///     macAddress3 - the third byte of the MAC address value.
    /// \param
    ///     macAddress4 - the fourth byte of the MAC address value.
    /// \param
    ///     macAddress5 - the fifth byte of the MAC address value.
    /// \param
    ///     macAddress6 - the sixth byte of the MAC address value.
    ///
    void Set(uint8 macAddress1, uint8 macAddress2, uint8 macAddress3,
             uint8 macAddress4, uint8 macAddress5, uint8 macAddress6);

    /// Allows the 6 bytes to be specified in a flat array, which is often how
    /// it is stored in a packet buffer.  It is assumed that the bytes are
    /// stored in network order; they will be converted to host endian order.
    ///
    /// \param
    ///      pMacAddress - pointer to the 6 byte mac address.
    ///
    inline void Set(const uint8 *pMacAddress);

    /// Allows the MAC address to be set by parsing from an ASCII string format
    /// (ie "01:02:03:04:05:06").
    ///
    /// Note that the numbers must be in hex, they can be separated by colons (:)
    /// or periods (.), and the string must be terminated by a '\0'.
    ///
    /// \param
    ///      pString - the ASCII string containing the 6 bytes of the MAC address.
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


    /// Performs the same as Set(const char *pString, unsigned int *pBytesRead = NULL);
    /// but checks each character to make sure it's valid hex MAC Address
    /// Spaces( ), periods(.), and dashes(-) are converted to colons(:) and then the
    /// rest of the Set function is executed
    ///
    /// Allows the MAC address to be set by parsing from an ASCII string format
    /// (ie "01:02:03:04:05:06").
    ///
    /// Note that the numbers must be in hex, they can be separated by colons (:)
    /// or periods (.), and the string must be terminated by a '\0'.
    ///
    /// \param
    ///      pString - the ASCII string containing the 6 bytes of the MAC address.
    /// \param
    ///      pBytesRead - output variable (optional) into which the number of
    ///                   ASCII characters read will be placed.
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem (NULL string, invalid format, etc.).
    ///
    bool VerifyAndSet(const char *pString, unsigned int *pBytesRead = NULL);

    /// Allows all 6 MAC address bytes to be retrieved.
    ///
    /// \param
    ///     macAddress1 - the first byte of the MAC address value.
    /// \param
    ///     macAddress2 - the second byte of the MAC address value.
    /// \param
    ///     macAddress3 - the third byte of the MAC address value.
    /// \param
    ///     macAddress4 - the fourth byte of the MAC address value.
    /// \param
    ///     macAddress5 - the fifth byte of the MAC address value.
    /// \param
    ///     macAddress6 - the sixth byte of the MAC address value.
    ///                     
    void Get(uint8 &macAddress1, uint8 &macAddress2, uint8 &macAddress3,
             uint8 &macAddress4, uint8 &macAddress5, uint8 &macAddress6) const;

    /// Converts the 6 MAC address bytes to an ASCII string.
    ///
    /// Note that the pString parameter must not be NULL, and it must point to a
    /// buffer of at least 18 bytes in order to hold the entire hex string, or
    /// 24 bytes to hold the entire decimal string, including the \0 string
    /// terminator.
    ///
    /// \param
    ///      pString - pointer to a string where the MAC address should be
    ///                written.
    /// \param
    ///      numbersAsHex - tells whether to format the string as hex or decimal
    ///                     numbers.
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem (NULL string).
    ///
    bool Get(char *pString, bool numbersAsHex = true, bool useColons = true) const;

    /// Copies the MAC address bytes to the location specified.
    ///
    /// \note  This function is an artifact of the defunct MacAddress class
    ///
    /// \param
    ///      Target - pointer to a buffer where the MAC address should be copied
    ///
    void getAddr (uint8 *Target) const
    {
        Target[0] = (uint8) ((fMacValue1 >> 24) & 0xff);
        Target[1] = (uint8) ((fMacValue1 >> 16) & 0xff);
        Target[2] = (uint8) ((fMacValue1 >>  8) & 0xff);
        Target[3] = (uint8) (fMacValue1 & 0xff);
    
        Target[4] = (uint8) ((fMacValue2 >> 8) & 0xff);
        Target[5] = (uint8) (fMacValue2 & 0xff);
    }

    /// Comparison (equivalence) operator.  Works the same as == for integers,
    /// etc.  Returns 1 if the MAC address specified is equal to this object;
    /// otherwise, it returns 0.
    ///
    /// Usage:
    ///
    /// \code
    ///      BcmMacAddress addr1, addr2;
    ///
    ///      if (addr1 == addr2)
    ///      {
    ///          // They have the same MAC address.
    ///      }
    /// \endcode
    ///
    /// \param
    ///      otherInstance - the other instance to compare against.
    ///
    /// \retval
    ///      1 if they contain the same MAC address.
    /// \retval
    ///      0 if they contain different MAC addresses.
    ///
    inline int operator == (const BcmMacAddress &otherInstance) const;

    /// Comparison (non-equivalence) operator.  Works the same as == for
    /// integers, etc.  Returns 0 if the MAC address specified is equal to this
    /// object; otherwise, it returns 1.
    ///
    /// Usage:
    ///
    /// \code
    ///      BcmMacAddress addr1, addr2;
    ///
    ///      if (addr1 != addr2)
    ///      {
    ///          // They have different MAC addresses.
    ///      }
    /// \endcode
    ///
    /// \param
    ///      otherInstance - the other instance to compare against.
    ///
    /// \retval
    ///      1 if they contain different MAC addresses.
    /// \retval
    ///      0 if they contain the same MAC address.
    ///
    inline int operator != (const BcmMacAddress &otherInstance) const;

    /// Comparison (less-than) operator.  Works the same as < for integers,
    /// etc.  Returns 1 if the MAC address specified is greater than this
    /// object's address; otherwise, it returns 0.
    ///
    /// Usage:
    ///
    /// \code
    ///      BcmMacAddress addr1, addr2;
    ///
    ///      if (addr1 < addr2)
    ///      {
    ///          // addr1 is less.
    ///      }
    /// \endcode
    ///
    /// \param
    ///      otherInstance - the other instance to compare against.
    ///
    /// \retval
    ///      true if this object is less than otherInstance.
    /// \retval
    ///      false if this object is not less than otherInstance.
    ///
    inline bool operator < (const BcmMacAddress &otherInstance) const;

    /// Comparison (greater-than) operator.  Works the same as > for integers,
    /// etc.  Returns 1 if the MAC address specified is less than this
    /// object's address; otherwise, it returns 0.
    ///
    /// Usage:
    ///
    /// \code
    ///      BcmMacAddress addr1, addr2;
    ///
    ///      if (addr1 > addr2)
    ///      {
    ///          // addr1 is greater.
    ///      }
    /// \endcode
    ///
    /// \param
    ///      otherInstance - the other instance to compare against.
    ///
    /// \retval
    ///      1 if this object is greater than otherInstance.
    /// \retval
    ///      0 if this object is not greater than otherInstance.
    ///
    inline int operator > (const BcmMacAddress &otherInstance) const;

    /// Comparison (less-than or equal) operator.  Works the same as <= for 
    /// integers, etc.  Returns 1 if the MAC address specified is greater than 
    /// this object's address; otherwise, it returns 0.
    ///
    /// Usage:
    ///
    /// \code
    ///      BcmMacAddress addr1, addr2;
    ///
    ///      if (addr1 <= addr2)
    ///      {
    ///          // addr1 is less or equal.
    ///      }
    /// \endcode
    ///
    /// \param
    ///      otherInstance - the other instance to compare against.
    ///
    /// \retval
    ///      true if this object is less than or equal to otherInstance.
    /// \retval
    ///      false if this object is not less than otherInstance.
    ///
    inline bool operator <= (const BcmMacAddress &otherInstance) const;

    /// Comparison (greater-than or equal) operator.  Works the same as >= for 
    /// integers, etc.  Returns 1 if the MAC address specified is less than this
    /// object's address; otherwise, it returns 0.
    ///
    /// Usage:
    ///
    /// \code
    ///      BcmMacAddress addr1, addr2;
    ///
    ///      if (addr1 >= addr2)
    ///      {
    ///          // addr1 is greater or equal.
    ///      }
    /// \endcode
    ///
    /// \param
    ///      otherInstance - the other instance to compare against.
    ///
    /// \retval
    ///      1 if this object is greater than or equal to otherInstance.
    /// \retval
    ///      0 if this object is not greater than otherInstance.
    ///
    inline int operator >= (const BcmMacAddress &otherInstance) const;

    /// Addition operator.  Adds the specified value to the MAC address.  Note
    /// that this might change the OUI portion of the address (the vendor id,
    /// first 3 bytes)!
    ///
    /// Added for PR5058.
    ///
    /// \param
    ///      value - the value to add to the MAC address.
    ///
    /// \return
    ///      A reference to this object.
    ///
    inline BcmMacAddress &operator += (uint32 value);

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
    inline int IsEqualWithMask(const BcmMacAddress &otherInstance,
                               const BcmMacAddress &mask) const;

    /// This method causes the object to write its 6 bytes out to the specified
    /// buffer in network order.  This is commonly used as part of the Ethernet
    /// protocol header.  The bytes are appended to the buffer.
    ///
    /// \param
    ///      octetBuffer - reference to the buffer where the bytes should be
    ///                    written.
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem (not enough bytes available in the
    ///          buffer).
    ///
    inline bool WriteTo(BcmOctetBuffer &octetBuffer) const;

    /// This methods causes the object to read its 6 bytes from the specified
    /// location in the buffer.  This is the opposite of WriteTo().  Note that
    /// the offset parameter will be adjusted to reflect the next position in the
    /// buffer that needs to read from.
    ///
    /// Note that if the read is successful, then the offset parameter will be
    /// incremented by 6.  If the read fails, then neither the MAC address nor
    /// the offset parameter will be modified.
    ///
    /// \param
    ///      octetBuffer - reference to the buffer from which the bytes should
    ///                    be read.
    /// \param
    ///      offset - the offset into the buffer where the first MAC address byte
    ///               is.
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem (not enough bytes of data in the
    ///          buffer).
    ///
    inline bool ReadFrom(const BcmOctetBuffer &octetBuffer, unsigned int &offset);

    /// Print formatted BcmMacAddress to specified ostream.
    ///
    /// \param
    ///      ostrm - reference to the destination output stream.
    ///
    /// \return
    ///      reference to the destination output stream.
    ///
    ostream& Print( ostream& ostrm ) const;

    /// Print formatted BcmMacAddress to specified ostream.
    ///
    /// \note  This function is an artifact of the defunct MacAddress class
    ///        print() and Print() have the same implementation.
    ///
    /// \param
    ///      ostrm - reference to the destination output stream.
    ///
    /// \return
    ///      reference to the destination output stream.
    ///
    inline ostream& print( ostream& ostrm ) const
    {
        return (Print (ostrm));
    }

  	/// Print MAC address in network format to specified ostream.
    ///
    /// \note  This function is an artifact of the defunct MacAddress class
    ///
    /// \param
    ///      ostrm - reference to the destination output stream.
    ///
    /// \return
    ///      reference to the destination output stream.
    ///
	ostream& printNet( ostream& ostrm ) const
	{
        uint8 macAddr[6];

        Get(macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);

        ostrm.write( (char *)macAddr, 6 );
        return ostrm;
	}

    /// Checks to see if the MAC address is multicast (the least significant bit
    /// of byte 0 is 1).
    ///
    /// \retval
    ///      true if multicast.
    /// \retval
    ///      false if not multicast.
    ///
    inline bool IsMulticast(void) const;

    /// Check if address is multicast
    ///
    /// \note  This function is an artifact of the defunct MacAddress class.
    ///        isMulti() has the same implementation as IsMulticast().
    ///
    /// \retval
    ///      true if multicast.
    /// \retval
    ///      false if not multicast.
    ///
	inline bool isMulti ()
	{
		return IsMulticast();
	}

    /// the size of the buffer to hold a MAC address
    enum { bufSize = 6 };

private:

    /// These are used for doing the numerical comparisons of MAC addresses.
    uint32 fMacValue1;
    uint16 fMacValue2;

};


//********************** Global instances ************************************

/// the following typdef allows the many references to the defunct MacAddress 
/// class to remain unchanged.
typedef BcmMacAddress MacAddress;

/// The two functions below are  artifacts of the defunct MacAddress class
///
/// Global function which returns a reference to BcmMacAddress object which
/// represents the broadcast MAC address for all CM.
///
/// enum class constants do not work well for MacAddress, since each address
/// is (6) bytes long.  hence the global function.
///
/// \return
///     A reference to the "all-CMs" broadcast mac address.
///
BcmMacAddress& bcastCmMacAddr();

/// Global function which returns a reference to BcmMacAddress object which
/// represents the broadcast MAC address for all CMTS.
///
/// \return
///     A reference to the "all-CMTSs" broadcast mac address.
///
BcmMacAddress& bcastCmtsMacAddr();

/// Special global object who's value is "00:00:00:00:00:00".  This is useful
/// for comparisons, and eliminates the need to create a temporary one.
extern const BcmMacAddress kAllZerosMac;

/// Special global object who's value is "ff:ff:ff:ff:ff:ff".  This is useful
/// for comparisons, and eliminates the need to create a temporary one.
extern const BcmMacAddress kBroadcastMac;

//********************** Inline Method Implementations ***********************


/// Allows you to do the following:
///
///  cout << macAddress << endl;
///
inline ostream& operator<<( ostream& ostrm, const BcmMacAddress& rhs )
{
    return rhs.Print( ostrm );
}


inline int BcmMacAddress::operator == (const BcmMacAddress &otherInstance) const
{
    // Use the previously calcuated numerical values for the comparison.
    //
    // Use funky math to do the comparison.  This eliminates a branch from the
    // comparison.  If the MAC addresses are equal, then I should be able to
    // subtract both values, OR the differences, and get a 0.  If I get non-0,
    // then they are not equal.
    if ((fMacValue1 - otherInstance.fMacValue1) |
        (fMacValue2 - otherInstance.fMacValue2))
    {
        return 0;
    }

    return 1;
}


inline int BcmMacAddress::operator != (const BcmMacAddress &otherInstance) const
{
    // Use the implementation of operator ==, then logically negate the result.
    return !(*this == otherInstance);
}


inline bool BcmMacAddress::operator < (const BcmMacAddress &otherInstance) const
{
    // Use the previously calcuated numerical values for the comparison.
    if (fMacValue1 < otherInstance.fMacValue1)
    {
        return true;
    }

    if ((fMacValue1 == otherInstance.fMacValue1) &&
        (fMacValue2 < otherInstance.fMacValue2))
    {
        return true;
    }

    return false;
}


inline int BcmMacAddress::operator > (const BcmMacAddress &otherInstance) const
{
    // Use the previously calcuated numerical values for the comparison.
    if (fMacValue1 > otherInstance.fMacValue1)
    {
        return 1;
    }

    if ((fMacValue1 == otherInstance.fMacValue1) &&
        (fMacValue2 > otherInstance.fMacValue2))
    {
        return 1;
    }

    return 0;
}


inline bool BcmMacAddress::operator <= (const BcmMacAddress &otherInstance) const
{
    // Use the combination of > and == operators for this comparison.
    return ((*this == otherInstance) || (*this < otherInstance));
}


inline int BcmMacAddress::operator >= (const BcmMacAddress &otherInstance) const
{
    // Use the combination of > and == operators for this comparison.
    return ((*this == otherInstance) || (*this > otherInstance));
}


inline BcmMacAddress &BcmMacAddress::operator += (uint32 value)
{
    // Add part of the value to the lower 2 bytes.
    uint16 newValue = fMacValue2 + ((uint16) (value & 0xffff));

    // If that overflowed, then increment the upper 4 bytes.
    if (newValue < fMacValue2)
    {
        fMacValue1++;
    }

    // Now add the rest of the value to the upper 4 bytes and store the new
    // value in the lower 2 bytes.
    fMacValue1 += value >> 16;
    fMacValue2 = newValue;

    return *this;
}


inline int BcmMacAddress::IsEqualWithMask(const BcmMacAddress &otherInstance,
                                          const BcmMacAddress &mask) const
{
    uint32 macValue1;
    uint16 macValue2;

    // Apply the mask.
    macValue1 = fMacValue1 & mask.fMacValue1;
    macValue2 = fMacValue2 & mask.fMacValue2;

    // Now compare the masked values with the instance specified.
    if ((macValue1 != otherInstance.fMacValue1) ||
        (macValue2 != otherInstance.fMacValue2))
    {
        return 0;
    }

    return 1;
}


inline bool BcmMacAddress::WriteTo(BcmOctetBuffer &octetBuffer) const
{
    if (octetBuffer.NetworkAddToEnd(fMacValue1) &&
        octetBuffer.NetworkAddToEnd(fMacValue2))
    {
        return true;
    }

    return false;
}


inline bool BcmMacAddress::ReadFrom(const BcmOctetBuffer &octetBuffer, unsigned int &offset)
{
    if (octetBuffer.NetworkRead(offset, fMacValue1) &&
        octetBuffer.NetworkRead(offset, fMacValue2))
    {
        return true;
    }

    return false;
}


inline bool BcmMacAddress::IsMulticast(void) const
{
    // This is faster than an if/else.
    if (fMacValue1 & 0x01000000)
    {
        return true;
    }

    return false;
}


inline void BcmMacAddress::Set(const uint8 *pMacAddress)
{
    // Simple paranoia check.  Not needed any more.
    //if (pMacAddress != NULL)
    {
        uint32 temp = (((uint32) pMacAddress) & 0x03);

        // Copy to the integer reps, converting to host order.  Generally, MAC
        // addresses are paired in the buffer; one will be aligned on a 4-byte
        // boundary, and the other will be on a 2-byte boundary.  The first two
        // sections of code handle these optimizable cases; the last section
        // handles the general case.  In all cases, we want to avoid unaligned
        // accesses to memory.
        if (temp == 0)
        {
            fMacValue1 = HostToNetwork(*(uint32 *) pMacAddress);
            fMacValue2 = HostToNetwork(*(uint16 *) (pMacAddress + 4));
        }
        else if (temp == 0x02)
        {
            temp = HostToNetwork(*(uint16 *) (pMacAddress + 0));
            temp <<= 16;
            temp |= HostToNetwork(*(uint16 *) (pMacAddress + 2));

            fMacValue1 = temp;

            fMacValue2 = HostToNetwork(*(uint16 *) (pMacAddress + 4));
        }
        else
        {
            temp = pMacAddress[0];
            temp <<= 8;
            temp |= pMacAddress[1];
            temp <<= 8;
            temp |= pMacAddress[2];
            temp <<= 8;
            temp |= pMacAddress[3];

            fMacValue1 = temp;

            temp = pMacAddress[4];
            temp <<= 8;
            temp |= pMacAddress[5];

            fMacValue2 = (uint16) temp;
        }
    }
}


#endif


