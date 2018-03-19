/***********************************************************************
 *
 *  Copyright (c) 2006-2007  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2011:proprietary:standard

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

#ifndef TR69C_DEFS_H
#define TR69C_DEFS_H

#include <time.h>
#include "cms.h"
#include "cms_tmr.h"
#include "cms_tms.h"

#define DATA_MODEL_TR98         98
#define DATA_MODEL_TR181        181
/* CWMP version: MAJOR | MINOR: 0x0100 for cwmpv1.0, 0x0102 for cwmp v1.2 */
#define CWMP_VERSION_MAJOR_1 0x0100
#define CWMP_VERSION_MINOR_1 0x0001
#define CWMP_VERSION_MINOR_2 0x0002
#define CWMP_VERSION_MINOR_3 0x0003
#define CWMP_VERSION_1_0     (CWMP_VERSION_MAJOR_1)
#define CWMP_VERSION_1_1     (CWMP_VERSION_MAJOR_1|CWMP_VERSION_MINOR_1)
#define CWMP_VERSION_1_2     (CWMP_VERSION_MAJOR_1|CWMP_VERSION_MINOR_2)
#define CWMP_VERSION_1_3     (CWMP_VERSION_MAJOR_1|CWMP_VERSION_MINOR_3)

typedef enum {
    TRX_OK=0,
    TRX_REBOOT,
    TRX_ERR, /* for internal error */
    TRX_INVALID_PARAMETER_VALUE
}TRX_STATUS;

typedef enum {
    NOTIFICATION_OFF = 0,
    PASSIVE_NOTIFICATION,
    ACTIVE_NOTIFICATION
} eNotification;

typedef enum {
   eACSNeverContacted=0,
   eACSContacted,
   eACSInformed,
   eACSUpload,
   eACSDownloadReboot,
   eACSSetValueReboot,
   eACSAddObjectReboot,
   eACSDelObjectReboot,
   eACSRPCReboot
} eInformState;

/* status enum for acs connection and msg xfer */
typedef enum {
   eOK,
   eConnectError,
   eGetError,
   ePostError,
   eAuthError,
   eDownloadDone,
   eUploadDone,
   eAcsDone
}AcsStatus;

#define NOREBOOT eACSNeverContacted

extern eInformState  informState;


/* TR-069 session enum 
 *  eSessionStart - sending 1st Inform
 *  eSessionAuthentication - sending 2nd Inform
 *  eSessionDeliveryConfirm - receiving InformResponse
 *  eSessionEnd - receiving 204 No Content or 200 OK 
 */
typedef enum {
   eSessionUnknown,
   eSessionStart,
   eSessionAuthenticating,
   eSessionDeliveryConfirm,
   eSessionEnd
} eSessionState;

/*
* Define CPEVARNAMEINSTANCE in standardparams.c to create an
 * instance of all the CPE parameter strings.
 * undef VARINSTANCE to create a extern to the string pointer
 * If CPEVARNAMEINSTANCE is defined 
 * SVAR(X) creates a char string constant of X and labels it with
 * the variable name X.
 * SSVAR(X,Y) creates a char string constant of Y and labels it with
 * the variable name X. This is used for strings that can't be C labels.
 *                                                                       
 * If CPEVARNAMEINSTANCE is NOT defined SVAR generates
 * a extern of the form   extern const char X[];
*/
#ifdef CPEVARNAMEINSTANCE
/*#define mkstr(S) # S  */
#define SVAR(X) const char X[]=#X
#define SSVAR(X,Y) const char X[]=#Y
#else
#define SVAR(X) extern char X[]
#define SSVAR(X,Y) extern char X[]
#endif

SSVAR(InternetGatewayDevice, InternetGatewayDevice.);
SSVAR(Device, Device.);

SSVAR(ManagementServer, ManagementServer.);
SSVAR(WANDevice, WANDevice.);
SSVAR(WANConnectionDevice, WANConnectionDevice.);
SVAR(URL);
SVAR(ConnectionRequestURL);
SVAR(Username);
SVAR(Password);
SVAR(PeriodicInformEnable);
SVAR(PeriodicInformInterval);
SVAR(ConnectionRequestUsername);
SVAR(ConnectionRequestPassword);

