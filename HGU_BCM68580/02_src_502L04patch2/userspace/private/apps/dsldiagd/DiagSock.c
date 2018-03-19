/*******************************************************************
 * LogSock.c
 *
 *	Description:
 *		Client/Server common functions for DSL LOG file support
 *
 * $Revision: 1.16 $
 *
 * $Id: DiagSock.c,v 1.16 2004/04/16 22:48:22 ilyas Exp $
 *
 * $Log: DiagSock.c,v $
 * Revision 1.16  2004/04/16 22:48:22  ilyas
 * Made compile in Linux
 *
 * Revision 1.15  2004/04/16 22:29:05  ilyas
 * Made proxy service work on board
 *
 * Revision 1.14  2004/04/14 18:37:31  ilyas
 * Makde compile for Linux and VxWorks
 *
 * Revision 1.13  2003/10/07 05:06:34  ilyas
 * Added support for Win32 overlapped IO.
 * Fixed connection resumption on board reset
 *
 * Revision 1.12  2003/09/03 19:45:11  ilyas
 * To refuse connection with older protocol versions
 *
 * Revision 1.11  2003/09/02 23:37:20  ilyas
 * To make work with modified client version for TMM
 *
 * Revision 1.10  2003/08/14 22:04:04  ilyas
 * Added support for proxy over TCP socket
 *
 * Revision 1.9  2003/08/07 23:49:20  ilyas
 * Implemented DslDiags proxy for remote debugging
 *
 * Revision 1.8  2003/05/21 19:26:14  ilyas
 * Removed unused #includes. Check return code from WSAStartup (for WinCE)
 *
 * Revision 1.7  2002/07/15 22:06:45  ilyas
 * Improved Diag connection establishement by re-sending connect packet if
 * the message of incorrect size was received
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

//#define DBG_PRINTF_ENABLED

#if !defined(TARG_OS_RTEMS)
#include "DiagSock.h"
#include <stdio.h>
#include <unistd.h>
#else
#include <stdio.h>
#include <unistd.h>
#include "DiagSock.h"
#endif

#ifndef FALSE
#define	FALSE	0
#endif

#ifndef TRUE
#define	TRUE	1
#endif

int LogRecvWithTimeout(SOCKET s, void* bufp, int count, struct sockaddr *pSndAddr, int msTimeout, int bTcp)
{
	struct sockaddr		sndAddr;
	int					sndAddrLen, res;
    fd_set				readfds, exceptfds;
    struct timeval		tv_timeout;

    tv_timeout.tv_sec	= msTimeout/1000;
    tv_timeout.tv_usec	= (msTimeout%1000)*1000;

    FD_ZERO(&readfds);
    FD_ZERO(&exceptfds);
    FD_SET(s, &readfds);
    FD_SET(s, &exceptfds);

    res =  select(s+1, &readfds, NULL, &exceptfds, &tv_timeout);
	if (LOG_SOCKET_ERROR(res)) {
		DbgPrintf (("select failed. Error = %d\n", LOG_SOCKET_ERRCODE() ));
		return -1;
	}
	if (res == 0)
		return 0;	/* timeout */

	if (NULL == pSndAddr)
		pSndAddr = &sndAddr;
	sndAddrLen = sizeof(sndAddr);
	res = recvfrom(s, bufp, count, 0, pSndAddr, (socklen_t *)&sndAddrLen);
	if (LOG_SOCKET_ERROR(res)) {
		res = LOG_SOCKET_ERRCODE();
#ifdef _MSC_VER
		if (WSAEMSGSIZE == res)
			return 0;
		if (WSAECONNRESET == res) {
			Sleep(msTimeout);
			return 0;
		}

		DbgPrintf (("Can't read from socket. Error = %d\n", res));
#endif
		return -1;
	}

	return res;
}

int LogListen(SOCKET s, struct sockaddr *pSndAddr, int msTimeout)
{
	struct sockaddr		sndAddr;
	int					sndAddrLen, res;
    fd_set				readfds, exceptfds;
    struct timeval		tv_timeout;

	do {
		res = listen(s, 0);
		if (LOG_SOCKET_ERROR(res)) {
			res = LOG_SOCKET_ERRCODE();
#ifdef _MSC_VER
			if (WSAEMSGSIZE == res)
				return 0;
			if (WSAECONNRESET == res)
				continue;

			DbgPrintf (("Can't listen on socket. Error = %d\n", res));
#endif
			return -1;
		}
		tv_timeout.tv_sec	= msTimeout/1000;
		tv_timeout.tv_usec	= (msTimeout%1000)*1000;

		FD_ZERO(&readfds);
		FD_ZERO(&exceptfds);
		FD_SET(s, &readfds);
		FD_SET(s, &exceptfds);

		res =  select(s+1, &readfds, NULL, &exceptfds, &tv_timeout);
		if (LOG_SOCKET_ERROR(res)) {
			DbgPrintf (("select failed. Error = %d\n", LOG_SOCKET_ERRCODE() ));
			return -1;
		}
		if (res == 0)
			return 0;	/* timeout */

		if (NULL == pSndAddr)
			pSndAddr = &sndAddr;
		sndAddrLen = sizeof(sndAddr);
		res = accept(s, pSndAddr, (socklen_t *)&sndAddrLen);
		if (LOG_SOCKET_ERROR(res)) {
			DbgPrintf (("accept failed. Error = %d\n", LOG_SOCKET_ERRCODE() ));
			return -1;
		}
		closesocket (s);
	} while (0);
	return res;
}

