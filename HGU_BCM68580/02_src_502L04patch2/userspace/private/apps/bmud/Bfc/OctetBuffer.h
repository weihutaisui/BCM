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
//  Filename:       OctetBuffer.h
//  Author:         David Pullen
//  Creation Date:  April 28, 1999
//
//****************************************************************************

#ifndef OCTETBUFFER_H
#define OCTETBUFFER_H

//********************** Include Files ***************************************

// For the portable types.                        
#include "typedefs.h"
#include <iostream>

// For the byte swapping functions.
#include "Utilities.h"

#include <string.h>

//********************** Global Types ****************************************

//********************** Global Constants ************************************

//********************** Global Variables ************************************

//********************** Class Declaration ***********************************


/**
*      This object provides a "safe" wrapper around a flat memory buffer, doing
*      bounds checking, etc., on the fly to keep you from going past the bounds
*      of the buffer.  It has methods that allow you to do type-safe reading
*      from and writing to various points in the buffer, and can either create
*      its own memory, or accept a pointer to existing memory provided by the
*      caller.
*/
class BcmOctetBuffer
{
public:

    /// Default Constructor.  It will create its own buffer (using operator
    /// new) of the appropriate size.
    ///
    /// \param
    ///      bufferSize - the number of bytes that should be in the buffer.
    ///
    BcmOctetBuffer(unsigned int bufferSize = 1024);

    /// Initializing constructor.  It uses the buffer specified rather than
    /// creating its own.  If ownTheBuffer is true, then this class becomes
    /// the owner of the buffer and will delete it when the object is deleted.
    /// If you don't want this object to delete the memory, then set ownTheBuffer
    /// to false.
    ///
    /// Note:  The buffer must be created with operator new if you want this
    ///        class to delete it!  If this is a buffer on the stack, then set
    ///        ownTheBuffer to false so that it won't be deleted!
    ///
    /// If you use this version of the constructor, then it is assumed that you
    /// are giving the correct buffer pointer and size.
    ///
    /// \param
    ///      pBuffer - pointer to the memory buffer to be adopted.
    /// \param
    ///      bufferSize - the number of bytes in the buffer.
    /// \param
    ///      bytesUsed - the number of valid bytes of data that are in the buffer.
    /// \param
    ///      ownTheBuffer - set this to true (default) if this class should be
    ///                     the owner of the buffer and should delete it; set it
    ///                     to false if the buffer should not be deleted.
    ///
    BcmOctetBuffer(uint8 *pBuffer, unsigned int bufferSize,
                   unsigned int bytesUsed = 0, bool ownTheBuffer = true);

    /// Copy Constructor.  Initializes the state of this object to match that
    /// of the instance passed in.  First, it will create its own memory, then
    /// it will copy the data from the other instance.
    ///
    /// \param
    ///      otherInstance - reference to the object to copy.
    ///
    BcmOctetBuffer(const BcmOctetBuffer &otherInstance);

    /// Destructor.  If the buffer was created by this class, or if it was given
    /// ownership of a buffer, then the buffer will be deleted.  If a buffer was
    /// given to this class but it was not told to own the buffer, then it just
    /// "forgets" the pointer to the buffer without deleting it.
    ///
    virtual ~BcmOctetBuffer();

    /// Assignment operator.  Copies the state of the instance passed in so that
    /// this object is "identical".  If the other instance's buffer is larger,
    /// then this class will delete the current buffer and new one of the
    /// appropriate size.  If the existing buffer is larger, then it just
    /// copies the data without shrinking the buffer.
    ///
    /// \param
    ///      otherInstance - reference to the object to copy.
    ///
    /// \return
    ///      A reference to "this" so that operator = can be chained.
    ///
    BcmOctetBuffer & operator = (const BcmOctetBuffer &otherInstance);

    /// Less than operator.
    ///
    /// \note
    ///      Some compilers (like Borland C++ v5.02) require operator<
    ///      for lists even if functions which require it are never
    ///      called.  It is used for ordering lists.
    ///
    /// \param
    ///      rhs - comparison object, a.k.a. right hand side.
    ///
    /// \retval
    ///      true if this object is less than the comparison object.
    /// \retval
    ///     false if this object is not less.
    ///
	bool operator < (const BcmOctetBuffer& rhs) const;

    /// Equality operator.
    ///
    /// \note
    ///      Some compilers (like Borland C++ v5.02) require operator==
    ///      for lists even if functions which require it are never
    ///      called.  It is used for ordering lists.
    ///
    /// \param
    ///      rhs - comparison object, a.k.a. right hand side.
    ///
    /// \retval
    ///      true if objects are equal.
    /// \retval
    ///     false if the objects are not equal.
    ///
	bool operator == (const BcmOctetBuffer& rhs) const;

    /// Inequality operator.
    inline bool operator != (const BcmOctetBuffer& rhs) const
    {
        return !(*this == rhs);
    }

public: // -------------------- Data I/O Methods -----------------------------

    /// Clears out (empties) the buffer.  The buffer is not deleted.  Note that
    /// this method may cause the buffer to be overwritten with known bytes
    /// (usually 0xcd) for debugging purposes.
    ///
    void Empty(void);

    /// Fills the octet buffer with the specified value.  This is roughly
    /// equivalent to doing a memset, except that it is also the opposite of
    /// Empty() - BytesAvailable() will return 0 after calling this method.
    ///
    /// \param
    ///      value - the value to fill the octet buffer with.
    ///
    void Fill(uint8 value);

    /// Overwrites the specified location in the buffer with the specified number
    /// of bytes.  The write must be completely within the bounds of the valid
    /// data within the buffer (can't append or write past the end of the data).
    ///
    /// This method is intended to be used only for C strings (char *), flat
    /// memory arrays (uint8 *), and other un-structured pointers to memory.
    ///
    /// \param
    ///      offset - the location (0-based) where the bytes should be written.
    /// \param
    ///      pData - pointer to the memory that is to be written.
    /// \param
    ///      numberOfBytes - the number of bytes that are to be written.
    ///
    /// \retval
    ///      true if the bytes were written.
    /// \retval
    ///      false if there wasn't enough room to write the bytes.
    ///
    bool Overwrite(unsigned int offset, const uint8 *pData, 
                   unsigned int numberOfBytes);

