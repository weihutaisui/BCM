/***********************************************************************
 *
 *  Copyright (c) 2004-2010  Broadcom Corporation
 *  All Rights Reserved
 *
 * <:label-BRCM:2004:proprietary:standard
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
 *
************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/poll.h>

#include "cms_mdm.h"
#include "cms_msg.h"
#include "cms_util.h"

#define TIME1970 2208988800UL
#define SERVER_MAX 5
#define SNTP_VERSION "0.1"
#define SNTP_SCHED_TIMEOUT_IN_SECS   1
#define SNTP_WAIT_TIMEOUT_IN_SECS    (60 * SNTP_SCHED_TIMEOUT_IN_SECS)
#define SNTP_DELAY_TIMEOUT_IN_SECS   (24 * 60 * 60 * SNTP_SCHED_TIMEOUT_IN_SECS)

extern long get_offset( long gmtime, int tz_idx );

/* should be in string.h, but above including it did not fix compiler warning
 * so  just declare the function here to avoid warning.
 */
extern char *strcasestr(const char *haystack, const char *needle);

/*
 * global variables
 */
void *sntp_msg_hndl = NULL;
void *sntp_tmr_hndl = NULL;
char *servers[SERVER_MAX];
static SINT32 tz_idx = -1;
static SINT32 srv_idx = 0;
static int currentSntpState = TIME_STATE_UNSYNCHRONIZED;
UBOOL8 srv_exit_by_message = FALSE;
UBOOL8 srv_exit_by_signal = FALSE;
UBOOL8 srv_stop = FALSE;

char *timeZones[] =
{"International Date Line West",
 "Midway Island, Samoa",
 "Hawaii",
 "Alaska",
 "Pacific Time, Tijuana",
 "Arizona, Mazatlan",
 "Chihuahua, La Paz",
 "Mountain Time",
 "Central America",
 "Central Time",
 "Guadalajara, Mexico City, Monterrey",
 "Saskatchewan",
 "Bogota, Lima, Quito",
 "Eastern Time",
 "Indiana",
 "Atlantic Time",
 "Caracas, La Paz",
 "Santiago",
 "Newfoundland",
 "Brasilia",
 "Buenos Aires, Georgetown",
 "Greenland",
 "Mid-Atlantic",
 "Azores",
 "Cape Verde Is.",
 "Casablanca, Monrovia",
 "Greenwich Mean Time: Dublin, Edinburgh, Lisbon, London",
 "Amsterdam, Berlin, Bern, Rome, Stockholm, Vienna",
 "Belgrade, Bratislava, Budapest, Ljubljana, Prague",
 "Brussels, Copenhagen, Madrid, Paris",
 "Sarajevo, Skopje, Warsaw, Zagreb",
 "West Central Africa",
 "Athens, Istanbul, Minsk",
 "Bucharest",
 "Cairo",
 "Harare, Pretoria",
 "Helsinki, Kyiv, Riga, Sofia, Tallinn, Vilnius",
 "Jerusalem",
 "Baghdad",
 "Kuwait, Riyadh",
 "Moscow, St. Petersburg, Volgograd",
 "Nairobi",
 "Tehran",
 "Abu Dhabi, Muscat",
 "Baku, Tbilisi, Yerevan",
 "Kabul",
 "Ekaterinburg",
 "Islamabad, Karachi, Tashkent",
 "Chennai, Kolkata, Mumbai, New Delhi",
 "Kathmandu",
 "Almaty, Novosibirsk",
 "Astana, Dhaka",
 "Sri Jayawardenepura",
 "Rangoon",
 "Bangkok, Hanoi, Jakarta",
 "Krasnoyarsk",
 "Beijing, Chongquing, Hong Kong, Urumqi",
 "Irkutsk, Ulaan Bataar",
 "Kuala Lumpur, Singapore",
 "Perth",
 "Taipei",
 "Osaka, Sapporo, Tokyo",
 "Seoul",
 "Yakutsk",
 "Adelaide",
 "Darwin",
 "Brisbane",
 "Canberra, Melbourne, Sydney",
 "Guam, Port Moresby",
 "Hobart",
 "Vladivostok",
 "Magadan",
 "Solomon Is., New Caledonia",
 "Auckland, Wellington",
 "Fiji, Kamchatka, Marshall Is.",
 NULL };


