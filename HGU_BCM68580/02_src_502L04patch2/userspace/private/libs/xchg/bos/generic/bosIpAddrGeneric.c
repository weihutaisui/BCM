/****************************************************************************
*
* <:copyright-BRCM:2009:proprietary:standard
* 
*    Copyright (c) 2009 Broadcom 
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
*  file    bosIpAddrGeneric.c
*
*  Contains the IP address abstrction.
*
****************************************************************************/


/* ---- Include Files ---------------------------------------------------- */
#include <bosCfg.h>
#if BOS_CFG_IP_ADDRESS
#include <bosIpAddr.h>
#include <bosLog.h>
#include <bosSocket.h>
#include <bosError.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <str.h>

/* ---- Variable Externs ------------------------------------------------- */
/* ---- Function Prototypes ---------------------------------------------- */

/* ---- Private Variables ------------------------------------------------ */

static   BOS_BOOL gBosIpAddrInitialized = BOS_FALSE;

#if BOS_CFG_IP_ADDR_V6_SUPPORT
static BOS_UINT8 *bcm_inet_ntop6(const BOS_UINT8 *puIpAddr, BOS_UINT8 *pcBuf, const int bufSize);
static int bcm_inet_pton6( const BOS_UINT8 *pcIpAddr, BOS_UINT8 *puBuf, const int bufSize );
#endif
static BOS_UINT8 *bcm_inet_ntop4(const BOS_UINT8 *puAddr, BOS_UINT8 *puBuf, const int bufSize);
static int bcm_inet_pton4( const BOS_UINT8 *pcIpAddr, BOS_UINT8 *puBuf, const int bufSize );
#define BCM_INET_ADDR_V4_STRING_LEN    (BOS_IP_ADDRESS_MAX_STR_LEN_V4-1)
#define BCM_INET_ADDR_V4_STRING_SIZE   BOS_IP_ADDRESS_MAX_STR_LEN_V4
#define BCM_INET_ADDR_V6_STRING_LEN    (BOS_IP_ADDRESS_MAX_STR_LEN_V6-1)
#define BCM_INET_ADDR_V6_STRING_SIZE   BOS_IP_ADDRESS_MAX_STR_LEN_V6

#define BOS_IPV6__16BIT(p)			(((p)[0] << 8) | (p)[1])
#define BOS_IPV6__32BIT(p)			(((p)[0] << 24) | ((p)[1] << 16) | \
                              ((p)[2] << 8) | (p)[3])
#define BOS_IN6_IS_ADDR_ZERO(p) (\
p[0] == 0 && \
p[1] == 0 && \
p[2] == 0 && \
p[3] == 0 && \
p[4] == 0 && \
p[5] == 0 && \
p[6] == 0 && \
p[7] == 0 && \
p[8] == 0 && \
p[9] == 0 && \
p[10] == 0 && \
p[11] == 0 && \
p[12] == 0 && \
p[13] == 0 && \
p[14] == 0 && \
p[15] == 0 )


static const unsigned char IPV4_LOOPBACK_ADDRESS[] = "127.0.0.1";
static const unsigned char IPV6_LOOPBACK_ADDRESS[] = "::1";




/*****************************************************************************
 ** FUNCTION:  bosIpAddrInit
 **
 ** PURPOSE:   Initialize the BOS ipAddr module.
 **
 ** PARAMETERS:
 **
 **
 ** RETURNS:   BOS_STATUS_OK always
 *****************************************************************************/
BOS_STATUS bosIpAddrInit( void )
{
   BOS_ASSERT( !gBosIpAddrInitialized);

   gBosIpAddrInitialized = BOS_TRUE;

   return BOS_STATUS_OK;
}


/*****************************************************************************
 ** FUNCTION:  bosIpAddrTerm
 **
 ** PURPOSE:   Terminate BOS ipAddr module
 **
 ** PARAMETERS:
 **
 **
 ** RETURNS:   BOS_STATUS_OK always
 *****************************************************************************/
BOS_STATUS bosIpAddrTerm( void )
{
   BOS_ASSERT( gBosIpAddrInitialized);

   gBosIpAddrInitialized = BOS_FALSE;

   return BOS_STATUS_OK;

}


/*****************************************************************************
 ** FUNCTION:  bosIpAddrCreateFromStr
 **
 ** PURPOSE:   Create a BOS IP address structure from a string (v4 xx.xx.xx.xx)
 **            or (v6 xxxx:xxxx: ...)
 **
 ** PARAMETERS:   IN - ipstr - pointer to the string to be used
 **               OUT - ipp - pointer to BOS IP address to be created.
 **
 **
 ** RETURNS:   BOS_STATUS_OK if Success, BOS_STATUS_ERR otherwise
 *****************************************************************************/
BOS_STATUS bosIpAddrCreateFromStr
(
   const char          *ipstr,
   BOS_IP_ADDRESS      *ipp
)
{

#if BOS_CFG_IP_ADDR_V6_SUPPORT
   int type;
   char *p;
#endif
   BOS_ASSERT( gBosIpAddrInitialized);
   BOS_ASSERT( ipp != NULL );

   memset(ipp, 0, sizeof(BOS_IP_ADDRESS));
#if BOS_CFG_IP_ADDR_V6_SUPPORT
   if ( strchr(ipstr, '.') )
   {
      type = BOS_IPADDRESS_TYPE_V4;
   }
   else if ( strchr(ipstr, ':') )
   {
      type = BOS_IPADDRESS_TYPE_V6;
      if ( (p = strchr(ipstr, '/')) != NULL )
      {
         *p = '\0';
      }
   }
   else
   {
      return BOS_STATUS_ERR;
   }

   switch (type)
   {
      case BOS_IPADDRESS_TYPE_V4:
         ipp->ipType = BOS_IPADDRESS_TYPE_V4;
         if (bcm_inet_pton4((BOS_UINT8 *)ipstr,
                            (BOS_UINT8 *)ipp->u.ipv4Address.s4Addr,
                            sizeof(ipp->u.ipv4Address.s4Addr)))
         {
            return BOS_STATUS_OK;
         }
         else
         {
            return BOS_STATUS_ERR;
         }
         break;
      case BOS_IPADDRESS_TYPE_V6:
         ipp->ipType = BOS_IPADDRESS_TYPE_V6;
         if (bcm_inet_pton6( (BOS_UINT8 *)ipstr,
                             ipp->u.ipv6Address.s6Addr,
                             sizeof( ipp->u.ipv6Address.s6Addr)))
         {
            return BOS_STATUS_OK;
         }
         else
         {
            return BOS_STATUS_ERR;
         }
         break;
      default:
         bosLogErr("%s: type unknown.", __FUNCTION__);
         return BOS_STATUS_ERR;
   }
#else
   if( !bcm_inet_pton4((BOS_UINT8 *)ipstr,
                       (BOS_UINT8 *)ipp->s4Addr,
                       sizeof(ipp->s4Addr)))
   {
     return BOS_STATUS_ERR;
   }
   else
   {
      return BOS_STATUS_OK;
   }
#endif

}


