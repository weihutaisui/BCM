//****************************************************************************
//
//  Copyright (c) 1999-2007  Broadcom Corporation
//  All Rights Reserved
//  No portions of this material may be reproduced in any form without the
//  written permission of:
//          Broadcom Corporation
//          16251 Laguna Canyon Road
//          Irvine, California 92618
//  All information contained in this document is Broadcom Corporation
//  company private, proprietary, and trade secret.
//
//****************************************************************************
//  $Id$
//
//  Filename:       OctetBuffer.cpp
//  Author:         David Pullen
//  Creation Date:  April 29, 1999
//
//****************************************************************************
//  Description:
//      This object provides a "safe" wrapper around a flat memory buffer, doing
//      bounds checking, etc., on the fly to keep you from going past the bounds
//      of the buffer.  It has methods that allow you to do type-safe reading
//      from and writing to various points in the buffer, and can either create
//      its own memory, or accept a pointer to existing memory provided by the
//      caller.
//
//****************************************************************************

// Set this to 1 to enable the regression test code.
#define REGRESSION_TEST 0

//********************** Include Files ***************************************

// My api and definitions...
#include "OctetBuffer.h"

#include "MessageLog.h"

// For memset() and other memory functions, etc.
#include <string.h>
#include <stdlib.h>

#include <iomanip>

// For isprint().
#include <ctype.h>

//********************** Local Types *****************************************

//********************** Local Constants *************************************

//********************** Local Variables *************************************

//********************** Class Method Implementations ************************


// Default Constructor.  It will create its own buffer (using operator
// new) of the appropriate size.
//
// Parameters:
//      bufferSize - the number of bytes that should be in the buffer.
//
// Returns:  N/A
//
BcmOctetBuffer::BcmOctetBuffer(unsigned int bufferSize)
{
    // Just call the common initialization method.
    Initialize(bufferSize);
}


// Initializing constructor.  It uses the buffer specified rather than
// creating its own.  If ownTheBuffer is true, then this class becomes
// the owner of the buffer and will delete it when the object is deleted.
// If you don't want this object to delete the memory, then set ownTheBuffer
// to false.
//
// Note:  The buffer must be created with operator new if you want this
//        class to delete it!  If this is a buffer on the stack, then set
//        ownTheBuffer to false so that it won't be deleted!
//
// If you use this version of the constructor, then it is assumed that you
// are giving the correct buffer pointer and size.
//
// Parameters:
//      pBuffer - pointer to the memory buffer to be adopted.
//      bufferSize - the number of bytes in the buffer.
//      bytesUsed - the number of valid bytes of data that are in the buffer.
//      ownTheBuffer - set this to true (default) if this class should be
//                     the owner of the buffer and should delete it; set it
//                     to false if the buffer should not be deleted.
//
// Returns:  N/A
//
BcmOctetBuffer::BcmOctetBuffer(uint8 *pBuffer, unsigned int bufferSize,
                               unsigned int bytesUsed, bool ownTheBuffer)
{
    // First, call the common initialization method, with size of 0 so that
    // it won't allocate a buffer.
    Initialize(0);

    // Now call my buffer adoption method which will store the information.
    AdoptBuffer(pBuffer, bufferSize, bytesUsed, ownTheBuffer);
}


// Copy Constructor.  Initializes the state of this object to match that
// of the instance passed in.  First, it will create its own memory, then
// it will copy the data from the other instance.
//
// Parameters:
//      otherInstance - reference to the object to copy.
//
// Returns:  N/A
//
BcmOctetBuffer::BcmOctetBuffer(const BcmOctetBuffer &otherInstance)
{
    // First, call the common initialization method, with size of 0 so that
    // it won't allocate a buffer.
    Initialize(0);

    // Now call the assignment operator to do the work.
    *this = otherInstance;
}


// Destructor.  If the buffer was created by this class, or if it was given
// ownership of a buffer, then the buffer will be deleted.  If a buffer was
// given to this class but it was not told to own the buffer, then it just
// "forgets" the pointer to the buffer without deleting it.
//
// Parameters:  N/A
//
// Returns:  N/A
//
BcmOctetBuffer::~BcmOctetBuffer()
{
    // Call the common cleanup method.
    DeleteBuffer();
}


// Assignment operator.  Copies the state of the instance passed in so that
// this object is "identical".  If the other instance's buffer is larger,
// then this class will delete the current buffer and new one of the
// appropriate size.  If the existing buffer is larger, then it just
// copies the data without shrinking the buffer.
//
// Parameters:
//      otherInstance - reference to the object to copy.
//
// Returns:
//      A reference to "this" so that operator = can be chained.
//
BcmOctetBuffer & BcmOctetBuffer::operator = (const BcmOctetBuffer &otherInstance)
{
    // If I need a larger buffer, then get rid of mine and create a new one.
    if (otherInstance.fBufferSize > fBufferSize)
    {
        // Let helper methods do the work.
        DeleteBuffer();
        Initialize(otherInstance.fBufferSize);
    }

    if (pfBuffer != NULL)
    {
        // Copy the data from the other buffer.
        memcpy(pfBuffer, otherInstance.pfBuffer, otherInstance.fUsedBytes);
    }
    fUsedBytes = otherInstance.fUsedBytes;

    return *this;
}


