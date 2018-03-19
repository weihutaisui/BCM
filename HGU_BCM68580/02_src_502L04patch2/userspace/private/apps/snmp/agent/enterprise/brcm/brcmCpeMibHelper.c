#ifdef BRCM_CPE_MIB

/***************************************************************************
** <:copyright-BRCM:2005:proprietary:standard
** 
**    Copyright (c) 2005 Broadcom 
**    All Rights Reserved
** 
**  This program is the proprietary software of Broadcom and/or its
**  licensors, and may only be used, duplicated, modified or distributed pursuant
**  to the terms and conditions of a separate, written license agreement executed
**  between you and Broadcom (an "Authorized License").  Except as set forth in
**  an Authorized License, Broadcom grants no license (express or implied), right
**  to use, or waiver of any kind with respect to the Software, and Broadcom
**  expressly reserves all rights in and to the Software and all intellectual
**  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
**  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
**  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
** 
**  Except as expressly set forth in the Authorized License,
** 
**  1. This program, including its structure, sequence and organization,
**     constitutes the valuable trade secrets of Broadcom, and you shall use
**     all reasonable efforts to protect the confidentiality thereof, and to
**     use this information only in connection with your use of Broadcom
**     integrated circuit products.
** 
**  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
**     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
**     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
**     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
**     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
**     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
**     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
**     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
**     PERFORMANCE OF THE SOFTWARE.
** 
**  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
**     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
**     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
**     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
**     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
**     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
**     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
**     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
**     LIMITED REMEDY.
 * :>
 *
 * $brcm_Workfile:$
 * $brcm_Revision:$
 * $brcm_Date:$
 *
 * [File Description:]
 *      CPE DSL MIB handler helper functions.
 * Revision History:
 *      Create 6/2005   yt
 * $brcm_Log:$
 *
 ***************************************************************************/

/* General includes */
#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* SNMP includes */
#include "asn1.h"
#include "snmp.h"
#include "agtMib.h"
#include "agtEngine.h"
#include "brcmCpeMib.h"
#include "syscall.h"

#ifdef BUILD_SNMP_DEBUG
#include "snmpDebug.h"
extern SNMP_DEBUG_FLAG snmpDebug;
#endif

/* TBD: Stub out function */
void bcmSystemMute(char *s) {};

int brcmCpeGetWirelessTransceiver(void) 
{
  FILE *fs;
  char line[SYS_CMD_LEN];
  int result = BRCM_CPE_WIRELESS_TRANSCEIVER_OFF;

  bcmSystemMute("wlctl isup > /var/wlResult");

  fs = fopen("/var/wlResult", "r");
  if (fs != NULL) {
    /* isup is 1 (on)    OR    isup is 0 (off) */
    if (fgets(line, SYS_CMD_LEN, fs) != NULL) {
      if (strstr(line,"on") != NULL) {
	result = BRCM_CPE_WIRELESS_TRANSCEIVER_ON;
      }
    }
    close(fs);
  }
  if (access("/var/wlResult",F_OK) == 0)
    bcmSystemMute("rm /var/wlResult");

  return result;
}

void brcmCpeSetWirelessTransceiver(int mode) 
{
  if (mode == BRCM_CPE_WIRELESS_TRANSCEIVER_OFF)
    bcmSystemMute("wlctl down");
  else
    bcmSystemMute("wlctl up");
}

#endif /* BRCM_CPE_MIB */