/*****************************************************************************
 ** FUNCTION:  bosIpAddrCreateFromNumU8
 **
 ** PURPOSE:   Create a BOS IP address structure from an array of bytes.
 **
 ** PARAMETERS:   IN - bytes - array of bytes used to create the BOS IP address
 **               IN - number of bytes of the input array (4 bytes
 **               (BOS_IP_ADDRESS_LEN_BYTES_V4) or 16 bytes (
 **               BOS_IP_ADDRESS_LEN_BYTES_V6))
 **               OUT - created BOS IP address structure
 **
 ** NOTES:     The array of input 'bytes' is assumed to be in network byte order.
 **
 ** RETURNS:   BOS_STATUS_OK if Success, BOS_STATUS_ERR otherwise
 *****************************************************************************/
BOS_STATUS bosIpAddrCreateFromNumU8
(
   const BOS_UINT8     *bytes,
   unsigned int         numBytes,
   BOS_IP_ADDRESS      *ipp
)
{
   BOS_ASSERT( gBosIpAddrInitialized);
   BOS_ASSERT( ipp != NULL );

   memset(ipp, 0, sizeof(BOS_IP_ADDRESS));

   if ( numBytes == BOS_IP_ADDRESS_LEN_BYTES_V4 )
   {
      bosIpAddrCreateFromU8( BOS_IPADDRESS_TYPE_V4, bytes, ipp );
   }
   else if ( numBytes == BOS_IP_ADDRESS_LEN_BYTES_V6 )
   {
      bosIpAddrCreateFromU8( BOS_IPADDRESS_TYPE_V6, bytes, ipp );
   }
   else
   {
      return( BOS_STATUS_ERR);
   }

   return BOS_STATUS_OK;
}


/*****************************************************************************
 ** FUNCTION:  bosIpAddrCreateFromU8
 **
 ** PURPOSE:   Create a BOS IP address structure from an array of bytes using
 **            type as input.
 **
 ** PARAMETERS:   IN - type of address (BOS_IP_ADDRESS_LEN_BYTES_V4 or
 **                    BOS_IP_ADDRESS_LEN_BYTES_V6)
 **               IN - bytes - array of bytes used to create the BOS IP address
 **               OUT - created BOS IP address structure
 **
 ** NOTES:     The array of input 'bytes' is assumed to be in network byte order.
 **
 ** RETURNS:   BOS_STATUS_OK if Success, BOS_STATUS_ERR otherwise
 *****************************************************************************/
BOS_STATUS bosIpAddrCreateFromU8
(
   BOS_IP_ADDRESS_TYPE  type,
   const BOS_UINT8     *bytes,
   BOS_IP_ADDRESS      *ipp
)
{
   (void)type;
   BOS_ASSERT( gBosIpAddrInitialized);
   BOS_ASSERT( ipp != NULL );

   memset(ipp, 0, sizeof(BOS_IP_ADDRESS));

#if BOS_CFG_IP_ADDR_V6_SUPPORT
   ipp->ipType = type;
   switch (ipp->ipType)
   {
      case BOS_IPADDRESS_TYPE_V4:
         memcpy(ipp->u.ipv4Address.s4Addr, bytes, sizeof(ipp->u.ipv4Address));
         break;
      case BOS_IPADDRESS_TYPE_V6:
         memcpy(ipp->u.ipv6Address.s6Addr, bytes, sizeof(ipp->u.ipv6Address));
         break;
      default:
         bosLogErr("%s: type unknown.", __FUNCTION__);
         return BOS_STATUS_ERR;
   }
#else
   memcpy(ipp->s4Addr, bytes, sizeof(ipp->s4Addr));
#endif

	return BOS_STATUS_OK;
}

/*****************************************************************************
 ** FUNCTION:  bosIpAddrV6CreateFromV4
 **
 ** PURPOSE:   Creates an IPv4 mapped IPv6 address,
 **            xx.xx.xx.xx to xxxx:xxxx:... xx.xx.xx.xx
 **
 ** PARAMETERS:   IN - ipv4 - pointer to IPv4 BOS IP address structure
 **               OUT - ipv6 - pointer to IPv6 BOS IP address structure
 **
 **
 ** RETURNS:   BOS_STATUS_OK - success, BOS_STATUS_ERR otherwise
 *****************************************************************************/
BOS_STATUS bosIpAddrV6CreateFromV4( BOS_IP_ADDRESS *ipv4, BOS_IP_ADDRESS *ipv6 )
{
#if BOS_CFG_IP_ADDR_V6_SUPPORT
   BOS_ASSERT( gBosIpAddrInitialized);
   BOS_ASSERT( ipv6 != NULL );
   BOS_ASSERT( ipv4 != NULL );

  if (bosIpAddrIsV4(ipv4))
  {
     BOS_UINT32 ipAddr;

     /* IPv4 mapped IPv6 address format
	    80       : 16 : 32  bits ipv4 address
	0000000000000:FFFF:xxxxxxxxx
     */
     memset((void*)ipv6, 0, sizeof(BOS_IP_ADDRESS));
     ipv6->ipType = BOS_IPADDRESS_TYPE_V6;
     bosIpAddrV4GetU32(ipv4, &ipAddr);
     ipv6->u.ipv6Address.s6Addr16[5] = 0xFFFF;
     ipv6->u.ipv6Address.s6Addr32[3] = ipAddr;
  }
  else
  {
     return BOS_STATUS_ERR;
  }
  return BOS_STATUS_OK;
#else
   (void) ipv4;
   (void) ipv6;

  return BOS_STATUS_ERR;
#endif
}