// Less than operator.
//
// Parameters:
//      rhs - comparison object, a.k.a. right hand side.
//
// Returns:
//      bool - true if this object is less than the comparison object.
//
// Note:
//      Some compilers (like Borland C++ v5.02) require operator<
//      for lists even if functions which require it are never
//      called.  It is used for ordering lists.
//
bool BcmOctetBuffer::operator < (const BcmOctetBuffer& rhs) const
{
    // Use a form of memcmp to test this.  Mostly, we gauge the contents of
    // the buffers, but if equal, then we use the shortest buffer.
    int result;
    unsigned int shorterLength = fUsedBytes;

    if (rhs.BytesUsed() < shorterLength)
    {
        shorterLength = rhs.BytesUsed();
    }

    // Compare up to the end of the shortest buffer.
    result = memcmp(pfBuffer, rhs.AccessBuffer(), shorterLength);

    // If my values are less, then return true.
    if (result < 0)
    {
        return true;
    }

    // If the values are the same but my buffer is shorter, then return
    // true.
    else if ((result == 0) && (fUsedBytes < rhs.BytesUsed()))
    {
        return true;
    }

    return false;
}


// Equality operator.
//
// Parameters:
//      rhs - comparison object, a.k.a. right hand side.
//
// Returns:
//      bool - true if objects are equal, else false.
//
// Note:
//      Some compilers (like Borland C++ v5.02) require operator==
//      for lists even if functions which require it are never
//      called.  
//
bool BcmOctetBuffer::operator == (const BcmOctetBuffer& rhs) const
{
    // If some data members are equal, and the contents of the buffers
    // are equal, then t hey are equal.  If the used bytes are not the
    // same, then they can't be equal.
    if ((fUsedBytes == rhs.BytesUsed()) &&
        (memcmp(pfBuffer, rhs.AccessBuffer(), fUsedBytes) == 0))
    {
        return true;
    }

    return false;
}


// Clears out (empties) the buffer.  The buffer is not deleted.  Note that
// this method may cause the buffer to be overwritten with known bytes
// (usually 0xcd) for debugging purposes.
//
// Parameters:  None.
//
// Returns:  Nothing.
//
void BcmOctetBuffer::Empty(void)
{
    // For debugging, set the entire buffer to an unusual value.
    //Fill(0xcd);

    // Just reset the index so that there are no bytes used.
    fUsedBytes = 0;
}


// Fills the octet buffer with the specified value.  This is roughly
// equivalent to doing a memset, except that it is also the opposite of
// Empty() - BytesAvailable() will return 0 after calling this method.
//
// Parameters:
//      value - the value to fill the octet buffer with.
//
// Returns:  Nothing.
//
void BcmOctetBuffer::Fill(uint8 value)
{
    if (pfBuffer != NULL)
    {
        memset(pfBuffer, value, fBufferSize);

        fUsedBytes = fBufferSize;
    }
}


// Overwrites the specified location in the buffer with the specified number
// of bytes.  The write must be completely within the bounds of the valid
// data within the buffer (can't append or write past the end of the data).
//
// This method is intended to be used only for C strings (char *), flat
// memory arrays (uint8 *), and other un-structured pointers to memory.
//
// Note:  If you are trying to write a value from a portable type (uint8,
//        uint32, etc.) or a packed structure, then you should call one the
//        template versions of this method (below).  They are type-safe and
//        do quite a lot of work for you.
//
// Parameters:
//      offset - the location (0-based) where the bytes should be written.
//      pData - pointer to the memory that is to be written.
//      numberOfBytes - the number of bytes that are to be written.
//
// Returns:
//      true if the bytes were written.
//      false if there wasn't enough room to write the bytes.
//
bool BcmOctetBuffer::Overwrite(unsigned int offset, const uint8 *pData, 
                               unsigned int numberOfBytes)
{
    // Make sure we have room for the data.
    if ((pData == NULL) || (offset + numberOfBytes > fUsedBytes) ||
        (pfBuffer == NULL))
    {
        return false;
    }

    if (numberOfBytes > 0)
    {
        // Copy the data.
        memcpy(pfBuffer + offset, pData, numberOfBytes);
    }

    return true;
}


// Appends the specified number of bytes to the end of the buffer.  If there
// isn't enough room, then none of the bytes are appended and it returns
// false.
//
// This method is intended to be used only for C strings (char *), flat
// memory arrays (uint8 *), and other un-structured pointers to memory.
//
// Note:  If you are trying to append a value from a portable type (uint8,
//        uint32, etc.) or a packed structure, then you should call one the
//        template versions of this method (below).  They are type-safe and
//        do quite a lot of work for you.
//
// Parameters:
//      pData - pointer to the memory that is to be appended.
//      numberOfBytes - the number of bytes that are to be appended.
//
// Returns:
//      true if the bytes were appended.
//      false if there wasn't enough room to append the bytes.
//

