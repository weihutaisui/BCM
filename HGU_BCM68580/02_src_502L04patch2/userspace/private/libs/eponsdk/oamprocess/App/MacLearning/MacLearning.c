/*
* <:copyright-BRCM:2013:proprietary:epon
* 
*    Copyright (c) 2013 Broadcom 
*    All Rights Reserved
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
*/

////////////////////////////////////////////////////////////////////////////////
/// \file MacLearning.c
/// \brief Generic interface for MAC address learning configuration
/// \author Jason Armstrong
/// \date August 31, 2010
///
/// This module is a generic interface for configuration of MAC address
/// learning.  Any module implementing this interface needs to implement every
/// function defined within.  Storage of provisioned values is up to the
/// implementor.
////////////////////////////////////////////////////////////////////////////////
#include <string.h>
#include <errno.h>
#include <linux/sockios.h>
#include <sys/ioctl.h>

#include "cms_log.h"
#include "Teknovus.h"
#include "Ethernet.h"
#include "PortCapability.h"
#include "VlanConfig.h"
#include "MacLearning.h"

#include <net/if.h>
#include <bcm_local_kernel_include/linux/if_bridge.h>
#include <bridgeutil.h>

#define MacLearnSaSearch        0
#define MacLearnDaSearch        1
#define MacLearnSrchUpdateMap   1
#define MacLearnAgeTime         15      /* 2^15*10ms, approximately 5:46 */
#define MacLearnPriority        3
#define MacLearnDropVid         0xFFF

#define MirrorPriority 1 /* warning!, should define the same as value defined in Mirror.c */
#define MacLocalSwitchPriority (MirrorPriority+1)/* less than Mirror priority */
#define MacFloodPriority        2

#define MacLearnOnuMacDef       2048
#define MacLearnMaxMacDef       512
#define MacLearnMinMacDef       64

#define MacLearnCtcOamDefaultMacAgingTime      0x0000012c
#define MacLearnCtcAgingTimeFakeVal1 14
#define MacLearnCtcAgingTimeFakeVal2 15

#define MAC_LMT_PER_PORT    1
#define MAC_LMT_PER_ONU     0


////////////////////////////////////////////////////////////////////////////////
/// \brief Delete an entry from the learning table
///
///
/// \param port
/// \param entry 
/// \param type
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void MacLearnDelEntry (const char *bridge, 
                       const char *ifName,
                       const MacAddr *pMac)
{
    int rc = br_util_del_fdb_entry(ifName, pMac->mac);
    if ( rc != 0 )
    {
        printf("MacLearnDelEntry: unable to delete entry\n");
    }
}/* MacLearnDelEntry */



////////////////////////////////////////////////////////////////////////////////
/// \brief Add entry to learning/forwarding tables
///
///
/// \param port
/// \param entry 
/// \param type
///
/// \return
/// TRUE if add successful
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL MacLearnAddEntry (const char *bridge, 
                             const char *ifName,
                             const MacAddr *pMac,
                             EntryType et)
{
   int rc;
   if ( et == EntryTypeStatic )
   {
      rc = br_util_add_fdb_entry(ifName, pMac->mac, BR_UTIL_FDB_TYPE_STATIC);
   }
   else
   {
      rc = br_util_add_fdb_entry(ifName, pMac->mac, BR_UTIL_FDB_TYPE_DYNAMIC);
   }
   if ( rc == 0 )
   {
      return TRUE;
   }
   return FALSE;
} /* MacLearnAddEntry */


////////////////////////////////////////////////////////////////////////////////
/// MacLearnClearAllDyn - Clear all dynamic entries for a port
///
/// This function deletes all dynamically learned entries for a port.  Static or
/// blocked entries will be left alone.
///
 // Parameters:
/// \param port User port number
///
/// \return
/// TRUE/FALSE
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL MacLearnClearAll (const char *bridge, 
                           const char *ifName,
                           BOOL isStatic)
{
   int rc;
   if ( isStatic )
   {
      rc = br_util_flush_fdb(bridge, ifName, BR_UTIL_FDB_TYPE_STATIC);
   }
   else
   {
      rc = br_util_flush_fdb(bridge, ifName, BR_UTIL_FDB_TYPE_DYNAMIC);
   }
   if (rc == 0 )
   {
      return TRUE;
   }
   return FALSE;
} /* MacLearnClearAllDyn */

