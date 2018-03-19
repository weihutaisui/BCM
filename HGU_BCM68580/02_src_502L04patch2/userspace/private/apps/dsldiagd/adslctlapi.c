/*****************************************************************************
 *
 *  Copyright (c) 2000-2001  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2012:proprietary:standard

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

#ifdef SUPPORT_DSL

/*
 * This whole file should get moved to a separate app called dsldiagd.
 * For now, only compile it for DSL systems. 
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <net/if_arp.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "adslctlapi.h"
#ifndef __ECOS
#include "syscall.h"
#endif
#include "bcmadsl.h"
#include "adsldrv.h"


#include "devctl_adsl.h"

//**************************************************************************
//
//		DslDiag Socket support
//
//**************************************************************************

#include "DiagSock.h"
#define	MAX_LOG_TIME	512

SOCKET			diagLibSock = -1;
SOCKET			diagSock = -1;
struct sockaddr	diagSrcAddr;
int				diagSrcAddrLen;
int				diagDataMap;
int				diagLogTime = MAX_LOG_TIME;

SOCKET			guiLibSock = -1;
SOCKET			guiSock = -1;
struct sockaddr	guiSrcAddr;
int				guiSrcAddrLen;
int				guiDataMap;
int				guiLogTime = MAX_LOG_TIME;

SOCKET			gdbLibSock = -1;
SOCKET			gdbSock = -1;
struct sockaddr	gdbSrcAddr;
int				gdbSrcAddrLen;
int				gdbDataMap;
int				gdbLogTime = MAX_LOG_TIME;

#define	SYS_CMD_LEN	256
	
void XdslConnectionCheck(void);
int BcmAdslCtl_GetDiagsSocket(void);
void BcmAdslCtl_ProcessDiagsFrames(void);
int BcmAdslCtl_GetGuiSocket(void);
void BcmAdslCtl_ProcessGuiFrames(void);
int BcmAdslCtl_GetGdbSocket(void);
void BcmAdslCtl_ProcessGdbFrames(void);
static int GuiConnect(DiagProtoFrame *diagCmd);
static int DiagConnect(DiagProtoFrame *diagCmd);
static int GdbConnect(DiagProtoFrame *diagCmd);
static void GuiProcessCommands(void);
static void DiagProcessCommands(void);
static void GdbProcessCommands(void);

void XdslConnectionCheck(void)
{
	DiagProcessCommands();
	GuiProcessCommands();
	GdbProcessCommands();
}

static int DiagGetConnectInfo(int ipAddr, DiagConnectInfo *pDiagConnectInfo)
{
	int	i, iMac[6], res = 0;
	char	*pToken, *pLast, line[SYS_CMD_LEN], tokenSeperator[] = " \n";
	FILE	*fs = NULL;
	struct in_addr	in;
	char	*pIpAddrStr, buf[16];
	
	in.s_addr = ipAddr;
	pIpAddrStr = inet_ntop(AF_INET, &in, buf, sizeof(buf));
	if(NULL == pIpAddrStr)
		return res;
	
	system("cat /proc/net/arp > /var/arp_table");
	fs = fopen("/var/arp_table", "r");
	if (NULL == fs)
		return res;
	
	/* IP address     HW type     Flags     HW address     Mask     Device */
	while ( fgets(line, SYS_CMD_LEN, fs) != NULL ) {
		pToken = strtok_r(line, tokenSeperator, &pLast);
		if(NULL == pToken)
			goto getout;
		
		if(0 != strcmp(pToken, pIpAddrStr))
			continue;
		
		/* HW address */
		for(i=0; i < 3; i++) {
			pToken = strtok_r(NULL, tokenSeperator, &pLast);
			if(NULL == pToken)
				goto getout;
		}
		sscanf(pToken, "%x:%x:%x:%x:%x:%x", &iMac[0], &iMac[1], &iMac[2], &iMac[3], &iMac[4], &iMac[5]);
		for(i=0;i<6;i++)
			pDiagConnectInfo->macAddr[i] = (unsigned char)iMac[i];
		
		/* Device */
		for(i=0; i < 2; i++) {
			pToken = strtok_r(NULL, tokenSeperator, &pLast);
			if(NULL == pToken)
				goto getout;
		}
		strncpy(pDiagConnectInfo->devName, pToken, sizeof(pDiagConnectInfo->devName)-1);
#if 0
		printf("%s: ipAddr = %s, macAddr = %02x:%02x:%02x:%02x:%02x:%02x, devName = %s\n", __FUNCTION__,
			pIpAddrStr, pDiagConnectInfo->macAddr[0], pDiagConnectInfo->macAddr[1], pDiagConnectInfo->macAddr[2],
			pDiagConnectInfo->macAddr[3], pDiagConnectInfo->macAddr[4], pDiagConnectInfo->macAddr[5], pDiagConnectInfo->devName);
#endif
		res =1;
		break;
	}

