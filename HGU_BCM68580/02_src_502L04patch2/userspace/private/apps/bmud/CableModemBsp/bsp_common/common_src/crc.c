/***************************************************************************
//
//    Copyright 2001 Broadcom Corporation
//    All Rights Reserved
//    No portions of this material may be reproduced in any form without the
//    written permission of:
//             Broadcom Corporation
//             16251 Laguna Canyon Road
//             Irvine, California  92618
//    All information contained in this document is Broadcom Corporation
//    company private, proprietary, and trade secret.
//
// *************************************************************************
//    Filename: crc.c
//    Author:   Dannie Gay
//    Creation Date: ???
//
// *************************************************************************
//    Description:
//
//    Helper routines to calculate various CRC algorithms on buffers of data.
//
//		
// *************************************************************************/


/************************ Include Files ************************************/

#include "crc.h"

/************************ Local Types **************************************/

/************************ Local Constants **********************************/

/* NOTE:  These were pulled from Common\typedefs.h; they need to be pulled into
          a common header file that can be included from both the BSP and the
          app. */
#define M68360    0
#define MPC850    1
#define BCMMIPS   2
#define INTEL     3
#define BCMMIPSEL 4

#ifndef PROCESSOR_TYPE

/* Make sure that PROCESSOR_TYPE is defined */
#   error "PROCESSOR_TYPE is not defined!  It must be defined as one of the supported values in typedefs.h!\
     E.g. -DPROCESSOR_TYPE=MPC850 should be on the compiler's command line."

#endif

/* If this is a big-endian processor, then allow the super fast CRC32 to be
   used. */
#if ((PROCESSOR_TYPE != INTEL) && (PROCESSOR_TYPE != BCMMIPSEL)) 	

#if !defined(WIN32)
    #define USE_SUPER_FAST_CRC32
    // #warning Including Super Fast CRC32 code.
#else
    #warning Not including Super Fast CRC32 code (little endian processor).
#endif
#endif

/* Used for generating the fast CRC32 table. */
#define QUOTIENT 0x04c11db7

/************************ Local Variables **********************************/

/* This is the table for FastCrc32(). */
static unsigned long gFastCrc32Table[256];

/* This tells whether or not the table has been initialized. */
static int gFastCrc32TableInitialized = 0;

/************************ Function Implementations *************************/

/* This initializes the Fast CRC32 table. */
static void FastCrc32Init(void)
{
    unsigned int i, j;
    unsigned long crc;

    for (i = 0; i < 256; i++)
    {
        crc = i << 24;

        for (j = 0; j < 8; j++)
        {
            if (crc & 0x80000000)
            {
                crc = (crc << 1) ^ QUOTIENT;
            }
            else
            {
                crc = crc << 1;
            }
        }

        gFastCrc32Table[i] = crc;
    }
}


