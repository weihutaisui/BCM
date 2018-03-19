/***********************************************************************
 *
 *  Copyright (c) 2007  Broadcom Corporation
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <getopt.h>

#include "os_defs.h"
#include "gponctl_api.h"

static const char *usage_msg =
"Usage: gpond {-o|--omci}\n"
"       gponif {-h|--help} \n";

struct option longopts[] = {
    /* { name  has_arg, *flag  val } */
    {"omci",   0, 0, 'o'}, /* Enable OMCI event reporting */
    {"help",  0, 0, 'h'}, /* Give help */
    {0, 0, 0, 0}
};

static int readn(int filed, uint8_t* buf, size_t nbyte) {
  int rc;

  while (nbyte) {
    if ((rc = read(filed, buf, nbyte)) < 0) {
      perror("read error");
      return rc;
    }

    buf += rc;
    nbyte -= rc;
  }

  return 0;
}

#ifdef SUPPORT_GPONCTL
static void logAlarm(BCM_Ploam_AlarmStatusInfo* ploamAlarms) {
  /*                    "A L L L L S S S M D D P R\n" */
  char statusString[] = "                           ";
  char eventString[] =  "                           ";
  int i;
  uint32_t alarmStatusBitmap = ploamAlarms->alarmStatusBitmap;
  uint32_t alarmEventBitmap = ploamAlarms->alarmEventBitmap;

  for (i=0; i<BCM_PLOAM_NUM_ALARMS; ++i) {
    statusString[i*2] = (alarmStatusBitmap&(1<<i)) !=0 ? '1' : '0';
    eventString[i*2] = (alarmEventBitmap&(1<<i)) !=0 ? '1' : '0';
  }

  printf("gpond: Alarm:  A L L L L S S S M D D P R\n");
  printf("gpond:         P O O O O F D U E A I E D\n");
  printf("gpond:         C S L F G     F M C S E I\n");
  printf("gpond:         -------------------------\n");
  printf("gpond: Status: %s\n", statusString);
  printf("gpond: Event:  %s\n", eventString);
}

static void ploamEventHandler(int ploamFd) {
  BCM_Ploam_EventStatusInfo eventStatusInfo;
  uint32_t maskedEventBitmap;
  int rc;

  if ((rc=gponCtl_getEventStatus(&eventStatusInfo))!=0) {
    fprintf(stderr, "gpond: gponCtl_getEventStatus returned error code %d\n", rc);
    return;
  }

  printf("gpond: ploamEvent: eventBitmap=0x%x, eventMask=0x%x\n",
         eventStatusInfo.eventBitmap, eventStatusInfo.eventMask);

  maskedEventBitmap = (eventStatusInfo.eventBitmap & ~eventStatusInfo.eventMask);

  if (maskedEventBitmap & BCM_PLOAM_EVENT_ONU_STATE_CHANGE) {
    BCM_Ploam_StateInfo stateInfo;

    printf("gpond: ploamEvent: BCM_PLOAM_EVENT_ONU_STATE_CHANGE\n");

    if ((rc=gponCtl_getControlStates(&stateInfo))!=0) {
      fprintf(stderr, "gponCtl_getControlStates returned error code %d\n", rc);
      return;
    }

    printf("gpond: adminState: %s, operState O%d\n",
           (stateInfo.adminState == BCM_PLOAM_ASTATE_ON) ? "On" : "Off",
           stateInfo.operState);
  }

  if (maskedEventBitmap & BCM_PLOAM_EVENT_ALARM) {
    BCM_Ploam_AlarmStatusInfo alarmStatusInfo;

    printf("gpond: ploamEvent: BCM_PLOAM_EVENT_ALARM\n");

    if ((rc=gponCtl_getAlarmStatus(&alarmStatusInfo))!=0) {
      fprintf(stderr, "gpond: gponCtl_getAlarmStatus returned error code %d\n", rc);
      return;
    }

    logAlarm(&alarmStatusInfo);
  }

  if (maskedEventBitmap & BCM_PLOAM_EVENT_OMCI_PORT_ID) {
    BCM_Ploam_OmciPortInfo omciPortInfo;

    printf("gpond: ploamEvent: BCM_PLOAM_EVENT_OMCI_PORT_ID\n");

    if ((rc=gponCtl_getOmciPort(&omciPortInfo))!=0) {
      fprintf(stderr, "gpond: gponCtl_getOmciPort returned error code %d\n", rc);
      return;
    }

    printf("gpond: OMCI port %s, %s encrypted\n",
           omciPortInfo.omciGemPortActivated ? "activated" : "deactivated",
           omciPortInfo.encrypted ? "" : "not");
    printf("gpond: OMCI port ID: 0x%x\n", omciPortInfo.omciGemPortID);
  }

  if (maskedEventBitmap & BCM_PLOAM_EVENT_USER_PLOAM_RX_OVERFLOW) {
    printf("gpond: ploamEvent: BCM_PLOAM_EVENT_USER_PLOAM_RX_OVERFLOW\n");
  }

  if (maskedEventBitmap & BCM_PLOAM_EVENT_USER_PLOAM_TX_OVERFLOW) {
    printf("gpond: ploamEvent: BCM_PLOAM_EVENT_USER_PLOAM_TX_OVERFLOW\n");
  }

  if (maskedEventBitmap & BCM_PLOAM_EVENT_GEM_STATE_CHANGE) {
    printf("gpond: ploamEvent: BCM_PLOAM_EVENT_GEM_STATE_CHANGE\n");
  }
}
#endif