getout:
	fclose(fs);
	system("rm /var/arp_table");
	
	return res;
}


static int DiagGetDefaultGateway(int srvIpAddr) 
{
	int	gwIpAddr = 0;
	char	line[SYS_CMD_LEN];
	char	gwIpStr[32];
	FILE	*fs = NULL;
	
	system("route -n > /var/adslgw");
	fs = fopen("/var/adslgw", "r");
	if (fs == NULL)
		return 0;

	gwIpStr[0] = 0;
	while ( fgets(line, SYS_CMD_LEN, fs) != NULL ) {
		parseAdslInfo(line, "default", gwIpStr, sizeof(gwIpStr)-1);
		gwIpAddr = inet_addr(gwIpStr);
		if (gwIpAddr == -1)
			gwIpAddr = 0;
		if (gwIpAddr != 0) {
			printf ("GW = %s, len = %d, gwIpAddr = 0x%X\n", gwIpStr, strlen(gwIpStr), gwIpAddr);
			break;
		}
		gwIpStr[0] = 0;
	}

	if( 0 == gwIpAddr ) {
		char	dstIpStr[32], maskStr[32], *pChar;
		int	dstIpAddr, maskVal, i;

		rewind(fs);
		while ( fgets(line, SYS_CMD_LEN, fs) != NULL ) {
			pChar = &line[0];

			// Remove spaces from the beginning of line
			while ( *pChar != '\0' && isspace((int)*pChar) != 0 )
				pChar++;
			// get dstIpAddr until end of line, or comma, or space char
			for ( i = 0; i < sizeof(dstIpStr) && *pChar != '\0' && *pChar != ',' && isspace((int)*pChar) == 0; i++, pChar++ )
				dstIpStr[i] = *pChar;
			dstIpStr[i] = '\0';
			dstIpAddr = inet_addr(dstIpStr);
			if ( -1 == dstIpAddr ) {
				gwIpAddr = 0;
				continue;
			}
			// Remove spaces from the beginning of line
			while ( *pChar != '\0' && isspace((int)*pChar) != 0 )
				pChar++;
			// get gwIpAddr until a comma, or space char
			for ( i = 0; i < sizeof(gwIpStr) && *pChar != '\0' && *pChar != ',' && isspace((int)*pChar) == 0; i++, pChar++ )
				gwIpStr[i] = *pChar;
			gwIpStr[i] = '\0';
			gwIpAddr = inet_addr(gwIpStr);
			
			if ( -1 == gwIpAddr ) {
				gwIpAddr = 0;
				continue;
			}

			// Remove spaces from the beginning of line
			while ( *pChar != '\0' && isspace((int)*pChar) != 0 )
				pChar++;
			// get maskVal until a comma, or space char
			for ( i = 0; i < sizeof(maskStr) && *pChar != '\0' && *pChar != ',' && isspace((int)*pChar) == 0; i++, pChar++ )
				maskStr[i] = *pChar;
			maskStr[i] = '\0';
			maskVal = inet_addr(maskStr);
			
			if( (-1 == maskVal) || ((dstIpAddr & maskVal) != (srvIpAddr & maskVal)) ) {
				gwIpAddr = 0;
				continue;
			}
			else	 {
				printf ("GW = %s, len = %d, gwIpAddr = 0x%X\n", gwIpStr, strlen(gwIpStr), gwIpAddr);
				break;
			}
		}
	}

	fclose(fs);
	system("rm /var/adslgw");
	
	return gwIpAddr;
}