int LogConnect(SOCKET s, struct sockaddr *remAddr, int *pAbortFlag)
{
	int		addrLen, res;

	addrLen = sizeof(*remAddr);
	do {
		res = connect(s, remAddr, addrLen);
		if (LOG_SOCKET_ERROR(res)) {
			res = LOG_SOCKET_ERRCODE();
			if ((LOG_SOCKET_TIMEDOUT != res) && (LOG_SOCKET_CON_REFUSED != res)) {
				DbgPrintf (("Can't connect to socket. Error = %d\n", res ));
				return -1;
			}
			if (*pAbortFlag)
				return -1;
		}
	} while (0 != res);

	return s;
}

SOCKET LogClientInit(
	SOCKET			*pSock, 
	int				port, 
	struct sockaddr *srvIpAddr, 
	int				*srvIpAddrLen, 
	int				*pDiagData, 
	int				toMs, 
	int				bTcp)
{
	struct sockaddr_in	saLocal;
	LogProtoHeader		logHeader;
	char				gdbBuff[1400];
	int					res, bufSize, resLen;

	if (-1 == *pSock) {
		if (LOG_SOCKET_INIT() != 0) {
			DbgPrintf (("Can't initialize socket library\n"));
			return -1;
		}

		*pSock = socket(AF_INET, bTcp ? SOCK_STREAM : SOCK_DGRAM, 0);
		if (LOG_SOCKET_INVALID(*pSock)) {
			res = LOG_SOCKET_ERRCODE();
			DbgPrintf (("Can't open socket. Error = %d\n", res));
			return -1;
		}

		saLocal.sin_family = AF_INET;
		saLocal.sin_port   = htons(port);
		saLocal.sin_addr.s_addr = 0;

		res = bind (*pSock, (struct sockaddr *) &saLocal, sizeof(saLocal));
		if (LOG_SOCKET_ERROR(res)) {
			res = LOG_SOCKET_ERRCODE();
			DbgPrintf (("Can't bind to socket. Error = %d\n", res));
			closesocket (*pSock);
			*pSock = -1;
			return -1;
		}

		bufSize = LOG_MAX_BUF_SIZE;
		res = setsockopt(*pSock, SOL_SOCKET, SO_SNDBUF, (char *)&bufSize, sizeof(bufSize));
		if (LOG_SOCKET_ERROR(res)) {
			DbgPrintf (("Can't set buffer size\n"));
			closesocket (*pSock);
			*pSock = -1;
			return -1;
		}

#if 1
		/* some info */
		bufSize = 0;
		resLen  = sizeof(bufSize);
		getsockopt(*pSock, SOL_SOCKET, SO_SNDBUF, (char *)&bufSize, (socklen_t *)&resLen);
		DbgPrintf (("Socket xmt buffer size = %d\n", bufSize));
		bufSize = 0;
		resLen  = sizeof(bufSize);
		getsockopt(*pSock, SOL_SOCKET, SO_RCVBUF, (char *)&bufSize, (socklen_t *)&resLen);
		DbgPrintf (("Socket rcv buffer size = %d\n", bufSize));
#endif

		if (bTcp) {
			res = LogListen(*pSock, srvIpAddr, toMs);
			if (LOG_SOCKET_ERROR(res)) {
				DbgPrintf (("listen failed. Error = %d\n", LOG_SOCKET_ERRCODE() ));
				closesocket (*pSock);
				*pSock = -1;
				return -1;
			}
			if (res == 0) {		
				DbgPrintf (("timeout\n"));
#ifdef _MSC_VER
				closesocket (*pSock);
				*pSock = -1;
#endif
				return -1;
			}
			*pSock = res;
		}
		else {
#if 1 && defined(IPPROTO_UDP) && defined(UDP_NOCHECKSUM)
			bufSize = 0;
			setsockopt(*pSock, IPPROTO_UDP, UDP_NOCHECKSUM, (char *)&bufSize, sizeof(bufSize));
#endif
		}
	}

	/* get packet from LOG server */

	*srvIpAddrLen = sizeof(*srvIpAddr);
	if (port == GDB_PORT)
	  res = LogRecvWithTimeout(*pSock, (char *) &gdbBuff, sizeof(gdbBuff), srvIpAddr, toMs, bTcp);
	else
	  res = LogRecvWithTimeout(*pSock, (char *) &logHeader, sizeof(logHeader), srvIpAddr, toMs, bTcp);

	if (LOG_SOCKET_ERROR(res)) {
	  DbgPrintf (("Can't read from socket. Error = %d\n", LOG_SOCKET_ERRCODE() ));
	  closesocket (*pSock);
	  *pSock = -1;
	  return -1;
	}

	/* leave the socket open (if timeout or invalid message) */

	if (res == 0) {		
	  DbgPrintf (("timeout\n"));
#ifdef _MSC_VER
	  closesocket (*pSock);
	  *pSock = -1;
#endif
	  return -1;
	}
	if (port != GDB_PORT) {
	  if ((res < sizeof(logHeader)) || ((logHeader.logPartyId & DIAG_PARTY_ID_MASK_EX) != LOG_PARTY_SERVER_EX) ||
	     (*(short *)logHeader.logProtoId != *(short *)LOG_PROTO_ID) || (logHeader.logCommmand != LOG_CMD_CONNECT)) {
	    DbgPrintf (("Invalid message received\n"));
	    closesocket(*pSock);
	    *pSock = -1;
	    return -1;
	  }
	  *pDiagData = logHeader.logPartyId & (DIAG_DATA_MASK | DIAG_LOCK_SESSION);
	  if (*pDiagData & DIAG_DATA_LOG_TIME)
	    *pDiagData |= (*(short *)logHeader.logProtoId) << 16;

	  DbgPrintf (("Connected to LOG server\n"));
	  logHeader.logPartyId = 0x40 | LOG_PARTY_CLIENT;
	  if (bTcp)
	    res = send(*pSock, (char *)&logHeader, sizeof(logHeader), 0);
	  else
	    res = sendto(*pSock, (char *)&logHeader, sizeof(logHeader), 0, srvIpAddr, *srvIpAddrLen);
	  if (LOG_SOCKET_ERROR(res)) {
	    DbgPrintf (("Can't write to socket. Error = %d\n", LOG_SOCKET_ERRCODE() ));
	    closesocket (*pSock);
	    *pSock = -1;
	    return -1;
	  }
	  if (res < sizeof(logHeader)) {
	    DbgPrintf (("Too few bytes (%d) written\n", res));
	    closesocket (*pSock);
	    *pSock = -1;
	    return -1;
	  }
	}
	else {
	  res = sendto(*pSock, "-", 1, 0, srvIpAddr, *srvIpAddrLen);
	}
	
	return *pSock;
}