    //@{
    /// These are inline helper methods that perform the correct manipulations to
    /// the value-based parameter in order to call the previous implementation
    /// of Overwrite().  They are designed to be extremely efficient!
    ///
    /// Note that no byte swapping is performed on multi-byte types.
    ///
    /// If you have an array of data, then you should call the previous version
    /// of Overwrite() to give it the base address and size (this cannot be
    /// determined from the array or pointer type).
    ///
    /// \param
    ///      offset - the location (0-based) where the bytes should be written.
    /// \param
    ///      data - the data (passed by value) to be written to the buffer.
    ///
    /// \retval
    ///      true if the bytes were written.
    /// \retval
    ///      false if there wasn't enough room to write the bytes.
    ///
    inline bool Overwrite(unsigned int offset, uint8 data) 
        { return Overwrite(offset, (uint8 *) &data, sizeof(data)); }

    inline bool Overwrite(unsigned int offset, uint16 data) 
        { return Overwrite(offset, (uint8 *) &data, sizeof(data)); }

    inline bool Overwrite(unsigned int offset, uint32 data) 
        { return Overwrite(offset, (uint8 *) &data, sizeof(data)); }

    inline bool Overwrite(unsigned int offset, int8 data) 
        { return Overwrite(offset, (uint8 *) &data, sizeof(data)); }

    inline bool Overwrite(unsigned int offset, int16 data) 
        { return Overwrite(offset, (uint8 *) &data, sizeof(data)); }

    inline bool Overwrite(unsigned int offset, int32 data) 
        { return Overwrite(offset, (uint8 *) &data, sizeof(data)); }

#ifndef __WATCOMC__
    inline bool Overwrite(unsigned int offset, bool data) 
    { 
        uint8 value = 0;
    
        if (data) value = 1;

        return Overwrite(offset, &value, sizeof(value)); 
    }
#endif

    inline bool Overwrite(unsigned int offset, int data) 
        { return Overwrite(offset, (uint8 *) &data, sizeof(data)); }

    inline bool Overwrite(unsigned int offset, char data) 
        { return Overwrite(offset, (uint8 *) &data, sizeof(data)); }

    inline bool Overwrite(unsigned int offset, char *pData, bool includeTerminator = true) 
    { 
        unsigned int length;

        if (pData == NULL)
        {
            return false;
        }

        length = strlen(pData);

        if (includeTerminator)
        {
            length++;
        }

        return Overwrite(offset, (uint8 *) pData, length); 
    }

    /// As it turns out, float is a fairly consistent format with a standardized
    /// size (4 bytes); the only difference between platforms is the byte endian.
    inline bool Overwrite(unsigned int offset, float data)
        { return Overwrite(offset,  (uint8 *) &data, sizeof(data)); }
    //@}

    //@{
    /// These methods are the same as the previous ones, except that they
    /// perform byte swapping to network order before calling Overwrite().
    ///
    /// \param
    ///      data - the data (passed by value) to be written to the buffer,
    ///             after being converted to network byte order.
    ///
    /// \retval
    ///      true if the bytes were written.
    /// \retval
    ///      false if there wasn't enough room to write the bytes.
    ///
    inline bool NetworkOverwrite(unsigned int offset, uint8 data) 
    {
        // no swapping needed for byte type.
        // Now vector to the non-template Overwrite() method, giving the
        // correct address and size parameters.
        return Overwrite(offset, (uint8 *) &data, sizeof(data)); 
    }

    inline bool NetworkOverwrite(unsigned int offset, uint16 data) 
    {
        // Convert the data to network order.
        uint16 networkData = HostToNetwork(data);

        // Now vector to the non-template Overwrite() method, giving the
        // correct address and size parameters.
        return Overwrite(offset, (uint8 *) &networkData, sizeof(networkData)); 
    }

    inline bool NetworkOverwrite(unsigned int offset, uint32 data) 
    {
        // Convert the data to network order.
        uint32 networkData = HostToNetwork(data);

        // Now vector to the non-template Overwrite() method, giving the
        // correct address and size parameters.
        return Overwrite(offset, (uint8 *) &networkData, sizeof(networkData)); 
    }

    inline bool NetworkOverwrite(unsigned int offset, int8 data) 
    {
        // no swapping needed for byte type.
        // Now vector to the non-template Overwrite() method, giving the
        // correct address and size parameters.
        return Overwrite(offset, (uint8 *) &data, sizeof(data)); 
    }

    inline bool NetworkOverwrite(unsigned int offset, int16 data) 
    {
        // Convert the data to network order.
        uint16 networkData = HostToNetwork(data);

        // Now vector to the non-template Overwrite() method, giving the
        // correct address and size parameters.
        return Overwrite(offset, (uint8 *) &networkData, sizeof(networkData)); 
    }

    inline bool NetworkOverwrite(unsigned int offset, int32 data) 
    {
        // Convert the data to network order.
        uint32 networkData = HostToNetwork(data);

        // Now vector to the non-template Overwrite() method, giving the
        // correct address and size parameters.
        return Overwrite(offset, (uint8 *) &networkData, sizeof(networkData)); 
    }

#ifndef __WATCOMC__
    inline bool NetworkOverwrite(unsigned int offset, bool data) 
    {
        // No swapping needed for this type.
        // Now vector to the non-template Overwrite() method, giving the
        // correct address and size parameters.
        return Overwrite(offset, data); 
    }
#endif

    inline bool NetworkOverwrite(unsigned int offset, int data) 
    {
        // Convert the data to network order.
        uint32 networkData = HostToNetwork(data);

        // Now vector to the non-template Overwrite() method, giving the
        // correct address and size parameters.
        return Overwrite(offset, (uint8 *) &networkData, sizeof(networkData)); 
    }

