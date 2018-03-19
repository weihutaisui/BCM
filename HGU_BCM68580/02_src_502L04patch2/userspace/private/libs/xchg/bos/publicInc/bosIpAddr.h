/****************************************************************************
*
* <:copyright-BRCM:2014:DUAL/GPL:standard
* 
*    Copyright (c) 2014 Broadcom 
*    All Rights Reserved
* 
* Unless you and Broadcom execute a separate written software license
* agreement governing use of this software, this software is licensed
* to you under the terms of the GNU General Public License version 2
* (the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
* with the following added to such license:
* 
*    As a special exception, the copyright holders of this software give
*    you permission to link this software with independent modules, and
*    to copy and distribute the resulting executable under terms of your
*    choice, provided that you also meet, for each linked independent
*    module, the terms and conditions of the license of that module.
*    An independent module is a module which is not derived from this
*    software.  The special exception does not apply to any modifications
*    of the software.
* 
* Not withstanding the above, under no circumstances may you combine
* this software in any way with any other Broadcom software provided
* under a license other than the GPL, without Broadcom's express prior
* written consent.
* 
* :>
*
*****************************************************************************/
/**
*
*  @file    bosIpAddr.h
*
*  @brief   Contains the IP address abstrction.
*
****************************************************************************/


/* ---- Include Files ---------------------------------------------------- */

#if !defined( BOSIPADDR_H )
#define BOSIPADDR_H           /**< Include Guard                           */

#if !defined( BOSCFG_H )
#  include <bosCfg.h>
#endif

#if !defined( BOSTYPES_H )
#  include <bosTypes.h>
#endif

#ifndef BOS_OS_LinuxKernel
#if !BOS_CFG_IP_ADDRESS
#error   BOS IP address module is not selected in bosCfg.h
#endif
#endif