static int diagSrvAbortInit = FALSE;

SOCKET LogServerInit (
	unsigned long	ipAddr, 
	int				port, 
	int				portSearch,
	int				sendIntervalMs,
	int				toMs, 
	int				diagDataMap, 
	int				logTime,
	int				bTcp,
	int				rcvBufSize)
{
	SOCKET				fd;
	struct sockaddr_in	saLocal;
	struct sockaddr		clIpAddr;
	int					clIpAddrLen;
	LogProtoHeader		logHeaderRcv, logHeaderSnd;
	int					res, bufSize, resLen;
#ifdef _MSC_VER
	int					rcvPort = port;
#endif
   int dummy;

	diagSrvAbortInit = FALSE;
//   LOG_SOCKET_INIT();
	dummy = LOG_SOCKET_INIT(); /* to remove compiler warning */

#ifdef _MSC_VER
	fd = WSASocket(AF_INET, bTcp ? SOCK_STREAM : SOCK_DGRAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
#else
	fd = socket(AF_INET, bTcp ? SOCK_STREAM : SOCK_DGRAM, 0);
#endif
	if (LOG_SOCKET_INVALID(fd)) {
		res = LOG_SOCKET_ERRCODE();
		DbgPrintf (("Can't open socket. Error = %d\n", res));
		return -1;
	}

	saLocal.sin_family = AF_INET;
	saLocal.sin_port   = htons(port);
	saLocal.sin_addr.s_addr = 0;

	do {
		res = bind (fd, (struct sockaddr *) &saLocal, sizeof(saLocal));
		if (LOG_SOCKET_ERROR(res)) {
			res = LOG_SOCKET_ERRCODE();

#ifdef _MSC_VER
			if ((WSAEADDRINUSE == res) && (portSearch != 0)) {
				rcvPort = portSearch++;
				saLocal.sin_port = htons(rcvPort);
				continue;
			}
#endif

			DbgPrintf (("Can't bind to socket. Error = %d\n", res));
			closesocket(fd);
			return -1;
		}

		bufSize = LOG_MAX_BUF_SIZE;
		res = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char *)&bufSize, sizeof(bufSize));
		if (LOG_SOCKET_ERROR(res)) {
			DbgPrintf (("Can't set buffer size\n"));
			closesocket(fd);
			return -1;
		}
		break;
	} while (1);

