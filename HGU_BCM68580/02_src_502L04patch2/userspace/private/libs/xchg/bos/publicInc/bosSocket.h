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
*  @file    bosSocket.h
*
*  @brief   Contains the BOS Socket interface.
*
****************************************************************************/
/**
*  @defgroup   bosSocket   BOS Socket
*
*  @brief      Provides TCP/IP socket access.
*
*  The BOS Socket interface provides access to stream and UDP based
*  socket I/O.
*
*  Typical client side usage:
*
*  @code
*  @endcode
*
*  Typical server side usage:
*
*  @code
*  @endcode
*
****************************************************************************/

#if !defined( BOSSOCKET_H )
#define BOSSOCKET_H           /**< Include Guard                           */

/* ---- Include Files ---------------------------------------------------- */

#if !defined( BOSCFG_H )
#  include <bosCfg.h>
#endif

#if !defined( BOSTYPES_H )
#  include <bosTypes.h>
#endif

#if !defined( BOSIPADDR_H )
#  include <bosIpAddr.h>
#endif

#if !BOS_OS_LINUXKERNEL
#if !BOS_CFG_SOCKET
#error   BOS Socket module is not selected in bosCfg.h
#endif
#endif

#if BOS_OS_ECOS
#  include <../eCos/bosSocketECOS.h>
#elif BOS_OS_PSOS
#  include <../pSOS/bosSocketPSOS.h>
#elif BOS_OS_VXWORKS
#  include <../VxWorks/bosSocketVxWorks.h>
#elif BOS_OS_WIN32
#  include <../Win32/bosSocketWin32.h>
#elif BOS_OS_LINUXKERNEL
#  include <../LinuxKernel/bosSocketLinuxKernel.h>
#elif BOS_OS_LINUXUSER
#  include <../LinuxUser/bosSocketLinuxUser.h>
#else
#  error Unknown OS configuration from bosCfg.h
#endif

#if BOS_CFG_TASK
#  include <bosTask.h>
#endif


