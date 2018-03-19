/* ====================================================================
 * <:label-3PIP:1998:NONE:standard
 * 
 * :>
 * ====================================================================*/
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <asn1.h>
#include <snmp.h>
#include <agtEngine.h>
#include <agtTransport.h>
#include <snmpCommunity.h>
#include "snmpv2Mib.h"
#include "ifMib.h"
#include "cpeDslMib.h"
#include "atmMib.h"
#include "pppSecMib.h"
#include "vdslMib.h"
#include "clPktc.h"

#include "RMON.h"
#include "qBridge.h"
#include "pBridge.h"

#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"

#ifdef BUILD_SNMP_DEBUG
#include "snmpDebug.h"
SNMP_DEBUG_FLAG snmpDebug = { .level = SNMP_DEBUG_LEVEL_SNMP,
                              .transport = SNMP_DEBUG_LEVEL_TRANSPORT_ALL,
                              .mib = SNMP_DEBUG_LEVEL_MIB_CPE,
                            };
#endif /* BUILD_SNMP_DEBUG */

extern unsigned long current_sysUpTime(void);
struct timeval snmpUpTime;
void *msgHandle = NULL;
int trapCode = SNMP_TRAP_WARMSTART;
int	snmp_port = SNMP_PORT;
int   snmp_socket = 0;
unsigned char outputBuffer[512];

UBOOL8 isWanConnectionUp(void);
int sendStartTrap(int trapCode);
void registerInterestedEvent(int event);

