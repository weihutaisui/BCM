//****************************************************************************
//
// Copyright (c) 2002-2008 Broadcom Corporation
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
//  Filename:       Counter64.h
//  Author:         Kevin O'Neal/David Pullen
//  Creation Date:  October 28, 2002
//
//****************************************************************************

#ifndef Counter64_H
#define Counter64_H

/* "C" friendly 64 bit counter type */
typedef struct _Counter64
{
  unsigned long Hi;
  unsigned long Lo;
} Counter64;

/* Macros for 64 bit counters */
#define C64_SET(N, H, L)    ((N)->Hi = (H), (N)->Lo = (L))
#define C64_LO(N)           (N)->Lo
#define C64_HI(N)           (N)->Hi
#define C64_ZERO(N)         ((N)->Hi = 0, (N)->Lo = 0)
#define C64_INCR(N)         {if (++((N)->Lo) == 0) (N)->Hi++;}
#define C64_ADD32(N, I)     {(N)->Lo += I; if ((N)->Lo < I) (N)->Hi++;}
#define C64_ADD64(N, M)     {(N)->Lo += (M)->Lo; (N)->Hi += (M)->Hi; \
                            if ((N)->Lo < (M)->Lo) (N)->Hi++;}
#define C64_SUB64(X, Y, Z)  {(X)->Lo = (Y)->Lo - (Z)->Lo; \
                            (X)->Hi = (Y)->Hi - (Z)->Hi; \
                            if ((X)->Lo > (Y)->Lo) (X)->Hi--; }
#define C64_COPY(X, Y)      {(X)->Lo = (Y)->Lo; (X)->Hi = (Y)->Hi;}
#define C64_COMPARE(X, Y)   {(((X)->Hi == (Y)->Hi) ? \
                            (((X)->Lo == (Y)->Lo)  ? (0) : (((X)->Lo > (Y)->Lo) ? (1) : (-1))) : \
                            (((X)->Hi > (Y)->Hi) ? (1) : (-1)));}
                            
#if defined (__cplusplus)

//********************** Include Files ***************************************

#include "typedefs.h"

//********************** Global Types ****************************************

//********************** Global Constants ************************************

//********************** Global Variables ************************************

//********************** Forward Declarations ********************************

//********************** Class Declaration ***********************************


/**
*      This is a numerical class that represents a 64-bit number.  It is most
*      often used as a counter, and was originally used for the ifXTable MIB
*      counters, which is where the name came from.  This is a very shallow
*      implementation, with the bare minimum numerical operations that we
*      need, most of which are implemented inline.
*
*      This class was originally implemented by Kevin O'Neal in CoreObjs.h/cpp
*      in the SNMP directory.
*
*      The class was moved to the common directory and 'improved' by David
*      Pullen to support other uses.  I decided against renaming it because of
*      the rather large amount of existing code that uses it, and because this
*      isn't really a generic number class.  It's pretty much just designed
*      for counting.
*/
class BcmCounter64
{
public:

    /// Default constructor.
    ///
    /// \param
    ///     lowValue - the low 32 bits.
    /// \param
    ///     highValue - the upper 32 bits.
    ///
    inline BcmCounter64(uint32 lowValue = 0, uint32 highValue = 0);
    
    /// Alternate constructor.
    ///
    /// \param
    ///     c64 - a Counter64 structure.
    ///
    inline BcmCounter64(const Counter64 &value);

    /// Copy constructor.
    ///
    /// \param
    ///     value - the instance to be copied.
    ///
    inline BcmCounter64(const BcmCounter64 &value);

    /// Destructor.
    ///
    inline ~BcmCounter64();

    //@{
    /// Accessors for the high and low uint32 values.
    ///
    /// \return
    ///     The high/low uint32 value.
    ///
    inline uint32 High32(void) const;
    inline uint32 Low32(void) const;
    //@}

    /// Helper to store 64 bits of data.
    ///
    /// \param
    ///     lowValue - the low 32 bits.
    /// \param
    ///     highValue - the upper 32 bits.
    ///
    /// \return
    ///     A reference to this object.
    ///
    inline BcmCounter64 &Set(uint32 lowValue, uint32 highValue);
    
    inline BcmCounter64 &UpdateWithRolloverCheck (uint32 value32);

    //@{
    /// Assignment operators.  These should be sufficient; most values can be
    /// cast up to a uint32 safely.
    ///
    /// \param
    ///     value - the value to be stored.
    ///
    /// \return
    ///     A reference to this object.
    ///
    inline BcmCounter64 &operator = (const BcmCounter64 &value);
    inline BcmCounter64 &operator = (uint32 value);
    //@}

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
    inline BcmCounter64 &operator += (const BcmCounter64 &value);
    inline BcmCounter64 &operator += (uint32 value);
    inline BcmCounter64 &operator ++ ();
    inline BcmCounter64 &operator ++ (int);
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
    inline BcmCounter64 &operator -= (const BcmCounter64 &value);
    inline BcmCounter64 &operator -= (uint32 value);
    inline BcmCounter64 &operator -- ();
    inline BcmCounter64 &operator -- (int);
    //@}

