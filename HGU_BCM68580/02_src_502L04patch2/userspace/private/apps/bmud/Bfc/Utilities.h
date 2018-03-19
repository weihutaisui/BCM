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
//  Filename:       Utilities.h
//  Author:         David Pullen
//  Creation Date:  April 28, 1999
//
//****************************************************************************
//  Description:
//      General utility functions that are used everywhere.
//
//****************************************************************************

#ifndef UTILITIES_H
#define UTILITIES_H

//********************** Include Files ***************************************

#include "typedefs.h"

//********************** Global Types ****************************************

//********************** Global Constants ************************************

//********************** Global Variables ************************************

//********************** Class Declaration ***********************************

/// This helper function converts an abstract bitmask (of up to 32 bits) into
/// a string of names, where each name corresponds to a bit in the mask.  It
/// starts iterating at bit 31, and continues until it has checked through bit 0
/// or has filled the output string.
///
/// The bitmask is passed in by the user, and can be any size up to 32 bits
/// (smaller sizes will be converted up to 32 bits, padded with 0's so that they
/// will be skipped).
///
/// The array of bit names must be specified, and must not be NULL.  It is
/// assumed that pBitNames[0] corresponds to bit 0, and so on.  If there are
/// gaps in the bits (reserved, etc.), then the pBitNames index corresponding
/// to that index can be NULL.  Also, you don't have to specify unused MSB names,
/// e.g. if only the 3 LSBs are used, then pBitNames can have only 3 entries.
///
/// If a bit in the mask is set, and there is no corresponding string in the bit
/// names array, then the string {Unknown} will be used.
///
/// The separator string that is specified will be used in between each name.
///
/// If the bitmask is 0 (no bits set), then the string {None} will be stored.
///
/// \param
///      bitmask - the mask of bits to be converted into strings.
/// \param
///      pString - pointer to the string into which the names should be stored.
/// \param
///      maxLength - the maximum number of characters (including the \0) that
///                  can be stored in pString.
/// \param
///      pBitNames - pointer to an array of strings for the bit names.
/// \param
///      numberOfNames - the number of bit names in the pBitNames array.
/// \param
///      pSeparatorString - the string to be placed between each name.
///
/// \retval
///      true if the string was created successfully.
/// \retval
///      false if the string pointer was NULL, or if the string was too short to
///          hold all of the characters (truncated).
///
bool ConvertBitmaskToString(uint32 bitmask, char *pString, unsigned int maxLength,
                            const char *pBitNames[], unsigned int numberOfNames,
                            const char *pSeparatorString = "|");

//********************** Inline Method Implementations ***********************

// This first set of inline functions are for big-endian machines (primarily
// Intel).  The second set (in the #else) are for big-endian machines that
// require no swapping - they evaluate to NOPs.
#if (BCM_LITTLE_ENDIAN == 1)

/// Swaps the bytes of a 16-bit unsigned value to convert to/from host and
/// network order.
///
/// \param
///      value - the value whose bytes should be swapped.
///
/// \return
///      The value with its bytes swapped.
///
inline uint16 HostToNetwork(uint16 value)
{
    uint16 newValue;

    newValue = (value >> 8) & 0x00ff;
    newValue |= (value << 8) & 0xff00;

    return newValue;
}


/// Swaps the bytes of a 32-bit unsigned value to convert to/from host and
/// network order.
///
/// May want to do this better...
///
/// \param
///      value - the value whose bytes should be swapped.
///
/// \return
///      The value with its bytes swapped.
///
inline uint32 HostToNetwork(uint32 value)
{
    uint32 newValue;

    newValue = HostToNetwork((uint16) (value & 0xffff));
    newValue <<= 16;
    newValue |= HostToNetwork((uint16) ((value >> 16) & 0xffff));

    return newValue;
}


/// Simple handler for a single byte...useful when the type value is not known
/// (because it is a template parameter).
///
/// \param
///      value - the value whose bytes should be swapped.
///
/// \return
///      The value with its bytes swapped.
///
inline uint8 HostToNetwork(uint8 value)
{
    return value;
}


/// Swaps the bytes of a 16-bit signed value to convert to/from host and
/// network order.
///
/// \param
///      value - the value whose bytes should be swapped.
///
/// \return
///      The value with its bytes swapped.
///
inline int16 HostToNetwork(int16 value)
{
    int16 newValue;

    newValue = (value >> 8) & 0x00ff;
    newValue |= (value << 8) & 0xff00;

    return newValue;
}


/// Swaps the bytes of a 32-bit signed value to convert to/from host and
/// network order.
///
/// May want to do this better...
///
/// \param
///      value - the value whose bytes should be swapped.
///
/// \return
///      The value with its bytes swapped.
///
inline int32 HostToNetwork(int32 value)
{
    int32 newValue;

    newValue = HostToNetwork((int16) (value & 0xffff));
    newValue <<= 16;
    newValue |= (HostToNetwork((int16) ((value >> 16) & 0xffff)) & 0xffff);

    return newValue;
}


/// Swaps the bytes of a 32-bit signed value to convert to/from host and
/// network order.
///
/// May want to do this better...
///
/// \param
///      value - the value whose bytes should be swapped.
///
/// \return
///      The value with its bytes swapped.
///
inline int HostToNetwork(int value)
{
    return HostToNetwork((uint32) value);
}


/// Simple handler for a single byte...useful when the type value is not known
/// (because it is a template parameter).
///
/// \param
///      value - the value whose bytes should be swapped.
///
/// \return
///      The value with its bytes swapped.
///
inline int8 HostToNetwork(int8 value)
{
    return value;
}


/// This just "renames" the previous functions.  They both do exactly the same
/// thing, but the names show the intent of the operations, and so they are
/// important.
#define NetworkToHost(x) HostToNetwork(x)

#else

/// For machines that already are big endian, don't need to do anything.
/// This turns the call into a NOP for all types.
#define HostToNetwork(x) (x)
#define NetworkToHost(x) (x)

#endif

#if (__DCPLUSPLUS__ == 1) || defined(TARGETOS_vxWorks) || defined(TARGETOS_eCos)

#include <ctype.h> // JRM - added for vxWorks isupper() & tolower() 

/// The Diab and eCos C RTL don't supporthave strnicmp() or stricmp() (case-
/// insensitive string comparisons.
int strnicmp(const char *str1, const char *str2, int cnt);
int stricmp(const char *str1, const char *str2);

#endif // end of #if (__DCPLUSPLUS__ == 1) || defined(TARGETOS_vxWorks)


#endif // end of #ifndef UTILITIES_H