#if defined( __cplusplus )
extern "C"
{
#endif

/**
 * @addtogroup bosSocket
 * @{
 */

/* ---- Constants and Types ---------------------------------------------- */

#define  BOS_SOCKET_AF_INET      AF_INET
#define  BOS_SOCKET_PF_INET      PF_INET
#define  BOS_SOCKET_SOL_SOCKET   SOL_SOCKET
#define  BOS_SOCKET_SO_SNDBUF    SO_SNDBUF
#define  BOS_SOCKET_SO_RCVBUF    SO_RCVBUF
#define  BOS_SOCKET_SO_REUSEADDR SO_REUSEADDR
#define  BOS_SOCKET_SO_REUSEPORT SO_REUSEPORT
#define  BOS_SOCKET_SO_BROADCAST SO_BROADCAST
#define  BOS_SOCKET_SO_ERROR     SO_ERROR
#define  BOS_SOCKET_SO_KEEPALIVE SO_KEEPALIVE
#define  BOS_SOCKET_SO_LINGER    SO_LINGER
#define  BOS_SOCKET_SO_TYPE      SO_TYPE
#define  BOS_SOCKET_TCP_NODELAY  TCP_NODELAY
#define  BOS_SOCKET_IPPROTO_IP   IPPROTO_IP
#define  BOS_SOCKET_IPPROTO_TCP  IPPROTO_TCP
#define  BOS_SOCKET_IPPROTO_UDP  IPPROTO_UDP
#define  BOS_SOCKET_IP_TOS       IP_TOS
#define  BOS_SOCKET_IP_TTL       IP_TTL
#define  BOS_SOCKET_IP_ADD_MEMBERSHIP       IP_ADD_MEMBERSHIP
#define  BOS_SOCKET_IP_DROP_MEMBERSHIP      IP_DROP_MEMBERSHIP

#define  BOS_SOCKET_STREAM       SOCK_STREAM
#define  BOS_SOCKET_DGRAM        SOCK_DGRAM

#define  BOS_SOCKET_INADDR_ANY   INADDR_ANY

#if BOS_CFG_IP_ADDR_V6_SUPPORT
   #define  BOS_SOCKET_AF_INET6         AF_INET6
   #define  BOS_SOCKET_PF_INET6         PF_INET6
   #define  BOS_SOCKET_INADDR_ANY_IN6   in6addr_any

   #define  BOS_SOCKET_IPPROTO_IP6      IPPROTO_IPV6
   #define  BOS_SOCKET_IPV6_PKTINFO     IPV6_PKTINFO
   #define  BOS_SOCKET_IP6_HOPS         IPV6_UNICAST_HOPS 
#endif

/* Scope Id is used in bosSocketBindIp for IPv6. For IPv4, it is not used */
#define BOS_SOCKET_SCOPE_ID_NOTUSED    (int)0

/*
 * Many OS's don't define these constants.
 */

#if !defined( SD_RECEIVE )
#define SD_RECEIVE      0x00
#endif
#if !defined( SD_SEND )
#define SD_SEND         0x01
#endif
#if !defined( SD_BOTH )
#define SD_BOTH         0x02
#endif

typedef enum
{
   BOS_SOCKET_SHUTDOWN_SEND      = SD_SEND,
   BOS_SOCKET_SHUTDOWN_RECEIVE   = SD_RECEIVE,
   BOS_SOCKET_SHUTDOWN_BOTH      = SD_BOTH

} BOS_SOCKET_SHUTDOWN_HOW;

/**
*  Type which contains a port number used for TCP/IP.
*/

typedef  BOS_UINT16  BOS_SOCKET_IP_PORT;

#if BOS_CFG_SOCKET_BSD_4_3
   typedef BOS_UINT16   BOS_SOCKET_FAMILY;
#elif BOS_CFG_SOCKET_BSD_4_4
   typedef BOS_UINT8    BOS_SOCKET_FAMILY;
#else
  #if !BOS_OS_LINUXKERNEL
     #error "Unknown BSD compatibility version!"
  #endif
#endif


/***************************************************************************/
/**
*  Defines the socket address. Since most of the time we're dealing with
*  TCP/IP addresses, this is the one that's presented.
*
*  @note The fields in this structure are stored in @b network order.
*
*  An IP address like 192.187.10.1 would have 192 stored in element zero
*  of addr.u8, 187 in element 1, 10 in element 2, and 1 in element 3.
*/

typedef struct
{
#if BOS_CFG_SOCKET_BSD_4_4
   BOS_UINT8             length;
#endif
   BOS_SOCKET_FAMILY    family;     /**< Address family.                   */
   BOS_SOCKET_IP_PORT   port;    /**< TCP/IP port.                         */
   union
   {
      BOS_UINT32           u32;     /**< Address as a 32 bit entity        */
      BOS_UINT8            u8[ 4 ]; /**< Address as 4 x 8 bit entities.    */

   } ipAddr;   /**< Union containing 8 and 32 bit representations.         */

   BOS_UINT8   zero[ 8 ];  /**< Reserved.                                  */

   /* Need to add extra padding for eCos */
#if BOS_OS_ECOS
   BOS_UINT8   pad[ 16 ];
#endif
} BOS_SOCKET_ADDR_IN4;

/* Argument structure for BOS_SOCKET_IP_ADD_MEMBERSHIP and BOS_SOCKET_IP_DROP_MEMBERSHIP. */
typedef struct
{
   union
   {
      BOS_UINT32           u32;     /**< Address as a 32 bit entity        */
      BOS_UINT8            u8[ 4 ]; /**< Address as 4 x 8 bit entities.    */

   } multiaddr;   /* IP multicast address of group */
   union
   {
      BOS_UINT32           u32;     /**< Address as a 32 bit entity        */
      BOS_UINT8            u8[ 4 ]; /**< Address as 4 x 8 bit entities.    */

   } interface_;   /* local IP address of interface */
} BOS_SOCKET_MREQ;


#if BOS_CFG_IP_ADDR_V6_SUPPORT
typedef struct 
{
#if BOS_CFG_SOCKET_BSD_4_4
    BOS_UINT8             length;
#endif
    BOS_SOCKET_FAMILY             family;     /**< Address family.                   */
    BOS_SOCKET_IP_PORT            sin6_port;          /* Transport layer port # */
    BOS_UINT32                    sin6_flowinfo;      /* IPv6 flow information */
    BOS_IP_ADDRESS_V6             sin6_addr;          /* IPv6 address */
    BOS_UINT32                    sin6_scope_id;      /* IPv6 scope-id */
} BOS_SOCKET_ADDR_IN6;


/* To support IPv6 BOS_SOCKET_ADDR_STORAGE is introduced. From RFC 3493: 
 This data structure is designed with the
 following goals.

 - Large enough to accommodate all supported protocol-specific address
 structures.

 - Aligned at an appropriate boundary so that pointers to it can be
 cast as pointers to protocol specific address structures and used
 to access the fields of those structures without alignment
 problems.

 The sockaddr_storage structure contains field ss_family which is of
 type sa_family_t.  When a sockaddr_storage structure is cast to a
 sockaddr structure, the ss_family field of the sockaddr_storage
 structure maps onto the sa_family field of the sockaddr structure.
 When a sockaddr_storage structure is cast as a protocol specific
 address structure, the ss_family field maps onto a field of that
 structure that is of type sa_family_t and that identifies the
 protocol's address family.
 *
 * */
/* Support IPv4 or IPv6 */
#define  _ss_maxsize    28
#define	_ss_alignsize	(sizeof(BOS_UINT64))

#if BOS_CFG_SOCKET_BSD_4_3
   #define  _ss_pad1size   (_ss_alignsize - sizeof(BOS_SOCKET_FAMILY))
   #define  _ss_pad2size   (_ss_maxsize - sizeof(BOS_SOCKET_FAMILY) - \
            _ss_pad1size - _ss_alignsize)
#elif BOS_CFG_SOCKET_BSD_4_4
   #define  _ss_pad1size   (_ss_alignsize - sizeof(BOS_UINT8) - sizeof(BOS_SOCKET_FAMILY))
   #define  _ss_pad2size   (_ss_maxsize -  sizeof(BOS_UINT8) - sizeof(BOS_SOCKET_FAMILY) - \
  				         _ss_pad1size - _ss_alignsize)
#else
   #error "Unknown BSD compatibility version!"
#endif



typedef struct
{
#if BOS_CFG_SOCKET_BSD_4_4
   BOS_UINT8         length;
#endif
   BOS_SOCKET_FAMILY family;     /**< Address family.                   */
   BOS_UINT8  __ss_pad1[_ss_pad1size];
   BOS_UINT64 __ss_align;	            /* force desired structure storage alignment */
   BOS_UINT8  __ss_pad2[_ss_pad2size];

} BOS_SOCKET_ADDR_STORAGE;
#endif


