//****************************************************************************
//
//  Copyright (c) 1999  Broadcom Corporation
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
//
//  Filename:       typedefs.h
//  Author:         David Pullen
//  Creation Date:  Feb 8, 1999
//
//****************************************************************************
//  Description:
//      Contains platform-specific types that map to a specific memory width,
//      as well as other useful types.  This is one of very few instances of
//      conditional compile code in the system (at least, it should be).
//
//****************************************************************************

#ifndef TYPEDEFS_H
#define TYPEDEFS_H

//********************** Compiler-specific stuff *****************************

#ifdef WIN32

    // Place pragmas and other things that are specific to the Microsoft Visual C++
    // compiler here.
#   pragma warning( disable : 4786 4800 4305 4097 4100 4127)

    // Define TRUE and FALSE
    #ifndef FALSE
    #define FALSE 0
    #endif
    
    #ifndef TRUE
    #define TRUE 1
    #endif

#endif

#if (__DCPLUSPLUS__ == 1)

    // Place pragmas and other things that are specific to the Diab Data C++
    // compiler here.

#endif

//********************** Include Files ***************************************

// This is primarily for the definition of NULL.                            
#include <stdlib.h>

#ifndef __WATCOM_CPLUSPLUS__
#ifdef __cplusplus
#   include <iostream>
using namespace std; 
#endif
#endif

//********************** Global Types ****************************************

// These are the values that must be used for defining PROCESSOR_TYPE.  This
// can be done either in the makefile or on the command line (e.g. 
// -DPROCESSOR_TYPE=MIPS), but should not be done in any of the source files.
//
#define M68360    0
#define MPC850    1
#define BCMMIPS   2
#define INTEL     3
#define BCMMIPSEL 4

#ifndef PROCESSOR_TYPE

    // Make sure that PROCESSOR_TYPE is defined, so that we know how to set the
    // types.
#   error "PROCESSOR_TYPE is not defined!  It must be defined as one of the supported values in typedefs.h!\
     E.g. -DPROCESSOR_TYPE=MPC850 should be on the compiler's command line."

#   elif ((PROCESSOR_TYPE == BCMMIPS) || (PROCESSOR_TYPE == MPC850) || \
          (PROCESSOR_TYPE == M68360) || (PROCESSOR_TYPE == INTEL)   || \
          (PROCESSOR_TYPE == BCMMIPSEL))

    // This guards against the similarly named types in the V2 BSP.  We need to
    // unify the typedefs for both the BSP and the V2 app code!
#   ifndef BCMTYPES_H
        typedef unsigned char  byte;
        typedef unsigned char  uint8;
        typedef unsigned short  uint16;
        typedef unsigned long   uint32;
        typedef unsigned long  BcmHandle;

        typedef unsigned long long  uint64;
        typedef signed char     int8;
        typedef signed short    int16;
        typedef signed long     int32;
        typedef signed long long    int64;
#   endif

        // UNFINISHED!  This needs to be deleted!!!!!  It is not a valid reason
        // to define a new type.
        typedef unsigned int uint;

#else

    // Catch-all for other values that aren't supported.
#   error "PROCESSOR_TYPE must be defined as one of the supported values in typedefs.h!\
      E.g. -DPROCESSOR_TYPE=MPC850 should be on the compiler's command line."

#endif



// simulate C++ bool if compiler does not provide it.  most of ours do...
#ifdef __WATCOMC__
    #ifndef __BOOL_DEFINED
    #define __BOOL_DEFINED
    
       // further qualify this due to definition of bool in some WATCOM support code (ie: curses.h)
    #  ifndef CXX_BUILTIN_BOOL
          typedef int bool;
    #  else
    #     if CXX_BUILTIN_BOOL
             typedef int bool;
    #     endif
    #  endif
    
    #define true 1
    #define false 0
    #endif
#endif

#if defined(TARGETOS_eCos)
    #ifndef FALSE
    #define FALSE 0
    #endif
    
    #ifndef TRUE
    #define TRUE 1
    #endif
    
    using namespace std;
#endif