/*****************************************************************************
 ** FUNCTION:  bosIpAddrV4CreateFromU32
 **
 ** PURPOSE:   Create a BOS IP address structure from an interger,
 **            for IPv4 addresses only.
 **
 ** PARAMETERS:   IN - ipnum - the IP address represenetd as integer
 **               OUT - ipp - pointer to the created BOS IP address structure
 **
 ** NOTES:     The input binary IP address value 'ipnum' is assumed to be in
 **            host network order.
 **
 ** RETURNS:   BOS_STATUS_OK - success, BOS_STATUS_ERR otherwise
 *****************************************************************************/
BOS_STATUS bosIpAddrV4CreateFromU32( BOS_UINT32 ipnum, BOS_IP_ADDRESS *ipp )
{
   BOS_ASSERT( gBosIpAddrInitialized);
   BOS_ASSERT( ipp != NULL );

   memset(ipp, 0, sizeof(BOS_IP_ADDRESS));

#if BOS_CFG_IP_ADDR_V6_SUPPORT
   ipp->ipType = BOS_IPADDRESS_TYPE_V4;
   ipp->u.ipv4Address.s4Addr32 = bosSocketHtoNL(ipnum);
#else
   ipp->s4Addr32 = bosSocketHtoNL(ipnum);
#endif

	return BOS_STATUS_OK;
}


/*****************************************************************************
 ** FUNCTION:  bosIpAddrCreateCopy
 **
 ** PURPOSE:   Create a new copy of the BOS IP address structure.
 **
 ** PARAMETERS:   IN - fromip - pointer to BOS IP address structure to be copied from
 **               OUT - toip - pointer to BOS IP address structure to be copied to
 **
 **
 ** RETURNS:   BOS_STATUS_OK always
 *****************************************************************************/
BOS_STATUS bosIpAddrCreateCopy( const BOS_IP_ADDRESS *srcip, BOS_IP_ADDRESS *destip)
{
   BOS_ASSERT( gBosIpAddrInitialized);
   BOS_ASSERT( destip != NULL );

   memcpy(destip, srcip, sizeof(BOS_IP_ADDRESS));
   return BOS_STATUS_OK;
}


/*****************************************************************************
 ** FUNCTION:  bosIpAddrCreateZero
 **
 ** PURPOSE:   Create a BOS IP address containing all zeros (0.0.0.0 or ::)
 **
 ** PARAMETERS:   IN - type - type of IP address to be created
 **               OUT - ipp - pointer to the created BOS IP address
 **
 **
 ** RETURNS:   BOS_STATUS_OK - success, BOS_STATUS_ERR otherwise
 ******************************************************************************/
BOS_STATUS bosIpAddrCreateZero( BOS_IP_ADDRESS_TYPE type, BOS_IP_ADDRESS *ipp )
{
   (void)type;
   BOS_ASSERT( gBosIpAddrInitialized);
   BOS_ASSERT( ipp != NULL );

   memset(ipp, 0, sizeof(BOS_IP_ADDRESS));

#if BOS_CFG_IP_ADDR_V6_SUPPORT
   ipp->ipType = type;
   switch (type)
   {
      case BOS_IPADDRESS_TYPE_V4:
         ipp->u.ipv4Address.s4Addr32 = 0;
         break;
      case BOS_IPADDRESS_TYPE_V6:
         memset(ipp->u.ipv6Address.s6Addr, 0, sizeof(ipp->u.ipv6Address.s6Addr));
         break;
      default:
         bosLogErr("%s: type unknown. %d", __FUNCTION__, type);
         return BOS_STATUS_ERR;
   }
#else
   ipp->s4Addr32 = 0;
#endif
   return BOS_STATUS_OK;
}

/*****************************************************************************
 ** FUNCTION:  bosIpAddrCreateAny
 **
 ** PURPOSE:   Create a BOS IP address structure for inaddr_any
 **
 ** PARAMETERS:   IN - type - type of IP address to be created
 **               OUT - ipp - pointer to the created BOS IP address
 **
 **
 ** RETURNS:   BOS_STATUS_OK - success, BOS_STATUS_ERR otherwise
 ******************************************************************************/
BOS_STATUS bosIpAddrCreateAny( BOS_IP_ADDRESS_TYPE type, BOS_IP_ADDRESS *ipp )
{
   (void)type;
   BOS_ASSERT( gBosIpAddrInitialized);
   BOS_ASSERT( ipp != NULL );

   memset(ipp, 0, sizeof(BOS_IP_ADDRESS));

#if BOS_CFG_IP_ADDR_V6_SUPPORT
   ipp->ipType = type;
   switch (type)
   {
      case BOS_IPADDRESS_TYPE_V4:
         ipp->u.ipv4Address.s4Addr32 = BOS_SOCKET_INADDR_ANY;
         break;
      case BOS_IPADDRESS_TYPE_V6:
         memset(ipp->u.ipv6Address.s6Addr, 0, sizeof(ipp->u.ipv6Address));
         break;
      default:
         bosLogErr("%s: type unknown.", __FUNCTION__);
         return BOS_STATUS_ERR;
   }
#else
   ipp->s4Addr32 = BOS_SOCKET_INADDR_ANY;
#endif
   return BOS_STATUS_OK;
}


/*****************************************************************************
 ** FUNCTION:  bosIpAddrCreateLoopback
 **
 ** PURPOSE:   Create a BOS IP address structure for loopback
 **
 ** PARAMETERS:      IN - type - type of IP address to be created
 **                  OUT - ipp - pointer to the created BOS IP address
 **
 **
 ** RETURNS:   0 - success.
 ******************************************************************************/