/* 
   This is a very fast, table-based implementation of CRC_32.  You should
   call this instead of compute_crc for CRC_32.  It returns the CRC, rather
   than requiring you to pass tons of arguments. 
   
   There is a second optimization to this version thanks to a public-domain
   white paper from Richard Black.  As of August 16, 2001, Richard does not
   require any licensing agreements or fees; if he changes his mind, then we
   can easily remove his optimizations.
   
   I have modified his algorithm for our platform (which is big endian), and to
   remove one of his restrictions (that the buffer size be a multiple of 4; we
   rarely run CRC32 on buffers that meet this requirement).
   
   See www.cl.cam.ac.uk/Research/SRG/bluebook/21/crc/node6.html for descriptions
   of the fast CRC32 algorithm, as well as his optimizations.  His optimization
   is copyright (c) 1993 Richard Black.  All rights are reserved (by him).
*/
unsigned long FastCrc32(const void *pData, unsigned int numberOfBytes)
{
    unsigned long result;
    
    /* Make sure my table has been initialized. */
    if (gFastCrc32TableInitialized == 0)
    {
        FastCrc32Init();

        gFastCrc32TableInitialized = 1;
    }

    /* UNFINISHED - what do I do if there are less than 4 bytes?
    if (numberOfBytes < 4)
    {
        return 0;
    }*/

    #ifdef USE_SUPER_FAST_CRC32
    /* If the data pointer is aligned on a 32-bit boundary, and this is a big
       endian platform, then we can use the modified version of Richard Black's
       optimized CRC32. */
    if ((((unsigned long) pData) & 0x03) == 0)
    {
        const unsigned long *pDataUint32 = (const unsigned long *) pData;
        const unsigned long *pDataEnd = (pDataUint32 + (numberOfBytes / 4));
        int numExtraBytes = numberOfBytes & 0x03;
    
        result = ~*pDataUint32++;

        while (pDataUint32 < pDataEnd)
        {
            result = gFastCrc32Table[result >> 24] ^ result << 8;
            result = gFastCrc32Table[result >> 24] ^ result << 8;
            result = gFastCrc32Table[result >> 24] ^ result << 8;
            result = gFastCrc32Table[result >> 24] ^ result << 8;
    
            result ^= *pDataUint32++;
        }
    
        // Handle any extra bytes that don't fit in a dword.
        if (numExtraBytes > 0)
        {
            const unsigned char *pDataUint8 = (unsigned char *) pData;
    
            pDataUint8 += numberOfBytes - numExtraBytes;
    
            while (numExtraBytes > 0)
            {
                result = (result << 8 | *pDataUint8++) ^ gFastCrc32Table[result >> 24];
    
                numExtraBytes--;
            }
        }
    
        // Handle the assumed 4 bytes of 0.
        result = gFastCrc32Table[result >> 24] ^ result << 8;
        result = gFastCrc32Table[result >> 24] ^ result << 8;
        result = gFastCrc32Table[result >> 24] ^ result << 8;
        result = gFastCrc32Table[result >> 24] ^ result << 8;
    }
    else
    #endif
    {
        const unsigned char *pDataUint8 = (const unsigned char *) pData;

        /* This is the slower CRC32 version (relatively speaking) that handles
           unaligned buffers. */
        result = *pDataUint8++ << 24;
        result |= *pDataUint8++ << 16;
        result |= *pDataUint8++ << 8;
        result |= *pDataUint8++;
        result = ~result;
        numberOfBytes -=4;

        while (numberOfBytes > 0)
        {
            result = (result << 8 | *pDataUint8++) ^ gFastCrc32Table[result >> 24];

            numberOfBytes--;
        }
    
        /* There is an assumption in the algorithm that there are 4 bytes of 0 at
           the end, which is where the CRC would be placed.  We need to do this
           ourselves because our buffers never have the four 0 bytes at the end. */
        result = (result << 8) ^ gFastCrc32Table[result >> 24];
        result = (result << 8) ^ gFastCrc32Table[result >> 24];
        result = (result << 8) ^ gFastCrc32Table[result >> 24];
        result = (result << 8) ^ gFastCrc32Table[result >> 24];
    }

    return ~result;
}


// This is a modified version of the fast CRC32 algorithm.  It accepts data
// one block at a time, instead of all at once.  It would usually be called
// like this:
//
// remaining = numberOfBytes;
// chunkSize = min( remaining, 64*1024 );
// result = FastCrc32First( pData, chunkSize );
// remaining -= chunkSize;
// while ( remaining > 0 )
// {
//     chunkSize = min( remaining, 64*1024 );
//     result = FastCrc32More( result, pData, chunkSize );
//     remaining -= chunkSize;
// }
// result = FastCrc32Finish( result );
unsigned long FastCrc32First( const void *pData, unsigned int numberOfBytes )
{
    unsigned long result;
    
    /* Make sure my table has been initialized. */
    if (gFastCrc32TableInitialized == 0)
    {
        FastCrc32Init();

        gFastCrc32TableInitialized = 1;
    }

    /* UNFINISHED - what do I do if there are less than 4 bytes?
    if (numberOfBytes < 4)
    {
        return 0;
    }*/

    #ifdef USE_SUPER_FAST_CRC32
    /* If the data pointer is aligned on a 32-bit boundary, and this is a big
       endian platform, then we can use the modified version of Richard Black's
       optimized CRC32. */
    if ((((unsigned long) pData) & 0x03) == 0)
    {
        const unsigned long *pDataUint32 = (const unsigned long *) pData;
        const unsigned long *pDataEnd = (pDataUint32 + (numberOfBytes / 4));
        int numExtraBytes = numberOfBytes & 0x03;
    
        result = ~*pDataUint32++;

        while (pDataUint32 < pDataEnd)
        {
            result = gFastCrc32Table[result >> 24] ^ result << 8;
            result = gFastCrc32Table[result >> 24] ^ result << 8;
            result = gFastCrc32Table[result >> 24] ^ result << 8;
            result = gFastCrc32Table[result >> 24] ^ result << 8;
    
            result ^= *pDataUint32++;
        }
        // Handle any extra bytes that don't fit in a dword.
        if (numExtraBytes > 0)
        {
            const unsigned char *pDataUint8 = (unsigned char *) pData;
    
            pDataUint8 += numberOfBytes - numExtraBytes;
    
            while (numExtraBytes > 0)
            {
                result = (result << 8 | *pDataUint8++) ^ gFastCrc32Table[result >> 24];
    
                numExtraBytes--;
            }
        }
    
    }
    else
    #endif
    {
        const unsigned char *pDataUint8 = (const unsigned char *) pData;

        /* This is the slower CRC32 version (relatively speaking) that handles
           unaligned buffers. */
        result  = *pDataUint8++ << 24;
        result |= *pDataUint8++ << 16;
        result |= *pDataUint8++ << 8;
        result |= *pDataUint8++;
        result = ~result;
        numberOfBytes -=4;

        while (numberOfBytes > 0)
        {
            result = (result << 8 | *pDataUint8++) ^ gFastCrc32Table[result >> 24];

            numberOfBytes--;
        }
    }
    return ~result;
}