static void sntp_send_message_to_dhcpd(long delta)
{
    char buf[sizeof(CmsMsgHeader) + sizeof(long)]={0};
    CmsMsgHeader *msg=(CmsMsgHeader *) buf;
    long *offset = (long *) (msg+1);
    CmsRet ret = CMSRET_SUCCESS;

    msg->type = CMS_MSG_EVENT_TIME_SYNC;
    msg->src = EID_SNTP;
    msg->dst = EID_DHCPD;
    msg->flags_event = 1;
    msg->flags_bounceIfNotRunning = 1;
    msg->dataLength = sizeof(long);
    *offset = delta;

    if ((ret = cmsMsg_send(sntp_msg_hndl, msg)) != CMSRET_SUCCESS)
    {
        cmsLog_error("could not send out CMS_MSG_EVENT_TIME_SYNC, ret=%d", ret);
    }
    else
    {
        cmsLog_notice("sent out CMS_MSG_EVENT_TIME_SYNC (delta=%d)", delta);
    }
}


static void sntp_send_message_to_smd(int state)
{
    CmsMsgHeader msg = EMPTY_MSG_HEADER;
    CmsRet ret = CMSRET_SUCCESS;

    /* send a message to SSK to let it know the state change */
    msg.type = CMS_MSG_TIME_STATE_CHANGED;
    msg.src = EID_SNTP;
    msg.dst = EID_SMD;
    msg.flags_event = 1;
    msg.wordData = state;

    if ((ret = cmsMsg_send(sntp_msg_hndl, &msg)) != CMSRET_SUCCESS)
    {
        cmsLog_error("Fail to send state change msg failed. ret=%d", ret);
    }
    else
    {
        cmsLog_debug("Send state change msg. State %d", state);
    }
}


/** Register or unregister our interest for some event events with smd.
 *
 * @param msgType (IN) The notification message/event that we are
 *                     interested in or no longer interested in.
 * @param positive (IN) If true, then register, else unregister.
 * @param data     (IN) Any optional data to send with the message.
 * @param dataLength (IN) Length of the data
 */
static void sntp_register_event
    (CmsMsgType msgType,
     UBOOL8 positive,
     void *msgData,
     UINT32 msgDataLen)
{
    CmsMsgHeader *msg = NULL;
    char *data = NULL;
    void *msgBuf = NULL;
    char *action __attribute__ ((unused)) = (positive) ? "REGISTER" : "UNREGISTER";
    CmsRet ret = CMSRET_SUCCESS;

    if (msgData != NULL && msgDataLen != 0)
    {
        /* for msg with user data */
        msgBuf = cmsMem_alloc(sizeof(CmsMsgHeader) + msgDataLen, ALLOC_ZEROIZE);
    } 
    else
    {
        msgBuf = cmsMem_alloc(sizeof(CmsMsgHeader), ALLOC_ZEROIZE);
    }

    msg = (CmsMsgHeader *)msgBuf;

    /* fill in the msg header */
    msg->type = (positive) ? CMS_MSG_REGISTER_EVENT_INTEREST : CMS_MSG_UNREGISTER_EVENT_INTEREST;
    msg->src = EID_SNTP;
    msg->dst = EID_SMD;
    msg->flags_request = 1;
    msg->wordData = msgType;

    if (msgData != NULL && msgDataLen != 0)
    {
        data = (char *) (msg + 1);
        msg->dataLength = msgDataLen;
        memcpy(data, (char *)msgData, msgDataLen);
    }      

    ret = cmsMsg_sendAndGetReply(sntp_msg_hndl, msg);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("%s_EVENT_INTEREST for 0x%x failed, ret=%d", action, msgType, ret);
    }
    else
    {
        cmsLog_debug("%s_EVENT_INTEREST for 0x%x succeeded", action, msgType);
    }

    cmsMem_free(msgBuf);
}


static void sntp_stop_handler(int signal __attribute__ ((unused)))
{
    cmsLog_notice("SNTP is received signal to terminate");
    srv_exit_by_signal = TRUE;
}


