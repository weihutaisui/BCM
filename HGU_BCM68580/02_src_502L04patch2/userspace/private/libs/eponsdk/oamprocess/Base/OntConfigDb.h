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


#if !defined(OntConfigDb_h)
#define OntConfigDb_h
////////////////////////////////////////////////////////////////////////////////
/// \file OntConfigDb.h
/// \brief Manages the high level ONU wide configuration
///
///
////////////////////////////////////////////////////////////////////////////////

#if defined(__cplusplus)
extern "C" {
#endif

#include "Build.h"
#include "Ethernet.h"
#include "Teknovus.h"

#define UpstreamRecordReportModeOffset 5
#define SystemMaxMdioAddresses 16
#define ExtendedIdLen 80


typedef struct
    {
    BOOL backupBattery;
    BOOL PolicerIpg[2];
    BOOL ShaperIpg[2];
    } PACK PersOnuOptions;

typedef struct
    {
    U16 year;
    U8  month;
    U8  day;
    } PACK DateOfMan;


////////////////////////////////////////////////////////////////////////////////
/// \brief ONT cfg database init
///
/// Parameters:
/// none
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
extern
void OntCfgDbInit(void);

////////////////////////////////////////////////////////////////////////////////
/// \brief  Get name of manufacturer
///
/// \return
/// name of manufacturer
////////////////////////////////////////////////////////////////////////////////
extern
U8 OntCfgManNameGet(U8 BULK * name, U8 bufSize);


////////////////////////////////////////////////////////////////////////////////
/// \brief ONT cfg database link count
///
/// Parameters:
/// none
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
extern
U8 OntCfgDbGetNumUserLinks(void);



////////////////////////////////////////////////////////////////////////////////
/// \brief  Get BCD date of manufacture
///
/// \return
/// BCD date of manufacture
////////////////////////////////////////////////////////////////////////////////
extern
DateOfMan OntCfgDateOfManGet(void);


/******************************************************************************/
/*  \brief                                                                    */
/*                                                                            */
/*  \return                                                                   */
/*                                                                            */
/******************************************************************************/
extern
void OntCfgDbUpInfoSet(U8 linkCount, U8 queueCount);


#if defined(__cplusplus)
}
#endif

#endif // OntConfigDb.h