typedef enum {
   rpcUnknown=0,
   rpcGetRPCMethods,
   rpcSetParameterValues,
   rpcGetParameterValues,
   rpcGetParameterNames,
   rpcGetParameterAttributes,
   rpcSetParameterAttributes,
   rpcAddObject,
   rpcDeleteObject,
   rpcReboot,
   rpcDownload,
   rpcUpload,
   rpcGetQueuedTransfers,
   rpcScheduleInform,
   rpcFactoryReset, 
   rpcChangeDuState,           /******** last rpc method ******/
   rpcInformResponse,          /* responses start here */
   rpcTransferCompleteResponse,
   rpcGetRPCMethodsResponse,
   rpcFault,                   /* soapenv:Fault response from ACS */
   rpcChangeDuStateResponse,
   rpcDuStateChangeCompleteResponse,
   rpcVendorSpecific,
   rpcAutonTransferCompleteResponse,
} eRPCMethods;

#define LAST_RPC_METHOD    rpcChangeDuState


/* must match eRPCMethods (enumeration of methods, see above) */
/* this is only used for debugging purposes; it checks if this method is supported, and print YES or NO. */
typedef struct RpcMethods {
    unsigned   rpcGetRPCMethods:1;
    unsigned   rpcSetParameterValues:1;
    unsigned   rpcGetParameterValues:1;
    unsigned   rpcGetParameterNames:1;
    unsigned   rpcGetParameterAttributes:1;
    unsigned   rpcSetParameterAttributes:1;
    unsigned   rpcReboot:1;
    unsigned   rpcDownload:1;
    unsigned   rpcUpload:1;
    unsigned   rpcGetQueuedTransfers:1;
    unsigned   rpcFactoryReset:1;
    unsigned   rpcScheduleInform:1;
    unsigned   rpcChangeDuState:1;
    unsigned   rpcVendorSpecific:1;
} RpcMethods;

typedef struct ACSState {
    char        *acsURL;        /* URL of ACS */
    char        *acsUser;
    char        *acsPwd;
    time_t      informTime;     /* next ACS inform Time */
    time_t      informInterval; /* inform interval */
    int         informEnable;   /* True if inform to be performed*/
    int         maxEnvelopes;   /* Number of max env returned in inform response*/
    int         holdRequests;   /* hold request to ACS if true */
    int         noMoreRequests; /* don't send any more Req to ACS */
    RpcMethods  acsRpcMethods;  /* methods from GetRPCMethods response*/
    char        *rebootCommandKey; /* key for reboot command key */
    char        *downloadCommandKey;    /* key for download cmd*/
    int         noneConnReqAuth;  /* no connection request authentication*/
    char        *boundIfName;     /* name of interface which tr69c works on, see description in data model */
    char        *connReqURL;
    char        *connReqIpAddr; /* IP address part of connReqURL -- part of the inform msg */
    char        *connReqIpAddrFullPath; /* full path to the parameter that holds the connReqIpAddr, part of the inform msg */
    char        *connReqPath;   /* path part of connReqURL -- used by listener.c */
    char        *connReqUser;
    char        *connReqPwd;
    char        *kickURL;
    char        upgradesManaged;
    char        *provisioningCode;
    int         retryCount;     /* reset on each ACS response*/
    int         fault;          /* last operation fault code */
    int         dlFaultStatus;  /* download fault status */
    char        *dlFaultMsg;    /* download fault message */
    time_t      startDLTime;    /* start download time */
    time_t      endDLTime;      /* complete download time*/
    char        *scheduleInformCommandKey;    /* key for scheduleInform*/
    char        *manufacturer;      /**< from deviceInfo */
    char        *manufacturerOUI;   /**< from deviceInfo */
    char        *productClass;      /**< from deviceInfo */
    char        *serialNumber;      /**< from deviceInfo */
    struct ChangeDuStateOpReqInfo *duStateOpList;  /**< each DU Operatation save, opResultStruct can be built later  */
    char        *duStateChangeCommandKey; /*  command key */
    unsigned    currentDuChangeReqId;
    unsigned    pendingDuReq;
    int         dataModel;        /* the data model which client communicates with ACS: TR98, TR181 */
    int         cwmpVersion;      /* default is v1.2, but can revert back to 1.0 */
    char        *redirectURL;     /* redirect ACS URL */
} ACSState;


