//****************************************************************************
//
//  Copyright (c) 2002  Broadcom Corporation
//  All Rights Reserved
//  No portions of this material may be reproduced in any form without the
//  written permission of:
//          Broadcom Corporation
//          16215 Alton Parkway
//          Irvine, California 92619
//  All information contained in this document is Broadcom Corporation
//  company private, proprietary, and trade secret.
//
//****************************************************************************
//
//  Filename:       LnxInterface.h
//  Author:         
//  Creation Date:  October 25, 2002
//
//****************************************************************************
//  Description: 
//                  
//
//****************************************************************************
#ifndef LNX_INTERFACE_H
#define LNX_INTERFACE_H

//********************** Include Files ***************************************
#include <pthread.h>
#include <time.h>
#include <arpa/inet.h> 
#include <unistd.h>
#include <errno.h>
#include <net/if_arp.h>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <net/if.h>

#include <netinet/in.h>
#include <unistd.h>

#include "bcmtypes.h"
//********************** Global Constants ************************************

#define MAX_MSGQ_LENGTH    64

//********************** Global Types ****************************************

/* FROM "AIX Version 4.3 Base Documentation "
 * "http://nscp.upenn.edu/aix4.3html/aixgen/topnav/topnav.htm"
 * The PTHREAD_COND_INITIALIZER macro initializes the static
 * condition variable cond, setting its attributes to default 
 * values. This macro should only be used for static condition 
 * variables, since no error checking is performed
 */
#define msgQueueInitializer      { PTHREAD_COND_INITIALIZER, PTHREAD_MUTEX_INITIALIZER, 0, 0 }

typedef struct _MSGQUEUE
{
    pthread_cond_t condition;
    pthread_mutex_t lock;
    int busy;
    char message[MAX_MSGQ_LENGTH];
	int messageSize;
}MSGQUEUE, *PTR_MSGQUEUE;

typedef PTR_MSGQUEUE	msgQid;
typedef char			MessageNo;

typedef struct _THREADINFO
{
	char *name;
	pthread_t tid;
	pid_t pid;
	struct _THREADINFO *next;
}THREADINFO, *PTHREADINFO;

typedef void *(*PFN_TREAD_ENTRY)(void* arg);

typedef struct _THRADARG
{
	void * arg0; // the first one is for our ThreadMain
	void * arg1; // the second one is the name of the thread
	void * arg2; // the third one is the real argument for the startup routine
}THREADARG, *PTHREADARG;


//********************** Global Variables ************************************


#if defined __cplusplus
extern "C" {
#endif

//********************** Function Declaration ***********************************
int StartMyThread( char *name, int *taskId, void * (*start_routine)(void *) );
void WaitForMsgQNotification(msgQid msgQ, MessageNo *pmsgNo);
msgQid CreateMyMsgQueue(void);
void MsgQueueSend(msgQid msgQID, MessageNo *pMsgPtr, unsigned long nbytes);
void osSleep( unsigned long seconds );
void Lnx_CtrlC_Catch();
void RegisterThreadInfo(char *name);
void UnRegisterThreadInfo(char *name);

int lnxgetline(char *str, int maxStringSize);
void DsgTestPrompt(char * choice);
uint32 GetIpStackAddress(uint32* ipAddress, char * interfaceName);
uint32 GetIpStackNetmask(uint32* ipAddress, char * interfaceName);
int InterfaceIsUp(char * interfaceName);

void BcmAddRoute(unsigned long routerAddress, char *pInterfaceName, BOOL isDefault);
BOOL BcmDeleteRoute(unsigned long routerAddress, char *pInterfaceName);
void SetStaticIpAddress(unsigned long ipAddressLong, char *pInterfaceName);
void SetSubnetMask(unsigned long netMaskLong, char *pInterfaceName);
void RenewDhcpPriIpAddress(char *pInterfaceName);

uint32 SetInterfaceFlags(uint32 ipAddress, char *pInterfaceName, uint32 flags);

BOOL SetEcmArpEntry(unsigned long ipAddress, uint8* macAddress, char* devName);
extern int oldstdout;
extern int oldstderr;
int Print2File(int fd, char *FmtStr, ...);

#if defined __cplusplus
}
#endif

#endif //LNX_INTERFACE_H