    inline bool NetworkOverwrite(unsigned int offset, char data) 
    {
        // no swapping needed for byte type.
        // Now vector to the non-template Overwrite() method, giving the
        // correct address and size parameters.
        return Overwrite(offset, (uint8 *) &data, sizeof(data)); 
    }

    inline bool NetworkOverwrite(unsigned int offset, char *pData, bool includeTerminator = true) 
    {
        // no swapping needed for this type.
        return Overwrite(offset, pData, includeTerminator);
    }

    /// As it turns out, float is a fairly consistent format with a standardized
    /// size (4 bytes); the only difference between platforms is the byte endian.
    inline bool NetworkOverwrite(unsigned int offset, float data) 
    {
        // Convert the data to network order.  Slightly dangerous thing to do,
        // but the C/C++ standard requires floats to be 4 bytes.
        uint32 *pData = (uint32 *) &data;
        uint32 networkData = HostToNetwork(*pData);

        // Now vector to the non-template Overwrite() method, giving the
        // correct address and size parameters.
        return Overwrite(offset, (uint8 *) &networkData, sizeof(networkData)); 
    }
    //@}

    /// Appends the specified number of bytes to the end of the buffer.  If there
    /// isn't enough room, then none of the bytes are appended and it returns
    /// false.
    ///
    /// This method is intended to be used only for C strings (char *), flat
    /// memory arrays (uint8 *), and other un-structured pointers to memory.
    ///
    /// \param
    ///      pData - pointer to the memory that is to be appended.
    /// \param
    ///      numberOfBytes - the number of bytes that are to be appended.
    /// \param
    ///      resizeBuffer - this parameter will resize the destination buffer to 
    ///                     accept the otherInstance buffer size number of bytes if
    ///                     there is not enough room in current buffer.
    /// \retval
    ///      true if the bytes were appended.
    /// \retval
    ///      false if there wasn't enough room to append the bytes.
    ///
    bool AddToEnd(const uint8 *pData, unsigned int numberOfBytes, bool resizeBuffer=false);

    //@{
    /// These are inline helper methods that do the correct manipulations to
    /// the value-based parameter in order to call the previous implementation
    /// of AddToEnd().  They are designed to be extremely efficient!
    ///
    /// Note that no byte swapping is performed on multi-byte types.
    ///
    /// If you have an array of data, then you should call the previous version
    /// of AddToEnd() to give it the base address and size (this cannot be
    /// determined from the array or pointer type).
    ///
    /// \param
    ///      data - the data (passed by value) to be appended to the buffer.
    ///
    /// \retval
    ///      true if the bytes were appended.
    /// \retval
    ///      false if there wasn't enough room to append the bytes.
    ///
    inline bool AddToEnd(uint8 data)
        { return AddToEnd((uint8 *) &data, sizeof(data)); }

    inline bool AddToEnd(uint16 data)
        { return AddToEnd((uint8 *) &data, sizeof(data)); }

    inline bool AddToEnd(uint32 data)
        { return AddToEnd((uint8 *) &data, sizeof(data)); }

    inline bool AddToEnd(int8 data)
        { return AddToEnd((uint8 *) &data, sizeof(data)); }

    inline bool AddToEnd(int16 data)
        { return AddToEnd((uint8 *) &data, sizeof(data)); }

    inline bool AddToEnd(int32 data)
        { return AddToEnd((uint8 *) &data, sizeof(data)); }

#ifndef __WATCOMC__
    inline bool AddToEnd(bool data)
    { 
        uint8 value = 0;
        
        if (data) value = 1;
        
        return AddToEnd(&value, sizeof(value)); 
    }
#endif

    inline bool AddToEnd(int data)
        { return AddToEnd((uint8 *) &data, sizeof(data)); }

    inline bool AddToEnd(char data)
        { return AddToEnd((uint8 *) &data, sizeof(data)); }

    inline bool AddToEnd(const char *pData, bool includeTerminator = true)
    {
        unsigned int length;

        if (pData == NULL)
        {
            return false;
        }

        length = strlen(pData);

        if (includeTerminator)
        {
            length++;
        }

        return AddToEnd((const uint8 *) pData, length);
    }

    /// As it turns out, float is a fairly consistent format with a standardized
    /// size (4 bytes); the only difference between platforms is the byte endian.
    inline bool AddToEnd(float data)
        { return AddToEnd((uint8 *) &data, sizeof(data)); }
    //@}

    //@{
    /// These methods are the same as the previous ones, except that they 
    /// perform byte swapping to network order before calling AddToEnd().
    ///
    /// \param
    ///      data - the data (passed by value) to be appended to the buffer,
    ///             after being converted to network byte order.
    ///
    /// \retval
    ///      true if the bytes were appended.
    /// \retval
    ///      false if there wasn't enough room to append the bytes.
    ///
    inline bool NetworkAddToEnd(uint8 data)
    {
        // no network conversion needed for byte type.
        return AddToEnd((uint8 *) &data, sizeof(data)); 
    }

    inline bool NetworkAddToEnd(uint16 data)
    {
        // Convert the data to network order.
        uint16 networkData = HostToNetwork(data);
                
        // Now vector to the non-template AddToEnd() method, giving the
        // correct address and size parameters.
        return AddToEnd((uint8 *) &networkData, sizeof(networkData)); 
    }

    inline bool NetworkAddToEnd(uint32 data)
    {
        // Convert the data to network order.
        uint32 networkData = HostToNetwork(data);
                
        // Now vector to the non-template AddToEnd() method, giving the
        // correct address and size parameters.
        return AddToEnd((uint8 *) &networkData, sizeof(networkData)); 
    }

    inline bool NetworkAddToEnd(int8 data)
    {
        // no network conversion needed for byte type.
        return AddToEnd((uint8 *) &data, sizeof(data)); 
    }

