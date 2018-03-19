//****************************************************************************
//
// Copyright (c) 2001-2010 Broadcom Corporation
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
//    Filename: BcmBasicString.h
//    Author:   Kevin O'Neal
//    Creation Date: 18-Sept-2001
//
//**************************************************************************
//    Description:
//
//		Cablemodem V2 code
//		
//    Wrapper for an STL basic_string which eliminates inline expansions, as
//    well as providing conversion to / from other BFC object types.
//
//    The original reason for doing this was to reduce code size.  Turns out 
//    that such savings can be considerable.  Note that this object does not 
//    wrap all basic_string methods, since one purpose of this object is code
//    size reduction.
//
//		
//**************************************************************************
//    Revision History:
//
//**************************************************************************

#ifndef BCMBASICSTRING_H
#define BCMBASICSTRING_H

#include <string>
#include <iostream>

#if (__GNUC__ >= 3)
using namespace std;
#endif

// forward references:
class BcmOctetBuffer;
class BcmIpAddress;
class BcmMacAddress;

class BcmBasicString
{
  public:
    typedef char E;
    typedef E & reference;
    typedef const E & const_reference;
    typedef unsigned int size_type;
    
    BcmBasicString ();
    BcmBasicString (const E *s);
    BcmBasicString (const E *s, size_type n);
    BcmBasicString (const string &s);
    BcmBasicString (const BcmBasicString &s);
    
   ~BcmBasicString ();
   
    bool operator < (const BcmBasicString &s) const;
    bool operator == (const E *s) const;
    bool operator == (const BcmBasicString &s) const;
    bool operator != (const E *s) const;
    bool operator != (const BcmBasicString &s) const;
    BcmBasicString &operator = (E c);
    BcmBasicString &operator = (const E *s);
    BcmBasicString &operator = (const BcmBasicString &s);
    BcmBasicString &operator += (E c);
    BcmBasicString &operator += (const E *s);
    BcmBasicString &operator += (const BcmBasicString &rhs);
    //const_reference operator [] (size_type pos) const;
    const E operator [] (size_type pos) const;
    reference operator [] (size_type pos);
    size_type length () const;
    size_type size () const;
    bool empty () const;
    size_type capacity () const;
    void reserve();
    void reserve (size_type num);
    const E *c_str () const;
    const E *data () const;
    BcmBasicString &assign (const E *s, size_type n);
    BcmBasicString &assign (const BcmBasicString &str, size_type pos, size_type n);
    BcmBasicString &insert(size_type p0, const E *s);
    BcmBasicString& append(const E *s, size_type n);
    size_type find_first_of (E c, size_type pos = 0) const;
    size_type find (const BcmBasicString& str, size_type pos = 0) const;
    BcmBasicString substr (size_type pos = 0, size_type n = 0) const;
    void swap (BcmBasicString &str);
    void clear();
    ostream &Print(ostream &outStream) const;
    
    /* V2 specific methods, not STL string replacements */
    
    // Accepts an ASCII-formatted text string, converts to hex buffer.  
    // For exampe, the string "01 02 0A FF" (12 byte, NULL terminated ASCII 
    // string) will be converted into a 4 byte octet buffer 
    // {0x01, 0x02, 0x0A, 0xFF}.  Originally designed for accepting console 
    // input which is formatted as HEX.
    int ParseHexBufferFromAscii (const char *pHexString);
    
    // Convert to/from BcmOctetBuffer object
    BcmBasicString (const BcmOctetBuffer &Buffer);
    BcmBasicString &operator = (const BcmOctetBuffer &Buffer);
    BcmBasicString &Set (const BcmOctetBuffer &Buffer);
    void Get (BcmOctetBuffer &Buffer) const;
    bool WriteTo(BcmOctetBuffer &octetBuffer) const;
    bool ReadFrom(const BcmOctetBuffer &octetBuffer, unsigned int &offset, unsigned int numBytes);
    
    // Convert to/from BcmIpAddress object
    BcmBasicString (const BcmIpAddress &IP, bool DottedDecimal = false);
    BcmBasicString &operator = (const BcmIpAddress &IP);
    BcmBasicString &Set (const BcmIpAddress &IP, bool DottedDecimal=false);
    bool Get (BcmIpAddress &IP, bool DottedDecimal=false) const;
    
    // Convert to/from BcmMacAddress object
    BcmBasicString (const BcmMacAddress &MAC, bool Ascii = false);
    BcmBasicString &operator = (const BcmMacAddress &IP);
    BcmBasicString &Set (const BcmMacAddress &MAC, bool Ascii=false);
    bool Get (BcmMacAddress &MAC, bool Ascii=false) const;

    // Eliminate need to cast unsigned character operations.  We have
    // been bitten many times by this in the past!  Particularly when
    // forgetting to cast a call to the signed version of the constructor,
    // for some reason the compiler will wind up picking a different
    // version of the constructor, with the result that the string winds up
    // being 4 bytes long instead of the length specified in the 'n' argument.
    BcmBasicString (const unsigned char *us, size_type n);    
    BcmBasicString &assign (const unsigned char *us, size_type n);
    BcmBasicString& append(const unsigned char *us, size_type n);
   
  protected:
    basic_string <E> fString;
};

inline ostream &operator << (ostream &outStream, const BcmBasicString &String)
{
    return String.Print (outStream);
}

#endif