// This is a modified version of the fast CRC32 algorithm.  It accepts data
// one block at a time, instead of all at once.
unsigned long FastCrc32More( unsigned long oldResult, const void *pData, unsigned int numberOfBytes )
{
    unsigned long result;
    
    result = ~oldResult;

    #ifdef USE_SUPER_FAST_CRC32
    /* If the data pointer is aligned on a 32-bit boundary, and this is a big
       endian platform, then we can use the modified version of Richard Black's
       optimized CRC32. */
    if ((((unsigned long) pData) & 0x03) == 0)
    {
        const unsigned long *pDataUint32 = (const unsigned long *) pData;
        const unsigned long *pDataEnd = (pDataUint32 + (numberOfBytes / 4));
        int numExtraBytes = numberOfBytes & 0x03;
    
        while (pDataUint32 < pDataEnd)
        {
            result = gFastCrc32Table[result >> 24] ^ result << 8;
            result = gFastCrc32Table[result >> 24] ^ result << 8;
            result = gFastCrc32Table[result >> 24] ^ result << 8;
            result = gFastCrc32Table[result >> 24] ^ result << 8;
    
            result ^= *pDataUint32++;
        }
        // Handle any extra bytes that don't fit in a dword.
        if (numExtraBytes > 0)
        {
            const unsigned char *pDataUint8 = (unsigned char *) pData;
    
            pDataUint8 += numberOfBytes - numExtraBytes;
    
            while (numExtraBytes > 0)
            {
                result = (result << 8 | *pDataUint8++) ^ gFastCrc32Table[result >> 24];
    
                numExtraBytes--;
            }
        }
    
    }
    else
    #endif
    {
        const unsigned char *pDataUint8 = (const unsigned char *) pData;

        /* This is the slower CRC32 version (relatively speaking) that handles
           unaligned buffers. */

        while (numberOfBytes > 0)
        {
            result = (result << 8 | *pDataUint8++) ^ gFastCrc32Table[result >> 24];

            numberOfBytes--;
        }
    }
    return ~result;
}


// This is a modified version of the fast CRC32 algorithm.  It accepts data
// one block at a time, instead of all at once.
unsigned long FastCrc32Finish( unsigned long oldResult )
{
    unsigned long result;
    
    result = ~oldResult;

    /* There is an assumption in the algorithm that there are 4 bytes of 0 at
       the end, which is where the CRC would be placed.  We need to do this
       ourselves because our buffers never have the four 0 bytes at the end. */
    result = (result << 8) ^ gFastCrc32Table[result >> 24];
    result = (result << 8) ^ gFastCrc32Table[result >> 24];
    result = (result << 8) ^ gFastCrc32Table[result >> 24];
    result = (result << 8) ^ gFastCrc32Table[result >> 24];

    return ~result;
}