    inline bool NetworkAddToEnd(int16 data)
    {
        // Convert the data to network order.
        uint16 networkData = HostToNetwork(data);
                
        // Now vector to the non-template AddToEnd() method, giving the
        // correct address and size parameters.
        return AddToEnd((uint8 *) &networkData, sizeof(networkData)); 
    }

    inline bool NetworkAddToEnd(int32 data)
    {
        // Convert the data to network order.
        uint32 networkData = HostToNetwork(data);
                
        // Now vector to the non-template AddToEnd() method, giving the
        // correct address and size parameters.
        return AddToEnd((uint8 *) &networkData, sizeof(networkData)); 
    }

#if defined( TARGETOS_Qnx6 )
    inline bool NetworkAddToEnd(time_t data)
    {
        // Convert the data to network order.
        uint32 networkData = HostToNetwork((uint32)data);
                
        // Now vector to the non-template AddToEnd() method, giving the
        // correct address and size parameters.
        return AddToEnd((uint8 *) &networkData, sizeof(networkData)); 
    }
#endif

#ifndef __WATCOMC__
    inline bool NetworkAddToEnd(bool data)
    {
        // no network conversion needed for this type.
        return AddToEnd(data); 
    }
#endif

    inline bool NetworkAddToEnd(int data)
    {
        // Convert the data to network order.
        uint32 networkData = HostToNetwork(data);
                
        // Now vector to the non-template AddToEnd() method, giving the
        // correct address and size parameters.
        return AddToEnd((uint8 *) &networkData, sizeof(networkData)); 
    }

    inline bool NetworkAddToEnd(char data)
    {
        // no network conversion needed for byte type.
        return AddToEnd((uint8 *) &data, sizeof(data)); 
    }

    inline bool NetworkAddToEnd(const char *pData, bool includeTerminator = true)
    {
        // no network conversion needed for this type.
        return AddToEnd(pData, includeTerminator); 
    }

    /// As it turns out, float is a fairly consistent format with a standardized
    /// size (4 bytes); the only difference between platforms is the byte endian.
    inline bool NetworkAddToEnd(float data)
    {
        // Convert the data to network order.  Slightly dangerous thing to do,
        // but the C/C++ standard requires floats to be 4 bytes.
        uint32 *pData = (uint32 *) &data;
        uint32 networkData = HostToNetwork(*pData);
                
        // Now vector to the non-template AddToEnd() method, giving the
        // correct address and size parameters.
        return AddToEnd((uint8 *) &networkData, sizeof(networkData)); 
    }
    //@}

    /// Appends the data from the specified Octet Buffer to this one.  It simply
    /// pulls out the data members from the other instance and vectors to the
    /// main AddToEnd() handler.
    ///
    /// \param
    ///      otherInstance - the source octet buffer to be appended.
    /// \param
    ///      offset - the 0-based offset into the source buffer where the copy
    ///               should begin.  If this is past the end of valid data, then
    ///               no bytes will be appended.
    /// \param
    ///      numberOfBytes - the number of bytes to copy.  If this is 0 then it
    ///                      will go to the end of the source buffer.
    /// \param
    ///      resizeBuffer - this parameter will resize the destination buffer to 
    ///                     accept the otherInstance buffer size number of bytes if
    ///                     there is not enough room in current buffer.
    ///
    /// \retval
    ///      true if the bytes were appended.
    /// \retval
    ///      false if there wasn't enough room to append the bytes.
    ///
    inline bool Append(const BcmOctetBuffer &otherInstance, 
                       unsigned int offset = 0, unsigned int numberOfBytes = 0, bool resizeBuffer=false);

    /// Reads the specified number of bytes from the buffer, storing them in
    /// the location specified.
    ///
    /// This method is intended to be used only for C strings (char *), flat
    /// memory arrays (uint8 *), and other un-structured pointers to memory.
    ///
    /// \note  The offset is modified to contain the offset for the next unread
    ///        byte.  This allows you to treat it as an automatically
    ///        incrementing iterator, reading out one value after another without
    ///        incrementing the offset yourself.
    /// \note  If the data pointer (pData) is NULL, then the read will fail (for
    ///        obvious reasons).
    /// \note  If there isn't enough data in the buffer to satisfy the read, then
    ///        the whole operation will fail, with no data consumed.  I.e. if you
    ///        try to read 20 bytes, but there are only 10, then no data will be
    ///        consumed from the buffer, your memory (pData) will not be modified,
    ///        and the offset will not be updated.
    ///
    /// \param
    ///      offset - the location (0-based) into the buffer from which the data
    ///               should be read.
    /// \param
    ///      pData - pointer to the memory that is to receive the data that is
    ///              read.
    /// \param
    ///      numberOfBytes - the number of bytes that are to be read.
    ///
    /// \retval
    ///      true if the read succeeded.
    /// \retval
    ///      false if there wasn't enough data, pData was NULL, etc.
    ///
    bool Read(unsigned int &offset, uint8 *pData, unsigned int numberOfBytes) const;

    //@{
    /// These are inline helper methods that do the correct manipulations to
    /// the reference-based parameter in order to call the previous
    /// implementations of Read().  They are designed to be extremely efficient!
    ///
    /// Note that no byte swapping is performed on multi-byte types.
    ///
    /// If you have an array of data, then you should call the previous version
    /// of Read() to give it the base address and size (this cannot be
    /// determined from the array or pointer type).
    ///
    /// \param
    ///      offset - the location (0-based) where the bytes should be read.
    /// \param
    ///      data - the variable (passed by reference) that is to receive the
    ///             data that will be read from the buffer.
    ///
    /// \retval
    ///      true if the bytes were read.
    /// \retval
    ///      false if there weren't enough bytes for the data type.
    ///
    /// Unfortunately, the compiler won't let me define this outside of this
    /// scope, so I have to place the implementation here.  Stupid compiler...
    ///
    inline bool Read(unsigned int &offset, uint8 &data) const
        { return Read(offset, (uint8 *) &data, sizeof(data)); }