static BOOL isStaticMac(struct __fdb_entry *fdb)
{
    BOOL ret = FALSE;
    if(fdb->is_local == 0)
    {
        if(fdb->ageing_timer_value == 0)
        {
            ret = TRUE;
        }
    }
    return ret;
}

static BOOL isDynamicMac(struct __fdb_entry *fdb)
{
    BOOL ret = FALSE;
    if (fdb->is_local == 0)
    {
        if (fdb->ageing_timer_value != 0)
        {
            ret = TRUE;
        }
    }
    return ret;
}

////////////////////////////////////////////////////////////////////////////////
static char * StrMacAddr (char * pMacAddrStr, const MacAddr * pMacAddr)
    {
    sprintf(pMacAddrStr, "%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x",
            pMacAddr->u8[0],
            pMacAddr->u8[1],
            pMacAddr->u8[2],
            pMacAddr->u8[3],
            pMacAddr->u8[4],
            pMacAddr->u8[5]);
    return (pMacAddrStr);
    } /* StrMacAddr */

////////////////////////////////////////////////////////////////////////////////
U16 MacLearnLrnTblGetByType(char *brname,
                            Port port,
                            U16 curOffset, 
                            MacAddr *mac, 
                            EntryType type)
    {    
    struct          __fdb_entry fdb;
    char            ifName[10]={0};
    int             port_no;
    U16             n;
    U16             newOffset = curOffset;

    if(port.pift == PortIfPon)
        {
        sprintf(ifName, "epon%d.1", port.inst);
        }
    else
        {
        sprintf(ifName, "eth%d.0", port.inst);
        }

    port_no = br_util_get_port_number(ifName);
    if(port_no < 0)
        {
        printf("interface error\n");
        return MacEntryInvalidIndex;
        }

    
    for (;;)
        {       
        char macStr[MacAddrStrSize];        
        
        memset(&fdb, 0, sizeof(fdb));
        n = br_util_read_fdb(brname, &fdb, newOffset, 1);
        if (n == 0)
            {
            newOffset = MacEntryInvalidIndex;
            break;
            }

        if (n < 0)
            {
            cmsLog_error("read of forward table failed: %s\n",
                strerror(errno));
            newOffset =  MacEntryInvalidIndex;
            break;
            }

        StrMacAddr(&macStr[0], (MacAddr *)&fdb.mac_addr);

        if(port_no != fdb.port_no)
            {
            newOffset += n;            
            }
        else if(isStaticMac(&fdb) && (type == EntryTypeStatic))
            {
            break;
            }
        else if (isDynamicMac(&fdb) && (type == EntryTypeDynamic))
            {
            break;
            }
        else
            {
            newOffset += n;            
            }
        }
    
    if(newOffset != MacEntryInvalidIndex)
        {
        memcpy(mac, &fdb.mac_addr, sizeof(MacAddr));
        newOffset++; /* pointer to next item */
        }

    return newOffset;
    }

/* Get MAC limit 
 *  paras:
 * brname - bridge name
 * lmtType - 0: bridge, 1: port 
 * port -  port index
 * isMin - min limit
 *
 * return
 * TRUE/FALSE
 * *maclimit
 */
static BOOL MacLimitGet(char *brname, U16 lmtType, TkOnuEthPort port, 
                        U16 *macLimit, U16 isMin)
{
    U32  fdb_lmt;
    char ifname[10]={0};

    if(lmtType == MAC_LMT_PER_PORT)
    {
        sprintf(ifname, "eth%d.0", port);
        fdb_lmt = br_util_get_fdb_limit(ifname, 
            isMin ? BR_UTIL_LIMIT_TYPE_MIN : BR_UTIL_LIMIT_TYPE_MAX);
    }
    else
    {
        fdb_lmt = br_util_get_fdb_limit(brname,
            isMin ? BR_UTIL_LIMIT_TYPE_MIN : BR_UTIL_LIMIT_TYPE_MAX);
    }

    if ( fdb_lmt < 0 )
    {
        return FALSE;
    }

    *macLimit = (U16)fdb_lmt;

    return TRUE;
}

/* Set MAC limit 
 *  paras:
 * brname - bridge name
 * lmtType - 0: bridge, 1: port 
 * port -  port index
 * macLimit - MAC limit
 * isMin - min limit
 *
 * return
 * TRUE/FALSE
 */