bool BcmOctetBuffer::AddToEnd(const uint8 *pData, unsigned int numberOfBytes, bool resizeBuffer)
{
    // Make sure we have room for the data.
    if ((pData == NULL) || (pfBuffer == NULL))
    {
        return false;
    }

    if (fUsedBytes + numberOfBytes > fBufferSize)
    {
        if (resizeBuffer == false)
        {
            return false;
        }
        else
        {
            uint8* pCurrentBuffer = new uint8[fBufferSize+numberOfBytes];
            
            memcpy(pCurrentBuffer, pfBuffer, fUsedBytes);

            AdoptBuffer(pCurrentBuffer,fBufferSize+numberOfBytes,fUsedBytes,true); 
        }
    }

    if (numberOfBytes > 0)
    {
        // Copy the data and adjust the indices.
        memcpy(pfBuffer + fUsedBytes, pData, numberOfBytes);
        fUsedBytes += numberOfBytes;
    }

    return true;
}


// Reads the specified number of bytes from the buffer, storing them in
// the location specified.
//
// This method is intended to be used only for C strings (char *), flat
// memory arrays (uint8 *), and other un-structured pointers to memory.
//
// NOTES:
//      - If you are trying to read a value from a portable type (uint8,
//        uint32, etc.) or a packed structure, then you should call one the
//        template versions of this method (below).  They are type-safe and
//        do quite a lot of work for you.
//      - The offset is modified to contain the offset for the next unread
//        byte.  This allows you to treat it as an automatically
//        incrementing iterator, reading out one value after another without
//        incrementing the offset yourself.
//      - If the data pointer (pData) is NULL, then the read will fail (for
//        obvious reasons).
//      - If there isn't enough data in the buffer to satisfy the read, then
//        the whole operation will fail, with no data consumed.  I.e. if you
//        try to read 20 bytes, but there are only 10, then no data will be
//        consumed from the buffer, your memory (pData) will not be modified,
//        and the offset will not be updated.
//
// Parameters:
//      offset - the location (0-based) into the buffer from which the data
//               should be read.
//      pData - pointer to the memory that is to receive the data that is
//              read.
//      numberOfBytes - the number of bytes that are to be read.
//
// Returns:
//      true if the read succeeded.
//      false if there wasn't enough data, pData was NULL, etc.
//
bool BcmOctetBuffer::Read(unsigned int &offset, uint8 *pData, 
                          unsigned int numberOfBytes) const
{
    // If, for whatever reason, they want to read 0 bytes, then this should
    // not fail, even if anything below would have caused it to fail.
    if (numberOfBytes == 0)
    {
        return true;
    }

    // Do some quick validation.
    if ((pData == NULL) || (pfBuffer == NULL) ||
        (offset + numberOfBytes > fUsedBytes))
    {
        return false;
    }

    // Copy the data, and adjust the offset to show that the data was read.
    memcpy(pData, pfBuffer + offset, numberOfBytes);
    offset += numberOfBytes;

    return true;
}


// Causes the current buffer to be resized to the specified size.  This is
// basically the same as creating a new BcmOctetBuffer with the size
// specified.  It will cause the current buffer to be deleted and a new one
// created; any data in the buffer will be lost!
//
// Parameters:
//      bufferSize - the number of bytes in the buffer.
//
// Returns:  Nothing.
//
void BcmOctetBuffer::ResizeBuffer(unsigned int bufferSize)
{
    // Don't need to do this if my buffer is already the correct size.
    if (bufferSize != fBufferSize)
    {
        // Let other helper functions to the work.
        DeleteBuffer();

        Initialize(bufferSize);
    }

    // Either way, we need to cause the buffer to be emptied.
    Empty();
}


// Causes the current buffer to be deleted and replaced with the one
// specified.
//
// Parameters:
//      pBuffer - pointer to the memory buffer to be adopted.
//      bufferSize - the number of bytes in the buffer.
//      bytesUsed - the number of valid bytes of data that are in the buffer.
//      ownTheBuffer - set this to true (default) if this class should be
//                     the owner of the buffer and should delete it; set it
//                     to false if the buffer should not be deleted.
//
// Returns:  Nothing.
//
void BcmOctetBuffer::AdoptBuffer(uint8 *pBuffer, unsigned int bufferSize,
                                 unsigned int bytesUsed, bool ownTheBuffer)
{
    // Delete the current buffer (based on the rules in the helper method).
    DeleteBuffer();

    // Note that if someone frees this memory without telling me to release it,
    // then attempts to access the buffer (via Read(), AddToEnd(), etc.) will
    // cause memory exceptions!
    //
    // Store the current buffer.
    pfBuffer = pBuffer;
    fBufferSize = bufferSize;
    fUsedBytes = bytesUsed;
    fBufferIsMine = ownTheBuffer;
}