    inline bool Read(unsigned int &offset, uint16 &data) const
        { return Read(offset, (uint8 *) &data, sizeof(data)); }

    inline bool Read(unsigned int &offset, uint32 &data) const
        { return Read(offset, (uint8 *) &data, sizeof(data)); }

    inline bool Read(unsigned int &offset, int8 &data) const
        { return Read(offset, (uint8 *) &data, sizeof(data)); }

    inline bool Read(unsigned int &offset, int16 &data) const
        { return Read(offset, (uint8 *) &data, sizeof(data)); }

    inline bool Read(unsigned int &offset, int32 &data) const
        { return Read(offset, (uint8 *) &data, sizeof(data)); }

#ifndef __WATCOMC__
    inline bool Read(unsigned int &offset, bool &data) const
    { 
        uint8 value;
        bool result;
        
        result = Read(offset, &value, sizeof(value));

        if (value == 1)
        {
            data = true;
        }
        else
        {
            data = false;
        }
        
        return result;
    }
#endif

    inline bool Read(unsigned int &offset, int &data) const
        { return Read(offset, (uint8 *) &data, sizeof(data)); }

    inline bool Read(unsigned int &offset, char &data) const
        { return Read(offset, (uint8 *) &data, sizeof(data)); }

    /// As it turns out, float is a fairly consistent format with a standardized
    /// size (4 bytes); the only difference between platforms is the byte endian.
    inline bool Read(unsigned int &offset, float &data) const
        { return Read(offset, (uint8 *) &data, sizeof(data)); }

    inline bool Read(unsigned int &offset, BcmOctetBuffer &data, unsigned int numberOfBytes) const
    {
        // If the number of bytes is greater than the destination buffer, then
        // bail.
        if (numberOfBytes > data.BufferSize())
        {
            return false;
        }
        
        // Try to read directly into its buffer.  If that works, then update the
        // bytes used.
        if (Read(offset, data.AccessBuffer(), numberOfBytes))
        {
            data.SetBytesUsed(numberOfBytes);

            return true;
        }

        return false;
    }
    //@}

    //@{
    /// These are the same as the previous methods, except that they perform
    /// byte swapping on the data that is read in order to convert it from
    /// network to host order.
    ///
    /// \param
    ///      offset - the location (0-based) where the bytes should be read.
    /// \param
    ///      data - the variable (passed by reference) that is to receive the
    ///             data that will be read from the buffer.
    ///
    /// \retval
    ///      true if the bytes were read.
    /// \retval
    ///      false if there weren't enough bytes for the data type.
    ///
    /// Unfortunately, the compiler won't let me define this outside of this
    /// scope, so I have to place the implementation here.  Stupid compiler...
    ///
    inline bool NetworkRead(unsigned int &offset, uint8 &data) const
    {
        if (Read(offset, (uint8 *) &data, sizeof(data)))
        {
            // read ok. no conversion for byte data.
            return true;
        }
        return false;
    }

    inline bool NetworkRead(unsigned int &offset, uint16 &data) const
    {
        uint16 networkData;
        if (Read(offset, (uint8 *) &networkData, sizeof(networkData)))
        {
            // If it worked, then convert from network byte order to host order.
            data = NetworkToHost(networkData);
            return true;
        }
        return false;
    }

    inline bool NetworkRead(unsigned int &offset, uint32 &data) const
    {
        uint32 networkData;
        if (Read(offset, (uint8 *) &networkData, sizeof(networkData)))
        {
            // If it worked, then convert from network byte order to host order.
            data = NetworkToHost(networkData);
            return true;
        }
        return false;
    }

    inline bool NetworkRead(unsigned int &offset, int8 &data) const
    {
        if (Read(offset, (uint8 *) &data, sizeof(data)))
        {
            // read ok. no conversion for byte data.
            return true;
        }
        return false;
    }

    inline bool NetworkRead(unsigned int &offset, int16 &data) const
    {
        uint16 networkData;
        if (Read(offset, (uint8 *) &networkData, sizeof(networkData)))
        {
            // If it worked, then convert from network byte order to host order.
            data = NetworkToHost(networkData);
            return true;
        }
        return false;
    }

    inline bool NetworkRead(unsigned int &offset, int32 &data) const
    {
        uint32 networkData;
        if (Read(offset, (uint8 *) &networkData, sizeof(networkData)))
        {
            // If it worked, then convert from network byte order to host order.
            data = NetworkToHost(networkData);
            return true;
        }
        return false;
    }

#if defined( TARGETOS_Qnx6 )
    inline bool NetworkRead(unsigned int &offset, time_t &data) const
    {
        uint32 networkData;
        if (Read(offset, (uint8 *) &networkData, sizeof(networkData)))
        {
            // If it worked, then convert from network byte order to host order.
            data = (time_t) NetworkToHost(networkData);
            return true;
        }
        return false;
    }
#endif

#ifndef __WATCOMC__
    inline bool NetworkRead(unsigned int &offset, bool &data) const
    {
        if (Read(offset, data))
        {
            // read ok. no conversion for byte data.
            return true;
        }
        return false;
    }
#endif

    inline bool NetworkRead(unsigned int &offset, int &data) const
    {
        uint32 networkData;
        if (Read(offset, (uint8 *) &networkData, sizeof(networkData)))
        {
            // If it worked, then convert from network byte order to host order.
            data = NetworkToHost(networkData);
            return true;
        }
        return false;
    }

    inline bool NetworkRead(unsigned int &offset, char &data) const
    {
        if (Read(offset, (uint8 *) &data, sizeof(data)))
        {
            // read ok. no conversion for byte data.
            return true;
        }
        return false;
    }