int compute_crc(const void *in2_str, 
                PolyType polynomial_name,
                int Length,
                unsigned long *crc_word, 
                int reverse) 
{
    int hex_index=0;
    int old_hex_index=0; 
    int stop=0; 
    unsigned long value=0;  
    int digits=0; 
    int shift_count=0; 
    int max_shift_count=0;  
    unsigned long rvalue=0;  
    unsigned long data_word=0;
    unsigned long polynomial_value=0x00000000;  
    unsigned long xor_mask=0x00000000; 
    const unsigned char *DataPtr;
    int Counter;
    int i;

    DataPtr = (const unsigned char *) in2_str;
    *crc_word=0xffffffff; 

    if (polynomial_name == CRC_CCITT)    // Header checksum
    {
        digits=4; 
        polynomial_value=0x00001021; 
    }
    else if (polynomial_name == CRC_32)  // Ethernet CRC
    {
        digits=8; 
        polynomial_value=0x04c11db7; 
    }
    else
        return(-1);

    max_shift_count = digits*4; 

    Counter = 0;
    /* Read in hex digits : */ 
    while ((Counter < Length) || !stop)
    {
        if (Counter < Length)
            value = *DataPtr;
        else
        {
            stop=1; 
            value=0; 
            data_word=data_word<<((digits-(hex_index%digits))*4-8); 
            max_shift_count=(hex_index%digits)*4;  
            old_hex_index=hex_index; 
            hex_index=digits-2; 
        }

        if (reverse)
        {
            rvalue = 0;
            for (i=0; i<8; i++)
            {
                if (value & 0x01)
                    rvalue = rvalue | 0x01;
                value = value >> 1;
                rvalue = rvalue << 1;
            }
            rvalue = rvalue >> 1;
            value = rvalue;
        }

        data_word=data_word<<8; 
        data_word+=value; 

        if (hex_index%digits==(digits-2))
        {
            //if (polynomial_name == CRC_CCITT)
            //   Print("%04x ", (data_word & 0xffff));
            //else
            //   Print("%08lx ", data_word);

            /* Last hex digit has been reached - process CRC : */   

            for (shift_count=0; shift_count<max_shift_count; shift_count++)
            {
                xor_mask=((data_word&(1L<<(digits*4-1)))^
                          (*crc_word&(1L<<(digits*4-1))))? 
                         polynomial_value: 0L; 

                *crc_word = *crc_word << 1; 
                *crc_word = *crc_word ^ xor_mask; 

                data_word=data_word<<1; 
            } 
        }

        if (!stop)
        {
            hex_index += 2;
            DataPtr++;
            Counter++;
        }
        else
            hex_index=old_hex_index;  
    } 
    *crc_word = ~(*crc_word); 
    value = *crc_word;
    if (reverse)
    {
        rvalue = 0;
        for (i=0; i<32; i++)
        {
            if (value & 0x01)
                rvalue = rvalue | 0x01;
            value = value >> 1;
            if (i != 31)
                rvalue = rvalue << 1;
        }
        *crc_word = (unsigned long) ((rvalue & 0xFF000000)) >> 24 |
                    (unsigned long) ((rvalue & 0x00FF0000) >> 8) |
                    (unsigned long) ((rvalue & 0x0000FF00) << 8) |
                    (unsigned long) ((rvalue & 0x000000FF) << 24);
    }
    //if (polynomial_name == CRC_CCITT)
    //   Print("%04x ", (*crc_word & 0xffff));
    //else
    //   Print("%08lx ", *crc_word);
    return(0);
}

//		compute_dec_checksum
//
//    Computes a simple decimal checksum over the input
//		string - checksum character is return in ch
//
void compute_dec_checksum(const char string[], int n, char * ch)
{
    char c;
    int j,k=0,m=0;
    static int ip[10][8] =
    {
        {0,1,5,8,9,4,2,7},
        {1,5,8,9,4,2,7,0},
        {2,7,0,1,5,8,9,4},
        {3,6,3,6,3,6,3,6},
        {4,2,7,0,1,5,8,9},
        {5,8,9,4,2,7,0,1},
        {6,3,6,3,6,3,6,3},
        {7,0,1,5,8,9,4,2},
        {8,9,4,2,7,0,1,5},
        {9,4,2,7,0,1,5,8}
    };
    static int ij[10][10] = 
    {
        {0,1,2,3,4,5,6,7,8,9}, 
        {1,2,3,4,0,6,7,8,9,5},
        {2,3,4,0,1,7,8,9,5,6}, 
        {3,4,0,1,2,8,9,5,6,7}, 
        {4,0,1,2,3,9,5,6,7,8},
        {5,9,8,7,6,0,4,3,2,1}, 
        {6,5,9,8,7,1,0,4,3,2}, 
        {7,6,5,9,8,2,1,0,4,3},
        {8,7,6,5,9,3,2,1,0,4}, 
        {9,8,7,6,5,4,3,2,1,0}
    };
    // Group multiplication and permutation tables.

    for (j=0;j<n;j++)
    {                 // look at successive characters
        c=string[j];                  
        if (c >= 48 && c <= 57)         // ignore everything except digits
            k=ij[k][ip[(c+2) % 10][7 & m++]];
    }

    for (j=0;j<=9;j++)   // find which appended digit will check properly
        if (!ij[k][ip[j][m & 7]]) break;
    *ch=j+48;   // convert to ascii      
}