/***************************************************************************
 * Function Name: GuiConnect
 * Description  : Tries to connect to Gui server via sockets
 * Returns      : socket handle if successsful or -1 if error
 ***************************************************************************/
static int GuiConnect(DiagProtoFrame *diagCmd)
{
	int	port;
	int	*pData;

	guiSock = LogClientInit(&guiLibSock, LOG_FILE_PORT2, &guiSrcAddr, &guiSrcAddrLen, &guiDataMap, 0, 0);
	if (guiSock != -1) {
		if (guiDataMap & DIAG_DATA_LOG_TIME) {
			guiLogTime = ((guiDataMap >> 8) & 0xFF00) | (guiDataMap >> 24);
			guiDataMap &= (~DIAG_DATA_LOG_TIME & 0xFF);
		}
		else
			guiLogTime = MAX_LOG_TIME;
		diagCmd->diagHdr.logCommmand = LOG_CMD_CONNECT;
		pData = (int *)diagCmd->diagData;
		pData[0] = guiDataMap;
		port = ntohs(((struct sockaddr_in *) &guiSrcAddr)->sin_port);
		pData[0] |= (port << 16);
		pData[0] |= DIAG_DATA_GUI_ID;
		pData[1] = guiLogTime;
		pData[2] = *((int *)&guiSrcAddr.sa_data[2]);
		pData[3] = DiagGetDefaultGateway(*((int *)&guiSrcAddr.sa_data[2]));
		xdslCtl_DiagProcessCommandFrame(0, (void *)diagCmd, 20);
	}
	return guiSock;
}

static void GuiProcessCommands(void)
{
	struct sockaddr		srvIpAddr;
	int					res;
	DiagProtoFrame		guiCmd;
	int					bJustConnected = 0;
	unsigned char			lineId;
	
	while (1) {
		if (guiSock == -1) {
			guiSock = GuiConnect(&guiCmd);
			if (guiSock == -1)
				break;
			else
				bJustConnected = 1;
		}

		res = LogRecvWithTimeout(guiSock, (char *)&guiCmd, sizeof(guiCmd), &srvIpAddr, 0, 0);
		if (LOG_SOCKET_ERROR(res)) {
			res = LOG_SOCKET_ERRCODE();
			res =guiSock;
			guiSock = -1;
			guiLibSock = -1;
			closesocket(res);
			continue;
		}

		if (res == 0)	/* timeout */
			break;

#if 0
		printf ("GuiCmd received: len=%d, protoid=%c%c, cmd=%d\n", res,
			guiCmd.diagHdr.logProtoId[0], guiCmd.diagHdr.logProtoId[1], guiCmd.diagHdr.logCommmand);
#endif
		if ((res < sizeof(LogProtoHeader)) || ((guiCmd.diagHdr.logPartyId & DIAG_PARTY_ID_MASK) != LOG_PARTY_SERVER))
			continue;
		lineId = (guiCmd.diagHdr.logPartyId & DIAG_PARTY_LINEID_MASK) >> DIAG_PARTY_LINEID_SHIFT;

		if (LOG_CMD_CONNECT != guiCmd.diagHdr.logCommmand) {
			*(short *)guiCmd.diagHdr.logProtoId = 0;
			xdslCtl_DiagProcessCommandFrame(lineId, (void *)&guiCmd, res);
		}
		else {
			if (!bJustConnected)
				guiSock = -1;
		}
	}
}

//***************************************************************************
// Function Name: BcmAdslCtl_GetGuiSocket
// Description  : Get Gui connection socket
// Returns      : none.
//***************************************************************************/
int  BcmAdslCtl_GetGuiSocket(void)
{
	return guiLibSock;
}

/***************************************************************************
 * Function Name: DiagConnect
 * Description  : Tries to connect to Diag server via sockets
 * Returns      : socket handle if successsful or -1 if error
 ***************************************************************************/
