//****************************************************************************
//
//  Copyright (c) 1999-2003  Broadcom Corporation
//  All Rights Reserved
//  No portions of this material may be reproduced in any form without the
//  written permission of:
//          Broadcom Corporation
//          16215 Alton Parkway
//          Irvine, California 92618
//  All information contained in this document is Broadcom Corporation
//  company private, proprietary, and trade secret.
//
//****************************************************************************
//  $Id$
//
//  Filename:       IpAddress.h
//  Author:         Cliff Danielson
//  Creation Date:  August 3, 1999
//
//****************************************************************************

#ifndef IPV4ADDRESS_H
#define IPV4ADDRESS_H

//********************** Include Files ***************************************

#include "typedefs.h"
#include "OctetBuffer.h"

#include <iostream>

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
class BcmIpV4Address
{
public:

    enum
    {
        ADDR_LEN = 4    ///< length of IP address in bytes
    };

    /// Default constructor.
    ///
    BcmIpV4Address()
    {
        reset();
    }

    /// Initializing constructor.  Reads from the string specified to
    /// initialize the object.
    ///
    /// \param
    ///     pBuf - pointer to a buffer with the 16 bytes to use.
    ///
    BcmIpV4Address(const char *pString);

    /// Copy constructor.
    ///
    /// \param
    ///     rhs - the instance to be copied.
    ///
    BcmIpV4Address( const BcmIpV4Address& rhs )
    {
        fU32Rep = rhs.fU32Rep;
    }

    /// Initializing constructor.  Uses the values specified to initialize
    /// the object.
    ///
    /// \param
    ///     b0 - the first byte of the IP address.
    /// \param
    ///     b1 - the second byte of the IP address.
    /// \param
    ///     b2 - the third byte of the IP address.
    /// \param
    ///     b3 - the fourth byte of the IP address.
    ///
	BcmIpV4Address( byte b0, byte b1, byte b2, byte b3 );

    /// Initializing constructor.  Reads 4 bytes from the buffer specified to
    /// initialize the object.
    ///
    /// \param
    ///     buf - pointer to a buffer with the 4 bytes to use.
    ///
	BcmIpV4Address( const byte* buf );

    /// Destructor.
    ///
    ~BcmIpV4Address();