#if BOS_CFG_IP_ADDR_V6_SUPPORT
   typedef BOS_SOCKET_ADDR_STORAGE  BOS_SOCKET_ADDR;
#else
   typedef BOS_SOCKET_ADDR_IN4      BOS_SOCKET_ADDR;
#endif

/**
* Helper macro which creates a TCP based TCP/IP socket (stream).
*/

#define  bosSocketCreateStream( socketp ) \
            bosSocketCreate( BOS_SOCKET_AF_INET, BOS_SOCKET_STREAM, 0, socketp )

/**
* Helper macro which creates a UDP based TCP/IP socket (datagram).
*/

#define  bosSocketCreateDatagram( socketp ) \
            bosSocketCreate( BOS_SOCKET_AF_INET, BOS_SOCKET_DGRAM, 0, socketp )

/**
 * Function index, used with the error handler to report which function
 * an error originated from.
 *
 * These constants would typically only be used if you provide your own
 * error handler.
 *
 * @note If you change anything in this enumeration, please ensure that
 *       you make the corresponding updates in the bosSocketInitFunctionNames
 *       routine.
 */

typedef enum
{
   BOS_SOCKET_INIT_FUNC_ID,               /**< bosSocketInit               */
   BOS_SOCKET_TERM_FUNC_ID,               /**< bosSocketTerm               */
   BOS_SOCKET_ACCEPT_FUNC_ID,             /**< bosSocketAccept             */
   BOS_SOCKET_BIND_FUNC_ID,               /**< bosSocketBind               */
   BOS_SOCKET_CLOSE_FUNC_ID,              /**< bosSocketClose              */

#if BOS_CFG_TASK
   BOS_SOCKET_CLOSE_SHARED_FUNC_ID,       /**< bosSocketCloseShared        */
#endif   /* BOS_CFG_TASK */

   BOS_SOCKET_CONNECT_FUNC_ID,            /**< bosSocketConnect            */
   BOS_SOCKET_CREATE_FUNC_ID,             /**< bosSocketCreate             */
   BOS_SOCKET_CREATE_ADDR_FUNC_ID,        /**< bosSocketAddrCreate         */
   BOS_SOCKET_ADDR_GETIPINFO_FUNC_ID,     /**< bosSocketAddrGetIpInfo      */
   BOS_SOCKET_GET_OPTION_FUNC_ID,         /**< bosSocketGetOption          */
   BOS_SOCKET_LISTEN_FUNC_ID,             /**< bosSocketListen             */
   BOS_SOCKET_RECV_FUNC_ID,               /**< bosSocketRecv               */
   BOS_SOCKET_RECV_FROM_FUNC_ID,          /**< bosSocketRecvFrom           */

#if BOS_CFG_TASK
   BOS_SOCKET_SHARE_FUNC_ID,              /**< bosSocketShare              */
#endif   /* BOS_CFG_TASK */

   BOS_SOCKET_SELECT_FUNC_ID,             /**< bosSocketSelect             */
   BOS_SOCKET_SEND_FUNC_ID,               /**< bosSocketSend               */
   BOS_SOCKET_SEND_TO_FUNC_ID,            /**< bosSocketSendTo             */
   BOS_SOCKET_SET_NO_DELAY_FUNC_ID,       /**< bosSocketSetNoDelay         */
   BOS_SOCKET_SET_NON_BLOCKING_FUNC_ID,   /**< bosSocketSetNonBlocking     */
   BOS_SOCKET_SET_OPTION_FUNC_ID,         /**< bosSocketSetOption          */
   BOS_SOCKET_SHUTDOWN_FUNC_ID,           /**< bosSocketShutdown           */
   BOS_SOCKET_SET_TOS_ID,                 /**< bosSocketSetIpTos           */
   BOS_SOCKET_GET_NAME_FUNC_ID,           /**< bosSocketGetName            */
   BOS_SOCKET_GET_PEERNAME_FUNC_ID,        /**< bosSocketGetPeerName        */

#if BOS_CFG_SOCKET_INTERFACE_SUPPORT
   BOS_SOCKET_SET_INTERFACE_ID,           /**< bosSocketSetInterface       */
#endif
#if BOS_CFG_IP_ADDR_V6_SUPPORT
   BOS_SOCKET_SET_IPV6_PKTINFO_ID,           /**< bosSocketSetIPv6PktInfo  */
#endif

   BOS_SOCKET_NUM_FUNC_ID   /**< Number of functions in the Socket module. */

} BOS_SOCKET_FUNC_ID;

/***************************************************************************/
/**
*  Converts an unsigned 32 bit number from host order to network order.
*/

#define  bosSocketHtoNU32( num ) ((BOS_UINT32)bosSocketHtoNL( (BOS_UINT32)(num) ))
#define  bosSocketHtoNS32( num ) ((BOS_SINT32)bosSocketHtoNL( (BOS_UINT32)(num) ))

#define  bosSocketNtoHU32( num ) ((BOS_UINT32)bosSocketNtoHL( (BOS_UINT32)(num) ))
#define  bosSocketNtoHS32( num ) ((BOS_SINT32)bosSocketNtoHL( (BOS_UINT32)(num) ))

#define  bosSocketHtoNU16( num ) ((BOS_UINT16)bosSocketHtoNS( (BOS_UINT16)(num) ))
#define  bosSocketHtoNS16( num ) ((BOS_SINT16)bosSocketHtoNS( (BOS_UINT16)(num) ))

