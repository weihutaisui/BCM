/***********************************************************************
 *
 *  Copyright (c) 2012-2013  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2013:proprietary:standard

 This program is the proprietary software of Broadcom and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").  Except as set forth in
 an Authorized License, Broadcom grants no license (express or implied), right
 to use, or waiver of any kind with respect to the Software, and Broadcom
 expressly reserves all rights in and to the Software and all intellectual
 property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

 Except as expressly set forth in the Authorized License,

 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use
    all reasonable efforts to protect the confidentiality thereof, and to
    use this information only in connection with your use of Broadcom
    integrated circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
    RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
    ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
    FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
    TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
    PERFORMANCE OF THE SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
    ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
    INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
    WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
    IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
    SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
    SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
    LIMITED REMEDY.
:>
 *
 ************************************************************************/

#include <stdlib.h>		/* for exit() */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>		/* memset() */
#include <sys/time.h>		/* select() */
#include <time.h>

#define REMOTE_SERVER_PORT 30006
#define MAX_MSG 100

/* BEGIN jcs 3/30/05 */

#define SOCKET_ERROR -1

struct UdpEchoPlus {
	unsigned  int TestGenSN;
	unsigned  int TestRespSN;
	unsigned  int TestRespRecvTimeStamp;
	unsigned  int TestRespReplyTimeStamp;
	unsigned  int TestRespReplyFailureCount;
};

/* define the global variable for udpecho plus */
struct UdpEchoPlus g_uep;

	int
isReadable (int sd, int *error, int timeOut)
{				// milliseconds
	fd_set socketReadSet;
	FD_ZERO (&socketReadSet);
	FD_SET (sd, &socketReadSet);
	struct timeval tv;
	if (timeOut)
	{
		tv.tv_sec = timeOut / 1000;
		tv.tv_usec = (timeOut % 1000) * 1000;
	}
	else
	{
		tv.tv_sec = 0;
		tv.tv_usec = 0;
	}				// if
	if (select (sd + 1, &socketReadSet, 0, 0, &tv) == SOCKET_ERROR)
	{
		*error = 1;
		return 0;
	}				// if
	*error = 0;
	return FD_ISSET (sd, &socketReadSet) != 0;
}				/* isReadable */

/* END jcs 3/30/05 */

int main (int argc, char *argv[])
{

	int sd, rc, i, n, echoLen, flags, error, timeOut;
	struct sockaddr_in cliAddr, remoteServAddr, echoServAddr;
	struct hostent *h;
	char msg[MAX_MSG];
	struct UdpEchoPlus *pmsg;


	/* check command line args */
	if (argc < 3)
	{
		printf ("usage : %s <server> <data1> ... <dataN> \n", argv[0]);
		exit (1);
	}

	/* get server IP address (no check if input is IP address or DNS name */
	h = gethostbyname (argv[1]);
	if (h == NULL)
	{
		printf ("%s: unknown host '%s' \n", argv[0], argv[1]);
		exit (1);
	}

	printf ("%s: sending data to '%s' (IP : %s) \n", argv[0], h->h_name,
			inet_ntoa (*(struct in_addr *) h->h_addr_list[0]));

	remoteServAddr.sin_family = h->h_addrtype;
	memcpy ((char *) &remoteServAddr.sin_addr.s_addr,
			h->h_addr_list[0], h->h_length);
	remoteServAddr.sin_port = htons (REMOTE_SERVER_PORT);

	memset(&g_uep, 0, sizeof(struct UdpEchoPlus));
	/* socket creation */
	sd = socket (AF_INET, SOCK_DGRAM, 0);
	if (sd < 0)
	{
		printf ("%s: cannot open socket \n", argv[0]);
		exit (1);
	}

	/* bind any port */
	cliAddr.sin_family = AF_INET;
	cliAddr.sin_addr.s_addr = htonl (INADDR_ANY);
	cliAddr.sin_port = htons (0);

	rc = bind (sd, (struct sockaddr *) &cliAddr, sizeof (cliAddr));
	if (rc < 0)
	{
		printf ("%s: cannot bind port\n", argv[0]);
		exit (1);
	}

	/* BEGIN jcs 3/30/05 */

	flags = 0;

	timeOut = 100;		// ms

	/* END jcs 3/30/05 */

	/* send data , try three times*/

#if 0
	for (i = 2; i < argc; i++)
#endif
		for (i = 0; i < 3; i++)
		{
			g_uep.TestGenSN = ntohl(g_uep.TestGenSN) + 1;
			g_uep.TestGenSN = htonl(g_uep.TestGenSN);

#if 0
			rc = sendto (sd, argv[i], strlen (argv[i]) + 1, flags,
					(struct sockaddr *) &remoteServAddr,
					sizeof (remoteServAddr));
#endif
			rc = sendto (sd, &g_uep, sizeof(struct UdpEchoPlus) + 1, flags,
					(struct sockaddr *) &remoteServAddr,
					sizeof (remoteServAddr));
			if (rc < 0)
			{
				printf ("%s: cannot send data %d \n", argv[0], i - 1);
				close (sd);
				exit (1);
			}

			/* BEGIN jcs 3/30/05 */

			/* init buffer */
			memset (msg, 0x0, MAX_MSG);

			while (!isReadable (sd, &error, timeOut))
				printf (".");
			printf ("\n");

			/* receive echoed message */
			echoLen = sizeof (echoServAddr);
			n = recvfrom (sd, msg, MAX_MSG, flags,
					(struct sockaddr *) &echoServAddr, &echoLen);

			if (n < 0)
			{
				printf ("%s: cannot receive data \n", argv[0]);
				continue;
			}

			pmsg = (struct UdpEchoPlus *)msg;
			/* print received message */
#if 0
			printf ("%s: echo from %s:UDP%u : %s \n",
					argv[0], inet_ntoa (echoServAddr.sin_addr),
					ntohs (echoServAddr.sin_port), msg);
#else
			printf("TestGenSN = %d\r\n", ntohl(pmsg->TestGenSN));
			printf("TestRespSN = %d\r\n", ntohl(pmsg->TestRespSN));
			pmsg->TestRespRecvTimeStamp = ntohl(pmsg->TestRespRecvTimeStamp);
			printf("TestRespRecvTimeStamp = %s\r\n", ctime(&pmsg->TestRespRecvTimeStamp));
			pmsg->TestRespReplyTimeStamp = ntohl(pmsg->TestRespReplyTimeStamp);
			printf("TestTestRespReplyTimeStamp = %s\r\n", ctime(&pmsg->TestRespReplyTimeStamp));
#endif

			/* END jcs 3/30/05 */

		}

	return 1;

}