static int userPloamEventHandler(int usrPloamFd) {
  BCM_Ploam_Msg msg;
  int i;

  printf("gpond: usrPloamEvent:\n");

  if (readn(usrPloamFd, (uint8_t*)&msg, sizeof(msg)) < 0)
   return -1;

  printf("gpond: Ploam: seqNum: 0x%x, direction: 0x%x, onu_id: 0x%x, msg_id: 0x%x\n",
         msg.seqNum, msg.direction, msg.onu_id, msg.msg_id);
  printf("gpond: ");
  for (i=0; i<10; i++) {
    printf("%2.2x ", msg.payload[i]);
  }

  printf("\ngpond: ---EOM---\n");

  return 0;
}

static int omciEventHandler(int omciFd) {
  uint8_t msg[BCM_OMCI_RX_MSG_MAX_SIZE_BYTES];
  uint32_t msgLen;
  int res;
  int i;

  printf("gpond: omciEvent:\n");

  res = read(omciFd, msg, sizeof(msg));

  if (res < 0) {
    fprintf(stderr, "read error: 0x%x", res);
    return 0;
  }

  msgLen = res;

  printf("gpond: Message Length: %d\n", msgLen);
  printf("gpond: ");

  for (i=0; i<msgLen; i++) {
    printf("%2.2x ", msg[i]);
  }

  printf("\ngpond: ---EOM---\n");

  return 0;
}

#ifdef BUILD_STATIC
int gpond_main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
  int omci=0;
  int omciFd=0;
  int ploamFd=0;
  int usrPloamFd=0;
  int maxFd;
  fd_set workingSet;
  int c;

  while ((c = getopt_long(argc, argv, "oh",
           longopts, 0)) != EOF) {
    switch (c) {
      case 'o':
        omci=1;
        break;
      case 'h':
      default:
        fprintf(stderr, usage_msg);
        return -2;
    }
  }

  if ((ploamFd = open("/dev/" BCM_PLOAM_DEVICE_NAME, O_RDONLY)) < 0) {
    perror("file open error");
    goto fail_1;
  }

  if ((usrPloamFd = open("/dev/" BCM_PLOAM_USR_DEVICE_NAME, O_RDONLY)) < 0) {
    perror("file open error");
    goto fail_2;
  }

  if (omci) {
    if ((omciFd = open("/dev/" BCM_OMCI_DEVICE_NAME, O_RDONLY)) < 0) {
      perror("file open error");
      goto fail_3;
    }
  }

  maxFd = (ploamFd > usrPloamFd) ? ploamFd : usrPloamFd;

  if (omci)
    maxFd = (omciFd > maxFd) ? omciFd : maxFd;

  while (1) {
    int rc;

    FD_ZERO(&workingSet);
    FD_SET(ploamFd, &workingSet);
    FD_SET(usrPloamFd, &workingSet);

    if (omci)
      FD_SET(omciFd, &workingSet);

    //printf("calling select...\n");

    rc = select(maxFd+1, &workingSet, NULL, NULL, NULL);

    //printf("select returned, rc=0x%x\n",rc);

    if (rc) {
#ifdef SUPPORT_GPONCTL
      if (FD_ISSET(ploamFd, &workingSet))
        ploamEventHandler(ploamFd);
#endif
      if (FD_ISSET(usrPloamFd, &workingSet))
        if (userPloamEventHandler(usrPloamFd))
          goto fail_3;

      if (omci)
        if (FD_ISSET(omciFd, &workingSet))
          if (omciEventHandler(omciFd))
            goto fail_3;
    }
  }

  if (omci)
    close(omciFd);

  fail_3:
  close(usrPloamFd);

  fail_2:
  close(ploamFd);

  fail_1:
  return -1;
}