BOS_STATUS bosIpAddrCreateLoopback( BOS_IP_ADDRESS_TYPE type, BOS_IP_ADDRESS *ipp )
{
   (void)type;
   BOS_ASSERT( gBosIpAddrInitialized);
   BOS_ASSERT( ipp != NULL );

   memset(ipp, 0, sizeof(BOS_IP_ADDRESS));

#if BOS_CFG_IP_ADDR_V6_SUPPORT
   ipp->ipType = type;
   switch (type)
   {
      case BOS_IPADDRESS_TYPE_V4:
         bosIpAddrCreateFromStr((char *)IPV4_LOOPBACK_ADDRESS, ipp);
         break;
      case BOS_IPADDRESS_TYPE_V6:
         bosIpAddrCreateFromStr((char *)IPV6_LOOPBACK_ADDRESS, ipp);
         break;
      default:
         bosLogErr("%s: type unknown.", __FUNCTION__);
         return BOS_STATUS_ERR;
   }
#else
   bosIpAddrCreateFromStr((char *)IPV4_LOOPBACK_ADDRESS, ipp);
#endif
   return BOS_STATUS_OK;
}


/*****************************************************************************
 ** FUNCTION:  bosIpAddrDestroy
 **
 ** PURPOSE:   Destory the BOS IP address module
 **
 ** PARAMETERS:   IN - ipp - BOS IP address structure
 **
 **
 ** RETURNS:   BOS_STATUS_OK always
 *****************************************************************************/
BOS_STATUS bosIpAddrDestroy( BOS_IP_ADDRESS *ipp )
{
   (void) ipp;
   BOS_ASSERT( gBosIpAddrInitialized);
   return BOS_STATUS_OK;
}


/*****************************************************************************
 ** FUNCTION:  bosIpAddrGetStr
 **
 ** PURPOSE:   return a representation of the BOS IP address structure in a string
 **            (xx.xx.xx.xx, or xxxx:xxxx ....)
 **
 ** PARAMETERS:   IN - ipp - BOS IP address structure to be presented
 **               OUT - ipstr - the output string
 **               IN - maxlen - len is the max size of the output string
 **
 **
 ** RETURNS:   BOS_STATUS_OK - success, BOS_STATUS_ERR otherwise
 *****************************************************************************/
BOS_STATUS bosIpAddrGetStr( const BOS_IP_ADDRESS *ipp, char *ipstr, int maxlen )
{
   BOS_ASSERT( gBosIpAddrInitialized);
   BOS_ASSERT( ipp != NULL );

#if BOS_CFG_IP_ADDR_V6_SUPPORT
   switch (ipp->ipType)
   {
      case BOS_IPADDRESS_TYPE_V4:
         bcm_inet_ntop4((BOS_UINT8 *)ipp->u.ipv4Address.s4Addr, (BOS_UINT8 *)ipstr, maxlen);
         break;
      case BOS_IPADDRESS_TYPE_V6:
         bcm_inet_ntop6( ipp->u.ipv6Address.s6Addr, (BOS_UINT8 *)ipstr, maxlen);
         break;
      default:
         bosLogErr("%s: type unknown.", __FUNCTION__);
         strcpy(ipstr, "Unknown");
         return BOS_STATUS_ERR;
   }
#else
   bcm_inet_ntop4((BOS_UINT8 *)ipp->s4Addr, (BOS_UINT8 *)ipstr, maxlen);
#endif

   return BOS_STATUS_OK;
}


/*****************************************************************************
 ** FUNCTION:  bosIpAddrGetU8
 **
 ** PURPOSE:   Place the BOS IP address in an array of bytes
 **
 ** PARAMETERS:   IN - ipp - pointer to the BOS IP address structure
 **               OUT - bytes - array of bytes
 **               IN - maxSizeBytes - maximum size of the array
 **               out - numBytesWritten - number of bytes written to the input array
 **
 ** NOTES:     The array of output 'bytes' is in network byte order.
 **
 ** RETURNS:   BOS_STATUS_OK - success, BOS_STATUS_ERR otherwise
 *****************************************************************************/
BOS_STATUS bosIpAddrGetU8
(
   const BOS_IP_ADDRESS   *ipp,
   BOS_UINT8              *bytes,
   unsigned int            maxSizeBytes,
   unsigned int           *numBytesWritten
)
{
   (void) numBytesWritten;
   BOS_ASSERT( gBosIpAddrInitialized);
   BOS_ASSERT( ipp != NULL );


#if BOS_CFG_IP_ADDR_V6_SUPPORT
   switch (ipp->ipType)
   {
      case BOS_IPADDRESS_TYPE_V4:
         if ( maxSizeBytes < sizeof(ipp->u.ipv4Address.s4Addr))
            return BOS_STATUS_ERR;
         memcpy(bytes, ipp->u.ipv4Address.s4Addr, sizeof(ipp->u.ipv4Address.s4Addr));
         *numBytesWritten = sizeof(ipp->u.ipv4Address.s4Addr);
         break;
      case BOS_IPADDRESS_TYPE_V6:
         if ( maxSizeBytes < sizeof(ipp->u.ipv6Address.s6Addr))
            return BOS_STATUS_ERR;
         memcpy(bytes, ipp->u.ipv6Address.s6Addr, sizeof(ipp->u.ipv6Address.s6Addr));
         *numBytesWritten = sizeof(ipp->u.ipv6Address.s6Addr);
         break;
      default:
         bosLogErr("%s: type unknown.", __FUNCTION__);
         return BOS_STATUS_ERR;
   }
#else
   if ( maxSizeBytes < sizeof(ipp->s4Addr))
      return BOS_STATUS_ERR;
   memcpy(bytes, ipp->s4Addr, sizeof(ipp->s4Addr));
   *numBytesWritten = sizeof(ipp->s4Addr);
#endif

	return BOS_STATUS_OK;
}


/*****************************************************************************
 ** FUNCTION:  bosIpAddrV4GetU32
 **
 ** PURPOSE:   generate the interger number that the BOS IP address structure represents
 **
 ** PARAMETERS:   IN - ipp - pointer to BOS IP address
 **               out - ipNum - the IP address represented by the interger
 **
 ** NOTES:     The returned binary IP address value 'ipNum' is in
 **            host network order.
 **
 ** RETURNS:   BOS_STATUS_OK - success, BOS_STATUS_ERR otherwise
 *****************************************************************************/