    /// As it turns out, float is a fairly consistent format with a standardized
    /// size (4 bytes); the only difference between platforms is the byte endian.
    inline bool NetworkRead(unsigned int &offset, float &data) const
    {
        uint32 networkData;
        if (Read(offset, (uint8 *) &networkData, sizeof(networkData)))
        {
            uint32 *pData = (uint32 *) &data;

            // If it worked, then convert from network byte order to host order.
            // Slightly dangerous thing to do, but the C/C++ standard requires
            // floats to be 4 bytes.
            *pData = NetworkToHost(networkData);
            return true;
        }
        return false;
    }
    //@}

public:

    /** \name BITS encoding accessors
    *
    * These methods allow clients to read and write the octet buffer as though
    * it were BITS encoded.  A BITS encoding is a rather backward SNMP concept
    * which has been adopted by other protocols (such as DOCSIS).
    * 
    * In a BITS encoding, the array of bytes is treated as a continuous
    * bitmask, where bit 0 is the MSB of the first byte, bit 7 is the LSB of
    * the first byte, bit 8 is the MSB of the second byte, etc.  For example:
    * 
    * +-----------------------+-----------------------+---------------
    * | 0| 1| 2| 3| 4| 5| 6| 7| 8| 9|10|11|12|13|14|15|16|17|18|19|20|...
    * +-----------------------+-----------------------+---------------
    * |<-----  Byte 0   ----->|<-----  Byte 1   ----->|<-----  Byte 2 ...
    * 
    * The accessors related to BITS encodings do the byte/bit-swapping math
    * for you, so that you can simply say "Is bit 9 set?" or "Set bit 23".
    * Note that BITS encodings are variable length.  An empty BITS encoding
    * has length 0. 
    *  
    * When reading a bit beyond the end of the buffer (and/or valid bytes in the 
    * buffer), a value of 0 will be returned. 
    *  
    * When setting a bit beyond the end of the valid bytes used in the buffer 
    * (but still within the actual buffer), the bytes used will be increased and the
    * newly consumed bytes will be filled with 0.  For example, if the buffer 
    * size is 256 bytes, but it is empty, setting bit 8 to 1 will cause the buffer 
    * to have 2 bytes consumed, with the MSB of the 2nd byte set to 1. 
    *  
    * Attempts to set a bit beyond the end of the buffer will be rejected (the 
    * buffer is not automatically resized).  For example, if the buffer size is 2 
    * bytes, attempting to set bit 16 (or higher) will fail.  This is consistent 
    * with other methods that add data to the end of the buffer - the bytes used in 
    * the buffer can be increased, but you can't exceed the size of the buffer. 
    * 
    * Added for PR12948.
    */
    //@{

    /// Accessor to read a bit value from the BITS encoding.  If the bit number
    /// is beyond the end of the octet buffer (and/or valid bytes used in the
    /// buffer), the value will be 0.
    /// 
    /// \param
    ///     bitNumber - the bit number to read (starting from 0).
    /// 
    /// \retval
    ///     true if the specified bit is set.
    /// \retval
    ///     false if the specified bit is not set.
    /// 
    inline bool BitsEncodingIsSet(unsigned int bitNumber) const;

    /// Accessor to set a bit value in the buffer as a BITS encoding.  If the
    /// bit number is beyond the end of the valid bytes used in the buffer, the
    /// bytes used will be extended (and 0-filled) to include this bit.
    /// 
    /// \note The octet buffer will NOT be resized; attempts to set bits beyond
    ///       the end of the buffer will fail.
    /// 
    /// \param
    ///     bitNumber - the bit number to set (starting from 0).
    /// \param
    ///     value - true to set the bit value to 1, false to set the bit value
    ///             to 0.
    /// 
    /// \retval
    ///     true if the specified bit was set to the specified value.
    /// \retval
    ///     false if the specified bit is beyond the end of the buffer.
    /// 
    inline bool BitsEncodingSetValue(unsigned int bitNumber, bool value);

    //@}

public: // -------------------- Buffer Management Methods --------------------

    /// Causes the current buffer to be resized to the specified size.  This is
    /// basically the same as creating a new BcmOctetBuffer with the size
    /// specified.  It will cause the current buffer to be deleted and a new one
    /// created; any data in the buffer will be lost!
    ///
    /// \param
    ///      bufferSize - the number of bytes in the buffer.
    ///
    void ResizeBuffer(unsigned int bufferSize);

    /// Causes the current buffer to be deleted (if this class is the owner) and
    /// replaced with the one specified.
    ///
    /// \param
    ///      pBuffer - pointer to the memory buffer to be adopted.
    /// \param
    ///      bufferSize - the number of bytes in the buffer.
    /// \param
    ///      bytesUsed - the number of valid bytes of data that are in the buffer.
    /// \param
    ///      ownTheBuffer - set this to true (default) if this class should be
    ///                     the owner of the buffer and should delete it; set it
    ///                     to false if the buffer should not be deleted.
    ///
    void AdoptBuffer(uint8 *pBuffer, unsigned int bufferSize,
                     unsigned int bytesUsed = 0, bool ownTheBuffer = true);

    /// This is similar to AdoptBuffer(), except that it allows you to set this
    /// buffer as a window into the buffer of another octet buffer.  The window
    /// is defined as an offset from the beginning of the other buffer, going for
    /// the specified number of bytes.
    ///
    /// This is meant to be used to define a start and end point for reading data
    /// out of the other buffer.  Do not use this buffer to append data, since
    /// the other buffer's state is not updated.
    ///
    /// Note that this method clamps the input values so that you cannot define
    /// a window outside of the bounds of the other buffer.  Also, this buffer
    /// will have the correct number of valid bytes of data from the other
    /// buffer's window.
    ///
    /// \param
    ///      otherInstance - the octet buffer into which this buffer should be
    ///                      windowed.
    /// \param
    ///      offset - the offset into the buffer for the first byte.
    /// \param
    ///      numberOfBytes - the number of bytes of the other buffer that this
    ///                      one should overlay (0=all).
    ///
    void Overlay(const BcmOctetBuffer &otherInstance, unsigned int offset = 0,
                 unsigned int numberOfBytes = 0);