static int sntp(char *ntpserver, int tz_idx)
{
  struct hostent *h;
  char **clist;
  int s = 0;
  struct sockaddr_in to;
  unsigned int reply[12]={0};
  int rep_siz;
  char msg[48];
  int retval;
  static int errHide= 0;
  struct timeval ntp_time;
  struct pollfd poll_fd;
  long gmtime;

  memset( msg, 0, 48 );
  msg[0] = 0x1b;
  memset(&to, 0, sizeof(struct sockaddr_in));
  memset(&ntp_time, 0, sizeof(struct timeval));
  memset(&poll_fd, 0, sizeof(struct pollfd));

  h = gethostbyname( ntpserver );

  if( !h ) {
    if ( errHide == 0 ) {
      switch( h_errno ) {
        case HOST_NOT_FOUND:
          cmsLog_notice("sntp: host not found.");
          break;
        case NO_ADDRESS:
          cmsLog_notice("sntp: no IP addresses for that record.");
          break;
        case NO_RECOVERY:
          cmsLog_notice("sntp: non recoverable name server error.");
          break;
        case TRY_AGAIN:
          cmsLog_notice("sntp: try again later.");
          break;
        default:
          cmsLog_notice("sntp: unknown error (%d).", h_errno);
      }
      errHide++;
    }
    return TIME_STATE_ERROR;
  }

  errHide = 0;

  if( h->h_addrtype == AF_INET ) {
    to.sin_port = htons( 123 );
    to.sin_family = AF_INET;
    if( ( s = socket( PF_INET, SOCK_DGRAM, 0 ) ) == -1 )
      perror( "socket" );
    poll_fd.fd = s; /* We'll be polling shortly */
    poll_fd.events = POLLIN;
    for( clist = h->h_addr_list; *clist != NULL; clist++ ) {
      memcpy( (char *) &to.sin_addr.s_addr, *clist, h->h_length );
      retval =  sendto( s, msg, sizeof( msg ), 0, (struct sockaddr *) &to, sizeof( to ) );
      if( retval == -1 )
        perror( "sendto" );
      if( poll( &poll_fd, 1, 1000 ) ) {
        rep_siz = recv( s, reply, sizeof( reply ), 0 );
        if( rep_siz == sizeof( reply ) ) {
          long delta;
          gmtime = ntohl( reply[10] ) - TIME1970;
          /*      fprintf( stderr, "offset_secs == %d\n", get_offset( gmtime, tz_idx )); */
          
          delta = gmtime - time(0) + get_offset( gmtime, tz_idx );
          /*  When WAN connection is up first time, the gmtime is the real gm time from internet in seconds
          * and time(0) is just 40 to 54 seconds of the modem time since boots and delta is huge for the first time 
          * (something like 1297129698.  After the modem has done a set with settimeofday( &ntp_time, 0 ), 
          * the subsequent delta will almost always be 0 with occationally 1. So in all case, sendEventMEssage which
          * messages dhcpd to update its time will be used only at first time.
          */
          
          if (delta > 30)
              sntp_send_message_to_dhcpd(delta);
          ntp_time.tv_sec = gmtime + get_offset( gmtime, tz_idx );
          ntp_time.tv_usec = 0;
          settimeofday( &ntp_time, 0 );
          if ( s >= 0 )
             close(s);
          return TIME_STATE_SYNCHRONIZED;
        }
      }
    }
  }

  if ( s >= 0 )
    close(s);

  return TIME_STATE_FAIL_TO_SYNCHRONIZE;
}


static UINT32 sntp_check_status(void)
{
    SINT32 i = 0;
    SINT32 waitTime = SNTP_WAIT_TIMEOUT_IN_SECS;
    int status = TIME_STATE_UNSYNCHRONIZED;

    if (srv_stop == FALSE)
    {
        for (i = 0; i < srv_idx; i++) 
        {
            if ((status = sntp(servers[i], tz_idx)) == TIME_STATE_SYNCHRONIZED) 
            {
                waitTime = SNTP_DELAY_TIMEOUT_IN_SECS;
                break;
            }
        }

        if (status != currentSntpState)
        {
            currentSntpState = status;
            sntp_send_message_to_smd(currentSntpState);
        }
    }
    else
    {
        cmsLog_notice("SNTP is disabled");
        waitTime = SNTP_DELAY_TIMEOUT_IN_SECS;
    }

    return waitTime;
}