    /// Generic comparison helper, returns values similar to strcmp.
    ///
    /// \param
    ///     value - the value to be compared.
    ///
    /// \retval
    ///     <0 if this object is less than the one specified.
    /// \retval
    ///     0 if this object is equal to the one specified.
    /// \retval
    ///     >0 if this object is greater than the one specified.
    ///
    inline int Compare (const BcmCounter64 &value) const;

    //@{
    /// Specific comparison operators, all of which call the generic comparison
    /// helper.
    ///
    /// \param
    ///     value - the value to be compared.
    ///
    /// \retval
    ///     true if the comparison operation is valid.
    /// \retval
    ///     false if the comparison fails.
    ///
    inline bool operator == (const BcmCounter64 &value) const;
    inline bool operator != (const BcmCounter64 &value) const;
    inline bool operator <  (const BcmCounter64 &value) const;
    inline bool operator <= (const BcmCounter64 &value) const;
    inline bool operator >  (const BcmCounter64 &value) const;
    inline bool operator >= (const BcmCounter64 &value) const;
    //@}

    //@{
    /// Specific comparison operators for uint32.
    ///
    /// \param
    ///     value - the value to be compared.
    ///
    /// \retval
    ///     true if the comparison operation is valid.
    /// \retval
    ///     false if the comparison fails.
    ///
    inline bool operator == (uint32 value) const;
    inline bool operator != (uint32 value) const;
    inline bool operator <  (uint32 value) const;
    inline bool operator <= (uint32 value) const;
    inline bool operator >  (uint32 value) const;
    inline bool operator >= (uint32 value) const;
    //@}

    /// Prints to the specified ostream.
    ///
    /// \param
    ///     outStream - the ostream to print to.
    ///
    /// \return
    ///     A reference to the ostream.
    ///
    ostream &Print(ostream &outStream) const;

private:

    /// The lower and upper 32 bits of the value.
    uint32 fLowValue, fHighValue;
};


//********************** Inline Method Implementations ***********************

inline ostream & operator << (ostream &outStream, const BcmCounter64 &counter)
{
    return counter.Print(outStream);
}

inline BcmCounter64::BcmCounter64(uint32 lowValue, uint32 highValue) :
    fLowValue(lowValue),
    fHighValue(highValue)
{
    // Nothing else to do...
}

inline BcmCounter64::BcmCounter64(const Counter64 &value) :
    fLowValue(value.Lo),
    fHighValue(value.Hi)
{
    // Nothing else to do...
}

inline BcmCounter64::BcmCounter64(const BcmCounter64 &value)
{
    // Use operator = for the implementation.
    *this = value;
}

inline BcmCounter64::~BcmCounter64()
{
    // Nothing to do...
}

inline uint32 BcmCounter64::High32(void) const
{
    return fHighValue;
}

inline uint32 BcmCounter64::Low32(void) const
{
    return fLowValue;
}

inline BcmCounter64 &BcmCounter64::Set(uint32 lowValue, uint32 highValue)
{
    fLowValue = lowValue;
    fHighValue = highValue;

    return *this;
}

inline BcmCounter64 &BcmCounter64::UpdateWithRolloverCheck (uint32 value32)
{
    if (value32 < fLowValue)
      fHighValue++;
      
    fLowValue = value32;
    
    return *this;
}


inline BcmCounter64 &BcmCounter64::operator = (const BcmCounter64 &value)
{
    fLowValue = value.Low32();
    fHighValue = value.High32();

    return *this;
}

inline BcmCounter64 &BcmCounter64::operator = (uint32 value)
{
    fLowValue = value;
    fHighValue = 0;

    return *this;
}

inline BcmCounter64 &BcmCounter64::operator += (const BcmCounter64 &value)
{
    fHighValue += value.High32();
    fLowValue += value.Low32();

    // If the low value wrapped around to 0 (so that it is less than the other
    // value), then cary to the high value.
    if (fLowValue < value.Low32())
    {
        fHighValue++;
    }

    return *this;
}

inline BcmCounter64 &BcmCounter64::operator += (uint32 value)
{
    fLowValue += value;

    // If the low value wrapped around to 0 (so that it is less than the other
    // value), then cary to the high value.
    if (fLowValue < value)
    {
        fHighValue++;
    }

    return *this;
}

