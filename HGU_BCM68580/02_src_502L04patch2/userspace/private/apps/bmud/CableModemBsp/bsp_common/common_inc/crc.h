//**************************************************************************   
//    Broadcom MCNS Compliant Modem
//    Subscriber Modem MAC ASIC
//**************************************************************************
//   
//    Copyright 1997  Broadcom Corporation
//    All Rights Reserved
//    No portions of this material may be reproduced in any form without the
//    written permission of:
//             Broadcom Corporation 
//             16251 Laguna Canyon Road
//             Irvine, California  92618
//    All information contained in this document is Broadcom Corporation
//    company private, proprietary, and trade secret.
//
//
//
//**************************************************************************
//    Filename:       global.h
//    Author:         k. carmichael, d. gay
//    Creation Date:  03 mar 1997
//
//**************************************************************************
//    Description:    
//                    Global definitions
//
//**************************************************************************
//    Revision History:
//
//**************************************************************************

#ifndef crc_h
#define crc_h

#if __cplusplus
extern "C" {
#endif

typedef enum {
   CRC_CCITT = 0,
   CRC_32
} PolyType;

/* This is a very fast, table-based implementation of CRC_32.  You should
   call this instead of compute_crc for CRC_32.  It returns the CRC, rather
   than requiring you to pass tons of arguments. */
unsigned long FastCrc32(const void *pData, unsigned int numberOfBytes);

unsigned long FastCrc32First( const void *pData, unsigned int numberOfBytes );
unsigned long FastCrc32More( unsigned long oldResult, const void *pData, unsigned int numberOfBytes );
unsigned long FastCrc32Finish( unsigned long oldResult );


int compute_crc(const void *in2_str,
                PolyType polynomial_name, 
                int Length, 
                unsigned long *crc_word,
                int reverse);
void compute_dec_checksum(const char string[], int n, char * ch);

#if __cplusplus
}
#endif

#endif