#if defined( __cplusplus )
extern "C"
{
#endif


/* ---- Constants and Types ---------------------------------------------- */


/*
** Supported IP address types - IPv4 and IPv6.
*/
typedef enum BOS_IP_ADDRESS_TYPE
{
   BOS_IPADDRESS_TYPE_V4,
   BOS_IPADDRESS_TYPE_V6

} BOS_IP_ADDRESS_TYPE;


/*
** Length of IP address in bytes.
*/
#define BOS_IP_ADDRESS_LEN_BYTES_V4    4
#define BOS_IP_ADDRESS_LEN_BYTES_V6    16

#if BOS_CFG_IP_ADDR_V6_SUPPORT
   #define BOS_IP_ADDRESS_LEN_BYTES    BOS_IP_ADDRESS_LEN_BYTES_V6
#else
   #define BOS_IP_ADDRESS_LEN_BYTES    BOS_IP_ADDRESS_LEN_BYTES_V4
#endif


/*
** Max length for string representation of IP address.
*/
#define BOS_IP_ADDRESS_MAX_STR_LEN_V4  16 /* 255.255.255.255 */
#define BOS_IP_ADDRESS_MAX_STR_LEN_V6  46 /* ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255 */

#if BOS_CFG_IP_ADDR_V6_SUPPORT
   #define BOS_IP_ADDRESS_MAX_STR_LEN  BOS_IP_ADDRESS_MAX_STR_LEN_V6
#else
   #define BOS_IP_ADDRESS_MAX_STR_LEN  BOS_IP_ADDRESS_MAX_STR_LEN_V4
#endif


/*
** IP address types.
*/
typedef struct BOS_IP_ADDRESS_V4
{
   /* The fields in this structure are stored in network byte order.
   **
   ** An IP address like 192.187.10.1 would have 192 stored in element zero
   ** of 'u8', 187 in element 1, 10 in element 2, and 1 in element 3.*/
   union
   {
      BOS_UINT8   u8[4];
      BOS_UINT32  u32;
   } u;

   /* Note: Use of macro's and fields directly is not recommended.
    * Use bosIpAddr*() APIs.
    * */
   #define s4Addr    u.u8
   #define s4Addr32  u.u32

} BOS_IP_ADDRESS_V4;

typedef struct BOS_IP_ADDRESS_V6
{
   /* The fields in this structure are stored in network byte order. */
   union
   {
      BOS_UINT8   u8[16];
      BOS_UINT16  u16[8];
      BOS_UINT32  u32[4];
   } u;

   /* Note: Use of macro's and fields directly is not recommended.
    * Use bosIpAddr*() APIs.
    * */
   #define s6Addr    u.u8
   #define s6Addr16  u.u16
   #define s6Addr32  u.u32

} BOS_IP_ADDRESS_V6;


#if BOS_CFG_IP_ADDR_V6_SUPPORT
   typedef struct BOS_IP_ADDRESS
   {
      /* Note: Use of fields directly is not recommended.
       * Use bosIpAddr*() APIs.
       * */
      BOS_IP_ADDRESS_TYPE ipType; /* IPv4 or IPv6 */

      union
      {
         BOS_IP_ADDRESS_V4 ipv4Address;
         BOS_IP_ADDRESS_V6 ipv6Address;
      } u;

   } BOS_IP_ADDRESS;
#else
   typedef BOS_IP_ADDRESS_V4 BOS_IP_ADDRESS;
#endif



/*
** Helper macros to create IP address objects.
*/
#define bosIpAddrV4CreateFromU8( u8, ipAddr ) \
         bosIpAddrCreateFromU8( BOS_IPADDRESS_TYPE_V4, (u8), (ipAddr) )

#define bosIpAddrV6CreateFromU8( u8, ipAddr ) \
         bosIpAddrCreateFromU8( BOS_IPADDRESS_TYPE_V6, (u8), (ipAddr) )

#define bosIpAddrV4CreateZero( ipAddr ) \
         bosIpAddrCreateZero( BOS_IPADDRESS_TYPE_V4, (ipAddr) )

#define bosIpAddrV6CreateZero( ipAddr ) \
         bosIpAddrCreateZero( BOS_IPADDRESS_TYPE_V6, (ipAddr) )

#define bosIpAddrV4CreateLoopback( ipAddr ) \
         bosIpAddrCreateLoopback( BOS_IPADDRESS_TYPE_V4, (ipAddr) )

#define bosIpAddrV6CreateLoopback( ipAddr ) \
         bosIpAddrCreateLoopback( BOS_IPADDRESS_TYPE_V6, (ipAddr) )



/*
** Accessor macros.
*/
#if BOS_CFG_IP_ADDR_V6_SUPPORT
   #define bosIpAddrGetType( ipAddr )  ((ipAddr)->ipType)
   #define bosIpAddrIsV4( ipAddr )     ((ipAddr)->ipType == BOS_IPADDRESS_TYPE_V4)
   #define bosIpAddrIsV6( ipAddr )     ((ipAddr)->ipType == BOS_IPADDRESS_TYPE_V6)
#else
   #define bosIpAddrGetType( ipAddr )  (BOS_IPADDRESS_TYPE_V4)
   #define bosIpAddrIsV4( ipAddr )     (BOS_TRUE)
   #define bosIpAddrIsV6( ipAddr )     (BOS_FALSE)
#endif



/* ---- Variable Externs ------------------------------------------------- */
/* ---- Function Prototypes ---------------------------------------------- */


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
BOS_STATUS bosIpAddrInit( void );


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
BOS_STATUS bosIpAddrTerm( void );


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
);


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
);


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
);

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
BOS_STATUS bosIpAddrV6CreateFromV4
( 
  BOS_IP_ADDRESS *ipv4, 
  BOS_IP_ADDRESS *ipv6 
);


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
BOS_STATUS bosIpAddrV4CreateFromU32( BOS_UINT32 ipnum, BOS_IP_ADDRESS *ipp );


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
BOS_STATUS bosIpAddrCreateCopy( const BOS_IP_ADDRESS *fromip, BOS_IP_ADDRESS *toip );


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
BOS_STATUS bosIpAddrCreateZero( BOS_IP_ADDRESS_TYPE type, BOS_IP_ADDRESS *ipp );

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
BOS_STATUS bosIpAddrCreateAny( BOS_IP_ADDRESS_TYPE type, BOS_IP_ADDRESS *ipp );

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
BOS_STATUS bosIpAddrCreateLoopback( BOS_IP_ADDRESS_TYPE type, BOS_IP_ADDRESS *ipp );


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
BOS_STATUS bosIpAddrDestroy( BOS_IP_ADDRESS *ipp );


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
BOS_STATUS bosIpAddrGetStr( const BOS_IP_ADDRESS *ipp, char *ipstr, int maxlen );


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
);


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
BOS_STATUS bosIpAddrV4GetU32( const BOS_IP_ADDRESS *ipp, BOS_UINT32 *ipNum );


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
);


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
BOS_BOOL bosIpAddrIsEqual( const BOS_IP_ADDRESS *ip1, const BOS_IP_ADDRESS *ip2 );

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
void bosIpAddrPrint( const BOS_IP_ADDRESS *ipp );


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
BOS_BOOL bosIpAddrIsZero( const BOS_IP_ADDRESS *ipp );


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
BOS_BOOL bosIpAddrIsLoopback( const BOS_IP_ADDRESS *ipp );


#if defined( __cplusplus )
}
#endif

#endif /* BOSIPADDR_H */