inline BcmCounter64 &BcmCounter64::operator ++ ()
{
    // Use counter += 1 as the implementation.
    return operator += (1);
}

inline BcmCounter64 &BcmCounter64::operator ++ (int)
{
    // Use counter += 1 as the implementation.
    return operator += (1);
}

inline BcmCounter64 &BcmCounter64::operator -= (const BcmCounter64 &value)
{
    fHighValue -= value.High32();

    // If the low value would wrap around from 0 (so that it is greater than the
    // other value), then borrow from the high value.
    if (fLowValue < value.Low32())
    {
        fHighValue--;
    }

    fLowValue -= value.Low32();

    return *this;
}

inline BcmCounter64 &BcmCounter64::operator -= (uint32 value)
{
    // If the low value would wrap around from 0 (so that it is greater than the
    // other value), then borrow from the high value.
    if (fLowValue < value)
    {
        fHighValue--;
    }

    fLowValue -= value;

    return *this;
}

inline BcmCounter64 &BcmCounter64::operator -- ()
{
    // Use counter -= 1 as the implementation.
    return operator -= (1);
}

inline BcmCounter64 &BcmCounter64::operator -- (int)
{
    // Use counter -= 1 as the implementation.
    return operator -= (1);
}

inline int BcmCounter64::Compare(const BcmCounter64 &value) const
{
    // If the high values are equal, then only the low value matters.  Otherwise
    // only the high value matters.
    //
    // In either case, we need to return 0 if the values are the same, less than
    // 0 if my value is less than the other value, or greater than 0 if my value
    // is greater than the other value.
    //
    // We do this by subtracting the other value from my value, and casting this
    // as an int.  If the values are the same, then this will be 0, so no
    // problems.
    //
    // If my value is less, then this will wrap around making the upper bit a
    // 1; when we cast this as an int, this gets treated as a negative value.
    // For example:
    //      0 - 1 = 0xffffffff = -1 as an int.
    //      5 - 10 = 0xfffffffb = -5 as an int.
    //
    // If my value is greater, then this will produce a positive number.
    // For example:
    //      10 - 5 = 5, even as an int.
    //
/*    
    if (fHighValue == value.High32())
    {
        return(int) (fLowValue - value.Low32());
    }
    
    return (int) (fHighValue - value.High32());
*/    

    // KO: The above doesn't work if we start getting into values where the 
    // upper bit is set, because we get confused between signed & unsigned.
    // Example: 0xFFFFFFFF - 0 = 0xFFFFFFFF, which looks like -1 as an int,
    // thus incorrectly reporting 0xFFFFFFFF as < 0.
    //
    // Take a different approach.  Not as clever or as fast, but it works.
    
    // If the high values are equal, then only the low value matters.
    if (fHighValue == value.High32())
    {
      if (fLowValue == value.Low32())
        return 0;
      else if (fLowValue > value.Low32())
        return 1;
      return -1;
    }
    
    // Otherwise only the high value matters.
    if (fHighValue > value.High32())
      return 1;
    return -1;
}

inline bool BcmCounter64::operator == (const BcmCounter64 &value) const
{
    if (Compare(value) == 0)
    {
        return true;
    }

    return false;
}

inline bool BcmCounter64::operator != (const BcmCounter64 &value) const
{
    if (Compare(value) != 0)
    {
        return true;
    }

    return false;
}

inline bool BcmCounter64::operator <  (const BcmCounter64 &value) const
{
    if (Compare(value) < 0)
    {
        return true;
    }

    return false;
}

inline bool BcmCounter64::operator <= (const BcmCounter64 &value) const
{
    if (Compare(value) > 0)
    {
        return false;
    }

    return true;
}

inline bool BcmCounter64::operator >  (const BcmCounter64 &value) const
{
    if (Compare(value) > 0)
    {
        return true;
    }

    return false;
}

inline bool BcmCounter64::operator >= (const BcmCounter64 &value) const
{
    if (Compare(value) < 0)
    {
        return false;
    }

    return true;
}

inline bool BcmCounter64::operator == (uint32 value) const
{
    return (*this == BcmCounter64(value));
}

inline bool BcmCounter64::operator != (uint32 value) const
{
    return (*this != BcmCounter64(value));
}

inline bool BcmCounter64::operator <  (uint32 value) const
{
    return (*this < BcmCounter64(value));
}

inline bool BcmCounter64::operator <= (uint32 value) const
{
    return (*this <= BcmCounter64(value));
}

inline bool BcmCounter64::operator >  (uint32 value) const
{
    return (*this > BcmCounter64(value));
}

inline bool BcmCounter64::operator >= (uint32 value) const
{
    return (*this >= BcmCounter64(value));
}

#endif // __cplusplus

#endif


