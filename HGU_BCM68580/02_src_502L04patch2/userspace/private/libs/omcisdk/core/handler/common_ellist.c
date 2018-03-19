/***********************************************************************
 *
 *  Copyright (c) 2015 Broadcom
 *  All Rights Reserved
 *
<:label-BRCM:2015:proprietary:omcid

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

/*****************************************************************************
*    Description:
*
*      Extended common linked list utility.
*      Note: the user is responsible for entry memory alloc and free.
*
*****************************************************************************/

/* ---- Include Files ----------------------------------------------------- */

#include "common_ellist.h"
#include "owsvc_api.h"

#define CC_ELL_DEBUG 1

/* ---- Private Constants and Types --------------------------------------- */

#if defined(CC_ELL_DEBUG)
#define ell_debug(fmt, arg...) \
  printf("%s.%u: " fmt "\n", __FUNCTION__, __LINE__, ##arg)
#else
#define ell_debug(fmt, arg...)
#endif

#define ell_error(fmt, arg...) \
  printf("ERROR[%s.%u]: " fmt "\n", __FUNCTION__, __LINE__, ##arg)

/* ---- Private Function Prototypes --------------------------------------- */

/* ---- Public Variables -------------------------------------------------- */

/* ---- Private Variables ------------------------------------------------- */

/* ---- Functions --------------------------------------------------------- */

void ellInit(COMMON_ELL *ellP, UINT32 keyLen)
{
    if (ellP == NULL)
    {
        ell_error("NULL pointer\n");
        return;
    }

    BCM_COMMON_LL_INIT(&(ellP->list));
    ellP->keyLen = keyLen;
    ellP->count = 0;
}


COMMON_ELL_ENTRY *ellFindEntry(COMMON_ELL *ellP, void *key)
{
    COMMON_ELL_ENTRY *entryP = NULL;

    if ((ellP == NULL) || (key == NULL))
    {
        ell_error("NULL pointer\n");
        return NULL;
    }

    entryP = BCM_COMMON_LL_GET_HEAD(ellP->list);
    while (entryP)
    {
        if (memcmp(entryP->data, key, ellP->keyLen) == 0)
        {
            break;
        }
        entryP = BCM_COMMON_LL_GET_NEXT(entryP);
    };

    return entryP;
}


COMMON_ELL_ENTRY *ellAddEntry(COMMON_ELL *ellP, COMMON_ELL_ENTRY *entryP)
{
    void *keyP;
    COMMON_ELL_ENTRY *retEntryP = NULL;

    if ((ellP == NULL) || (entryP == NULL))
    {
        ell_error("NULL pointer\n");
        return NULL;
    }

    keyP = (void*)entryP->data;
    retEntryP = ellFindEntry(ellP, keyP);
    if (retEntryP != NULL)
    {
        /* Duplicated entry. */
        ell_debug("Entry with same key already exists\n");
        return retEntryP;
    }

    BCM_COMMON_LL_APPEND(&ellP->list, entryP);
    ellP->count++;
    return entryP;
}


COMMON_ELL_ENTRY *ellRemoveEntry(COMMON_ELL *ellP, void *key)
{
    COMMON_ELL_ENTRY *entryP = NULL;

    if ((ellP == NULL) || (key == NULL))
    {
        ell_error("NULL pointer\n");
        return NULL;
    }

    entryP = BCM_COMMON_LL_GET_HEAD(ellP->list);
    while (entryP)
    {
        if (memcmp(entryP->data, key, ellP->keyLen) == 0)
        {
            BCM_COMMON_LL_REMOVE(&ellP->list, entryP);
            ellP->count--;
            break;
        }
        entryP = BCM_COMMON_LL_GET_NEXT(entryP);
    };

    return entryP;
}


void ellIterateList(COMMON_ELL *ellP, LLIST_CB cb, void *usrDataP,
  UBOOL8 removeB)
{
    COMMON_ELL_ENTRY *entryP = NULL;
    COMMON_ELL_ENTRY *nextP = NULL;

    if (ellP == NULL)
    {
        ell_error("NULL pointer\n");
        return;
    }

    entryP = BCM_COMMON_LL_GET_HEAD(ellP->list);
    while (entryP)
    {
        nextP = BCM_COMMON_LL_GET_NEXT(entryP);
        if (removeB)
        {
            BCM_COMMON_LL_REMOVE(&ellP->list, entryP);
            ellP->count--;
        }
        if (cb != NULL)
        {
            cb(entryP, usrDataP);
        }
        entryP = nextP;
    };
}

void ellDestroy(COMMON_ELL *ellP, LLIST_CB cb)
{
    COMMON_ELL_ENTRY *entryP = NULL;
    COMMON_ELL_ENTRY *nextP = NULL;

    if (ellP == NULL)
    {
        ell_error("NULL pointer\n");
        return;
    }

    entryP = BCM_COMMON_LL_GET_HEAD(ellP->list);
    while (entryP)
    {
        nextP = BCM_COMMON_LL_GET_NEXT(entryP);
        BCM_COMMON_LL_REMOVE(&ellP->list, entryP);
        if (cb != NULL)
        {
            cb(entryP, NULL);
        }
        entryP = nextP;
    };
}