// This is similar to AdoptBuffer(), except that it allows you to set this
// buffer as a window into the buffer of another octet buffer.  The window
// is defined as an offset from the beginning of the other buffer, going for
// the specified number of bytes.
//
// This is meant to be used to define a start and end point for reading data
// out of the other buffer.  Do not use this buffer to append data, since
// the other buffer's state is not updated.
//
// Note that this method clamps the input values so that you cannot define
// a window outside of the bounds of the other buffer.  Also, this buffer
// will have the correct number of valid bytes of data from the other
// buffer's window.
//
// Parameters:
//      otherInstance - the octet buffer into which this buffer should be
//                      windowed.
//      offset - the offset into the buffer for the first byte.
//      numberOfBytes - the number of bytes of the other buffer that this
//                      one should overlay (0=all).
//
// Returns:  Nothing.
//
void BcmOctetBuffer::Overlay(const BcmOctetBuffer &otherInstance, unsigned int offset,
                             unsigned int numberOfBytes)
{
    unsigned int validBytes;

    // Do some validation so that the offset and size are within the bounds of
    // the other buffer.
    if (offset > otherInstance.BufferSize())
    {
        offset = otherInstance.BufferSize();
    }
    if (numberOfBytes == 0)
    {
        numberOfBytes = otherInstance.BufferSize();
    }
    if (offset + numberOfBytes > otherInstance.BufferSize())
    {
        numberOfBytes = otherInstance.BufferSize() - offset;
    }

    // Calculate the number of valid bytes of data that are within the specified
    // window.
    if (offset >= otherInstance.BytesUsed())
    {
        validBytes = 0;
    }
    else
    {
        validBytes = otherInstance.BytesUsed() - offset;
    }

    if (validBytes > numberOfBytes)
    {
        validBytes = numberOfBytes;
    }

    // NOTE:  This is not entirely safe!  It is possible for someone to delete
    // the buffer that was passed in to this method, but then continue to use
    // this class, which would cause a nasty memory exception.  It should be
    // noted, though, that this problem exists for all of the cases where the
    // buffer has been adopted from an external source - if the memory is freed
    // then continuing to use this object will cause a memory exception.

    // Also note that, if someone adds data to the end of the other buffer, that
    // information is not automatically updated here.  Likewise, if someone adds
    // data to this buffer, the other buffer is not updated with that
    // information!  This can lead to data overwriting.

    // Now do the normal buffer adoption stuff, making sure not to own the buffer.
    AdoptBuffer(otherInstance.pfBuffer + offset, numberOfBytes, validBytes, false);
}


// Causes the buffer to be released to the caller so that it will no longer
// be referenced by (and will not be deleted by) the OctetBuffer.  The
// caller becomes the owner of the memory and must delete it (if
// appropriate).
//
// The intent of this method is to get back a buffer that was previously
// given to this class via AdoptBuffer().
//
// Parameters:  None.
//
// Returns:
//      The pointer to the base address of the buffer that was previously
//      being used.
//
uint8 *BcmOctetBuffer::ReleaseBuffer(void)
{
    uint8 *pBuffer = pfBuffer;

    // Clear out my state.
    Initialize(0);

    return pBuffer;
}


// This method dumps the contents of the buffer to the specified ostream
// in ASCII-encoded hex format.  Obviously, this will be rather slow, so
// you don't want to do it except for debugging purposes.
//
// Parameters:
//      outstream - the ostream to which the data should be written.
//      showHeader - if true, displays a brief message about the portion of
//                   the buffer that is being printed.
//      offset - the 0-based offset into the buffer where the dump should
//               begin.  If this is past the end of valid data, then no
//               bytes will be dumped.
//      numberOfBytes - the number of bytes to dump.  If this is 0 then it
//                      will go to the end of the buffer.
//      showAddressLines - will print the memory address line in front
//
// Returns:
//      A reference to the ostream to support chaining.
//
ostream &BcmOctetBuffer::Print(ostream &outstream, bool showHeader,
                               unsigned int offset, unsigned int numberOfBytes,
                               bool showAddressLines ) const
{
    // Validate the parameters.  If the offset is past the end of the buffer,
    // then just bail.  There is a special case where the buffer is empty and
    // the offset is 0; this appears to be beyond the end of the buffer, but
    // should actually be printed.
    if ((offset > fUsedBytes) ||
        ((offset == fUsedBytes) && (fUsedBytes > 0)))
    {
        return outstream;
    }

    // If numberOfBytes is 0, then go to the end of the buffer.
    if (numberOfBytes == 0)
    {
        numberOfBytes = fUsedBytes;
    }

    // If the offset plus the number of bytes goes past the end of the buffer,
    // then set the numberOfBytes to go to the end of the buffer.
    if (offset + numberOfBytes > fUsedBytes)
    {
        numberOfBytes = fUsedBytes - offset;
    }

    if (showHeader)
    {
        // if printing header info
        outstream << "Contents of the buffer (all values are in Hex):  start "
                  << dec << offset << ", " << numberOfBytes << " bytes\n\n";
    }
    else
    {
        // else NOT printing header info...
        if( numberOfBytes > 8 )
        {
            // if more than 8 bytes to print...output a leading newline
            outstream << "\n";
        }
    }

    if (pfBuffer != NULL)
    {
        unsigned int i;
        int oldFill;
        char asciiRep[17];
        unsigned int lineOffset = 0;

        // Fill in the end of the ASCII representation for printing.
        asciiRep[16] = '\0';

        // Store the old fill character, and set it to 0 fill.
        oldFill = outstream.fill();
        outstream.fill('0');

        // Set the output mode to hex.
        outstream << hex;

        //_ I am as blind as a bat with my radar not functioning, I cant see the address lines.
        //_ Lets output it now to know what the heck we are looking at.
        if ( showAddressLines == true && numberOfBytes > 0 )
        {
            outstream << setw(8) << (unsigned int) pfBuffer << ": ";
        }

        for (i = 0; i < numberOfBytes; i++)
        {
            // Every 4 bytes, print an extra couple of spaces, to make the
            // dwords stand out.
            if( ((i & 0x03) == 0) && !((i == 0) && (numberOfBytes <= 8)) )
            {
                // every 4 bytes (except for special case of first byte when
                // total bytes to print is <= 8), print an extra couple of 
                // spaces, to make the dwords stand out.
                outstream << "  ";
            }

            // Is there a way to make setw sticky?  Even the .width() method
            // doesn't seem to be stick!
            outstream << setw(2) << (unsigned int) pfBuffer[offset + i] << " ";

            // Build the ASCII rep for this line.
            asciiRep[lineOffset] = (char) pfBuffer[offset + i];
            if (!isprint(asciiRep[lineOffset]))
            {
                asciiRep[lineOffset] = '.';
            }

            lineOffset++;

            // Print the ASCII rep and a newline after 16 bytes have been
            // printed.
            if ((i & 0x0f) == 0x0f)
            {
                outstream << " | " << asciiRep << '\n';

                lineOffset = 0;
            
                //_ I am as blind as a bat with my radar not functioning, I cant see the address lines.
                //_ Lets output it now to know what the heck we are looking at.
                if (showAddressLines == true)
                {
                    outstream << setw(8) << (unsigned int) pfBuffer+i+1 << ": ";
                }
            }
        }
    
        // Restore the old fill character, and set back to decimal.
        outstream.fill(oldFill);
        outstream << dec;

        // Print the ASCII rep and a newline if we didn't just do one.
        if ((i & 0x0f) != 0x00)
        {
            // If we printed more than one line, then we need to pad out to
            // the end of the line so that the ASCII rep lines up correctly.
            if (numberOfBytes > 16)
            {
                i &= 0x0f;

                while (i < 16)
                {
                    if ((i & 0x03) == 0)
                    {
                        outstream << "  ";
                    }

                    outstream << "   ";

                    i++;
                }
            }

            // Truncate the ASCII rep string.
            asciiRep[lineOffset] = '\0';

            // Print it.
            outstream << " | " << asciiRep << '\n';
        }
    }
    else
    {
        outstream << "NULL/Empty Buffer!\n";
    }

    if (showHeader)
    {
        outstream << "\n";
    }

    return outstream;
}