BOS_STATUS bosIpAddrV4GetU32( const BOS_IP_ADDRESS *ipp, BOS_UINT32 *ipNum )
{
   BOS_ASSERT( gBosIpAddrInitialized);
   BOS_ASSERT( ipp != NULL );

#if BOS_CFG_IP_ADDR_V6_SUPPORT
   switch (ipp->ipType)
   {
      case BOS_IPADDRESS_TYPE_V4:
         *ipNum = bosSocketNtoHL(ipp->u.ipv4Address.s4Addr32);
         break;
      case BOS_IPADDRESS_TYPE_V6:
         bosLogErr("%s: v6 is not supported for this function.", __FUNCTION__);
         return BOS_STATUS_ERR;
         break;
      default:
         bosLogErr("%s: type unknown.", __FUNCTION__);
         return BOS_STATUS_ERR;
   }
#else
   *ipNum = bosSocketNtoHL(ipp->s4Addr32);
#endif
	return BOS_STATUS_OK;
}

/*****************************************************************************
 ** FUNCTION:  bosIpAddrV6GetNetworkPrefix
 **
 ** PURPOSE:   Retrieve the network prefix from a IPv6 address.
 **
 ** PARAMETERS:   IN  - ipp       - pointer to BOS IP address.
 **               OUT - netPrefix - the network prefix to return.
 **               IN  - mask      - associated network mask
 **
 ** NOTES:     Given an IPv6 address and its network mask, retrieve the
 **            network prefix.
 **
 ** RETURNS:   BOS_STATUS_OK - success, BOS_STATUS_ERR otherwise
 *****************************************************************************/
BOS_STATUS bosIpAddrV6GetNetworkPrefix
(
   const BOS_IP_ADDRESS *ipp,
   BOS_UINT64 *netPrefix,
   BOS_UINT32 mask
)
{
   BOS_STATUS ret = BOS_STATUS_OK;
   BOS_ASSERT( gBosIpAddrInitialized);
   BOS_ASSERT( ipp != NULL );

#if BOS_CFG_IP_ADDR_V6_SUPPORT
   switch (ipp->ipType)
   {
      case BOS_IPADDRESS_TYPE_V6:
      {
         BOS_UINT8  temp[BOS_IP_ADDRESS_LEN_BYTES_V6];
         unsigned int len, i;
         if ( ( ret = bosIpAddrGetU8( ipp, temp, sizeof( temp ), &len ) )
                        == BOS_STATUS_OK )
         {
            BOS_UINT64 addr = 0;
            BOS_UINT32 maskBytes = mask / 8;

            for ( i = 0; i < maskBytes; i++ )
            {
               addr = ( temp[i] << ( maskBytes - 1 - i ) * maskBytes );
            }
            *netPrefix = addr;
         }
      }
      break;
      case BOS_IPADDRESS_TYPE_V4:
      {
         bosLogErr("bosIpAddrV6GetNetworkPrefix error: v4 is not supported for this "
                   "function.\n");
         ret = BOS_STATUS_ERR;
      }
      break;
      default:
      {
         bosLogErr("bosIpAddrV6GetNetworkPrefix error: type unknown.\n");
         ret = BOS_STATUS_ERR;
      }
   }
#endif
	return ret;
}

/*****************************************************************************
 ** FUNCTION:  bosIpAddrIsEqual
 **
 ** PURPOSE:   compare 2 BOS IP address structures
 **
 ** PARAMETERS:   IN - ip1 - BOS IP address 1
 **               IN - ip2 - BOS IP address 2
 **
 **
 ** RETURNS:   BOS_TRUE if equal, BOS_FALSE of not equal
 *****************************************************************************/
BOS_BOOL bosIpAddrIsEqual( const BOS_IP_ADDRESS *ip1, const BOS_IP_ADDRESS *ip2 )
{



#if BOS_CFG_IP_ADDR_V6_SUPPORT
   int    rc;

   BOS_ASSERT( gBosIpAddrInitialized);
   BOS_ASSERT( ip1 != NULL );
   BOS_ASSERT( ip2 != NULL );

   if ( ip1->ipType != ip2->ipType )
   {
      return ( BOS_FALSE );
   }

   switch ( ip1->ipType )
   {
      case BOS_IPADDRESS_TYPE_V4:
         rc = memcmp(&ip1->u.ipv4Address, &ip2->u.ipv4Address,
                     sizeof(ip1->u.ipv4Address));
         break;
      case BOS_IPADDRESS_TYPE_V6:
         rc = memcmp(&ip1->u.ipv6Address, &ip2->u.ipv6Address,
                     sizeof(ip1->u.ipv6Address));
         break;
      default:
         bosLogErr("%s: type unknown.", __FUNCTION__);
         return BOS_FALSE;
   }
   if (rc)
   {
      return ( BOS_FALSE );
   }
#else
   BOS_ASSERT( gBosIpAddrInitialized);
   BOS_ASSERT( ip1 != NULL );
   BOS_ASSERT( ip2 != NULL );

   if ( ip1->s4Addr32 != ip2->s4Addr32 )
   {
      return ( BOS_FALSE );
   }
#endif

   return ( BOS_TRUE );
}

/*****************************************************************************
 ** FUNCTION:  ipaddress_print_v4
 **
 ** PURPOSE:
 **
 ** PARAMETERS:
 **
 **
 ** RETURNS:
 *****************************************************************************/
void ipaddress_print_v4(const BOS_IP_ADDRESS *ipp)
{
   const BOS_IP_ADDRESS_V4 *ipAddr;

#if BOS_CFG_IP_ADDR_V6_SUPPORT
   ipAddr = &ipp->u.ipv4Address;
#else
   ipAddr = ipp;
#endif

   bosLog("%d.%d.%d.%d.",
          ipAddr->s4Addr[0],
          ipAddr->s4Addr[1],
          ipAddr->s4Addr[2],
          ipAddr->s4Addr[3]);
}

/*****************************************************************************
 ** FUNCTION:  ipaddress_print_v6
 **
 ** PURPOSE:
 **
 ** PARAMETERS:
 **
 **
 ** RETURNS:
 *****************************************************************************/
