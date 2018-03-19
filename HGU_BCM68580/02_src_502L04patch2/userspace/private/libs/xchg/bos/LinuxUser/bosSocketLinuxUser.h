/****************************************************************************
*
* <:copyright-BRCM:2008:proprietary:standard
* 
*    Copyright (c) 2008 Broadcom 
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
*****************************************************************************/
/**
*
*  @file    bosSocketLinuxUser.h
*
*  @brief   LinuxUser specific definitions for the BOS Socket module.
*
****************************************************************************/

#if !defined( BOSSOCKETLINUXUSER_H )
#define BOSSOCKETLINUXUSER_H         /**< Include Guard                           */

/* ---- Include Files ---------------------------------------------------- */

#if !defined( BOSLINUXUSER_H )
#  include "bosLinuxUser.h"
#endif

#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <netdb.h>

/**
 * @addtogroup bosSocket
 * @{
 */

/* ---- Constants and Types ---------------------------------------------- */

/**
 * Specifies that the generic socket interface file (generic/bosSocketGeneric.c)
 * can be used for the bulk of the socket implementation.
 */

#define  BOS_SOCKET_USE_GENERIC_IMPLEMENTATION  1

/**
 * This value is considered to be an "error" by LinuxUser.
 */

#define  BOS_SOCKET_INVALID   (-1)

/**
 * LinuxUser version of a BOS_SOCKET.
 */
typedef struct
{
   int         s;                /* Socket descriptor                         */
   BOS_BOOL    isNonBlocking;    /* True means socket is in non-blocking mode */
#if BOS_CFG_SOCKET_INTERFACE_SUPPORT
   int         ifno;             /* Socket's interface number  */
#endif
   int         family;               /* Ipv4 or IPv6 */
} BOS_SOCKET;

typedef int    BOS_SOCKET_ID;

#define bosSocketGetNativeId( pBosSocket )  ((pBosSocket)->s)
#define bosSocketGetNativeFamily( pBosSocket )  ((pBosSocket)->family)

#if BOS_CFG_SOCKET_INTERFACE_SUPPORT
   #define BOS_SOCKET_IFNO_INVALID (-1)
#define bosSocketSetInterfaceFlags( pSocket, flags )     /* empty */
#define bosSocketSetInterfaceAddr( pSocket, pAddr )      /* empty */
#endif

/**
 * LinuxUser version of a BOS_SOCKET_SET
 */

typedef fd_set   BOS_SOCKET_SET;

/* Setsockopt flags used to specify an interface to send directly to without
 * going through the LinuxUser routing table */
#define BCM0_IFFLAG    0x0400
#define BCM1_IFFLAG    0x0800
#define BCM2_IFFLAG    0x0c00


/**
 * Error type appropriate for LinuxUser.
 */

typedef int BOS_SOCKET_ERROR_TYPE;

/**
 * LinuxUser Socket error return value.
 */

#define  BOS_SOCKET_ERROR  SOCKET_ERROR

#define  bosSocketClearLastError()  errno = 0
#define  bosSocketGetLastError()    errno
#define  closesocket(s)             close(s)
#define  ioctlsocket(s,c,v)         ioctl(s,c,(int)(v))

/*
 * LinuxUser can be built on i386-linux (little-endian) or MIPS (big-endian).
 * The appropriate generic.h file will be loaded to ensure that our
 * byte order is correct when using hton or ntoh functions.
 */

/***************************************************************************/
/**
*  Converts an unsigned 32 bit number from host order to network order.
*/

#define  bosSocketHtoNL( num )   (htonl( num ))

/***************************************************************************/
/**
*  Converts an unsigned 16 bit number from host order to network order.
*/

#define  bosSocketHtoNS( num )   (htons( num ))

/***************************************************************************/
/**
*  Converts an unsigned 32 bit number from network order to host order.
*/

#define  bosSocketNtoHL( num )   (ntohl( num ))

/***************************************************************************/
/**
*  Converts an unsigned 16 bit number from network order to host order.
*/

#define  bosSocketNtoHS( num )   (ntohs( num ))


/* ---- Variable Externs ------------------------------------------------- */
/* ---- Function Prototypes ---------------------------------------------- */

/** @} */

#endif /* BOSSOCKETLINUXUSER_H */