// This method converts the data stored in the octet buffer to the ASCII-hex
// representation, placing it in the char buffer passed in.  Unlike the
// Print method above, there is no formatting done on the data (like adding
// spaces, byte swapping, etc).
//
// The ASCII representation will be \0 terminated, like normal strings.
//
// In order to store the whole string, you must provide a buffer that is
// just over twice as long as the amount of data:
//
//    asciiLength = (buffer.BytesUsed() * 2) + 1
//
// This is because each byte of data will be converted into 2 ASCII chars,
// then a \0 is added to the end.  If your buffer is too short, then the
// string will be truncated at the nearest data byte boundary (you won't
// have half of a byte of string present).
//
// Example:
//      The value 1522 (0x5f2) is stored as a uint32 to the octet buffer
//      in network order, producing 4 bytes of data.
//
//      Calling this method produces the 9 character string "000005f2"; note
//      that there is an implicit \0 at the end of the string.
//
// Parameters:
//      pString - pointer to a char string where the ASCII representation is
//                to be placed.
//      maxLength - the size of the string that was allocated, i.e. the max
//                  number of chars (including \0 terminator) that can be
//                  stored.
//      useUpperCase - true if you want hex letters to be uppercase (e.g.
//                     0xFF); false (default) if you want lowercase (0xff).
//
// Returns:
//      The number of chars that were stored, excluding the \0.  This is
//      equivalent to doing strlen(pString).
//
unsigned int BcmOctetBuffer::ConvertToAsciiHex(char *pString,
                                               unsigned int maxLength,
                                               bool useUpperCase) const
{
    const char lowerCase[] = "0123456789abcdef";
    const char upperCase[] = "0123456789ABCDEF";
    const char *pConversionString;

    unsigned int stringOffset, bufferOffset;

    if ((maxLength == 0) || (pString == NULL))
    {
        return 0;
    }

    if (useUpperCase)
    {
        pConversionString = upperCase;
    }
    else
    {
        pConversionString = lowerCase;
    }

    // Calculate the number of bytes of data to convert, leaving room for the
    // \0 terminator.  I can only copy a full byte, so if there are an odd
    // number of chars for output, then make it even (truncating).
    maxLength--;
    maxLength -= (maxLength & 0x01);

    // Convert data until we run out of data, or until we run out of output
    // space.
    bufferOffset = 0;
    stringOffset = 0;
    while ((bufferOffset < fUsedBytes) && (stringOffset < maxLength))
    {
        // Convert both nibbles of the current byte in the buffer.
        pString[stringOffset] = pConversionString[((pfBuffer[bufferOffset] >> 4) & 0x0f)];
        stringOffset++;
        pString[stringOffset] = pConversionString[(pfBuffer[bufferOffset] & 0x0f)];
        stringOffset++;

        bufferOffset++;
    }

    // Add the \0 terminator to the end of the string.
    pString[stringOffset] = '\0';

    // Return the number of chars placed in the string.
    return stringOffset;
}