#if 0
unsigned char str1[] = {
    0xc1,
    0x0b,
    0x00,
    0x24,
    0x03,
    0x01,
    0x02,
    0x03,
    0x10,
    0x21,
    0x01,
    0x33,
    0x01,
    0x02,
    0x03,
    0x00,
    0x00,
    0x00,
    0x00,
};

unsigned char str2[] = {
    0x01,
    0x02,
    0x03,
    0x04,
    0x05,
    0x06,
    0x07,
    0x08,
    0x09,
    0x0a,
    0x0b,
    0x0c,
    0x0d,
    0x0e,
    0x0f,
    0x10,
    0x11,
    0x12,
    0x13,
    0x14,
    0x15,
    0x16,
    0x17,
    0x18,
    0x19,
    0x1a,
    0x1b,
    0x1c,
    0x1d,
    0x1e,
    0x1f,
    0x20,
    0x21,
    0x22,
    0x23,
    0x24,
    0x25,
    0x26,
    0x27,
    0x28,
    0x00,
    0x00,
    0x00,
    0x28,
};


unsigned char str3[] = {
    0x08,
    0x00,
    0x3E,
    0x03,
    0x04,
    0x06,
    0x00,
    0x60,
    0x97,
    0x50,
    0x2A,
    0x0B,
    0x08,
    0x00,
    0x45,
    0x00,
    0x00,
    0x54,
    0x7C,
    0xD1,
    0x00,
    0x00,
    0xFF,
    0x01,
    0xED,
    0x67,
    0xCF,
    0x5D,
    0xD9,
    0x56,
    0xCF,
    0x5D,
    0xD9,
    0x5D,
    0x08,
    0x00,
    0xCE,
    0xE9,
    0x01,
    0x00,
    0x31,
    0x03,
    0x00,
    0x01,
    0x02,
    0x03,
    0x04,
    0x05,
    0x06,
    0x07,
    0x08,
    0x09,
    0x0A,
    0x0B,
    0x0C,
    0x0D,
    0x0E,
    0x0F,
    0x10,
    0x11,
    0x12,
    0x13,
    0x14,
    0x15,
    0x16,
    0x17,
    0x18,
    0x19,
    0x1A,
    0x1B,
    0x1C,
    0x1D,
    0x1E,
    0x1F,
    0x20,
    0x21,
    0x22,
    0x23,
    0x24,
    0x25,
    0x26,
    0x27,
    0x28,
    0x29,
    0x2A,
    0x2B,
    0x2C,
    0x2D,
    0x2E,
    0x2F,
    0x30,
    0x31,
    0x32,
    0x33,
    0x34,
    0x35,
    0x36,
    0x37,
    0xAC,
    0x66,
    0x98,
    0x74,
};

unsigned char str4[] = {
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x20,
    0x00,
    0x00,
    0x01,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x22,
    0x3c
};

unsigned char str5[] = {
    0xc0,
    0x00,
    0x00,
    0x1b
};


void test_crc(void)
{
    int Length;
    unsigned long crc_word;

#if 0
    compute_crc(str1, CRC_CCITT, 19, &crc_word, 0);
    Print("\nString yielded %04x\n", (crc_word & 0xffff));

    compute_crc(str2, CRC_32, 44, &crc_word, 0);
    Print("\nString yielded %08lx\n", crc_word);

    compute_crc(str3, CRC_32, 98, &crc_word, 1);
    Print("\nString yielded %08lx\n", crc_word);

    compute_crc(str4, CRC_32, 26, &crc_word, 1);
    Print("\nString yielded %08lx\n", crc_word);
#endif

    compute_crc(str5, CRC_CCITT, 4, &crc_word, 0);
    Print("\nString yielded %04lx\n", (crc_word & 0xffff));
    compute_crc(str5, CRC_CCITT, 4, &crc_word, 1);
    Print("\nString yielded %04lx\n", (crc_word & 0xffff));
}
#endif