typedef enum {
   eFirmwareUpgrade=1,
   eWebContent     =2,
   eVendorConfig   =3,
   eVendorLog      =4
} eFileType;

typedef enum {
   eTransferNotInitialized = 0, /* used internally */
   eTransferNotYetStarted =1,
   eTransferInProgress    =2,
   eTransferCompleted     =3,
   eTransferRejected      =4    /* used internally */
} eTransferState;
#define TRANSFER_QUEUE_SIZE 16

typedef struct DownloadReq {
   eFileType efileType;
   char   *commandKey;
   char    *url;
   char    *user;
   char    *pwd;
   int     fileSize;
   char    *fileName;   /* ignore in this implementation- everything is in memory */
   int     delaySec;
   eTransferState state;      
} DownloadReq;

typedef struct DownloadReqInfo {
   eFileType   efileType;
   char        commandKey[33];
   char        url[256];
   char        user[256];
   char        pwd[256];
   int         fileSize;
   char        fileName[256];  /* ignore in this implementation- everything is in memory */
   int         delaySec;
   eTransferState state;    
   eRPCMethods rpcMethod;     
} DownloadReqInfo;

typedef struct TransferQInfo {
   DownloadReq request;
   eRPCMethods rpcMethod;   
} TransferQInfo;

typedef struct TransferInfo {
   int mostRecentCompleteIndex;
   TransferQInfo queue[TRANSFER_QUEUE_SIZE];
} TransferInfo;

typedef struct LimitNotificationInfo {
   char *parameterFullPathName; /* for example, IGD.ManagementServer.ManageableDeviceNumberOfEntries */
   int notificationPending; /* 1 = pending; 0 = nothing to send */
   int limitValue;          /* how often do we send active notification, in ms rather than Second */
   CmsEventHandler func;    /* notification timer function */
   CmsTimestamp lastSent;   /* the time stamp at which the last notification was sent */
   struct LimitNotificationInfo *next;
} LimitNotificationInfo;

typedef struct LimitNotificationQInfo {
   int count;  
   LimitNotificationInfo *limitEntry;
} LimitNotificationQInfo;

#define VENDOR_CFG_INFO_TOKEN     "vendorCfg"
typedef struct DownloadVendorConfigInfo {
   char        name[BUFLEN_64];
   char        version[BUFLEN_16];
   char        date[BUFLEN_64];
   char        description[BUFLEN_256];
} DownloadVendorConfigInfo;

/*
 * MAXINFORMEVENTS is the maximum number of events and "method" sub-codes
 * that tr69c currently needs to send to the ACS.  Note that the event
 * codes are transfered to a separate GWStateData structure for saving
 * to flash.  That structure has 64 hardcoded slots to store the
 * event and method sub-codes.
 */
#define MAXINFORMEVENTS 16

typedef struct InformEvList {
   unsigned char    informEvList[MAXINFORMEVENTS];
   int              informEvCnt;   /* number of events in list */
   eRPCMethods      mMethod;      /* set if M <method> event required */
} InformEvList;


/*
 * Inform Events and their associated sub-code methods.
 * Notes:
 * (1) These values are stored in informEvList and in flash
 *     (GWStateData.informEvList) as single byte char, so the value must not
 *     exceed 255.
 * (2) The text corresponding to these values are defined in the
 *     informEventStrTable in bcmWrapper.c, so any additions to these values
 *     must also be reflected in that table.
 * (3) Values of 0-12 were used up to 4.12 release series.  Since these values
 *     can be written to flash, their values must not change in newer releases;
 *     otherwise, problems will occur when upgrading from 4.12 to 4.14.  Going
 *     forward, whatever values are defined must remain that value for the
 *     same reason.
 */