static int DiagConnect(DiagProtoFrame	*diagCmd)
{
	int	port;
	int	*pData;
	DiagProtoFrame	*pDiagCmd;
	
	diagSock = LogClientInit(&diagLibSock, LOG_FILE_PORT, &diagSrcAddr, &diagSrcAddrLen, &diagDataMap, 0, 0);
	if (diagSock != -1) {
		int	cmdFrameLen;
		
		if (diagDataMap & DIAG_DATA_LOG_TIME) {
			diagLogTime = ((diagDataMap >> 8) & 0xFF00) | (diagDataMap >> 24);
			diagDataMap &= (~DIAG_DATA_LOG_TIME & 0xFF);
		}
		else
			diagLogTime = MAX_LOG_TIME;

		diagCmd->diagHdr.logCommmand = LOG_CMD_CONNECT;
		pData = (int *)diagCmd->diagData;
		pData[0] = diagDataMap;
		port = ntohs(((struct sockaddr_in *) &diagSrcAddr)->sin_port);
		if (port != LOG_FILE_PORT)
			pData[0] |= (port << 16);
		pData[1] = diagLogTime;
		pData[2] = *((int *)&diagSrcAddr.sa_data[2]);
#ifdef __ECOS
		pData[3] = (int) diagSock;
#else
		pData[3] = DiagGetDefaultGateway(*((int *)&diagSrcAddr.sa_data[2]));
#endif
#ifdef LOG_CMD_DIAG_CONNECT_INFO
		pDiagCmd = (DiagProtoFrame *)&pData[4];
		memset((void *)pDiagCmd->diagData, 0, sizeof(DiagConnectInfo));
		if(DiagGetConnectInfo(pData[2], (DiagConnectInfo *)pDiagCmd->diagData) ||
			((0 != pData[3]) && DiagGetConnectInfo(pData[3], (DiagConnectInfo *)pDiagCmd->diagData))) {
			pDiagCmd->diagHdr.logCommmand = LOG_CMD_DIAG_CONNECT_INFO;
			pDiagCmd->diagHdr.logPartyId = 0;
			*(short *)pDiagCmd->diagHdr.logProtoId = 0;
			cmdFrameLen = sizeof(LogProtoHeader) + sizeof(DiagConnectInfo);
			xdslCtl_DiagProcessCommandFrame(0, (void *)pDiagCmd, cmdFrameLen);
		}
#endif
		cmdFrameLen = sizeof(LogProtoHeader) + (sizeof(int) * 4);
		xdslCtl_DiagProcessCommandFrame(0, (void *)diagCmd, cmdFrameLen);
	}

	return diagSock;
}

static void DiagProcessCommands(void)
{
	struct sockaddr		srvIpAddr;
	int					res;
	DiagProtoFrame		diagCmd;
	int					bJustConnected = 0;
	unsigned char			lineId;
	
	while (1) {
		if (diagSock == -1) {
			diagSock = DiagConnect(&diagCmd);
			if (diagSock == -1)
				break;
			else
				bJustConnected = 1;
		}

		res = LogRecvWithTimeout(diagSock, (char *)&diagCmd, sizeof(diagCmd), &srvIpAddr, 0, 0);
		if (LOG_SOCKET_ERROR(res)) {
			res = LOG_SOCKET_ERRCODE();
			res = diagSock;
			diagSock = -1;
			diagLibSock = -1;
			closesocket(res);
			continue;
		}

		if (res == 0)	/* timeout */
			break;

#if 0
		printf ("DiagCmd received: len=%d, protoid=%c%c, cmd=%d\n", res,
			diagCmd.diagHdr.logProtoId[0], diagCmd.diagHdr.logProtoId[1], diagCmd.diagHdr.logCommmand);
#endif
		if ((res < sizeof(LogProtoHeader)) || ((diagCmd.diagHdr.logPartyId & DIAG_PARTY_ID_MASK) != LOG_PARTY_SERVER) ||
			(*(short *)diagCmd.diagHdr.logProtoId != *(short *)LOG_PROTO_ID))
			continue;
		lineId = (diagCmd.diagHdr.logPartyId & DIAG_PARTY_LINEID_MASK) >> DIAG_PARTY_LINEID_SHIFT;

		if (LOG_CMD_CONNECT != diagCmd.diagHdr.logCommmand) {
			if(LOG_CMD_DISCONNECT == diagCmd.diagHdr.logCommmand)
				*((int *)diagCmd.diagData) = *((int *)&srvIpAddr.sa_data[2]);
			else
				*(short *)diagCmd.diagHdr.logProtoId = (short)htonl(*(int *)&srvIpAddr.sa_data[2]);
			xdslCtl_DiagProcessCommandFrame(lineId, (void *)&diagCmd, res);
		}
		else {
			if (!bJustConnected)
				diagSock = -1;
		}
	}
}