    /// Assignment operator.
    ///
    /// \param
    ///     rhs - the instance to be copied.
    ///
    /// \return
    ///     A reference to this object.
    ///
    BcmIpV4Address& operator=( const BcmIpV4Address& rhs )
    {
        if( this == &rhs )
        {
            // special case: assignment to self
            return *this;
        }

        fU32Rep = rhs.fU32Rep;
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
    bool operator<( const BcmIpV4Address& rhs ) const
    {
        bool ret_val = false;

        // use the previously calcuated numerical values for the comparison.
        if( fU32Rep < rhs.fU32Rep )
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
    bool operator<=( const BcmIpV4Address& rhs ) const
    {
        bool ret_val = false;

        // use the previously calcuated numerical values for the comparison.
        if( fU32Rep <= rhs.fU32Rep )
        {
            ret_val = true;
        }
        return ret_val;
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
    bool operator==( const BcmIpV4Address& rhs ) const
    {
        bool ret_val = true;
        if( fU32Rep != rhs.fU32Rep )
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
    bool operator!=( const BcmIpV4Address& rhs ) const
    {
        bool ret_val = true;
        if( fU32Rep == rhs.fU32Rep )
        {    
            ret_val = false;
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
    BcmIpV4Address &operator&= (const BcmIpV4Address &rhs)
    {
        // Apply the mask to all of the fields.
        fU32Rep &= rhs.fU32Rep;

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
    BcmIpV4Address &operator|= (const BcmIpV4Address &rhs)
    {
        // Apply the mask to all of the fields.
        fU32Rep |= rhs.fU32Rep;

        return *this;
    }
    
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
    unsigned char operator[] (int Index) const
    {
      if ((Index < 0) || (Index > 3))
        return 0;
        
      return (unsigned char) ((fU32Rep >> (8 * (3 - Index))) & 0xff);
    }
    
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
    inline BcmIpV4Address &operator += (const BcmIpV4Address &value);
    inline BcmIpV4Address &operator += (uint32 value);
    inline BcmIpV4Address &operator ++ ();
    inline BcmIpV4Address &operator ++ (int);
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
    inline BcmIpV4Address &operator -= (const BcmIpV4Address &value);
    inline BcmIpV4Address &operator -= (uint32 value);
    inline BcmIpV4Address &operator -- ();
    inline BcmIpV4Address &operator -- (int);
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
    inline int IsEqualWithMask(const BcmIpV4Address &otherInstance,
                               const BcmIpV4Address &mask) const
    {
        uint32 value;

        // Calculate the masked value.
        value = fU32Rep & mask.fU32Rep;

        // Now compare.
        if (value == otherInstance.fU32Rep)
        {
            return 1;
        }

        return 0;
    }

    /// Reset data members to default values.
    ///
	void reset()
    {
        fU32Rep = 0;
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
    bool WriteTo(BcmOctetBuffer &octetBuffer) const
    { 
        return octetBuffer.NetworkAddToEnd( fU32Rep ); 
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
    bool ReadFrom(const BcmOctetBuffer &octetBuffer, unsigned int &offset)
    {
        return octetBuffer.NetworkRead(offset, fU32Rep);
    }

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

    /// Allows the 4 bytes of the IP address to be set via an array of bytes,
    /// which is most often how they are stored in packets.  The array is assumed
    /// to be in network byte order, and will be converted to host endian order.
    ///
    /// \param
    ///      pBytes - pointer to the array of 4 bytes containing the IP address.
    ///
    inline void Set(const uint8 *pBytes);

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

    /// Allows the IP address to be set as a uint32 (all 4 bytes packed together).
    /// Note that the bytes are in the native processor (host) order.
    ///
    /// \param
    ///      ipAddress - the uint32 from with the IP address should be stored.
    ///  
    inline void Set(uint32 ipAddress);

    /// Same as above, but it converts the IP address from network order to host
    /// order first.
    ///
    /// \param
    ///      ipAddress - the uint32 from with the IP address should be stored.
    ///  
    inline void NetworkSet(uint32 ipAddress);

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
    ///      bufferSize - size of the buffer to write to
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem (NULL string).
    ///
    bool Get(char *pString, unsigned int bufferSize ) const;

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
    inline void Get(uint32 &ipAddress) const;
	inline uint32 Get( void ) const;
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
    inline void NetworkGet(uint32 &ipAddress) const;
    inline uint32 NetworkGet() const;
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
    inline bool IsClassA(void) const;
    inline bool IsClassB(void) const;
    inline bool IsClassC(void) const;
    //@}

    /// Returns the default subnet mask that should be used for the currently
    /// stored IP address, per RFC 791.
    ///
    /// \return
    ///      The default subnet mask for the IP address class.
    ///
    inline BcmIpV4Address DefaultSubnetMask(void) const;

protected:

    /// Represent IP address as uint32.  Useful for optimizing comparison
    /// operations.
    uint32 fU32Rep;

};


//********************** Global instances ************************************

/// Special global object who's value is "0.0.0.0".  This is useful
/// for comparisons, and eliminates the need to create a temporary one.
extern const BcmIpV4Address kIpV4AllZerosIp;

/// Special global object who's value is "255.255.255.255".  This is useful
/// for comparisons, and eliminates the need to create a temporary one.
extern const BcmIpV4Address kIpV4BroadcastIp;

/// Special global object who's value is "127.0.0.1".  This is useful
/// for comparisons, and eliminates the need to create a temporary one.
extern const BcmIpV4Address kIpV4SoftwareLoopback;

//********************** Inline Method Implementations ***********************


/// Allows you to do the following:
///
///  cout << ipAddress << endl;
///
inline ostream& operator<<( ostream& ostrm, const BcmIpV4Address& rhs )
{
    return rhs.Print( ostrm );
}


inline BcmIpV4Address &BcmIpV4Address::operator += (const BcmIpV4Address &value)
{
    uint32 asUint32;

    value.Get(asUint32);

    fU32Rep += asUint32;

    return *this;
}

inline BcmIpV4Address &BcmIpV4Address::operator += (uint32 value)
{
    fU32Rep += value;

    return *this;
}

inline BcmIpV4Address &BcmIpV4Address::operator ++ ()
{
    fU32Rep++;

    return *this;
}

inline BcmIpV4Address &BcmIpV4Address::operator ++ (int)
{
    fU32Rep++;

    return *this;
}

inline BcmIpV4Address &BcmIpV4Address::operator -= (const BcmIpV4Address &value)
{
    uint32 asUint32;

    value.Get(asUint32);

    fU32Rep -= asUint32;

    return *this;
}

inline BcmIpV4Address &BcmIpV4Address::operator -= (uint32 value)
{
    fU32Rep -= value;

    return *this;
}

inline BcmIpV4Address &BcmIpV4Address::operator -- ()
{
    fU32Rep--;

    return *this;
}

inline BcmIpV4Address &BcmIpV4Address::operator -- (int)
{
    fU32Rep--;

    return *this;
}


inline void BcmIpV4Address::Set(uint32 ipAddress)
{
    // Store the uint32 representation.
    fU32Rep = ipAddress;
}


inline void BcmIpV4Address::Set(const uint8 *pBytes)
{
    // Simple paranoia check.  Not needed any more.
    //if (pBytes != NULL)
    {
        uint32 alignment = (((uint32) pBytes) & 0x03);

        // Check to see if pBytes is on a word boundary; usually, the IP
        // addresses are only aligned on 2-byte boundaries, so we have optimized
        // code for this case.
        if (alignment == 0x02)
        {
            fU32Rep = HostToNetwork(*(uint16 *) pBytes);
            fU32Rep <<= 16;
            fU32Rep |= HostToNetwork(*(uint16 *) (pBytes + 2));
        }
        else if (alignment == 0)
        {
            fU32Rep = HostToNetwork(*(uint32 *) pBytes);
        }
        else
        {
            fU32Rep = pBytes[0];
            fU32Rep <<= 8;
            fU32Rep |= pBytes[1];
            fU32Rep <<= 8;
            fU32Rep |= pBytes[2];
            fU32Rep <<= 8;
            fU32Rep |= pBytes[3];
        }
    }
}


inline void BcmIpV4Address::NetworkSet(uint32 ipAddress)
{
    // Just swap the byte order and let the other version do the work.
    Set(HostToNetwork(ipAddress));
}


inline void BcmIpV4Address::Get(uint32 &ipAddress) const
{
    ipAddress = fU32Rep;
}

inline uint32 BcmIpV4Address::Get( void ) const
{
    return fU32Rep;
}

inline void BcmIpV4Address::NetworkGet(uint32 &ipAddress) const
{
    ipAddress = HostToNetwork(fU32Rep);
}


inline uint32 BcmIpV4Address::NetworkGet( void ) const
{
    return( HostToNetwork(fU32Rep) );
}



inline bool BcmIpV4Address::IsClassA(void) const
{
    // Here's the relevant text from RFC 791:
    /*
    Addresses are fixed length of four octets (32 bits).  An address
    begins with a network number, followed by local address (called the
    "rest" field).  There are three formats or classes of internet
    addresses:  in class a, the high order bit is zero, the next 7 bits
    are the network, and the last 24 bits are the local address; in
    class b, the high order two bits are one-zero, the next 14 bits are
    the network and the last 16 bits are the local address; in class c,
    the high order three bits are one-one-zero, the next 21 bits are the
    network and the last 8 bits are the local address.
    */

    // Class A addresses have the most significant bit cleared (0).
    if ((fU32Rep & 0x80000000) == 0)
    {
        return true;
    }

    return false;
}

inline bool BcmIpV4Address::IsClassB(void) const
{
    // Class B addresses have the 2 most significant bits set to 10
    if ((fU32Rep & 0xc0000000) == 0x80000000)
    {
        return true;
    }

    return false;
}

inline bool BcmIpV4Address::IsClassC(void) const
{
    // Class C addresses have the 3 most significant bits set to 110.
    if ((fU32Rep & 0xe0000000) == 0xc0000000)
    {
        return true;
    }

    return false;
}


inline BcmIpV4Address BcmIpV4Address::DefaultSubnetMask(void) const
{
    // Return the correct default subnet mask for the current IP address class.
    // The default subnet has 1's for all bits except the "local address"
    // portion.
    if (IsClassA())
    {
        // For Class A, the mask has 0's for the last 24 bits.
        return BcmIpV4Address(255, 0, 0, 0);
    }

    if (IsClassB())
    {
        // For Class B, the mask has 0's for the last 16 bits.
        return BcmIpV4Address(255, 255, 0, 0);
    }

    // Otherwise, it's Class C, where the mask has 0's for the last 8 bits
    return BcmIpV4Address(255, 255, 255, 0);
}


#endif