    /// Causes the buffer to be released to the caller so that it will no longer
    /// be referenced by (and will not be deleted by) the OctetBuffer.  The
    /// caller becomes the owner of the memory and must delete it (if
    /// appropriate).
    ///
    /// The intent of this method is to get back a buffer that was previously
    /// given to this class via AdoptBuffer().
    ///
    /// \return
    ///      The pointer to the base address of the buffer that was previously
    ///      being used.
    ///
    uint8 *ReleaseBuffer(void);

    /// Allows the buffer pointer to be accessed; this should only be used in
    /// cases (like C RTL functions, IP stack functions, etc.) where a void *
    /// or char * are required as parameters.
    ///
    /// \return
    ///      The pointer to the base address of the buffer that is currently
    ///      being used.
    ///
    inline uint8 *AccessBuffer(void) const;

    /// Returns the total size of the buffer.
    ///
    /// \return
    ///      The total size of the buffer.
    ///
    inline unsigned int BufferSize(void) const;

    /// Returns the current number of bytes of data that are in the buffer.
    ///
    /// \return
    ///      The number of bytes of data in the buffer.
    ///
    inline unsigned int BytesUsed(void) const;

    /// Allows the number of bytes used to be set to the value specified.  This
    /// is necessary when some external code has written data to the buffer
    /// without using the methods of this class.
    ///
    /// An example of this is reading bytes from a network socket.  The driver
    /// will store the bytes in the buffer, but the client code needs to tell the
    /// octet buffer how many bytes were read.
    ///
    /// \param
    ///      bytesUsed - the number of bytes that are used.
    ///
    /// \retval
    ///      true if the bytes used was set correctly.
    /// \retval
    ///      false if there was a problem (buffer too small, etc.).
    ///
    inline bool SetBytesUsed(unsigned int bytesUsed);

    /// Allows the specified number of bytes at the end of buffer to become used.
    /// This is necessary when some external code has appended data without using
    /// the methods of this class.
    ///
    /// An example of this is TFTPing a file from a server.  Each successive TFTP
    /// will write bytes to the end of the buffer, and the client code must tell
    /// the buffer how many bytes were consumed.
    ///
    /// \param
    ///      bytesConsumed - the number of additional bytes that were added to
    ///                      end of the buffer.
    ///
    /// \retval
    ///      true if the bytes were consumed.
    /// \retval
    ///      false if the bytes were not consumed (not enough bytes, etc.).
    ///
    inline bool ConsumeBytes(unsigned int bytesConsumed);

    /// Returns the number of bytes of data that can be added to the buffer.
    ///
    /// \return
    ///      The number of bytes of data that can be added to the buffer.
    ///
    inline unsigned int BytesAvailable(void) const;

    /// This method dumps the contents of the buffer to the specified ostream
    /// in ASCII-encoded hex format.  Obviously, this will be rather slow, so
    /// you don't want to do it except for debugging purposes.
    ///
    /// \param
    ///      outstream - the ostream to which the data should be written.
    /// \param
    ///      showHeader - if true, displays a brief message about the portion of
    ///                   the buffer that is being printed.
    /// \param
    ///      offset - the 0-based offset into the buffer where the dump should
    ///               begin.  If this is past the end of valid data, then no
    ///               bytes will be dumped.
    /// \param
    ///      numberOfBytes - the number of bytes to dump.  If this is 0 then it
    ///                      will go to the end of the buffer.
    /// \param
    ///      showAddressLines - if true, this will display the address lines
    ///                         when the buffer is being printed.
    ///
    /// \return
    ///      A reference to the ostream to support chaining.
    ///
    ostream &Print(ostream &outstream, bool showHeader = false, 
                   unsigned int offset = 0, unsigned int numberOfBytes = 0,
                   bool showAddressLines = false ) const;

    /// This method converts the data stored in the octet buffer to the ASCII-hex
    /// representation, placing it in the char buffer passed in.  Unlike the
    /// Print method above, there is no formatting done on the data (like adding
    /// spaces, byte swapping, etc).
    ///
    /// The ASCII representation will be \0 terminated, like normal strings.
    ///
    /// In order to store the whole string, you must provide a buffer that is
    /// just over twice as long as the amount of data:
    ///
    ///    asciiLength = (buffer.BytesUsed() * 2) + 1
    ///
    /// This is because each byte of data will be converted into 2 ASCII chars,
    /// then a \0 is added to the end.  If your buffer is too short, then the
    /// string will be truncated at the nearest data byte boundary (you won't
    /// have half of a byte of string present).
    ///
    /// Example:
    ///      The value 1522 (0x5f2) is stored as a uint32 to the octet buffer
    ///      in network order, producing 4 bytes of data.
    ///
    ///      Calling this method produces the 9 character string "000005f2"; note
    ///      that there is an implicit \0 at the end of the string.
    ///
    /// \param
    ///      pString - pointer to a char string where the ASCII representation is
    ///                to be placed.
    /// \param
    ///      maxLength - the size of the string that was allocated, i.e. the max
    ///                  number of chars (including \0 terminator) that can be
    ///                  stored.
    /// \param
    ///      useUpperCase - true if you want hex letters to be uppercase (e.g.
    ///                     0xFF); false (default) if you want lowercase (0xff).
    ///
    /// \return
    ///      The number of chars that were stored, excluding the \0.  This is
    ///      equivalent to doing strlen(pString).
    ///
    unsigned int ConvertToAsciiHex(char *pString, unsigned int maxLength,
                                   bool useUpperCase = false) const;


    /// This method will append a NULL to a buffer that is not already
    /// null terminated. If the buffer is already null terminate, the 
    /// buffer will not be altered.
    ///
    inline bool NullTerminate(void);

private:

    /// Does common initialization for multiple constructors.
    ///
    /// \param
    ///      bufferSize - if greater than 0, the number of bytes to allocate.
    ///
    void Initialize(unsigned int bufferSize);