void ipaddress_print_v6(const BOS_IP_ADDRESS *ipp)
{
#if BOS_CFG_IP_ADDR_V6_SUPPORT
   unsigned char   *p;
   int             j, n;
   int             zero_printed = 0;
#endif


   if ( ipp == NULL )
      return;

#if BOS_CFG_IP_ADDR_V6_SUPPORT
   if ( !BOS_IN6_IS_ADDR_ZERO(ipp->u.ipv6Address.s6Addr) )
   {
      char tmp[64] = { 0 };
      p = (unsigned char *)ipp->u.ipv6Address.s6Addr;
      for (j=0;j<16;j+=2)
      {
         n = BOS_IPV6__16BIT(p);
         if ( n )
         {
            zero_printed = 0;
            sprintf(tmp, "%s%x", tmp, n);
            if (j < 14)
            {
               sprintf(tmp, "%s:", tmp);
            }
         }
         else
         {
            if ( !zero_printed )
            {
               sprintf(tmp, "%s:", tmp);
               zero_printed = 1;
            }
         }
         p+=2;
      }
      bosLog("%s", tmp);
   }
   else
   {
      bosLog("::");
   }
#else
   bosLogErr("%s: This function is not supported for v4.", __FUNCTION__);
#endif
}

/*****************************************************************************
 ** FUNCTION:  bosIpAddrPrint
 **
 ** PURPOSE:   Display BOS IP address in dotted or : format
 **
 ** PARAMETERS:   IN - ipp - pointer to the BOS IP address structure
 **
 **
 ** RETURNS:
 *****************************************************************************/
void bosIpAddrPrint( const BOS_IP_ADDRESS *ipp )
{
   BOS_ASSERT( gBosIpAddrInitialized);
   BOS_ASSERT( ipp != NULL );


#if BOS_CFG_IP_ADDR_V6_SUPPORT
   switch (ipp->ipType)
   {
      case BOS_IPADDRESS_TYPE_V6:
         ipaddress_print_v6(ipp);
         break;
      case BOS_IPADDRESS_TYPE_V4:
         ipaddress_print_v4(ipp);
         break;
      default:
         break;
   }
#else
   ipaddress_print_v4(ipp);
#endif
}


/*****************************************************************************
 ** FUNCTION:  bosIpAddrIsZero
 **
 ** PURPOSE:   determine is a BOS IP address structure conatians all zero address
 **
 ** PARAMETERS:   IN - ipp - pointer to BOS IP address structure
 **
 **
 ** RETURNS:   BOS_TRUE if IP address is all zeros, BOS_FALSE if not all zeros
 ******************************************************************************/
BOS_BOOL bosIpAddrIsZero( const BOS_IP_ADDRESS *ipp )
{
   BOS_ASSERT( gBosIpAddrInitialized);
   BOS_ASSERT( ipp != NULL );

#if BOS_CFG_IP_ADDR_V6_SUPPORT
   switch (ipp->ipType)
   {
      case BOS_IPADDRESS_TYPE_V4:
         return (!ipp->u.ipv4Address.s4Addr32);
      case BOS_IPADDRESS_TYPE_V6:
         return (BOS_IN6_IS_ADDR_ZERO(ipp->u.ipv6Address.s6Addr) );
         break;
      default:
         bosLogErr("%s: type unknown.", __FUNCTION__);
         return BOS_FALSE;
   }
#else
   return (!ipp->s4Addr32);
#endif
}


/*****************************************************************************
 ** FUNCTION:  bosIpAddrIsLoopback
 **
 ** PURPOSE:   determine is a BOS IP address structure conatians loopback address
 **
 ** PARAMETERS:   IN - ipp - pointer to BOS IP address structure
 **
 **
 ** RETURNS:   BOS_TRUE if IP address is loopback, BOS_FALSE if not loopback
 ******************************************************************************/
BOS_BOOL bosIpAddrIsLoopback( const BOS_IP_ADDRESS *ipp )
{
   char     tmpstr[64];

   BOS_ASSERT( gBosIpAddrInitialized);
   BOS_ASSERT( ipp != NULL );

#if BOS_CFG_IP_ADDR_V6_SUPPORT
   bosIpAddrGetStr(ipp, tmpstr, sizeof(tmpstr));
   switch (ipp->ipType)
   {
      case BOS_IPADDRESS_TYPE_V4:
         return !strncmp(tmpstr, (char *)IPV4_LOOPBACK_ADDRESS,
               sizeof(IPV4_LOOPBACK_ADDRESS));
      case BOS_IPADDRESS_TYPE_V6:
         return !strncmp((const char*)tmpstr, (const char*)IPV6_LOOPBACK_ADDRESS,
                         sizeof(IPV6_LOOPBACK_ADDRESS));
         break;
      default:
         bosLogErr("%s: type unknown.", __FUNCTION__);
         return BOS_FALSE;
   }
#else
   bosIpAddrGetStr(ipp, tmpstr, sizeof(tmpstr));
   return !strncmp((const char*)tmpstr, (const char*)IPV4_LOOPBACK_ADDRESS,
                   sizeof(IPV4_LOOPBACK_ADDRESS));
#endif
}