/***************************************************************************
 * Function Name: GdbConnect
 * Description  : Tries to connect to GDB via sockets
 * Returns      : socket handle if successsful or -1 if error
 ***************************************************************************/
static int GdbConnect(DiagProtoFrame *diagCmd)
{
	int	port;
	int	*pData;

	gdbSock = LogClientInit(&gdbLibSock, GDB_PORT, &gdbSrcAddr, &gdbSrcAddrLen, &gdbDataMap, 0, 0);
	if (gdbSock != -1) {
		if (gdbDataMap & DIAG_DATA_LOG_TIME) {
			gdbLogTime = ((gdbDataMap >> 8) & 0xFF00) | (gdbDataMap >> 24);
			gdbDataMap &= (~DIAG_DATA_LOG_TIME & 0xFF);
		}
		else
			gdbLogTime = MAX_LOG_TIME;
		
		diagCmd->diagHdr.logCommmand = LOG_CMD_CONNECT;
		pData = (int *)diagCmd->diagData;
		pData[0] = gdbDataMap;
		port = ntohs(((struct sockaddr_in *) &gdbSrcAddr)->sin_port);
		pData[0] |= (port << 16);
		pData[0] |= DIAG_DATA_GDB_ID;
		pData[1] = gdbLogTime;
		pData[2] = *((int *)&gdbSrcAddr.sa_data[2]);
		pData[3] = DiagGetDefaultGateway(*((int *)&gdbSrcAddr.sa_data[2]));
		xdslCtl_DiagProcessCommandFrame(0, (void *)diagCmd, 20);
	}
	return gdbSock;
}

static void GdbProcessCommands(void)
{
	struct sockaddr		srvIpAddr;
	int					res;
	DiagProtoFrame		gdbCmd;
	int			bJustConnected = 0;
	int                     justReconnected = 0;
	char                    gdbConnectPack[] = "$qSupported#37";
	char                    gdbKillPack[] = "$k#6b";

	while (1) {
		if (gdbSock == -1) {
			gdbSock = GdbConnect(&gdbCmd);
			if (gdbSock == -1)
				break;
			else
				bJustConnected = 1;
		}

		res = LogRecvWithTimeout(gdbSock, (void *)&gdbCmd.diagData[0], sizeof(gdbCmd.diagData), &srvIpAddr, 0, 0);
		if (LOG_SOCKET_ERROR(res)) {
			res = LOG_SOCKET_ERRCODE();
			res = gdbSock;
			gdbSock = -1;
			gdbLibSock = -1;
			closesocket(res);
			continue;
		}

		if (res == 0)	/* timeout */
			break;

		//printf("***GdbProcessCommands::GdbCmd=%s GdbSock=%d bJustConnected=%d\n", (char *)&gdbCmd.diagData, gdbSock, bJustConnected);
		if (!strncmp((char *)&gdbCmd.diagData, gdbConnectPack, sizeof(gdbConnectPack)-1) && bJustConnected == 0 && justReconnected == 0) {
			printf("*** GDB socket reconnected\n");
			justReconnected = 1;
			gdbSock = -1;
		}
		else /*if (bJustConnected == 0)*/ {
			gdbCmd.diagHdr.logCommmand = LOG_CMD_GDB;
			gdbCmd.diagHdr.logProtoId[0]= 0;
			gdbCmd.diagHdr.logProtoId[1]= 0;
			gdbCmd.diagHdr.logPartyId = 0;
			res += sizeof(LogProtoHeader);
			xdslCtl_DiagProcessCommandFrame(0, (void *)&gdbCmd, res);
#if 1
			if (!strncmp((char *)&gdbCmd.diagData, gdbKillPack, sizeof(gdbKillPack)-1)) {
			  printf("*** GDB socket disconnected\n");
			  gdbSock = -1;
			  break;
			}
#endif
		}
	}
}