#define  bosSocketNtoHU16( num ) ((BOS_UINT16)bosSocketNtoHS( (BOS_UINT16)(num) ))
#define  bosSocketNtoHS16( num ) ((BOS_SINT16)bosSocketNtoHS( (BOS_UINT16)(num) ))

/***************************************************************************/
/**
*  Sets a socket in a socket set.
*/

#if !defined( bosSocketSetAddSocket )
#  define bosSocketSetAddSocket( socketp, socketSetp )    \
               FD_SET( bosSocketGetNativeId(socketp), socketSetp )
#endif

/***************************************************************************/
/**
*  Determines if a socket is contained within a socket set.
*
*  @return  BOS_TRUE if @a socket is contained within @a socketSet.
*/

#if !defined( bosSocketSetIsSocketSet )
#  define bosSocketSetIsSocketSet( socketp, socketSetp )  \
               FD_ISSET( bosSocketGetNativeId(socketp), socketSetp )
#endif

/***************************************************************************/
/**
*  Removes a socket from a socket set.
*/

#if !defined( bosSocketSetRemoveSocket )
#  define bosSocketSetRemoveSocket( socketp, socketSetp ) \
               FD_CLR( (unsigned int)bosSocketGetNativeId(socketp), socketSetp )
#endif

/***************************************************************************/
/**
*  Zeros out the entries for a socket set.
*/

#if !defined( bosSocketSetZero )
# define bosSocketSetZero( socketSetp )                   FD_ZERO( socketSetp )
#endif

/* ---- Variable Externs ------------------------------------------------- */

/**
 * Contains string versions of the socket function names. This table is
 * indexed using values from the BOS_SOCKET_FUNC_ID enumeration.
 */

extern const char *gBosSocketFunctionName[];

/* ---- Function Prototypes ---------------------------------------------- */

#if ( BOS_CFG_RESET_SUPPORT )

   #if ( !BUILDING_BOS)
      /* If reset support is enabled, application code (non-OS) will use the
      ** re-defined function prototypes for the reset-aware versions of the following
      ** blocking functions */
      #define bosSocketSelect       bosSocketSelectR
      #define bosSocketRecvFrom     bosSocketRecvFromR
      #define bosSocketRecvFromIp   bosSocketRecvFromIpR
   #else
      /***************************************************************************/
      /**
      *  This function is used to multiplex I/O requests among multiple sockets.
      *  Three sets of socket descriptors may be specified: a set of sockets from
      *  which to read, a set to which to write and a set that may have pending
      *  exceptional conditions.
      *
      *  If bosSocketSelectR returns successfully, the three sets indicate which
      *  socket descriptors can be read, which can be written to, or which have
      *  exceptional conditions pending. A timeout value may be specified.
      *
      *  @param   readset  (in)  Set of sockets from which to read
      *  @param   writeset (in)  Set of sockets from which to write
      *  @param   exceptset(in)  Set of sockets that may have an exceptional
      *                          condition pending
      *  @param   timeout  (in)  Specifies a timeout option. If the timeout is
      *                          set to 0, this function returns immediately. If
      *                          the timeout is OS_WAIT_FOREVER, this function
      *                          blocks until a descriptor is selectable.
      *  @param   numDesc  (out) On success a nonzero value is returned that
      *                          indicates the number of descriptors on which
      *                          events have occurred.
      *
      *  @return  BOS_STATUS_OK or error code
      *
      *  @note    bosSocketSelectR() is the reset equivalent of bosSocketSelect().
      *           It will return BOS_STATUS_RESET if the calling task has been
      *           notified of a reset.
      */
      BOS_STATUS bosSocketSelectR
      (
         int             maxSockets,
         BOS_SOCKET_SET *readset,
         BOS_SOCKET_SET *writeset,
         BOS_SOCKET_SET *exceptset,
         BOS_TIME_MS     timeout,
         int            *numDesc
      );

      /****************************************************************************/
      /**
      *  Receive a message from a socket
      *
      *  @param   sd          (in)  socket to receive from
      *  @param   buf         (in)  pointer to data buffer
      *  @param   buflen      (in)  length of buffer
      *  @param   flags       (in)  flags to underlying protocols
      *  @param   ifno        (in)  specifies outgoing interface for message
      *  @param                     (0 for default interface)
      *  @param   from        (out) where to copy sender's address
      *  @param   fromLen     (out) pointer to size of "from" buffer
      *  @param   numBytesRec (out) number of bytes received
      *
      *  @return  BOS_STATUS_OK or error code
      *
      *  @note    bosSocketRecvFromR() is the reset equivalent of bosSocketRecvFrom().
      *           It will return BOS_STATUS_RESET if the calling task has been
      *           notified of a reset.
      */
      BOS_STATUS bosSocketRecvFromR
      (
         BOS_SOCKET       *sd,
         char             *buf,
         int               buflen,
         int               flags,
         BOS_SOCKET_ADDR  *from,
         int              *fromLen,
         int              *numBytesRec
      );

      /* v6/v4 version */
      BOS_STATUS bosSocketRecvFromIpR
      (
         BOS_SOCKET          *sd,
         char                *buf,
         int                  buflen,
         int                  flags,
         BOS_IP_ADDRESS       *from,
         BOS_SOCKET_IP_PORT   *port,
         int                 *numBytesRec
      );
   #endif

#endif   /* BOS_CFG_RESET_SUPPORT */