static int sntp_msg_handler(CmsMsgHeader *msg)
{
    int rv = 0;
    CmsRet ret = CMSRET_SUCCESS;

    switch (msg->type)
    {
        case CMS_MSG_WAN_CONNECTION_UP:
            sntp_check_status();
            break;

        case CMS_MSG_STOP_APP:
            srv_stop = TRUE;
            break;

        case CMS_MSG_TERMINATE:
            cmsLog_notice("SNTP is received message to terminate");
            srv_exit_by_message = TRUE;
            // send reply msg to acknowledge
            msg->wordData = CMSRET_SUCCESS;
            if ((ret = cmsMsg_sendReply(sntp_msg_hndl, msg, CMSRET_SUCCESS)) != CMSRET_SUCCESS)
            {
               cmsLog_error("send response for msg 0x%x failed, ret=%d", msg->type, ret);
            }
            break;

#ifdef CMS_MEM_LEAK_TRACING
        case CMS_MSG_MEM_DUMP_TRACEALL:
            cmsMem_dumpTraceAll();
            break;

        case CMS_MSG_MEM_DUMP_TRACE50:
            cmsMem_dumpTrace50();
            break;

        case CMS_MSG_MEM_DUMP_TRACECLONES:
            cmsMem_dumpTraceClones();
            break;
#endif

        default:
            cmsLog_notice("Invalid message type (%x)", (unsigned int)msg->type);
            // make sure we don't bomb out for this reason.
            rv = 0;
    }

    return rv;
}


static int sntp_stack(void)
{
    int rv = 0;
    UINT32 counter = 0, waitTime = 0;
    CmsLogLevel logLevel = cmsLog_getLevel();
    CmsRet ret = CMSRET_SUCCESS;
    SINT32 commFd = 0;
    fd_set readFds;
    CmsMsgHeader *msg = NULL;
    struct timeval tm;

    cmsLog_setHeaderMask(0);

    cmsLog_setLevel(LOG_LEVEL_NOTICE);
    cmsLog_notice("Broadcom SNTP Stack v%s (%s, %s)", SNTP_VERSION,
        __DATE__, __TIME__);
    cmsLog_setLevel(logLevel);

    cmsLog_setHeaderMask(DEFAULT_LOG_HEADER_MASK);

    /* initialize timer events  */
    if (cmsTmr_getNumberOfEvents(sntp_tmr_hndl) > 0)
    {
        cmsTmr_dumpEvents(sntp_tmr_hndl);
    }

    /* get the CMS messaging handle */
    cmsMsg_getEventHandle(sntp_msg_hndl, &commFd);

    waitTime = sntp_check_status();

    while (!rv &&
           srv_exit_by_message == FALSE &&
           srv_exit_by_signal == FALSE)
    {
        tm.tv_sec = SNTP_SCHED_TIMEOUT_IN_SECS;
        tm.tv_usec = 0;

        /* set up all the fd stuff for select */
        FD_ZERO(&readFds);
        FD_SET(commFd, &readFds);

        rv = select(commFd+1, &readFds, NULL, NULL, &tm);
        if (rv < 0)
        {
            /* interrupted by signal or something, continue */
            rv = 0;

            continue;
        }

        /*
        * service all timer events that are due (there may be no events due
        * if we woke up from select because of activity on the fds).
        */
        cmsTmr_executeExpiredEvents(sntp_tmr_hndl);

        if ((++counter * SNTP_SCHED_TIMEOUT_IN_SECS) >= waitTime)
        {
            waitTime = sntp_check_status();
            counter = 0;
        }

        if (FD_ISSET(commFd, &readFds))
        {
            if ((ret = cmsMsg_receiveWithTimeout(sntp_msg_hndl, &msg, 1000)) != CMSRET_SUCCESS)
            {
                if (!cmsFil_isFilePresent(SMD_SHUTDOWN_IN_PROGRESS))
                {
                    cmsLog_error("Failed to receive message (ret=%d)", ret);
                }
                rv = -1;
            }
            else
            {
                rv = sntp_msg_handler(msg);
                CMSMEM_FREE_BUF_AND_NULL_PTR(msg);
            }
        }
    }

    printf("\n%s: EXITING (%d)\n\n", __FUNCTION__, rv);

    return rv;
}


static CmsRet sntp_cms_init(const CmsEntityId entityId)
{
    SINT32 sessionPid = 0, shmId = 0;
    CmsRet ret = CMSRET_SUCCESS;
    CmsMsgHeader *buf = NULL;

    /*
    * Detach myself from the terminal so I don't get any control-c/sigint.
    * On the desktop, it is smd's job to catch control-c and exit.
    * When omcid detects that smd has exited, omcid will also exit.
    */
    if ((sessionPid = setsid()) == -1)
    {
        cmsLog_error("Could not detach from terminal");
    }
    else
    {
        cmsLog_debug("Detached from terminal");
    }

    if ((ret = cmsMsg_init(entityId, &sntp_msg_hndl)) != CMSRET_SUCCESS)
    {
        cmsLog_error("msg initialization failed, ret=%d", ret);
        return ret;
    }

    ret = cmsMsg_receiveWithTimeout(sntp_msg_hndl, &buf, 100);
    if(ret == CMSRET_SUCCESS)
    {
        CMSMEM_FREE_BUF_AND_NULL_PTR(buf);
    }

    if ((ret = cmsMdm_init(entityId, sntp_msg_hndl, &shmId)) != CMSRET_SUCCESS)
    {
        cmsLog_error("cmsMdm_init failed, ret=%d", ret);
        return ret;
    }

    cmsLog_notice("cmsMdm_init successful, shmId=%d", shmId);

    if ((ret = cmsTmr_init(&sntp_tmr_hndl)) != CMSRET_SUCCESS)
    {
        cmsLog_error("cmsTmr_init failed, ret=%d", ret);
        return ret;
    }

    cmsLog_notice("cmsTmr_init successful");

    // Register SNTP interest events with smd.
    sntp_register_event(CMS_MSG_WAN_CONNECTION_UP, TRUE, NULL, 0);

    return ret;
}