// Does common initialization for multiple constructors.
//
// Parameters:
//      bufferSize - if greater than 0, the number of bytes to allocate.
//
// Returns:  Nothing.
//
void BcmOctetBuffer::Initialize(unsigned int bufferSize)
{
    pfBuffer = NULL;
    fBufferIsMine = false;
    fBufferSize = bufferSize;
    fUsedBytes = 0;

    if (fBufferSize > 0)
    {
        pfBuffer = new uint8[fBufferSize];
        fBufferIsMine = true;
    
        if (pfBuffer == NULL)
        {
            gLogMessageRaw << "BcmOctetBuffer::Initialize:  WARNING - Failed to allocate a buffer of " << fBufferSize << " bytes!!!" << endl;

            fBufferSize = 0;
        }

        // For debugging, set the entire buffer to an unusual value.
        Empty();
    }
}


// Called from several methods; checks to see if we are the owner of the
// buffer, and if so, then it deletes the buffer.
//
// Parameters:  None.
//
// Returns:  Nothing.
//
void BcmOctetBuffer::DeleteBuffer(void)
{
    // Only delete the buffer if I have been given absolute ownership of it.
    // Note that pfBuffer can be NULL; deleting NULL is fine.
    if (fBufferIsMine)
    {
        delete pfBuffer;
    }

    // Now clear out my state.
    Initialize(0);
}


#if (REGRESSION_TEST)

//#include "whatever.h"

#endif