/***************************************************************************/
/**
*  Permits an incoming connection attempt on a socket.
*
*  @return  BOS_STATUS_OK if the incoming connection was accepted sucessfully,
*           or an error code if an incoming connection could not be accepted.
*/

BOS_STATUS bosSocketAccept
(
   BOS_SOCKET        *listenSocket,    /**< (in) Socket being listened on for incoming connections.   */
   BOS_SOCKET_ADDR   *remoteAddr,      /**< (out) Place to store address of remote entity.            */
   size_t            *remoteAddrLen,   /**< (out) Length of received address.                         */
   BOS_SOCKET        *acceptedSocket   /**< (out) New socket created for the remote entity.           */
);

/* v6/v4 version */
BOS_STATUS bosSocketAcceptIp
(
   BOS_SOCKET           *listenSocket,    
   BOS_IP_ADDRESS       *remoteAddr,      
   BOS_SOCKET_IP_PORT   *remotePort,
   BOS_SOCKET           *acceptedSocket   
);

/***************************************************************************/
/**
*  Binds a local address to a socket. This is typically used by servers.
*
*  @return  BOS_STATUS_OK if the socket could be bound successfully, or an
*           error code if the bind failed.
*/

BOS_STATUS bosSocketBind
(
   BOS_SOCKET             *s,             /**< (mod) Socket to bind to an address         */
   const BOS_SOCKET_ADDR  *localAddr,     /**< (in) Local address to bind the socket to.  */
   size_t                  localAddrLen   /**< (in) Length of the local address.          */
);

/***************************************************************************/
/**
*  Binds an IP address to a socket. This is typically used by servers.
*
*  Note that this function expects the IP Address and port to be in native
*  order.
*
*  @return  BOS_STATUS_OK if the socket could be bound successfully, or an
*           error code if the bind failed.
*/

BOS_STATUS bosSocketBindIp
(
   BOS_SOCKET             *s,       /**< (mod) Socket to bind to an address   */
   int                    scope_id, /**< (in)  scopeid for v6 sockets         */
   BOS_IP_ADDRESS         *ipAddr,  /**< (in)  IP Address to bind to          */
   BOS_SOCKET_IP_PORT      port     /**< (in)  IP port to bind to             */
);

/***************************************************************************/
/**
*  Closes a previously opened socket.
*
*  @return  BOS_STATUS_OK if the socket was closed successfully, or an error
*           code if the socket couldn't be closed.
*/

BOS_STATUS bosSocketClose
(
   BOS_SOCKET  *s    /**< (mod) Socket to close                            */
);

#if BOS_CFG_TASK
/***************************************************************************/
/**
*  Closes a previously shared socket.
*
*  @param   s  (mod) socket to close
*
*  @return  BOS_STATUS_OK if the socket was closed successfully, or an error
*           code if the socket couldn't be closed.
*/

BOS_STATUS bosSocketCloseShared
(
   BOS_SOCKET    *s  /**< (mod) Socket to close                            */
);
#endif   /* BOS_CFG_TASK */

/***************************************************************************/
/**
*  Establishes a connection with the specified socket. This is typically
*  used by the client side.
*
*  @return  BOS_STATUS_OK if the connection was created successfully, or an
*           error code if the connection could not be established.
*/

BOS_STATUS bosSocketConnect
(
   BOS_SOCKET             *s,             /**< (mod) Local socket that we'd like to connect.                   */
   const BOS_SOCKET_ADDR  *remoteAddr,    /**< (in) Address of the remote entity that we'd like to connect to. */
   size_t                  remoteAddrLen  /**< (in) Length of address structure.                               */
);

/***************************************************************************/
/**
*  Establishes a connection with the specified socket. This is typically
*  used by the client side.
*
*  @note    @a ipAddr and @a port are expected to be in native order.
*
*  @return  BOS_STATUS_OK if the connection was created successfully, or an
*           error code if the connection could not be established.
*/

BOS_STATUS bosSocketConnectIp
(
   BOS_SOCKET             *s,       /**< (mod) Local socket that we'd like to connect. */
   BOS_IP_ADDRESS         *ipAddr,  /**< (in) IP address to connect to.                */
   BOS_SOCKET_IP_PORT      port     /**< (in) IP port to connect to.                   */
);

/***************************************************************************/
/**
*  Creates a socket that is bound to a specific service provider.
*
*  @return  BOS_STATUS_OK if the socket was created successfully, or an
*           error code if the socket could not be created.
*
*  @note    There are two helper macros #bosSocketCreateStream and
*           #bosSocketCreateDatagram which create TCP and UDP based sockets
*           for use with the TCP/IP protocol.
*/

BOS_STATUS bosSocketCreate
(
   int         addrFamily, /**< (in) Address family specification.                           */
   int         socketType, /**< (in) Type of socket to create (within the address family).   */
   int         protocol,   /**< (in) Protocol to use within the address family.              */
   BOS_SOCKET *outSocket   /**< (out) Place to put the newly created socket.                 */
);

/***************************************************************************/
/**
*  Creates a socket address from a BOS_IP_ADDRESS 
*
*  @return  BOS_STATUS_OK if the socket address was created successfully, or an
*           error code if the socket address could not be created.
*
*  @note    
*/
BOS_STATUS bosSocketAddrCreate
(
   BOS_IP_ADDRESS       *addr,
   BOS_SOCKET_IP_PORT   addrport,
   int                  scopeId,
   BOS_SOCKET_ADDR      *rAddr
);