    /// Called from several methods; checks to see if we are the owner of the
    /// buffer, and if so, then it deletes the buffer.
    ///
    void DeleteBuffer(void);

private:

    /// The actual buffer pointer.
    uint8 *pfBuffer;

    /// Tells whether or not I own (and should delete) the buffer.
    bool fBufferIsMine;

    /// The maximum number of bytes that can be stored in the buffer.
    unsigned int fBufferSize;

    /// The number of bytes currently in the buffer (added via AddToEnd()).
    unsigned int fUsedBytes;

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


//********************** Inline Method Implementations ***********************


/// Allows you to do the following:
///
///  cout << buffer << endl;
///
inline ostream & operator << (ostream &outstream, const BcmOctetBuffer &buffer)
{
    // By default, do the whole buffer, and show the header info.
    return buffer.Print(outstream);
}

/// Allows you to print with address lines
///
///  cout <<= buffer << endl;
///
inline ostream & operator <<= (ostream &outstream, const BcmOctetBuffer &buffer)
{
    // By default, do the whole buffer, and show the header info.
    return buffer.Print(outstream, false, 0, 0, true);
}

inline bool BcmOctetBuffer::Append(const BcmOctetBuffer &otherInstance,
                                   unsigned int offset, 
                                   unsigned int numberOfBytes, bool resizeBuffer)
{
    // If numberOfBytes is 0, then go to the end of the source buffer.
    if (numberOfBytes == 0)
    {
        numberOfBytes = otherInstance.BytesUsed() - offset;
    }

    // If the offset plus the number of bytes goes past the end of the buffer,
    // then this is an error.
    if (offset + numberOfBytes > otherInstance.BytesUsed())
    {
        return false;
    }

    // Optimization - if 0 bytes, then just return true.
    if (numberOfBytes == 0)
    {
        return true;
    }

    return AddToEnd(otherInstance.pfBuffer + offset, numberOfBytes,resizeBuffer);
}


inline bool BcmOctetBuffer::BitsEncodingIsSet(unsigned int bitNumber) const
{
    bool bitIsSet = false;

    // The byte offset is simply the bit number divided by 8 bits per byte.
    // i.e. bits 0-7 map to byte offset 0, bits 8-15 map to byte offset 1, etc.
    unsigned int byteOffset = bitNumber / 8;

    // The bits counter from left (MSB) to right (LSB), so I need to reverse the
    // offset.  i.e. bit 0 is actually bit offset 7 in the byte, and bit 7 is
    // actually bit offset 0 in the byte.
    unsigned int bitOffset = 7 - (bitNumber & 0x07);

    if (byteOffset < fUsedBytes)
    {
        if ((pfBuffer[byteOffset] & (1 << bitOffset)) != 0)
        {
            bitIsSet = true;
        }
    }

    return bitIsSet;
}


inline bool BcmOctetBuffer::BitsEncodingSetValue(unsigned int bitNumber,
                                                 bool value)
{
    // The byte offset is simply the bit number divided by 8 bits per byte.
    // i.e. bits 0-7 map to byte offset 0, bits 8-15 map to byte offset 1, etc.
    unsigned int byteOffset = bitNumber / 8;

    // The bits counter from left (MSB) to right (LSB), so I need to reverse the
    // offset.  i.e. bit 0 is actually bit offset 7 in the byte, and bit 7 is
    // actually bit offset 0 in the byte.
    unsigned int bitOffset = 7 - (bitNumber & 0x07);

    if (byteOffset >= fUsedBytes)
    {
        if (byteOffset >= fBufferSize)
        {
            // Beyond the end of the buffer.
            return false;
        }

        // Fill the new bytes with 0s.  I need to adjust the number of bytes
        // by 1 since byteOffset is 0-based, but fUsedBytes is a count
        // (1-based).
        memset(pfBuffer + fUsedBytes, 0, byteOffset - fUsedBytes + 1);

        // Show these bytes as having been used.  Again, I need to adjust the
        // length by 1.
        fUsedBytes = byteOffset + 1;
    }

    // Now set the bit to 1 or 0 depending on the value passed in.
    if (value == true)
    {
        pfBuffer[byteOffset] |= (1 << bitOffset);
    }
    else
    {
        pfBuffer[byteOffset] &= ~(1 << bitOffset);
    }

    return true;
}


inline uint8 *BcmOctetBuffer::AccessBuffer(void) const
{
    return pfBuffer;
}


inline unsigned int BcmOctetBuffer::BufferSize(void) const
{
    return fBufferSize;
}


inline unsigned int BcmOctetBuffer::BytesUsed(void) const
{
    return fUsedBytes;
}


inline bool BcmOctetBuffer::SetBytesUsed(unsigned int bytesUsed)
{
    if (bytesUsed <= fBufferSize)
    {
        fUsedBytes = bytesUsed;

        return true;
    }
    else
    {
        fUsedBytes = fBufferSize;

        return false;
    }
}


inline bool BcmOctetBuffer::ConsumeBytes(unsigned int bytesConsumed)
{
    if (fUsedBytes + bytesConsumed <= fBufferSize)
    {
        fUsedBytes += bytesConsumed;

        return true;
    }
    else
    {
        fUsedBytes = fBufferSize;

        return false;
    }
}


inline unsigned int BcmOctetBuffer::BytesAvailable(void) const
{
    return fBufferSize - fUsedBytes;
}


inline bool BcmOctetBuffer::NullTerminate(void)
{
    if (pfBuffer != NULL)
    {
        if(fUsedBytes)
        {
        char* pCurrentBuffer = (char *)(pfBuffer + (fUsedBytes - 1));

        if ( (int)(*pCurrentBuffer) != 0 )
        {
            return AddToEnd((uint8)0);
        }
    }
        // trw - empty buffer?
        else
        {
                return AddToEnd((uint8)0);
        }
    }
    return true;
}

#endif