#if BOS_CFG_IP_ADDR_V6_SUPPORT
/*
*****************************************************************************
** FUNCTION:   bcm_inet_pton6
**
** PURPOSE:    Converts charecter presented IPv6 address to network byte ordered
**             address.
**
** PARAMETERS: pcIpStr  - character string with an IPv6 address
**             puBuf    - pointer to where the converted address to be stored
**             bufSize  - buffer size
**
** RETURNS:    1 if conversion OK, 0 otherwise.
**
** NOTE:
**
*****************************************************************************
*/
static int bcm_inet_pton6( const BOS_UINT8 *pcIpAddr, BOS_UINT8 *puBuf, const int bufSize )
{
   const BOS_UINT8   cMaskLowercase[]="0123456789abcdef";
   const BOS_UINT8   cMaskUppercase[]="0123456789ABCDEF";
   const BOS_UINT8   *pcLast;
   BOS_UINT8   cIpstring[BCM_INET_ADDR_V6_STRING_LEN+1];
   BOS_UINT8   tmpVal[16];
   BOS_UINT8   *pch1, *pch2, *pch3, *pch4;
   BOS_UINT8   val = 0;
   BOS_BOOL    bRevScan = BOS_TRUE;
   BOS_BOOL    bDone = BOS_FALSE;
   int         i = 15;
   int         j = 1;
   int         k;
   int         shiftCnt;
   int         chCnt;

   if ( ( pcIpAddr == NULL ) ||
        ( strlen((char *)pcIpAddr) == 0 ) ||
        ( strlen((char *)pcIpAddr) > BCM_INET_ADDR_V6_STRING_LEN ) ||
        ( strlen((char *)pcIpAddr) < 2 ) ||
        ( bufSize < 16 ) )
   {
      return 0;
   }

   if ( pcIpAddr )

   memset( cIpstring, 0, sizeof(cIpstring) );
   memset( tmpVal, 0, sizeof(tmpVal) );

   /* Make a copy of the address string */
   strncpy((char *)cIpstring, (char *)pcIpAddr, sizeof( cIpstring ));
   pcLast = cIpstring + strlen((char *)cIpstring) - 1;

   /*
    * Conversion steps:
    *    1. Start from the end of the string first,
    *    2. Convert IPv4 if exists.
    *    3. When hitting 2 consective colons, stop and start converting from
    *      the beginning of the string
    */
   if ( (pch2 =(BOS_UINT8 *)strrchr( (char *)cIpstring, ':' )) == NULL )
   {
      /* Address has no colons, return error */
      return 0;
   }

   if ( ((pch1 = (BOS_UINT8 *)strrchr( (char *)cIpstring, '.' )) != NULL) &&
         (pch1 > pch2) )
   {
      if ( !bcm_inet_pton4( pch2+1, (BOS_UINT8 *)(tmpVal+12), 4 ) )
      {
         /* IPv4 conversion error */
         return 0;
      }
      /* Last 4 bytes filled with IPv4 address, 12 bytes remaining */
      i -= 4;
      *pch2 = '\0';  /* Modify the colon to null */
   }
   else
   {
      pch2 = NULL;
   }

   while (!bDone)
   {
      pch3 = NULL;
      if ( bRevScan )
      {
         k = i;
         /* Reverse scanning of the address string */
         if ( (pch1 = (BOS_UINT8 *)strrchr( (char *)cIpstring, ':' )) != NULL )
         {
            /* Modify the colon to null */
            *pch1 = '\0';
            if ( !pch2 && (pch1 == pcLast) )
            {
               /* String ends with a colon */
               if ( *(pch1-1) != ':' )
               {
                  /* Must end with two colons */
                  return 0;
               }
               else
               {
                  pch2 = pch1;
               }
               continue;
            }
            else if ( pch2 && (pch2==(pch1+1)) )
            {
               if ( pch1 != cIpstring )
               {
                  /* Consecutive colons, start scanning from beginning */
                  i-=2;
                  pch1 = cIpstring;
                  bRevScan = BOS_FALSE;
                  continue;
               }
               else
               {
                  /* Done conversion, consecutive colons at the beginning */
                  break;
               }
            }
            else
            {
               /* Set address chunk for conversion between two 2 colons */
               i-=2;
               pch3 = pch1 + 1;
               pch2 = pch1;
            }
         }
         else
         {
            /* No more colons */
            pch3 = cIpstring;
            bDone = BOS_TRUE;
         }
      }
      else
      {
         k = j;
         /* Forward scanning of address string */
         if ( (pch2 = (BOS_UINT8 *)strchr( (char *)pch1, ':' )) != NULL )
         {
            /* Set address chunk for conversion between 2 colons */
            j+=2;
            pch3 = pch1;
            /* Modify the colon to null */
            *pch2 = '\0';

            pch1 = pch2 + 1;
         }
         else
         {
            /* Last colon */
            pch3 = pch1;
            bDone = BOS_TRUE;
         }
      }

      /* Convert address chunck */
      BOS_ASSERT( pch3 );
      if ( j >= i+2 )
      {
         /* More than 8 sets of colon separated address blocks */
         return 0;
      }
      if ( k >=1 && k <=15 &&
           ((chCnt = strlen( (char *)pch3 )) > 0 ) &&
           (chCnt <= 4) )
      {
         shiftCnt = 0;
         while ( chCnt > 0 )
         {
            if ( (pch4 = (BOS_UINT8 *)strchr( (char *)cMaskLowercase,
                                              *(pch3+chCnt-1) )) != NULL )
            {
               val |= (pch4 - cMaskLowercase) << shiftCnt;
            }
            else if ( (pch4 = (BOS_UINT8 *)strchr( (char *)cMaskUppercase,
                                                   *(pch3+chCnt-1) )) != NULL )
            {
               val |= (pch4 - cMaskUppercase) << shiftCnt;
            }
            else
            {
               /* Conversion error: unexpected value between 2 colons */
               return 0;
            }
            chCnt--;
            if ( shiftCnt || chCnt == 0 )
            {
               tmpVal[k--] = val;
               val = 0;
            }
            shiftCnt = shiftCnt==0?4:0;
         }
      }
      else
      {
         return 0;
      }
   }

   memcpy( puBuf, tmpVal, 16 );
   return 1;

}
#endif   /* BOS_CFG_IP_ADDR_V6_SUPPORT */

