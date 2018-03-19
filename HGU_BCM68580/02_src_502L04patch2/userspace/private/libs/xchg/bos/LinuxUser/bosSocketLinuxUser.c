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
****************************************************************************/

/* ---- Include Files ---------------------------------------------------- */

#include <bosCfg.h>

#if BOS_CFG_SOCKET

#include <bosError.h>
#include <bosErrorLinuxUser.h>
#include <bosSocket.h>
#include <bosSocketPrivate.h>
#include <bosLog.h>

/* ---- Public Variables ------------------------------------------------- */
/* ---- Private Constants and Types -------------------------------------- */
/* ---- Private Variables ------------------------------------------------ */
/* ---- Private Function Prototypes -------------------------------------- */
/* ---- Functions -------------------------------------------------------- */

/**
 * @addtogroup bosSocket
 * @{
 */

/***************************************************************************/

BOS_STATUS bosSocketInit( void )
{
   BOS_ASSERT( !gBosSocketInitialized );

   bosSocketGenericInit();

   bosSocketInitFunctionNames();

   gBosSocketInitialized = BOS_TRUE;

   return BOS_STATUS_OK;

} /* bosSocketInit */

/***************************************************************************/

BOS_STATUS bosSocketTerm( void )
{
   BOS_ASSERT( gBosSocketInitialized );

   gBosSocketInitialized = BOS_FALSE;

   return BOS_STATUS_OK;

} /* bosSocketTerm */


#if BOS_CFG_TASK
/***************************************************************************/

BOS_STATUS bosSocketShare
(
   BOS_SOCKET    *srcSocket,
   BOS_TASK_ID   *dstTaskId,
   BOS_SOCKET    *dstSocket
)
{
   (void)dstTaskId;

   /*
    * LinuxUser doesn't need to share sockets.
    */

   *dstSocket = *srcSocket;
   return BOS_STATUS_OK;

} /* bosSocketShare */

/***************************************************************************/

BOS_STATUS bosSocketCloseShared
(
   BOS_SOCKET    *s
)
{
   (void)s;

   /*
    * LinuxUser doesn't need to share sockets, therefore there is no need to close
    * "shared sockets".
    */

   return BOS_STATUS_OK;

} /* bosSocketCloseShared */
#endif   /* BOS_CFG_TASK */


/***************************************************************************/

BOS_STATUS bosSocketCheckStatus( BOS_SOCKET_ERROR_TYPE err, BOS_SOCKET_FUNC_ID funcId )
{
   return bosErrorCheckStatusLinuxUser( err,
                                      BOS_MAKE_ERROR_SOURCE( BOS_SOCKET_MODULE, funcId ));

} /* bosSocketCheckStatus */

/***************************************************************************/

BOS_STATUS bosSocketSetIpTos
(
   BOS_SOCKET *s,
   BOS_UINT8   tos
)
{
   /* LinuxUser setsockopt doesn't like chars, it likes ints :) */
   int optValue = tos;

   if ( 0 != setsockopt(   GetSocket( s ),
                           BOS_SOCKET_IPPROTO_IP,
                           BOS_SOCKET_IP_TOS,
                           (char *)&optValue,
                           (int)sizeof( optValue )))
   {
      return bosSocketCheckStatus( bosSocketGetLastError(), BOS_SOCKET_SET_TOS_ID );
   }

   return BOS_STATUS_OK;

} /* bosSocketSetIpTos */


#if BOS_CFG_SOCKET_INTERFACE_SUPPORT
/***************************************************************************/

BOS_STATUS bosSocketSetInterface
(
   BOS_SOCKET *s,
   int         ifno
)
{
   BOS_STATUS   status;

   unsigned short bcmInterfaceSoOption = BCM0_IFFLAG;
   int optValue = 1;

   switch ( ifno  )
   {
      case 1:
      {
         bcmInterfaceSoOption = BCM0_IFFLAG;
         break;
      }
      case 2:
      {
         bcmInterfaceSoOption = BCM1_IFFLAG;
         break;
      }
      default:
      {
         bosLogErr(" bosSocketSetInterface: Invalid interface number");
         return BOS_STATUS_ERR;
      }
   }

   status = setsockopt( GetSocket( s ), BOS_SOCKET_SOL_SOCKET, bcmInterfaceSoOption,
            (char *)&optValue,sizeof (optValue));

   if ( status == BOS_STATUS_ERR )
   {
      return bosSocketCheckStatus( bosSocketGetLastError(), BOS_SOCKET_SET_INTERFACE_ID );
   }

   return BOS_STATUS_OK;

} /* bosSocketSetInterface */

/** @} */

#endif      /* BOS_CFG_SOCKET_INTERFACE_SUPPORT */

#endif      /* BOS_CFG_SOCKET */
