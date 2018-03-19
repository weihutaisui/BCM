
/*******************************************************************
 * LogSock.h
 *
 *	Description:
 *		Common header for Log client and server
 *
 * $Revision: 1.14 $
 *
 * $Id: DiagSock.h,v 1.14 2004/04/21 03:51:51 ilyas Exp $
 *
 * $Log: DiagSock.h,v $
 * Revision 1.14  2004/04/21 03:51:51  ilyas
 * Merged changes for RTEMS
 *
 * Revision 1.13  2004/04/16 22:57:44  ilyas
 * Made compile with RTEMS
 *
 * Revision 1.12  2004/04/16 22:29:05  ilyas
 * Made proxy service work on board
 *
 * Revision 1.11  2004/04/14 18:37:31  ilyas
 * Makde compile for Linux and VxWorks
 *
 * Revision 1.10  2003/10/07 05:06:34  ilyas
 * Added support for Win32 overlapped IO.
 * Fixed connection resumption on board reset
 *
 * Revision 1.9  2003/08/14 22:04:04  ilyas
 * Added support for proxy over TCP socket
 *
 * Revision 1.8  2003/08/07 23:49:20  ilyas
 * Implemented DslDiags proxy for remote debugging
 *
 * Revision 1.7  2003/05/21 19:26:14  ilyas
 * Removed unused #includes. Check return code from WSAStartup (for WinCE)
 *
 * Revision 1.6  2002/07/03 22:29:43  ilyas
 * Added test commands, parsing of command arguments and gracefull terination
 *
 * Revision 1.5  2002/04/02 22:56:39  ilyas
 * Support DIAG connection at any time; BERT commands
 *
 * Revision 1.4  2002/02/27 21:09:47  ilyas
 * Implemented timeout for ClientInit
 *
 * Revision 1.3  2002/02/08 04:51:00  ilyas
 * Added features to support LOG file recording over sockets
 *
 * Revision 1.2  2002/01/22 00:00:02  ilyas
 * Added support for Eye and log data
 *
 * Revision 1.1  2002/01/18 03:28:14  ilyas
 * Added socket support to get data out from DSL
 *
 ******************************************************************/

#include "DiagDef.h"

/* OS dependent socket definitions */

#ifdef _MSC_VER

#include <winsock2.h>
#include <windows.h>            /* required for all Windows applications */
#include <ws2tcpip.h>
#include <memory.h>
#include <stdlib.h>

#define	LOG_SOCKET_INVALID(sock)	((sock) == INVALID_SOCKET)
#define	LOG_SOCKET_ERROR(res_code)	((res_code) == SOCKET_ERROR)
#define LOG_SOCKET_ERRCODE()		WSAGetLastError()
#define LOG_SOCKET_NOBUFS			WSAENOBUFS
#define LOG_SOCKET_TIMEDOUT			WSAETIMEDOUT
#define LOG_SOCKET_CON_REFUSED		WSAECONNREFUSED

static __inline int LOG_SOCKET_INIT()
{
  WSADATA	WSAData;
  return	WSAStartup(MAKEWORD(1,1), &WSAData);
}

#else

#if !defined(TARG_OS_RTEMS)
#include <sys/socket.h>
#include <sys/types.h>
#else
#undef    fd_set
#define send(s,buf,len,flags) SKsend(s,buf,len,flags)
#include <errortypes.h>
#include <socket.h>
#include <in.h>
#endif

#ifdef VXWORKS
#include <sockLib.h>
#include <netinet/in.h>
#include <ioLib.h>
#endif

#ifdef LINUX
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <netinet/in.h>
#endif

#ifdef ECOS
#include <sys/socket.h>
#include <netdb.h>
#include <sys/time.h>
#endif

#define SOCKET						int
#define closesocket(x)				close(x)

#define	LOG_SOCKET_INVALID(sock)	((sock) < 0)
#define	LOG_SOCKET_ERROR(res_code)	((res_code) < 0)
#if !defined(TARG_OS_RTEMS)
#define LOG_SOCKET_ERRCODE()		errno
#else
#define LOG_SOCKET_ERRCODE()		res
#endif
#define LOG_SOCKET_NOBUFS			ENOBUFS
#define LOG_SOCKET_TIMEDOUT			ETIMEDOUT
#define LOG_SOCKET_CON_REFUSED		ECONNREFUSED

#define LOG_SOCKET_INIT()			0

#endif

/* macros */

#ifdef DBG_PRINTF_ENABLED
#if !defined(TARG_OS_RTEMS)
#define DbgPrintf(x)		printf x
#else
#define DbgPrintf(x)		\
	do						\
	{						\
		DiagPrintf x ;		\
		DiagPrintf("\r");	\
	}						\
	while(0)

extern void DiagPrintf( char *, ... );
#endif
#else
#define DbgPrintf(x)
#endif


SOCKET LogServerInit (unsigned long ipAddr, int port, int portSearch, int sendIntervalMs, int toMs, int diagDataMap, int logTime, int bTcp, int rcvBufSize);
SOCKET LogClientInit(SOCKET *pSock, int port, struct sockaddr *srvIpAddr, int *srvIpAddrLen, int *pDiagData, int toMs, int bTcp);
int LogRecvWithTimeout(SOCKET s, void* bufp, int count, struct sockaddr *pSndAddr, int msTimeout, int bTcp);
void LogServerAbortInit (void);
