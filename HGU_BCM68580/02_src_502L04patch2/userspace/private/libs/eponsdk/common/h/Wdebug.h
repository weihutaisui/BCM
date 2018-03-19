/*
* <:copyright-BRCM:2013:proprietary:epon
* 
*    Copyright (c) 2013 Broadcom 
*    All Rights Reserved
* 
*  This program is the proprietary software of Broadcom and/or its
*  licensors, and may only be used, duplicated, modified or distributed pursuant
*  to the terms and conditions of a separate, written license agreement executed
*  between you and Broadcom (an "Authorized License").  Except as set forth in
*  an Authorized License, Broadcom grants no license (express or implied), right
*  to use, or waiver of any kind with respect to the Software, and Broadcom
*  expressly reserves all rights in and to the Software and all intellectual
*  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
*  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
*  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
* 
*  Except as expressly set forth in the Authorized License,
* 
*  1. This program, including its structure, sequence and organization,
*     constitutes the valuable trade secrets of Broadcom, and you shall use
*     all reasonable efforts to protect the confidentiality thereof, and to
*     use this information only in connection with your use of Broadcom
*     integrated circuit products.
* 
*  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
*     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
*     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
*     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
*     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
*     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
*     PERFORMANCE OF THE SOFTWARE.
* 
*  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
*     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
*     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
*     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
*     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
*     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
*     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
*     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
*     LIMITED REMEDY.
* :> 
*/


/*
 * wdebug.h
 */


#ifndef _W_DEBUG_H_
#define _W_DEBUG_H_

#include <stdio.h>

// #define W_DEBUG

#ifdef W_DEBUG
//  #define Print(ss, value...)   fprintf( stderr, __FUNCTION__", %d: "ss"\n", __LINE__, ##value ) 
  #define Print					printf( "%s, %d: ", 		 __FUNCTION__, __LINE__), printf 
  #define PrintInt( x )		  	printf( "%s, %d: "#x"=%d\n", __FUNCTION__, __LINE__, (x) )  
  #define PrintIntX( x )		printf( "%s, %d: "#x"=%X\n", __FUNCTION__, __LINE__, (x) )  
  #define PrintStr( x )		  	printf( "%s, %d: "#x"=%s\n", __FUNCTION__, __LINE__, (x) ) 
  #define PrintFloat( x ) 	  	printf( "%s, %d: "#x"=%f\n", __FUNCTION__, __LINE__, (x) 
  #define PrintIp( ip )         Print( "ip : %d.%d.%d.%d\n", (ip)&0xff, ((ip)>>8)&0xff, ((ip)>>16)&0xff, ((ip)>>24)&0xff )           
  #define PrintN(bb, len)        do { 								\
          int i_HYS_90;  											\
          unsigned char *bufi_HYS_90 = (unsigned char *)bb; 		\
          if ( bufi_HYS_90!=NULL ) 									\
          	{ 														\
                Print( #bb",len=%d", len );  						\
                for( i_HYS_90=0; i_HYS_90<len; i_HYS_90++ ) {  		\
                        if ( i_HYS_90 % 16==0 ) 					\
                        	printf("\n"); 							\
                        printf( "%02X ", bufi_HYS_90[i_HYS_90] ); 	\
                        } 											\
                printf( "\n"); 										\
            }														\
          }while(0)  
  #define PrintMac( mac )      PrintN( (mac), 6 )
  #define PrintMainPara( argc, argv )     do { \
          int i_HYS_90; \
          for ( i_HYS_90=0; i_HYS_90<argc; i_HYS_90++ ) \
                  PrintStr( argv[i_HYS_90] ); \
          } while ( 0 ) 
#else
  #define Print	                                
  #define PrintInt( x )	
  #define PrintIntX( x )
  #define PrintStr( x )		
  #define PrintFloat( x )	
  #define PrintIp( ip )
  #define PrintN(bb, len)
  #define PrintMac( mac )
  #define PrintMainPara( argc, argv )
#endif // W_DEBUG

#ifndef PrintTest
  #define PrintTest()	Print("Trace\n")
#endif	
//*/


#endif /*  */