//***************************************************************************
// Function Name: BcmAdslCtl_GetGdbSocket
// Description  : Get GDB connection socket
// Returns      : none.
//***************************************************************************/
int  BcmAdslCtl_GetGdbSocket(void)
{
	return gdbLibSock;
}

//***************************************************************************
// Function Name: BcmAdslCtl_ProcessGdbFrames
// Description  : Processes received frames from GDB
// Returns      : none.
//***************************************************************************/
void BcmAdslCtl_ProcessGdbFrames(void)
{
	GdbProcessCommands();
}

//***************************************************************************
// Function Name: BcmAdslCtl_GetDiagsSocket
// Description  : Get DslDiags connection socket
// Returns      : none.
//***************************************************************************/
int  BcmAdslCtl_GetDiagsSocket(void)
{
	return diagLibSock;
}

//***************************************************************************
// Function Name: BcmAdslCtl_ProcessDiagsFrames
// Description  : Processes received frames from DslDiags
// Returns      : none.
//***************************************************************************/
void BcmAdslCtl_ProcessDiagsFrames(void)
{
	DiagProcessCommands();
}
//***************************************************************************
// Function Name: BcmAdslCtl_ProcessGuiFrames
// Description  : Processes received frames from GUI
// Returns      : none.
//***************************************************************************/
void BcmAdslCtl_ProcessGuiFrames(void)
{
	GuiProcessCommands();
}
#if 0
//***************************************************************************
// Function Name: BcmAdslCtl_GetStatistics
// Description  : Get ADSL statistics info.
// Returns      : none.
//***************************************************************************/
extern void BcmAdslCtl_GetStatistics(adslPhysEntry *pAdslPhysRcv,
                                     adslFullPhysEntry *pAdslPhysXmt,
                                     adslConnectionInfo *pAdslConnInfo,
                                     adsl2ConnectionInfo *pAdsl2ConnInfo,
                                     adslConnectionStat *pAdslConnStat,
                                     adslPerfDataEntry *pAdslRxPerfData,
                                     adslPerfCounters  *pAdslTxPerf,
                                     atmConnectionStat *pAtmConnStat,
                                     char *errMsg,
                                     int linkDown) 
{
	adslMibInfo		adslMib;
	int nRet;
	long			size = sizeof(adslMib);
	// initialize
	memset(pAdslPhysRcv, 0, sizeof(adslPhysEntry));
	memset(pAdslPhysXmt, 0, sizeof(adslFullPhysEntry));
	memset(pAdslConnInfo, 0, sizeof(adslConnectionInfo));
	memset(pAdsl2ConnInfo, 0, sizeof(adsl2ConnectionInfo));
	memset(pAdslConnStat, 0, sizeof(adslConnectionStat));
	memset(pAdslRxPerfData, 0, sizeof(adslPerfDataEntry));
	memset(pAdslTxPerf, 0, sizeof(adslPerfCounters));
	memset(pAtmConnStat, 0, sizeof(atmConnectionStat));
	errMsg[0] = '\0';

	nRet = BcmAdsl_GetObjectValue(NULL, 0, (char *)&adslMib, &size);
	if( nRet != BCMADSL_STATUS_SUCCESS) {
		strcpy(errMsg,"BcmAdsl_GetObjectValue error");
		return;
	}
	*pAdslPhysRcv=adslMib.adslPhys;
	*pAdslPhysXmt=adslMib.adslAtucPhys;
	*pAdslConnInfo=adslMib.adslConnection;
	*pAdsl2ConnInfo=adslMib.adsl2Info;
	*pAdslConnStat=adslMib.adslStat;
	*pAdslRxPerfData=adslMib.adslPerfData;
	*pAdslTxPerf=adslMib.adslTxPerfTotal;
	*pAtmConnStat=adslMib.atmStat;
	return;
}
//***************************************************************************
// Function Name: BcmAdslCtl_GetBertState
// Description  : Get ADSL BERT test results.
// Returns      : none.
//***************************************************************************/
int BcmAdslCtl_GetBertState(void) {
   int state = ADSL_BERT_STATE_STOP;
   char line[SYS_CMD_LEN], value[SYS_CMD_LEN];
   FILE* fs = NULL;

   // execute adslctl bert command
   bcmSystemMute("adslctl bert --show > /var/adslbert");

   fs = fopen("/var/adslbert", "r");
   if (fs == NULL)
      return state;

   // read pass title line (adslctl: BERT results:)
   if ( fgets(line, SYS_CMD_LEN, fs) == NULL )
      return state;
   // read Status line
   if ( fgets(line, SYS_CMD_LEN, fs) != NULL ) {
      parseAdslInfo(line, "=", value, SYS_CMD_LEN - 1);
      if ( strcasecmp(value, "RUNNING") == 0 )
         state = ADSL_BERT_STATE_RUN;
   }

   fclose(fs);
   bcmSystemMute("rm /var/adslbert");

   return state;
}


