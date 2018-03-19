/***********************************************************************
 *
 *  Copyright (c) 2007 Broadcom
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

/*****************************************************************************
*    Description:
*
*      OMCI common utility APIs.
*      APIs in this file may be used by an external process.
*
*****************************************************************************/

/* ---- Include Files ----------------------------------------------------- */

#include "omciutl_cmn.h"


/* ---- Private Constants and Types --------------------------------------- */

#define CRC_INITIAL_VALUE 0xFFFFFFFF


/* ---- Macro API definitions --------------------------------------------- */


/* ---- Private Function Prototypes --------------------------------------- */


/* ---- Public Variables -------------------------------------------------- */


/* ---- Private Variables ------------------------------------------------- */

static UINT32 crcTable[256]; /* Table of 8-bit remainders */


/* ---- Functions --------------------------------------------------------- */

/*****************************************************************************
*  FUNCTION:  omciUtl_initCrc32Table
*  PURPOSE:   Initialize CRC32 table as defined in [ITU-T I.363.5]. The CRC
*             is used for G.984 OMCI MIC and software image CRC-32.
*  PARAMETERS:
*      None.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void omciUtl_initCrc32Table(void)
{
    UINT16 i;
    UINT16 j;
    UINT32 crcAccum;

    for (i = 0; i < 256; ++i)
    {
        crcAccum = (UINT32)(i << 24);
        for (j = 0; j < 8; ++j)
        {
            if ( crcAccum & 0x80000000L )
            {
                crcAccum = (crcAccum << 1) ^ OMCI_CRC32_POLYNOMIAL;
            }
            else
            {
                crcAccum = (crcAccum << 1);
            }
        }

        crcTable[i] = crcAccum;
    }
}

/*****************************************************************************
*  FUNCTION:  omciUtl_getIncCrc32
*  PURPOSE:   Calculate CRC-32 incrementally.
*  PARAMETERS:
*      crcAccum - initial CRC-32 value.
*      pBuf - pointer to the buffer to be computed.
*      size - buffer size.
*  RETURNS:
*      Computed CRC-32 value.
*  NOTES:
*      None.
*****************************************************************************/
UINT32 omciUtl_getIncCrc32(UINT32 crcAccum, char *pBuf, UINT32 size)
{
   UINT32 i;
   UINT32 j;

   for (j = 0; j < size; j++)
   {
       i = ((int)(crcAccum >> 24) ^ *pBuf++) & 0xFF;

       crcAccum = (crcAccum << 8) ^ crcTable[i];
   }

   return crcAccum;
}

/*****************************************************************************
*  FUNCTION:  omciUtl_getCrc32
*  PURPOSE:   Calculate CRC-32 value.
*  PARAMETERS:
*      crcAccum - initial CRC-32 value. Fixed to be -1.
*      pBuf - pointer to the buffer to be computed.
*      size - buffer size.
*  RETURNS:
*      Computed CRC-32 value.
*  NOTES:
*      None.
*****************************************************************************/
UINT32 omciUtl_getCrc32(UINT32 crcAccum, char *pBuf, UINT32 size)
{
   UINT32 crcVal;

   crcVal = omciUtl_getIncCrc32(crcAccum, pBuf, size);

   crcVal = ~crcVal;

   return crcVal;
}

/*****************************************************************************
*  FUNCTION:  omciUtl_getCrc32Staged
*  PURPOSE:   Calculate CRC-32 value in stages. This is used for the
*             incremental image upgrade/flashing.
*  PARAMETERS:
*      stage - 0 : init; 1: incremental computing; 2: close.
*      crcAccum - current and updated CRC-32 value.
*      pBuf - pointer to the buffer to be computed.
*      size - buffer size.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void omciUtl_getCrc32Staged(UINT32 stage, UINT32 *crcAccumP, char *pBuf,
  UINT32 size)
{
   UINT32 crc = 0;

   if (stage == 0)
   {
      crc = CRC_INITIAL_VALUE;
   }
   else if (stage == 1)
   {
      crc = omciUtl_getIncCrc32(*crcAccumP, pBuf, size);
   }
   else
   {
      crc = ~(*crcAccumP);
   }

   *crcAccumP = crc;
}