bool BcmOctetBuffer::Regress(ostream &outStream)
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

        outStream << "BcmOctetBuffer::Regress() - Beginning regression tests...\n" << endl;

        // ----------------------- THE DEVIL IS IN THE DETAILS ---------------
        //
        // Detailed regression tests to cover internal state and basic low-level
        // operation of the class.
        //
        // The debug output strings serve as comments for the code.

        // UNFINISHED - needs to be fleshed out!  For now, I'm just testing
        // new code (BitsEncodingXxx stuff for PR12948).
        /*
        testNumber++;
        outStream << "Test " << testNumber << ":  basic internal state..." << endl;

        {
            BcmOctetBuffer obuf;

            {
                outStream << "\tDefault constructor..." << endl;

                if ()
                {
                    outStream << "\tERROR - Default state incorrect!" << endl;
                    result = false;
                }

                // Various accessors...
            }
        }

        testNumber++;
        outStream << "Test " << testNumber << ":  advanced internal state..." << endl;

        {
            {
                outStream << "\tCopy constructor..." << endl;

                if (x != y)
                {
                    outStream << "\tERROR - objects are not equal!" << endl;
                    result = false;
                }
            }

            {
                outStream << "\tAssignment operator..." << endl;

                y = x;

                if (x != y)
                {
                    outStream << "\tERROR - objects are not equal!" << endl;
                    result = false;
                }
            }

            {
                outStream << "\tPrinting...\n" << endl;

                outStream << x;
            }

            {
                outStream << "\tResetting to default state..." << endl;
            }
        }
        */

        testNumber++;
        outStream << "Test " << testNumber << ":  BITS Encoding accessors (PR12948)..." << endl;

        {
            const unsigned int obufSize = 256;

            unsigned int bitNumber;
            BcmOctetBuffer obuf(obufSize);

            {
                outStream << "\tAll BITS should be 0 by default..." << endl;
    
                for (bitNumber = 0; bitNumber < (obuf.BufferSize() * 8); bitNumber++)
                {
                    if (obuf.BitsEncodingIsSet(bitNumber) == true)
                    {
                        outStream << "\tERROR - BITS offset " << bitNumber << " is set to 1 by default!" << endl;
                        result = false;
                        break;
                    }
                }
            }

            {
                outStream << "\tFilling buffer with 0s, verifying BITS values..." << endl;

                obuf.Empty();
                obuf.Fill(0);
    
                for (bitNumber = 0; bitNumber < (obuf.BufferSize() * 8); bitNumber++)
                {
                    if (obuf.BitsEncodingIsSet(bitNumber) == true)
                    {
                        outStream << "\tERROR - BITS offset " << bitNumber << " is set to 1!" << endl;
                        result = false;
                        break;
                    }
                }
            }

            {
                outStream << "\tFilling buffer with 1s, verifying BITS values..." << endl;
    
                obuf.Empty();
                obuf.Fill(0xff);
    
                for (bitNumber = 0; bitNumber < (obuf.BufferSize() * 8); bitNumber++)
                {
                    if (obuf.BitsEncodingIsSet(bitNumber) == false)
                    {
                        outStream << "\tERROR - BITS offset " << bitNumber << " is set to 0!" << endl;
                        result = false;
                        break;
                    }
                }
            }

            {
                outStream << "\tFilling buffer with 0x55, verifying BITS values..." << endl;
    
                obuf.Empty();
                obuf.Fill(0x55);
    
                for (bitNumber = 0; bitNumber < (obuf.BufferSize() * 8); bitNumber++)
                {
                    if ((bitNumber & 0x01) == 0)
                    {
                        if (obuf.BitsEncodingIsSet(bitNumber) == true)
                        {
                            outStream << "\tERROR - BITS offset " << bitNumber << " is set to 1!" << endl;
                            result = false;
                            break;
                        }
                    }
                    else
                    {
                        if (obuf.BitsEncodingIsSet(bitNumber) == false)
                        {
                            outStream << "\tERROR - BITS offset " << bitNumber << " is set to 0!" << endl;
                            result = false;
                            break;
                        }
                    }
                }
            }

            {
                outStream << "\tFilling buffer with 0xaa, verifying BITS values..." << endl;
    
                obuf.Empty();
                obuf.Fill(0xaa);
    
                for (bitNumber = 0; bitNumber < (obuf.BufferSize() * 8); bitNumber++)
                {
                    if ((bitNumber & 0x01) == 0)
                    {
                        if (obuf.BitsEncodingIsSet(bitNumber) == false)
                        {
                            outStream << "\tERROR - BITS offset " << bitNumber << " is set to 0!" << endl;
                            result = false;
                            break;
                        }
                    }
                    else
                    {
                        if (obuf.BitsEncodingIsSet(bitNumber) == true)
                        {
                            outStream << "\tERROR - BITS offset " << bitNumber << " is set to 1!" << endl;
                            result = false;
                            break;
                        }
                    }
                }
            }

            {
                outStream << "\tChanging bits from 0 to 1..." << endl;
    
                obuf.Empty();
                obuf.Fill(0);
    
                for (bitNumber = 0; bitNumber < (obuf.BufferSize() * 8); bitNumber++)
                {
                    if (obuf.BitsEncodingIsSet(bitNumber) == true)
                    {
                        outStream << "\tERROR - BITS offset " << bitNumber << " is set to 1!" << endl;
                        result = false;
                        break;
                    }
    
                    if (obuf.BitsEncodingSetValue(bitNumber, true) == false)
                    {
                        outStream << "\tERROR - failed to set bit " << bitNumber << endl;
                        result = false;
                        break;
                    }
    
                    if (obuf.BitsEncodingIsSet(bitNumber) == false)
                    {
                        outStream << "\tERROR - BITS offset " << bitNumber << " is set to 0, but should be 1!" << endl;
                        result = false;
                        break;
                    }
                }
            }

            {
                outStream << "\tChanging bits from 1 to 0..." << endl;
    
                obuf.Empty();
                obuf.Fill(0xff);
    
                for (bitNumber = 0; bitNumber < (obuf.BufferSize() * 8); bitNumber++)
                {
                    if (obuf.BitsEncodingIsSet(bitNumber) == false)
                    {
                        outStream << "\tERROR - BITS offset " << bitNumber << " is set to 0!" << endl;
                        result = false;
                        break;
                    }
    
                    if (obuf.BitsEncodingSetValue(bitNumber, false) == false)
                    {
                        outStream << "\tERROR - failed to set bit " << bitNumber << endl;
                        result = false;
                        break;
                    }
    
                    if (obuf.BitsEncodingIsSet(bitNumber) == true)
                    {
                        outStream << "\tERROR - BITS offset " << bitNumber << " is set to 1, but should be 0!" << endl;
                        result = false;
                        break;
                    }
                }
            }

            {
                outStream << "\tChanging 0x55 to 0xaa..." << endl;
    
                obuf.Empty();
                obuf.Fill(0x55);
    
                for (bitNumber = 0; bitNumber < (obuf.BufferSize() * 8); bitNumber++)
                {
                    if (obuf.BitsEncodingIsSet(bitNumber) == false)
                    {
                        if (obuf.BitsEncodingSetValue(bitNumber, true) == false)
                        {
                            outStream << "\tERROR - failed to set bit " << bitNumber << endl;
                            result = false;
                            break;
                        }
    
                        if (obuf.BitsEncodingIsSet(bitNumber) == false)
                        {
                            outStream << "\tERROR - BITS offset " << bitNumber << " is set to 0!" << endl;
                            result = false;
                            break;
                        }
                    }
                    else
                    {
                        if (obuf.BitsEncodingSetValue(bitNumber, false) == false)
                        {
                            outStream << "\tERROR - failed to set bit " << bitNumber << endl;
                            result = false;
                            break;
                        }
    
                        if (obuf.BitsEncodingIsSet(bitNumber) == true)
                        {
                            outStream << "\tERROR - BITS offset " << bitNumber << " is set to 1!" << endl;
                            result = false;
                            break;
                        }
                    }
                }
            }

            {
                outStream << "\tVerifying that the buffer now contains 0xaa..." << endl;
    
                for (bitNumber = 0; bitNumber < obuf.BufferSize(); bitNumber++)
                {
                    if (obuf.AccessBuffer()[bitNumber] != 0xaa)
                    {
                        outStream << "\tERROR - byte offset " << bitNumber << " should be 0xaa!" << endl;
                        result = false;
                        break;
                    }
                }
            }

            {
                outStream << "\tChanging 0xaa to 0x55..." << endl;
    
                obuf.Empty();
                obuf.Fill(0xaa);
    
                for (bitNumber = 0; bitNumber < (obuf.BufferSize() * 8); bitNumber++)
                {
                    if (obuf.BitsEncodingIsSet(bitNumber) == false)
                    {
                        if (obuf.BitsEncodingSetValue(bitNumber, true) == false)
                        {
                            outStream << "\tERROR - failed to set bit " << bitNumber << endl;
                            result = false;
                            break;
                        }
    
                        if (obuf.BitsEncodingIsSet(bitNumber) == false)
                        {
                            outStream << "\tERROR - BITS offset " << bitNumber << " is set to 0!" << endl;
                            result = false;
                            break;
                        }
                    }
                    else
                    {
                        if (obuf.BitsEncodingSetValue(bitNumber, false) == false)
                        {
                            outStream << "\tERROR - failed to set bit " << bitNumber << endl;
                            result = false;
                            break;
                        }
    
                        if (obuf.BitsEncodingIsSet(bitNumber) == true)
                        {
                            outStream << "\tERROR - BITS offset " << bitNumber << " is set to 1!" << endl;
                            result = false;
                            break;
                        }
                    }
                }
            }

            {
                outStream << "\tVerifying that the buffer now contains 0x55..." << endl;

                for (bitNumber = 0; bitNumber < obuf.BufferSize(); bitNumber++)
                {
                    if (obuf.AccessBuffer()[bitNumber] != 0x55)
                    {
                        outStream << "\tERROR - byte offset " << bitNumber << " should be 0x55!" << endl;
                        result = false;
                        break;
                    }
                }
            }

            {
                unsigned int verifyBit;

                outStream << "\tAutomatically increasing bytes used to hold new bits..." << endl;

                obuf.Empty();

                for (bitNumber = 0; bitNumber < (obuf.BufferSize() * 8); bitNumber++)
                {
                    if (obuf.BitsEncodingSetValue(bitNumber, true) == false)
                    {
                        outStream << "\tERROR - failed to set bit " << bitNumber << endl;
                        result = false;
                        break;
                    }

                    if (obuf.BytesUsed() != ((bitNumber / 8) + 1))
                    {
                        outStream << "\tERROR - bytes used incorrect for bitNumber " << bitNumber << endl;
                        result = false;
                        break;
                    }

                    if (obuf.BufferSize() != obufSize)
                    {
                        outStream << "\tERROR - buffer size changed!" << endl;
                        result = false;
                        break;
                    }

                    for (verifyBit = 0; verifyBit <= bitNumber; verifyBit++)
                    {
                        if (obuf.BitsEncodingIsSet(verifyBit) == false)
                        {
                            outStream << "\tERROR - bit " << verifyBit << " is 0, should be 1!" << endl;
                            result = false;
                            break;
                        }
                    }

                    if (result == false)
                    {
                        break;
                    }

                    for (; verifyBit < (obuf.BytesUsed() * 8); verifyBit++)
                    {
                        if (obuf.BitsEncodingIsSet(verifyBit) == true)
                        {
                            outStream << "\tERROR - bit " << verifyBit << " is 1, should be 0!" << endl;
                            result = false;
                            break;
                        }
                    }

                    if (result == false)
                    {
                        break;
                    }
                }
            }

            {
                unsigned int verifyByte;

                outStream << "\tAutomatically increasing bytes used (0-fill)..." << endl;

                obuf.Empty();

                for (bitNumber = 7; bitNumber < (obuf.BufferSize() * 8); bitNumber += 8)
                {
                    if (obuf.BitsEncodingSetValue(bitNumber, true) == false)
                    {
                        outStream << "\tERROR - failed to set bit " << bitNumber << endl;
                        result = false;
                        break;
                    }

                    if (obuf.BytesUsed() != ((bitNumber / 8) + 1))
                    {
                        outStream << "\tERROR - bytes used incorrect for bitNumber " << bitNumber << endl;
                        result = false;
                        break;
                    }

                    if (obuf.BufferSize() != obufSize)
                    {
                        outStream << "\tERROR - buffer size changed!" << endl;
                        result = false;
                        break;
                    }

                    for (verifyByte = 0; verifyByte < obuf.BytesUsed(); verifyByte++)
                    {
                        if (obuf.AccessBuffer()[verifyByte] != 0x01)
                        {
                            outStream << "\tERROR - byte " << verifyByte << " should be 0x01!" << endl;
                            result = false;
                            break;
                        }
                    }

                    if (result == false)
                    {
                        break;
                    }
                }
            }

            {
                outStream << "\tRejecting bit set beyond end of buffer..." << endl;

                obuf.Empty();

                bitNumber = (obuf.BufferSize() * 8) - 1;
                if (obuf.BitsEncodingSetValue(bitNumber, true) == false)
                {
                    outStream << "\tERROR - Setting last bit failed, should have been ok!" << endl;
                    result = false;
                }

                bitNumber++;
                if (obuf.BitsEncodingSetValue(bitNumber, true) == true)
                {
                    outStream << "\tERROR - setting bit " << bitNumber << " succeeded, should have failed!" << endl;
                    result = false;
                }
            }
        }

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

        // ----------------------- END OF REGRESSION TESTS -------------------

        outStream << "\tBcmOctetBuffer::Regress() - Finished regression tests...";
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