//***************************************************************************
// Function Name: BcmAdslCtl_GetBertTotalBits
// Description  : Get ADSL BERT total bits.
// Returns      : none.
//***************************************************************************/
cnt64 BcmAdslCtl_GetBertTotalBits(void) {
			adslMibInfo		adslMib;
			long size = sizeof(adslMib);
      int nRet = BcmAdsl_GetObjectValue(NULL, 0, (char *)&adslMib, &size);

      if( nRet != BCMADSL_STATUS_SUCCESS) {
		     printf("BcmAdsl_GetObjectValue error\n");
         adslMib.adslBertStatus.bertTotalBits.cntHi = adslMib.adslBertStatus.bertTotalBits.cntLo = 0;
      }
      return (adslMib.adslBertStatus.bertTotalBits);
}

//***************************************************************************
// Function Name: BcmAdslCtl_GetBertErrBits
// Description  : Get ADSL BERT error bits.
// Returns      : none.
//***************************************************************************/
cnt64 BcmAdslCtl_GetBertErrBits(void) {
			adslMibInfo		adslMib;
			long size = sizeof(adslMib);
      int nRet = BcmAdsl_GetObjectValue(NULL, 0, (char *)&adslMib, &size);

      if( nRet != BCMADSL_STATUS_SUCCESS) {
		     printf("BcmAdsl_GetObjectValue error\n");
         adslMib.adslBertStatus.bertErrBits.cntHi = adslMib.adslBertStatus.bertErrBits.cntLo = 0;
      }
      return (adslMib.adslBertStatus.bertErrBits);
}

// old way of getting bert result
#if 0
//***************************************************************************
// Function Name: BcmAdslCtl_GetBertResults
// Description  : Get ADSL BERT test results.
// Returns      : none.
//***************************************************************************/
void BcmAdslCtl_GetBertResults(adslBertResults *pAdslBertRes) {
   char line[SYS_CMD_LEN], value[SYS_CMD_LEN];
   FILE* fs = NULL;

   // initialize pAdslBertRes
   memset(pAdslBertRes, 0, sizeof(adslBertResults));

   // execute adslctl bert command
   bcmSystemMute("adslctl bert --show > /var/adslbert");

   fs = fopen("/var/adslbert", "r");
   if (fs == NULL) return;

   // read pass title line (adslctl: BERT results:)
   if ( fgets(line, SYS_CMD_LEN, fs) == NULL ) return;
   // read pass Status line
   if ( fgets(line, SYS_CMD_LEN, fs) == NULL ) return;
   // read pass Total Time line
   if ( fgets(line, SYS_CMD_LEN, fs) == NULL ) return;
   // read pass Elapsed Time line
   if ( fgets(line, SYS_CMD_LEN, fs) == NULL ) return;
   // read Bits Tested line
   if ( fgets(line, SYS_CMD_LEN, fs) != NULL ) {
      parseAdslInfo(line, "=", value, SYS_CMD_LEN - 1);
      pAdslBertRes->bertTotalBits = strtoul(value, NULL, 16);
   }
   // read Errr Bits line
   if ( fgets(line, SYS_CMD_LEN, fs) != NULL ) {
      parseAdslInfo(line, "=", value, SYS_CMD_LEN - 1);
      pAdslBertRes->bertErrBits = strtoul(value, NULL, 16);
   }

   fclose(fs);
   bcmSystemMute("rm /var/adslbert");
}
#endif