static void sntp_cms_cleanup(void)
{
    // Unregister SNTP interest events with smd.
    // but not for termination by CMS_MSG_TERMINATE
    // to avoid error message on console
    if (srv_exit_by_message == FALSE)
    {
        sntp_register_event(CMS_MSG_WAN_CONNECTION_UP, FALSE, NULL, 0);
    }

    cmsTmr_cleanup(&sntp_tmr_hndl);
    cmsMdm_cleanup();
    cmsMsg_cleanup(&sntp_msg_hndl);
}


static SINT32 sntp_init(const CmsEntityId entityId)
{
    SINT32 rv = 0, i = 0;
    CmsRet ret = CMSRET_SUCCESS;

    /* Ignore broken pipes */
    signal(SIGPIPE, SIG_IGN);

    signal(SIGKILL, &sntp_stop_handler);
    signal(SIGHUP, &sntp_stop_handler);
    signal(SIGTERM, &sntp_stop_handler);
    signal(SIGQUIT, &sntp_stop_handler);

    /* initialize the CMS framework */
    if ((ret = sntp_cms_init(entityId)) == CMSRET_SUCCESS)
    {
        for (i = 0; i < SERVER_MAX; i++)
        {
            servers[i] = NULL;
        }
    }
    else
    {
        cmsLog_error("CMS initialization failed (%d), exit.", ret);
        rv = -1;
    }

    return rv;
}


static void sntp_exit(void)
{
    UINT32 i = 0;

    for (i = 0; i < SERVER_MAX; i++)
    {
        CMSMEM_FREE_BUF_AND_NULL_PTR(servers[i]);
    }

    /* clean CMS resources */
    sntp_cms_cleanup();
}


#ifdef BUILD_STATIC
int sntpd_main(int argc, char **argv) {
#else
int main(int argc, char **argv ) {
#endif	
    int rv = 0, i = 0;
    char zone[BUFLEN_1024];
    SINT32 c;
    CmsEntityId entityId = EID_SNTP;

    cmsLog_init(entityId);

    if (sntp_init(entityId) == -1) 
    {
        cmsLog_cleanup();
        exit(-1);
    }

    zone[0] = '\0';

    while ((c= getopt(argc, argv, "s:t:")) != -1)
    {
        switch (c)
        {
            case 's':
                if ( srv_idx < SERVER_MAX)
                {
                    CMSMEM_REPLACE_STRING(servers[srv_idx], optarg);
                    srv_idx++;
                }
                break;
            case 't':
                // append non-option argument to create zone
                for (i = optind - 1; i < argc; i++) 
                {
                    strcat(zone, argv[i]);
                    strcat(zone, " ");
                }
                // remove the last blank character
                zone[strlen(zone)-1] = '\0';
                for (i = 0; timeZones[i] != NULL; i++)
                {
                    if (strcasestr(timeZones[i], zone) != NULL) 
                    {
                        tz_idx = i;
                        break;
                    }            
                }
                if(tz_idx == -1) 
                {
                    cmsLog_error("Bad arguments %s for option t", optarg);
                }
                break;
            default:
                cmsLog_error("Unknown option %c", c);
        }
    }

    if (srv_idx == 0) 
    {
        cmsLog_error("No servers specified.");
        cmsLog_cleanup();
        exit(-1);
    }

    if(tz_idx == -1) 
    {
        cmsLog_error("No timezone specified.");
        cmsLog_cleanup();
        exit(-1);
    }

    /* sntp main while loop */
    rv = sntp_stack();

    sntp_exit();

    cmsLog_cleanup();

    return rv;
}