static void initLoggingFromConfig(UBOOL8 useConfiguredLogLevel)
{
   SnmpdCfgObject *obj;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   if ((ret = cmsObj_get(MDMOID_SNMPD_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get of MDMOID_SNMPD_CFG object failed, ret=%d", ret);
   }
   else
   {
      if (useConfiguredLogLevel)
      {
         cmsLog_setLevel(cmsUtl_logLevelStringToEnum(obj->loggingLevel));
      }

      cmsLog_setDestination(cmsUtl_logDestinationStringToEnum(obj->loggingDestination));
      cmsObj_free((void **) &obj);
   }
}

void snmpCleanUp(int junk)
{
   cmsLog_debug("entered");

   snmpCloseTransport();
}


#ifdef CPE_DSL_MIB 
void cpeCheckLinkUp(int junk)
{
#ifdef NEED_CHANGE

  int trapTime;
#ifdef BUILD_SNMP_DEBUG
  time_t now;
#endif    

  if (getAdslOperStatus() == IF_OPER_STATUS_DOWN) {
#ifdef BUILD_SNMP_DEBUG
    time(&now);
    cmsLog_debug("cpeCheckLinkUp(time is %s): link down.\n",ctime(&now));
#endif    
    signal(SIGALRM,cpeCheckLinkUp);
    alarm(1);
  }
  else {
    /* cancel any previous alarm */
    alarm(0);

    trapTime = cpeGenerateRandomNum(CPE_MAX_TRAP_TIME_INTERVAL);
#ifdef BUILD_SNMP_DEBUG
    time(&now);
    cmsLog_debug("cpeCheckLinkUp: link up.   Sending trap in %d seconds; currentTime %s...\n",
                 trapTime,ctime(&now));
#endif
    signal(SIGALRM,cpeSendBootTrap);
    alarm(trapTime);
  }
#else
  /* we need to register interested even here */
  return;
#endif /* need change */
}

#endif /* #ifndef CPE_DSL_MIB  */

/*
 * Register our interest for event with smd.
 */
CmsRet snmpRegisterInterestedEvent(int event)
{
   CmsMsgHeader *msg;
   CmsRet ret = CMSRET_SUCCESS;

   if ((msg = cmsMem_alloc(sizeof(CmsMsgHeader), ALLOC_ZEROIZE)) != NULL)
   {
      cmsLog_debug("registering interest for %x event msg",event);
      msg->src = EID_SNMPD;
      msg->dst = EID_SMD;
      msg->type = CMS_MSG_REGISTER_EVENT_INTEREST;
      msg->flags_request = 1;
      msg->flags_response = 0;
      msg->flags_event = 0;
      msg->wordData = event;
      if ((ret = cmsMsg_sendAndGetReply(msgHandle, msg)) != CMSRET_SUCCESS)
      {
         cmsLog_error("could not register for event %x",event);
      }
   }
   return ret;
}

/* 
 * The main routine with which the SNMP-agent is started.
 */
int main(int argc, char** argv)
{
   SINT32      c, logLevelNum;
   SINT32      shmId=0;          /* Use existing shared memory */
   CmsLogLevel logLevel=DEFAULT_LOG_LEVEL;
   UBOOL8      useConfiguredLogLevel=TRUE;
   CmsRet      ret;
   SnmpCfgObject *obj;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   fd_set	fdset;
   int count;

   /* init log util */
   cmsLog_init(EID_SNMPD);

   /* parse command line args */
   while ((c = getopt(argc, argv, "v:m:")) != -1)
   {
      switch(c)
      {
         case 'v':      /* Loglevel */
            logLevelNum = atoi(optarg);

            cmsLog_debug("option v with logLevelNum %d",logLevelNum);

            if (logLevelNum == 0)
            {
               logLevel = LOG_LEVEL_ERR;
            }
            else if (logLevelNum == 1)
            {
               logLevel = LOG_LEVEL_NOTICE;
            }
            else
            {
               logLevel = LOG_LEVEL_DEBUG;
            }
            cmsLog_setLevel(logLevel);
            useConfiguredLogLevel = FALSE;
            break;

         case 'm':      /* Shared memory ID */
            cmsLog_debug("option m with shmId %d",shmId);

            shmId = atoi(optarg);
            break;

         default:
            //            usage(argv[0]);
            cmsLog_error("Invalid argument '%d' (ascii number)", c);
            return 0;
      }
   }

   /*
    * Detach myself from the terminal so I don't get any control-c/sigint.
    * On the desktop, it is smd's job to catch control-c and exit.
    * When tr69c detects that smd has exited, tr69c will also exit.
    */
   if (setsid() == -1)
   {
      cmsLog_error("Could not detach from terminal");
   }
   else
   {
      cmsLog_debug("detached from terminal");
   }

   /* set signal masks */
   signal(SIGPIPE, SIG_IGN); /* Ignore SIGPIPE signals */

   cmsLog_notice("calling cmsMsg_init");
   if ((ret = cmsMsg_init(EID_SNMPD, &msgHandle)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMsg_init failed, ret=%d", ret);
      return 0;
   }

   cmsLog_notice("calling cmsMdm_init with shmId=%d", shmId);
   if ((ret = cmsMdm_init(EID_SNMPD, msgHandle, &shmId)) != CMSRET_SUCCESS)
   {
      cmsMsg_cleanup(&msgHandle);
      cmsLog_error("cmsMdm_init error ret=%d", ret);
      return 0;
   }

   if ((ret = cmsLck_acquireLockWithTimeout(SNMP_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      return 0;
   }

   initLoggingFromConfig(useConfiguredLogLevel);
   gettimeofday(&snmpUpTime, (struct timezone *)0);

   /* SNMP-agent initialization functions */
   cmsLog_notice("SNMP-agent initialization - register mibs");
   init_ifmib();
   init_SNMPv2_MIB();

#ifdef BUILD_SNMP_BRIDGE_MIB
   init_rmon();
   init_pBridge();
   init_qBridge();
#endif

#ifdef CPE_DSL_MIB
   init_vdsl_mib();
   init_cpe_dsl_mib();
#endif

#ifdef BRCM_CPE_MIB
   init_brcm_cpe_mib();
#endif

#ifdef BRCM_PKTCBL_SUPPORT
   init_pktcMib();
#endif

   if ((ret = cmsObj_get(MDMOID_SNMP_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get of SNMP_CFG object failed, ret=%d", ret);
      cmsLck_releaseLock();
      return 0;
   }

   set_community(obj->ROCommunity, READ_COMMUNITY);
   set_community(obj->RWCommunity, WRITE_COMMUNITY);

   snmp_set_trap_addr((unsigned char*)obj->trapIPAddress);

   cmsLog_notice("Calling snmp_open_connection(%d)", snmp_port);
   /* Open the network to receive/send messages, 
      also open a channel to receive and send message to/from SMD */
   snmp_socket = snmp_open_connection(snmp_port);

   cmsLck_releaseLock();   

   /* read the trap code from persistent storage */
   count = cmsPsp_get("snmp_state", &trapCode, sizeof(trapCode));
   if ((count <= 0) || (trapCode == -1))
   {
      /* warmStart */
      trapCode = SNMP_TRAP_WARMSTART;;
   }

   /* check trap ip address; if not on Lan, check WAN UP, if not up, register insterested event */
   if (cmsNet_isAddressOnLanSide(obj->trapIPAddress))
   {
      sendStartTrap(trapCode);
   }
   else 
   {
      /* find out if WAN connection is up; if not, register interest event */
      if (isWanConnectionUp())
      {
         cmsLog_debug("WAN connection is UP");
         sendStartTrap(trapCode);
      }
      else
      {
         cmsLog_debug("register interest for any WAN connection up");
         registerInterestedEvent(CMS_MSG_WAN_CONNECTION_UP);
      }
   }
         
   cmsLog_notice("Starting event loop");
   /* Listen to the network */
   for (;;)
   {
      count = snmpTransportSelect(&fdset);
      if (count > 0)
      {
         snmpTransportRead(&fdset);
      } 
      else
      {
         switch (count) 
         {
         case 0:
            break;
         case -1:
            if (errno == EINTR)
            {	
               continue;
            }
            else
            {
               perror("select");
            }
         default:
            cmsLog_error("select returned %d", count);
         } /* case */
      } /* count */
   } /* forever */
   return 1;
} /* main */

int sendStartTrap(int trapCode)
{
    oid oid_sysUpTime[] = { O_sysUpTime, 0 };
    oid oid_trapOid[] = {  O_snmpTrapOID, 0 };
    oid val_coldTrapOid[] = { O_coldStart };
    oid val_warmTrapOid[] = { O_warmStart };
    oid oid_enterprise[] = {O_snmpTrapEnterprise, 0 }; 
    oid val_enterprise[] = {SYS_OBJ, 2, 10 }; 
    Oid oid_obj;
    struct timeval  now;
    unsigned char *out_data;
    int out_length = SNMP_MAX_MSG_LENGTH;
    raw_snmp_info_t message;
    CmsRet      ret;
    SnmpCfgObject *obj;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

    cmsLog_debug("Sending trap type %d",trapCode);

    memset( (unsigned char *) &message, 0x00, sizeof(message) );
    if ((ret = cmsLck_acquireLockWithTimeout(SNMP_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
    {
       cmsLog_error("failed to get lock, ret=%d", ret);
       return 0;
    }
    ret = cmsObj_get(MDMOID_SNMP_CFG, &iidStack, 0, (void **) &obj);
    cmsLck_releaseLock();   

    if (ret != CMSRET_SUCCESS)
    {
       cmsLog_error("get of SNMP_CFG object failed, ret=%d", ret);
    }
    else
    {
       message.mesg.version = SNMP_VERSION_2C;
       message.mesg.pdutype = SNMP_TRP2_REQ_PDU;
       strcpy( message.mesg.community, obj->ROCommunity);
       message.mesg.community_length = strlen(obj->ROCommunity);
       message.mesg.community_id = 0;

       if (message.mesg.version == SNMP_VERSION_1)
       {
          message.trap_enterprise_oid.namelen =
             sizeof(oid_enterprise) / sizeof(oid);
          memcpy(message.trap_enterprise_oid.name, oid_enterprise,
                 sizeof(oid_enterprise));
          memcpy(message.trap_agent_ip_addr, "0.0.0.0", sizeof("0.0.0.0"));
          message.trap_specific_type = SNMP_TRAP_COLDSTART;
          message.trap_timestamp = (long)(gettimeofday(&now, (struct timezone *)0));
          out_data = (unsigned char*)build_snmp_trapv1_without_list_of_varbind(&message);
       }
       else
       {
          message.mesg.request_id = (long)current_sysUpTime();
          out_data = (unsigned char*)build_snmp_response_without_list_of_varbind(&message);
       }
       long_return = (long)current_sysUpTime();
       oid_obj.namelen = sizeof(oid_sysUpTime) / sizeof(oid);
       memcpy( oid_obj.name, oid_sysUpTime, sizeof(oid_sysUpTime) );
       out_data = snmp_build_varbind( out_data, &oid_obj, SNMP_TIMETICKS,
                                      sizeof(long), (unsigned char *) &long_return,
                                      &out_length);
       RETURN_ON_BUILD_ERROR(out_data, "build trap sysUpTime");

       oid_obj.namelen = sizeof(oid_trapOid) / sizeof(oid);
       memcpy( oid_obj.name, oid_trapOid, sizeof(oid_trapOid) );
       if (trapCode == SNMP_TRAP_COLDSTART)
       {
          out_data = snmp_build_varbind( out_data, &oid_obj, SNMP_OBJID,
                                         sizeof(val_coldTrapOid), (unsigned char *)val_coldTrapOid,
                                         &out_length);
          RETURN_ON_BUILD_ERROR(out_data, "build trap cold trapOid");
       }
       else
       {
          out_data = snmp_build_varbind( out_data, &oid_obj, SNMP_OBJID,
                                         sizeof(val_warmTrapOid), (unsigned char *)val_warmTrapOid,
                                         &out_length);
          RETURN_ON_BUILD_ERROR(out_data, "build trap warm trapOid");
       }
       
       oid_obj.namelen = sizeof(oid_enterprise) / sizeof(oid);
       memcpy( oid_obj.name, oid_enterprise, sizeof(oid_enterprise) );
       out_data = snmp_build_varbind( out_data, &oid_obj, SNMP_OBJID,
                                      sizeof(val_enterprise), (unsigned char *)val_enterprise,
                                      &out_length);
       RETURN_ON_BUILD_ERROR(out_data, "build trap enterpriseOid");

       message.response_packet_end = out_data;
       out_length = correct_snmp_response_with_lengths( &message, 0, 0 );
       out_data = asn_build_sequence(message.response_pdu, &out_length,
                                     message.mesg.pdutype,
                                     message.response_packet_end - message.response_request_id);
       RETURN_ON_BUILD_ERROR(out_data, "build trap pdu type");

       return( snmp_send_trap( &message ) );
    } /* get snmp config ok */
    return 1;
}


UBOOL8 isWanConnectionUp(void)
{
   CmsMsgHeader msg = EMPTY_MSG_HEADER;
   CmsRet ret;

   msg.type = CMS_MSG_GET_WAN_CONN_STATUS;
   msg.src = EID_SNMPD;
   msg.dst = EID_SSK;
   msg.flags_request = 1;

   /*
    * ssk will reply with TRUE or FALSE for wan connection up.
    * Need to check for that instead of the usual CMSRET enum.
    */
   ret = cmsMsg_sendAndGetReply(msgHandle, &msg);

   if (ret == TRUE)
   {
      cmsLog_debug("ssk says WAN Connection is up. ret=%d", ret);
      return TRUE;
   }
   else
   {
      cmsLog_debug("ssk says WAN Connection is not up. ret=%d", ret);
      return FALSE;
   }
}

void registerInterestedEvent(int event)
{
   CmsMsgHeader msg = EMPTY_MSG_HEADER;
   CmsRet ret;

	memset(&msg, 0, sizeof(msg));
   msg.type = CMS_MSG_REGISTER_EVENT_INTEREST;
   msg.src = EID_SNMPD;
   msg.dst = EID_SMD;
   msg.flags_request = 1;
   msg.wordData = event;

   /*
    * ssk will reply with TRUE or FALSE for wan connection up.
    * Need to check for that instead of the usual CMSRET enum.
    */
   ret = cmsMsg_sendAndGetReply(msgHandle, &msg);
   
   if ( ret != CMSRET_SUCCESS)
   {
      cmsLog_debug("register_EVENT_INTEREST for CMS_MSG_WAN_CONNECTION_UP failed.\n");
   }
   else
   {
      cmsLog_debug("register_EVENT_INTEREST for CMS_MSG_WAN_CONNECTION_UP passed\n");
   }
}