//***************************************************************************
// Function Name: BcmAdslCtl_GetPhyVersion
// Description  : Get ADSL PHY version.
// Returns      : none.
//***************************************************************************/
void BcmAdslCtl_GetPhyVersion(char *version, int len) {
   char line[SYS_CMD_LEN];
   FILE* fs = NULL;

   if ( version == NULL ) return;

   version[0] = '\0';

   fs = fopen("/var/adslversion", "r");
   if (fs == NULL)
   {
      // execute adslctl info command
      bcmSystemNoHang("adslctl --version 2> /var/adslversion");

      fs = fopen("/var/adslversion", "r");
      if (fs == NULL)
         return;
   }

   // read pass adslctl version line (adslctl version 1.0)
   if ( fgets(line, SYS_CMD_LEN, fs) == NULL ) return;

   // read adsl phy version line
   if ( fgets(line, SYS_CMD_LEN, fs) != NULL )
      parseAdslInfo(line, "-", version, len);

   fclose(fs);
//   bcmSystemNoHang("rm /var/adslversion");
}

//***************************************************************************
// Function Name: BcmAdslCtl_GetConnectionInfo
// Description  : Get ADSL connection information.
// Returns      : STS_SUCCESS if successful or error status.
//***************************************************************************/
BCMADSL_STATUS BcmAdslCtl_GetConnectionInfo( PADSL_CONNECTION_INFO pConnInfo ) {
   ADSLDRV_CONNECTION_INFO Arg;
   Arg.bvStatus = BCMADSL_STATUS_ERROR;
   int nFd = -1;
   nFd = open( "/dev/bcmadsl0", O_RDWR );
   if( nFd > 0 ) {
      DiagProcessCommands( nFd );
      GuiProcessCommands( nFd );
      GdbProcessCommands( nFd );
      ioctl( nFd, ADSLIOCTL_GET_CONNECTION_INFO, &Arg );
      close(nFd);
      pConnInfo->LinkState = Arg.ConnectionInfo.LinkState;
      //LGD_FOR_TR098
      pConnInfo->ShowtimeStart = Arg.ConnectionInfo.ShowtimeStart;
      pConnInfo->ulFastUpStreamRate = Arg.ConnectionInfo.ulFastUpStreamRate;
      pConnInfo->ulFastDnStreamRate = Arg.ConnectionInfo.ulFastDnStreamRate;
      pConnInfo->ulInterleavedUpStreamRate = Arg.ConnectionInfo.ulInterleavedUpStreamRate;
      pConnInfo->ulInterleavedDnStreamRate = Arg.ConnectionInfo.ulInterleavedDnStreamRate;
   } else
      printf( "BcmAdslCtl_GetConnectionInfo : open error %d\n", errno );

   return( Arg.bvStatus );
} // BcmAdslCtl_GetConnectionInfo

/* from adslctl.c -- get annexM string if any */
char* BcmAdslCtl_GetModulationStr(int modType, int adsl2Type)
{
   static char annexMStr[] = " AnnexM EU-  ";

   if (adsl2Type & kAdsl2ModeAnnexMask)
   {
      int mVal = 32 + (((adsl2Type & kAdsl2ModeAnnexMask) - 1) << 2);

      annexMStr[sizeof(annexMStr)-3] = '0' + mVal/10;
      annexMStr[sizeof(annexMStr)-2] = '0' + mVal%10;
      return annexMStr;
   }

   return "";
}
#endif

#endif /* SUPPORT_DSL */