#define INFORM_EVENT_BOOTSTRAP                0
#define INFORM_EVENT_BOOT                     1
#define INFORM_EVENT_PERIODIC                 2
#define INFORM_EVENT_SCHEDULED                3
#define INFORM_EVENT_VALUE_CHANGE             4
#define INFORM_EVENT_KICKED                   5
#define INFORM_EVENT_CONNECTION_REQUEST       6
#define INFORM_EVENT_TRANSER_COMPLETE         7
#define INFORM_EVENT_DIAGNOSTICS_COMPLETE     8
#define INFORM_EVENT_REBOOT_METHOD            9
#define INFORM_EVENT_SCHEDULE_METHOD          10
#define INFORM_EVENT_DOWNLOAD_METHOD          11
#define INFORM_EVENT_UPLOAD_METHOD            12

#define INFORM_EVENT_REQUEST_DOWNLOAD         109
#define INFORM_EVENT_AUTON_TRANSFER_COMPLETE  110
#define INFORM_EVENT_DU_CHANGE_COMPLETE       111
#define INFORM_EVENT_AUTON_DU_CHANGE_COMPLETE 112

#define INFORM_EVENT_SCHEDULE_DOWNLOAD_METHOD 150
#define INFORM_EVENT_CHANGE_DU_CHANGE_METHOD  151

struct informEvtTableEntry {
   UINT32 informEvt;
   const char *str;
};



#define CHANGEDUSTATE_TIMEOUT 1440000      /* 24 hours * 60 min * 1000 ms */
#define MAX_DU_OP_REQUEST   16
#define RESULT_BIT_DU_OP_INSTALL     0x1
#define RESULT_BIT_DU_OP_UPDATE      0x2
#define RESULT_BIT_DU_OP_UNINSTALL   0x4
#define RESULT_FAULT_CODE_COUNT_MAX  120

typedef enum {
   eDuNoop=0,
   eDuInstall,
   eDuUpdate,
   eDuUninstall
} eOperationType;

typedef struct ChangeDuStateOp {
   struct ChangeDuStateOp *next;
   eOperationType operationType;
   char    *url;
   char    *uuid;
   char    *user;
   char    *pwd;
   char    *exeEnvRef;
   char    *version;
   unsigned fault;   /* fault code on this operation */
} ChangeDuStateOp;

typedef struct faultStruct
{
   unsigned faultCode;
   char *faultString;
}faultStruct;

/* we basically need to save operationType, URL for install (UUID might be empty from ACS), UUID (for update and uninstall) */ 
typedef struct ChangeDuStateOpReqInfo
{
   struct ChangeDuStateOpReqInfo *next;
   eOperationType operationType;
   char *url;
   char uuid[BUFLEN_40];  // uuid is 36 bytes, 40 to match DUrequestStateChangedMsgBody
   char *version;
   char *user;
   char *pwd;
   char execEnvFullPath[BUFLEN_256];
   char *startTime;
   char *completeTime;
   struct faultStruct fault;
   /* these are retrieved and allocated later when sendDuComplete needs to be sent */
   char *duRef;
   char *currentState;
   UBOOL8 resolved;
   char *euRef;
} ChangeDuStateOpReqInfo;

#ifdef SUPPORT_TR69C_AUTONOMOUS_TRANSFER_COMPLETE

#define FILETYPE_FIRMWARE_UPGRADE "1 Firmware Upgrade Image"
#define FILETYPE_WEB_CONTENT      "2 Web Content"
#define FILETYPE_VENDOR_CONFIG    "3 Vendor Configuration File"
#define FILETYPE_VENDOR_LOG       "4 Vendor Log File"
#define FILETYPE_TONE_FILE        "4 Tone File"
#define FILETYPE_RINGER_FILE      "5 Ringer File"
#define FILETYPE_VENDOR_CONFIG_I  "6 Vendor Configuration File i"
#define FILETYPE_VENDOR_LOG_I     "7 Vendor Log File i"

typedef struct autonomousFileTransferStats
{
   UBOOL8 isDownload;
   char *fileType;
   UINT32 fileSize;
   struct faultStruct fault;
   char *startTime;
   char *completeTime;
} AutonomousFileTransferStats;
#endif /* SUPPORT_TR69C_AUTONOMOUS_TRANSFER_COMPLETE */

#ifdef USE_DMALLOC
#include "dmalloc.h"
#endif // USE_DMALLOC

#endif   // TR69C_DEFS_H