/***************************************************************************/
/**
*  get IP_ADDRESS info from a socket addr struct 
*
*  @return  BOS_STATUS_OK if the info was extracted successfully 
*           error code otherwise 
*
*  @note    
*/
BOS_STATUS bosSocketAddrGetIpInfo
(
   BOS_SOCKET_ADDR      *addr,
   int                  *addrfamily,
   BOS_SOCKET_IP_PORT   *addrport,
   BOS_IP_ADDRESS       *ipaddr
);


/***************************************************************************/
/**
*   get IP_ADDRESS family info from a socket addr struct
*
*  @return  BOS_STATUS_OK if the family info was extracted successfully 
*           error code otherwise 
*  @note
*/
BOS_STATUS bosSocketGetIpAddrFamily
(
   BOS_IP_ADDRESS       *ipaddr,
   int                  *addrFamily
);



/***************************************************************************/
/**
*  Retrieves the value of a socket option.
*
*  @return  BOS_STATUS_OK if the socket option was retrieved successfully,
*           or an error code if the option could not be retrieved.
*
*  @note    See the documentation for the OS specific getsockopt function
*           for complete documentation on the various options which may
*           be retrieved.
*
*  @note    @a optionLen should contain the size of the @a optionVal buffer,
*           and it will be modified to contain the size of the data actually
*           copied into the buffer.
*/

BOS_STATUS bosSocketGetOption
(
   BOS_SOCKET *s,          /**< (mod) Socket to set the option for.        */
   int         level,      /**< (in) Level at which the option is for.     */
   int         option,     /**< (in) Option to set.                        */
   void       *optionVal,  /**< (out) Place to store retrieved value.      */
   size_t     *optionLen   /**< (mod) Length of option that was retrieved. */
);

/***************************************************************************/
/**
*  Initializes the BOS Socket Module.
*
*  @return  BOS_STATUS_OK id the intialization was successful, or an error
*           code if some type of error occurred.
*/

BOS_STATUS bosSocketInit( void );

/***************************************************************************/
/**
*  Places a socket in a state where it is listening for an incoming
*  connection.
*
*  @return  BOS_STATUS_OK if an incoming connection was established, or an
*           code if some type of error occurred.
*/

BOS_STATUS bosSocketListen
(
   BOS_SOCKET *s,       /**< Socket to put into a listening state.         */
   int         backLog  /**< Max length of queue of pending connections.   */
);

/***************************************************************************/
/**
*  Translates a string version of an IP address into a numeric equivalent.
*
*  @return  BOS_STATUS_OK if @a str was parsed successfully, BOS_STATUS_ERR
*           otherwise.
*
*  @note This function can only do symbolic lookups on some platforms.
*
*        For example: 10.10.10.10:10 will work on all platforms, but
*        somehost:20 will only work on platforms which have resolver
*        capabilities.
*/

BOS_STATUS bosSocketLookupServerAndPort
(
   const char           *str,    /**< String to translate                  */
   BOS_IP_ADDRESS       *ipAddr, /**< Place to store IP address.           */
   BOS_SOCKET_IP_PORT   *ipPort, /**< Place to store IP port.              */
   char                 *errStr, /**< Place to store reason lookup failed. */
   size_t                maxLen  /**< Length of errStr.                    */
);

/***************************************************************************/
/**
*  Receives data from connected or bound socket.
*
*  @return  BOS_STATUS_OK if data was received successfully, or an error
*           code if some type of error occurred.
*
*  @note    A graceful shutdown of the connection will return success with
*           @a bytesRcvd set to zero.
*/

BOS_STATUS bosSocketRecv
(
   BOS_SOCKET  *s,         /**< (mod) Socket to receive the data from.        */
   void        *buf,       /**< (out) Place to store the received data.       */
   size_t      bufLen,     /**< (in) Size of @a buf.                          */
   int         flags,      /**< (in) Flags which modify how recv functions.   */
   size_t     *bytesRcvd   /**< (out) Number of bytes actually received.      */
);

/***************************************************************************/
/**
*  Receives data from a bound socket.
*
*  @return  BOS_STATUS_OK if data was received successfully, or an error
*           code if some type of error occurred.
*
*  @note    @a fromLen should contain the size of the @a fromAddr buffer.
*           On return, it contains the actual size of the addr stored in @a fromAddr
*/

BOS_STATUS bosSocketRecvFrom
(
   BOS_SOCKET       *s,          /**< (mod) Socket to receive the data from.     */
   void             *buf,        /**< (out) Place to store the received data.    */
   size_t            bufLen,     /**< (in) Size of @a buf.                       */
   int               flags,      /**< (in) Flags which modify how recv functions.*/
   BOS_SOCKET_ADDR  *fromAddr,   /**< (out) Address data was received from.      */
   int              *fromLen,    /**< (mod) Size of @a fromAddr.                 */
   size_t           *bytesRcvd   /**< (out) Number of bytes actually received.   */
);

BOS_STATUS bosSocketRecvFromIp
(
   BOS_SOCKET           *s,          /**< (mod) Socket to receive the data from.     */
   void                 *buf,        /**< (out) Place to store the received data.    */
   size_t                bufLen,     /**< (in) Size of @a buf.                       */
   int                   flags,      /**< (in) Flags which modify how recv functions.*/
   BOS_IP_ADDRESS       *fromAddr,   /**< (out) Address data was received from.      */
   BOS_SOCKET_IP_PORT   *fromPort,    /**< (out) port              .                 */
   size_t               *bytesRcvd   /**< (out) Number of bytes actually received.   */
);