#if defined(__linux__)

    #define strnicmp strncasecmp

    #ifndef FALSE
    #define FALSE 0
    #endif

    #ifndef TRUE
    #define TRUE 1
    #endif

#endif

typedef enum
{
    kNo = 0,
    kYes = 1
} YesNo;

// These can be used by various modules to determine whether or not byte endian
// swapping must be performed when going to/from network byte order.
#if ((PROCESSOR_TYPE == INTEL) || (PROCESSOR_TYPE == BCMMIPSEL)) 

#   define BCM_LITTLE_ENDIAN 1

#else

#   define BCM_BIG_ENDIAN 1

#endif

//********************** Global Constants ************************************

// This guards against the similarly named types in the V2 BSP.  We need to
// unify the typedefs for both the BSP and the V2 app code!
#ifndef BCMTYPES_H

#ifdef __cplusplus
    // Defining these as constants (rather than #defines) allows the compiler to
    // do type-checking.
    const int16 MAX_INT16 = 32767;
    const int16 MIN_INT16 = -32768;
#else
    #define MAX_INT16    32767
    #define MIN_INT16   -32768
#endif

#endif

// These are time constants (in milliseconds) that can be used as parameters to
// various OS methods that expect a time value (Wait(), Sleep(), Timer Start(),
// etc.).
enum TimeConstantsMs
{
    kOneMillisecond = 1,
    kOneHundredthSecond = 10,
    kOneTenthSecond = 100,
    kOneQuarterSecond = 250,
    kOneThirdSecond = 333,
    kOneHalfSecond = 500,
    kOneSecond = 1000,
    kOneMinute = 60 * 1000,
    kOneHour = 60 * 60 * 1000,
    kOneDay = 24 * 60 * 60 * 1000
};

//********************** Global Variables ************************************

//********************** Class Declaration ***********************************

//********************** Inline Method Implementations ***********************

#ifndef __WATCOM_CPLUSPLUS__
#ifdef __cplusplus 

#if (__GNUC__ >= 3)
// GNU C++ 3.x has built in bool type and extraction operator for bool.
#else
inline ostream & operator << (ostream &outputStream, bool value)
{
    if (value)
    {
        return outputStream << "true";
    }

    return outputStream << "false";
}
#endif

inline ostream & operator << (ostream &outputStream, YesNo value)
{
    if (value)
    {
        return outputStream << "yes";
    }

    return outputStream << "no";
}

#endif
#endif



#ifdef __cplusplus 
#   if (__DCPLUSPLUS__ == 1)

        // This is as specified in the Diab Data C++ language user's manual (P. 82).
        extern "C" void __ERROR__(char *, ...);

#       define COMPILER_TYPESIZE_ASSERT(test) if ((test)) __ERROR__("Type size assertion failed - " #test)

#   else

#       include <stdio.h>
#       include <assert.h>

        // This handles other compilers...it doesn't really map to a compile-time check.
#       define COMPILER_TYPESIZE_ASSERT(test) assert(!(test))

#   endif

    // This function simply makes sure that the width-specific types actually are
    // the correct size.  If not, then the compiler will abort with an error.  This
    // function is never called, and will be removed by the linker so that it
    // consumes no code space.
    //
    // Parameters:  None.
    //
    // Returns:  Nothing.
    //
    static inline void CompileTimeTypeSizeCheck(void)
    {
        COMPILER_TYPESIZE_ASSERT(sizeof(byte)   != 1);
        COMPILER_TYPESIZE_ASSERT(sizeof(uint8)  != 1);
        COMPILER_TYPESIZE_ASSERT(sizeof(uint16) != 2);
        COMPILER_TYPESIZE_ASSERT(sizeof(uint32) != 4);
    
        COMPILER_TYPESIZE_ASSERT(sizeof(int8)  != 1);
        COMPILER_TYPESIZE_ASSERT(sizeof(int16) != 2);
        COMPILER_TYPESIZE_ASSERT(sizeof(int32) != 4);
    }

#endif

// Cast an integer to a void pointer so it will print in hex.
#define inHex(x) ((void*)(uint32)(x))

#endif