static BOOL MacLimitSet(char *brname, U16 lmtType, TkOnuEthPort port,
                        U16 macLimit, U16 isMin)
{
    char ifname[10]={0};
    int  err;

    if (macLimit > MAX_MAC_ENTRIES_NUM)
    {
        return FALSE;
    }
    
    if(lmtType == MAC_LMT_PER_PORT)
    {
        /* Set Port limit */
        sprintf(ifname, "eth%d.0", port);
        err = br_util_set_fdb_limit(brname, ifname, isMin ? 1 : 2, macLimit);
    }
    else
    {
        err = br_util_set_fdb_limit(brname, NULL, 2, macLimit);
    }

    return ((err < 0) ? FALSE : TRUE);
}

////////////////////////////////////////////////////////////////////////////////
//extern
BOOL MacLearnGetMaxLimit (char *brname, TkOnuEthPort port, U16 *max)
    {
    return MacLimitGet(brname, MAC_LMT_PER_PORT, port, max, 0);
    }

////////////////////////////////////////////////////////////////////////////////
//extern
BOOL MacLearnSetMaxLimit (char *brname, TkOnuEthPort port, U16 max)
    {
    return MacLimitSet(brname, MAC_LMT_PER_PORT, port, max, 0);
    }

////////////////////////////////////////////////////////////////////////////////
//extern
BOOL MacLearnGetMinLimit (char *brname, TkOnuEthPort port, U16 *min)
    {
    return MacLimitGet(brname, MAC_LMT_PER_PORT, port, min, 1);
    }

////////////////////////////////////////////////////////////////////////////////
//extern
BOOL MacLearnSetMinLimit (char *brname, TkOnuEthPort port, U16 min)
    {
    return MacLimitSet(brname, MAC_LMT_PER_PORT, port, min, 1);
    }

////////////////////////////////////////////////////////////////////////////////
//extern
BOOL MacLearnGetOnuLimit (char *brname, U16 *limit)
    {
    return MacLimitGet(brname, MAC_LMT_PER_ONU, 0, limit, 0);
    }

////////////////////////////////////////////////////////////////////////////////
//extern
BOOL MacLearnSetOnuLimit (char *brname, U16 limit)
    {
    return MacLimitSet(brname, MAC_LMT_PER_ONU, 0, limit, 0);
    }

////////////////////////////////////////////////////////////////////////////////
//extern
BOOL MacLearnSetAgeingTime (char *brname, U32 ageingTime)
    {
    FILE* f = NULL;
    char path[BR_SYSFS_PATH_MAX];

    snprintf(path, BR_SYSFS_PATH_MAX, BR_SYSFS_CLASS_NET"%s/bridge/ageing_time", brname);
        
    f = fopen(path, "w");
    if (f == NULL)
        {
        return FALSE;
        }

    fprintf(f, "%u", ageingTime);
    fclose(f);
    return TRUE;
    }

////////////////////////////////////////////////////////////////////////////////
//extern
BOOL MacLearnGetAgeingTime (char *brname, U32 *ageingTime)
    {
    FILE* f = NULL;
    char path[BR_SYSFS_PATH_MAX];

    snprintf(path, BR_SYSFS_PATH_MAX, BR_SYSFS_CLASS_NET"%s/bridge/ageing_time", brname);
        
    f = fopen(path, "r");
    if (f == NULL)
        {
        return FALSE;
        }

    fscanf(f, "%u", ageingTime);
    fclose(f);
    return TRUE;
    }

////////////////////////////////////////////////////////////////////////////////
//extern
Bool MacLearnSearchSaPort (char *brname, const MacAddr * mac, U8 * port)
{
    struct          __fdb_entry fdb;
    U16             n;
    U16             newOffset = 0;

    for (;;)
    {       
        memset(&fdb, 0, sizeof(fdb));
        n = br_util_read_fdb(brname, &fdb, newOffset, 1);

        if (n == 0)
        {
            break;
        }

        if (0 == memcmp(mac, (MacAddr *)&fdb.mac_addr, sizeof(MacAddr)))
        {
            *port = fdb.port_no;
            return TRUE;
        }

        newOffset += n;
    }

    return FALSE;
}

/* End of file MacLearning.c */