/*
*****************************************************************************
** FUNCTION:   bcm_inet_pton4
**
** PURPOSE:    Converts charecter presented IPv4 address to network byte ordered
**             address.
**
** PARAMETERS: pcIpStr  - character string with an IPv4 address
**             puBuf    - pointer to where the converted address to be stored
**             bufSize  - buffer size
**
** RETURNS:    1 if conversion OK, 0 otherwise.
**
** NOTE:
**
*****************************************************************************
*/
static int bcm_inet_pton4( const BOS_UINT8 *pcIpAddr, BOS_UINT8 *puBuf,
   const int bufSize )
{
   BOS_UINT8   cIpstring[BCM_INET_ADDR_V4_STRING_LEN+1] = {0};
   BOS_UINT8   tmp[4] = {0};
   BOS_UINT8   *pcPtr1, *pcPtr2;
   int         i = 0;
   int         j;

   if ( ( pcIpAddr == NULL ) ||
        ( strlen((char *)pcIpAddr) == 0 ) ||
        ( strlen((char *)pcIpAddr) > BCM_INET_ADDR_V4_STRING_LEN ) ||
        ( bufSize < 4 ) )
   {
      return 0;
   }

   strncpy((char *)cIpstring, (char *)pcIpAddr, sizeof( cIpstring ));

   pcPtr2 = cIpstring;

   for (j=24; j>=0; j-=8)
   {
      pcPtr1 = pcPtr2;
      while(( *pcPtr2 ) && ( *pcPtr2 != '.' ))
      {
         pcPtr2++;
      }
      if( *pcPtr2 )
      {
         *pcPtr2++ = '\0';
      }

      if (pcPtr1)
      {
         if ( *pcPtr1 == '0' )
         {
            tmp[i++] = 0;
         }
         else
         {
            if ( (tmp[i++] = (BOS_UINT8)atoi((char *)pcPtr1))== 0 )
            {
               return 0;
            }
         }
      }
      else
      {
         break;
      }
   }

   if ( i != 4 )
   {
      return 0;
   }

   memcpy( puBuf, tmp, 4 );

   return 1;

} /* end of bcm_inet_pton4() */


/*
*****************************************************************************
** FUNCTION:   bcm_inet_ntop4
**
** PURPOSE:    Return an IP address in xxx.xxx.xxx.xxx format,
**             or NULL if conversion fails.
**
** PARAMETERS: puIpaddr - Network byte ordered internet address
**             pcBuf    - pointer to a buffer to be filled in with a string
**                        representing the ethernet address given by
**                        EthernetAddressBytes
**             bufSize  - Size of pcBuf including NULL termination.
**
** RETURNS:    pointer to puBuf
**
*****************************************************************************
*/
static BOS_UINT8 *bcm_inet_ntop4(const BOS_UINT8 *puIpAddr, BOS_UINT8 *pcBuf,
   const int bufSize)
{
   if ( (puIpAddr == NULL) ||
        (pcBuf == NULL) ||
        (bufSize < BCM_INET_ADDR_V4_STRING_SIZE) )
   {
      return NULL;
   }

   memset( pcBuf, 0, bufSize );
   strPrintf((char *)pcBuf, bufSize, "%d.%d.%d.%d", *puIpAddr,
      *(puIpAddr+1), *(puIpAddr+2), *(puIpAddr+3));

   return pcBuf;
} /*bcm_inet_ntop4( )*/

#if BOS_CFG_IP_ADDR_V6_SUPPORT
/*
*****************************************************************************
** FUNCTION:   bcm_inet_ntop6
**
** PURPOSE:    Return an IP address in
**             xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx format with the
**             additional rules:
**             - Leading zeros are removed
**             - collapse the longest run of zeros with ::
**             - Recognize IPv4 address, and to use the form ::xx.xx.xx.xx
**
**             Return NULL if conversion fails.
**
** PARAMETERS: puIpaddr - Network byte ordered internet address
**             pcBuf    - pointer to a buffer to be filled in with a string
**                        representing the ethernet address given by
**                        EthernetAddressBytes
**             bufSize  - Size of pcBuf including NULL termination.
**
** RETURNS:    pointer to puBuf
**
*****************************************************************************
*/
static BOS_UINT8 *bcm_inet_ntop6(const BOS_UINT8 *puIpAddr, BOS_UINT8 *pcBuf,
   const int bufSize)
{
   BOS_UINT8 *puZero = NULL;     /* ptr to the of the end of the zeros run */
   int c;
   BOS_UINT8 tmp = 0;
   BOS_UINT8 offset = 0;
   BOS_UINT8 *puPtr = (BOS_UINT8 *)puIpAddr;
   BOS_UINT8 ctmpBuf[16];
   BOS_UINT8 *pcPtr;

   if ( (puIpAddr == NULL) ||
        (pcBuf == NULL) ||
        (bufSize < BCM_INET_ADDR_V6_STRING_SIZE) )
   {
      return NULL;
   }

   /*
    * Scan the 128-bit (16 bytes) network address and mark the location of
    * the longest run of '0x0000's.
    */
   for ( puPtr = (BOS_UINT8 *)puIpAddr; puPtr < (puIpAddr+16); puPtr+=2 )
   {
      if ( bosSocketNtoHU16(*(BOS_UINT16*)puPtr) == 0x0000 )
      {
         tmp+=2;
         if ( tmp > offset )
         {
            offset = tmp;
            puZero = puPtr+2;
         }
      }
      else
      {
         /* reset */
         tmp = 0;
      }
   }
   /* Check whether it is an IPv4 address */
   if ( (puZero == puIpAddr+12) && (offset == 12) )
   {
      if ( bcm_inet_ntop4( puZero, ctmpBuf, 16) != NULL )
      {
         strPrintf( (char *)pcBuf, bufSize, "::%s", ctmpBuf );
         return( pcBuf );
      }
      else
      {
         return NULL;
      }
   }

   /* IPv6 here */
   pcPtr = pcBuf;
   for ( puPtr = (BOS_UINT8 *)puIpAddr; puPtr < (puIpAddr+16);)
   {
      if ( puPtr == (puZero-offset) && (offset > 2) )
      {
         *pcPtr++ = ':';
         puPtr += offset;
         if ( puPtr == puIpAddr+16 )
         {
            strPrintf( (char *)pcPtr, bufSize - (pcPtr - pcBuf), ":" );
            break;
         }
      }
      else
      {
         if ( puPtr != (BOS_UINT8 *)puIpAddr )
         {
            *pcPtr++ = ':';
         }
         c = strPrintf( (char *)pcPtr, bufSize - (pcPtr - pcBuf),
                "%x", bosSocketNtoHU16(*(BOS_UINT16*)puPtr));
         puPtr+=2;
         pcPtr+=c;

      }
   }
   return( pcBuf );
}
#endif   /* BOS_CFG_IP_ADDR_V6_SUPPORT */

#endif   /* BOS_CFG_IP_ADDRESS */