/***************************************************************************/
/**
*  Determines the status of one or more sockets, waiting if necessary,
*  to perform synchronous I/O.
*
*  @return  BOS_STATUS_OK if one or more of the sockets are ready to
*           perform synchronous I/O. BOS_STATUS_TIMEOUT is returned if
*           the time limit expired, or an error code is returned for other
*           errors.
*
*  @note    The various socket sets are modified to indicate which sockets
*           are ready for the indicated type of I/O.
*/

BOS_STATUS bosSocketSelect
(
   int             maxSockets,      /**< (in) Highest numbered socket in any of the socket sets             */
   BOS_SOCKET_SET *readSet,         /**< (mod) Set of sockets to be checked for readability. May be NULL.   */
   BOS_SOCKET_SET *writeSet,        /**< (mod) Set of sockets to be checked for writability. May be NULL.   */
   BOS_SOCKET_SET *exceptSet,       /**< (mod) Set of sockets to be checked for errors.      May be NULL.   */
   BOS_TIME_MS     timeLimit,       /**< (in) Time to wait for an operation. May be BOS_WAIT_FOREVER.       */
   int            *socketsSelected  /**< (out) Number of sockets that are ready.                            */
);

/***************************************************************************/
/**
*  Determine from an IP address the type of socket that is required
*
*  @return  BOS_STATUS_OK if successful, BOS_STATUS_ERR otherwise 
*           
*
*  @note    
*/

BOS_STATUS bosSocketMapIpAddrType
(
   BOS_IP_ADDRESS      *ipAddr,    /**< IP Address */
   int                 *sockType
);


/***************************************************************************/
/**
*  Sends data to a connected or bound socket.
*
*  @return  BOS_STATUS_OK if the data was sent successfully, or an error
*           code if the data couldn't be sent.
*
*  @note    This function may not send all of the data. This is NOT
*           considered to be an error.
*/

BOS_STATUS bosSocketSend
(
   BOS_SOCKET  *s,         /**< Socket to send the data to.                */
   const void  *data,      /**< Pointer to data to send.                   */
   size_t       dataLen,   /**< Number of bytes of data to send.           */
   int          flags,     /**< Flags indicating how data should be sent.  */
   size_t      *bytesSent  /**< Number of bytes actually sent.             */
);

/***************************************************************************/
/**
*  Sends data to a bound socket.
*
*  @return  BOS_STATUS_OK if the data was sent successfully, or an error
*           code if the data couldn't be sent.
*
*  @note    This function may not send all of the data. This is NOT
*           considered to be an error.
*
*           @a toLen should contain the size of the @a toAddr buffer.
*           On return, it contains the actual size of the addr stored in @a toAddr
*/

BOS_STATUS bosSocketSendTo
(
   BOS_SOCKET       *s,         /**< Socket to send the data to.              */
   const void       *data,      /**< Pointer to data to send.                 */
   size_t            dataLen,   /**< Number of bytes of data to send.         */
   int               flags,     /**< Flags indicating how data should be sent.*/
   BOS_SOCKET_ADDR  *toAddr,    /**< Address to send data to.                 */
   int               toLen,     /**< Size of @a toAddr.                       */
   size_t           *bytesSent  /**< Number of bytes actually sent.           */
);

/* v4/v6 version */
BOS_STATUS bosSocketSendToIp
(
   BOS_SOCKET          *s,         /**< Socket to send the data to.              */
   const void          *data,      /**< Pointer to data to send.                 */
   size_t               dataLen,   /**< Number of bytes of data to send.         */
   int                  flags,     /**< Flags indicating how data should be sent.*/
   BOS_IP_ADDRESS      *toAddr,    /**< Address to send data to.                 */
   BOS_SOCKET_IP_PORT  toPort,     /**< port                                     */
   int                 scopeId,    /**< scopeId for v6 multiple interfaces       */
   size_t              *bytesSent  /**< Number of bytes actually sent.           */
);

#if BOS_CFG_TASK
/***************************************************************************/
/**
*  Shares a socket with another task. Some OS's require this for a socket
*  created in one task to be used in another.
*/

BOS_STATUS bosSocketShare
(
   BOS_SOCKET    *srcSocket,  /**< The socket that we want to share.       */
   BOS_TASK_ID   *dstTaskId,  /**< The task we want to share it with.      */
   BOS_SOCKET    *dstSocket   /**< The new socket that should be used.     */
);

#endif   /* BOS_CFG_TASK */

/***************************************************************************/
/**
*  Enables the BOS_SOCKET_TCP_NODELAY option on the socket. This causes the send
*  function to send data as quickly as possible, rather than waiting for a
*  timeout to occur.
*
*  @param   s  (mod) Socket to set to BOS_SOCKET_TCP_NODELAY option on.
*
*  @note BOS_SOCKET_TCP_NODELAY is off by default.
*/

BOS_STATUS bosSocketSetNoDelay( BOS_SOCKET *s, BOS_BOOL noDelay );

/***************************************************************************/
/**
*  Makes the socket a non-blocking socket. This causes the bosSocketSend()
*  and bosSocketRecv() functions to return BOS_STATUS_WOULD_BLOCK.
*
*  @param   s  (mod) Socket to set to non-blocking.
*
*  @note Sockets are blocking by default.
*/