#if 1
	/* some info */
	bufSize = 0;
	resLen  = sizeof(bufSize);
	getsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char *)&bufSize, (socklen_t *)&resLen);
	DbgPrintf (("Socket xmt buffer size = %d\n", bufSize));
	bufSize = 0;
	resLen  = sizeof(bufSize);
	getsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char *)&bufSize, (socklen_t *)&resLen);
	DbgPrintf (("Socket rcv buffer size = %d\n", bufSize));
	if (bufSize < rcvBufSize) {
		bufSize = rcvBufSize + 0x1000;
		do {
			bufSize -= 0x1000;
			setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char *)&bufSize, sizeof(bufSize));
			res = 0;
			getsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char *)&res, (socklen_t *)&resLen);
		} while (res != bufSize);
	}
#endif

	clIpAddrLen = sizeof(clIpAddr);
	clIpAddr.sa_family = AF_INET;
	((struct sockaddr_in *) &clIpAddr)->sin_addr.s_addr = ipAddr;
	((struct sockaddr_in *) &clIpAddr)->sin_port = htons(port);

	if (bTcp) {
		res = LogConnect(fd, &clIpAddr, &diagSrvAbortInit);
		if (LOG_SOCKET_ERROR(res)) {
			res = LOG_SOCKET_ERRCODE();
			DbgPrintf (("Can't connect to socket. Error = %d\n", res ));
			closesocket(fd);
			return -1;
		}
		fd = res;
	}
	else {
#if 1 && defined(IPPROTO_UDP) && defined(UDP_NOCHECKSUM)
		bufSize = 0;
		setsockopt(fd, IPPROTO_UDP, UDP_NOCHECKSUM, (char *)&bufSize, sizeof(bufSize));
#endif
	}

	/* send packets until we get response from LOG client */

	if ((logTime != 0) && (diagDataMap & DIAG_DATA_LOG)) {
		*(short *)logHeaderSnd.logProtoId = logTime & 0xFFFF;
		diagDataMap |= DIAG_DATA_LOG_TIME;
	}
	else
		*(short *)logHeaderSnd.logProtoId = *(short *) LOG_PROTO_ID;
	logHeaderSnd.logPartyId = DIAG_DATA_EX | LOG_PARTY_SERVER | diagDataMap;
	logHeaderSnd.logCommmand = (char)LOG_CMD_CONNECT;


	do {
		if (diagSrvAbortInit) {
			closesocket(fd);
			return -1;
		}

		if (bTcp)
			res = send(fd, (char *)&logHeaderSnd, sizeof(logHeaderSnd), 0);
		else
			res = sendto(fd, (char *)&logHeaderSnd, sizeof(logHeaderSnd), 0, &clIpAddr, clIpAddrLen);
		if (LOG_SOCKET_ERROR(res)) {
			DbgPrintf (("Can't write to socket. Error = %d\n", LOG_SOCKET_ERRCODE() ));
			closesocket(fd);
			return -1;
		}
		if (res < sizeof(logHeaderSnd)) {
			DbgPrintf (("Too few bytes (%d) written\n", res));
			closesocket(fd);
			return -1;
		}

		res = LogRecvWithTimeout(fd, &logHeaderRcv, sizeof(logHeaderRcv), NULL, sendIntervalMs, bTcp);
		if (res < 0) {
			DbgPrintf (("LogRecvWithTimeout failed\n"));
			closesocket(fd);
			return -1;
		}

		if (toMs >= 0) {
			toMs -= sendIntervalMs;
			if (toMs <= 0) {
				DbgPrintf (("Timeout: toMs = %d\n", toMs));
				closesocket(fd);
				return -1;
			}
		}

		if (res == 0) 
			continue;

		if ((res < sizeof(logHeaderRcv)) || ((logHeaderRcv.logPartyId & ~0x40) != LOG_PARTY_CLIENT) ||
			(*(short *)logHeaderRcv.logProtoId != *(short *) LOG_PROTO_ID) || (logHeaderRcv.logCommmand != LOG_CMD_CONNECT)) {
			DbgPrintf (("Invalid message received\n"));
			closesocket(fd);
			return -1;
		}
	} while (0 == res);

	DbgPrintf (("Connected to LOG client\n"));
	return fd;
}

void LogServerAbortInit (void)
{
	diagSrvAbortInit = TRUE;
}