BOS_STATUS bosSocketSetNonBlocking( BOS_SOCKET *s, BOS_BOOL nonBlocking );

/***************************************************************************/
/**
*  Sets a socket option.
*
*  @return  BOS_STATUS_OK if the socket option was set successfully, or an
*           error code if the option could not be set.
*
*  @note    See the documentation for the OS specific setsockopt function
*           for complete documentation on the various options which may
*           be set.
*/

BOS_STATUS bosSocketSetOption
(
   BOS_SOCKET *s,          /**< (mod) Socket to set the option for.        */
   int         level,      /**< (in) Level at which the option is for.     */
   int         option,     /**< (in) Option to set.                        */
   const void *optionVal,  /**< (in) Pointer to value to set.              */
   size_t      optionLen   /**< (in) Length of option being set.           */
);

#if BOS_CFG_SOCKET_INTERFACE_SUPPORT
/***************************************************************************/
/**
*  Sets a socket's interface.
*
*  @return  BOS_STATUS_OK if the socket interface was set successfully, or an
*           error code if the interface could not be set.
*
*  @note    This functionality is not supported on all OSes and all versions.
*/

BOS_STATUS bosSocketSetInterface
(
   BOS_SOCKET *s,          /**< (mod) Socket to set the interface for.     */
   int         ifno        /**< (in) Interface number.                     */
);
#endif

#if BOS_CFG_IP_ADDR_V6_SUPPORT
/***************************************************************************/
/**
*  Sets a socket's interface.
*
*  @return  BOS_STATUS_OK if the ipv6 pktinfo socket extension was set successfully, or an
*           error code if this extension could not be set.
*
*  @note    This functionality is not supported on all OSes and all versions.
*/
BOS_STATUS bosSocketSetIPv6PktInfo
( 
   BOS_SOCKET    *s,       /**< (mod) Socket to set the interface for.     */
   int           scope_id  /**< (in) v6 scope id.                          */ 
);
#endif

/***************************************************************************/
/**
*  Disables sends or receives on a socket. This function is normally used
*  to perform a graceful shutdown of the socket connection.
*
*  To ensure that no data is lost, the typical sequence it as follows:
*
*  bosSocketShutdown( &s, BOS_SOCKET_SHUTDOWN_SEND );
*
*  Then wait for the recv call to return zero.
*  Call bosSocketClose().
*
*  @return  BOS_STATUS_OK if the socket shutdown was initiated successfully,
*           or an error code if the shutdown could not be performed.
*/

BOS_STATUS bosSocketShutdown
(
   BOS_SOCKET *s,                /**< (mod) Socket to set the option for.  */
   BOS_SOCKET_SHUTDOWN_HOW how   /**< (in) Type of shutdown to perform.    */
);

/***************************************************************************/
/**
*  Terminates the BOS Socket Module.
*
*  @return  BOS_STATUS_OK id the termination was successful, or an error
*           code if some type of error occurred.
*/

BOS_STATUS bosSocketTerm( void );

/***************************************************************************/
/**
*  Translates an array of BOS_UINT16's from host order to network order.
*
*  @param   dst   (out) Place to store output (can be the same as the input).
*  @param   src   (in)  Place to get input from.
*  @param   n     (in)  Number of bytes to copy and swap.
*
*  @return  BOS_STATUS_OK id the termination was successful, or an error
*           code if some type of error occurred.
*/

BOS_STATUS bosSocketNtoHU16Array( BOS_UINT16 *dst, BOS_UINT16 *src, size_t n  );

/***************************************************************************/
/**
*  Translates an array of BOS_UINT16's from host order to network order.
*
*  @param   dst   (out) Place to store output (can be the same as the input).
*  @param   src   (in)  Place to get input from.
*  @param   n     (in)  Number of bytes to copy and swap.
*
*  @return  BOS_STATUS_OK id the termination was successful, or an error
*           code if some type of error occurred.
*/

#define  bosSocketHtoNU16Array( dst, src, n )   bosSocketNtoHU16Array( dst, src, n )

/***************************************************************************/
/**
*  Set the IP TOS value for the socket
*
*  @return  BOS_STATUS_OK if the IP TOS value was set successfully, or an
*           error code if the value could not be set.
*/

BOS_STATUS bosSocketSetIpTos
(
   BOS_SOCKET *s,          /**< (mod)  Socket to set the TOS for        */
   BOS_UINT8   tos         /**< (in)   TOS value to set                 */
);

/***************************************************************************/
/**
*  Gets a socket name.
*
*  @return  BOS_STATUS_OK if the socket name was got successfully, or an
*           error code.
*
*  @note    See the documentation for the OS specific setsockopt function
*           for complete documentation on the various options which may
*           be set.
*/
BOS_STATUS bosSocketGetName
(
   BOS_SOCKET *s,
   BOS_SOCKET_ADDR *sockName,
   size_t *nameLen
);

/***************************************************************************/
/**
*  Gets the name of a connected peer.
*
*  @return  BOS_STATUS_OK if the peer name was got successfully, or an
*           error code.
*
*  @note    See the documentation for the OS specific getpeername function
*           for complete documentation on the various options which may
*           be set.
*/
BOS_STATUS bosSocketGetPeerName
(
   BOS_SOCKET *s,
   BOS_SOCKET_ADDR *sockName,
   size_t *nameLen
);

/** @} */
#if defined( __cplusplus )
}
#endif

#endif /* BOSSOCKET_H */

