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
/// \file CtcOptDiag.c
/// \brief China Telecom ether port control module
/// \author Chen Lingyong
/// \date Nov 2, 2010
///
////////////////////////////////////////////////////////////////////////////////
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <errno.h>

#include "Build.h"
#include "MultiByte.h"
#include "CtcAlarms.h"
#include "CtcOptDiag.h"
#include "laser.h"
#include "OamOnu.h"

////////////////////////////////////////////////////////////////////////////////
// Constant Definition
////////////////////////////////////////////////////////////////////////////////
#define OptDiagMaxSignedVal			0x7FFFFFFFUL
#define OptDiagMinSignedVal			0xFFFFFFFFUL
#define OptDiagMaxUnsignedVal		0xFFFFFFFFUL


////////////////////////////////////////////////////////////////////////////////
// Alarm, Oam, Sff Conversion
////////////////////////////////////////////////////////////////////////////////
#define OamCtcAlm2OptDiagAlm(alm) (CtcOptDiagInst)((alm) - OamCtcPonIfAlarmBase)
#define OptDiagAlm2OamCtcAlm(alm) (OamCtcAlarmId)((alm) + OamCtcPonIfAlarmBase)
#define OptDiagAlm2SffRegAddr(alm) (Sff8472RxPower - ((((alm) % 10) / 2) * 2))


////////////////////////////////////////////////////////////////////////////////
// Ctc Optical Diagnosis Instance
////////////////////////////////////////////////////////////////////////////////
typedef enum 
    {
    //Alarm Section
    CtcOptDiagRxPowerAlmHi, 
    CtcOptDiagRxPowerAlmLo, 
    CtcOptDiagTxPowerAlmHi,
    CtcOptDiagTxPowerAlmLo,
    CtcOptDiagTxBiasAlmHi,
    CtcOptDiagTxBiasAlmLo,
    CtcOptDiagVccAlmHi,
    CtcOptDiagVccAlmLo,
    CtcOptDiagTempAlmHi,
    CtcOptDiagTempAlmLo,    
    //Warn Section
    CtcOptDiagRxPowerWrnHi,
    CtcOptDiagRxPowerWrnLo,
    CtcOptDiagTxPowerWrnHi,
    CtcOptDiagTxPowerWrnLo,
    CtcOptDiagTxBiasWrnHi,
    CtcOptDiagTxBiasWrnLo,
    CtcOptDiagVccWrnHi,
    CtcOptDiagVccWrnLo,
    CtcOptDiagTempWrnHi,
    CtcOptDiagTempWrnLo,

    CtcOptDiagNums
	}CtcOptDiagInst;


////////////////////////////////////////////////////////////////////////////////
// Ctc Optical Diagnosis Threshold
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    U32        	ctcRaise;
    U32        	ctcClear;
    U16        	sffRaise;
    U16        	sffClear;
    } PACK CtcOptDiagThd;


////////////////////////////////////////////////////////////////////////////////
// Static Varible
////////////////////////////////////////////////////////////////////////////////
static CtcOptDiagInst diagInst = 0;
static CtcOptDiagThd diagThd[CtcOptDiagNums];

////////////////////////////////////////////////////////////////////////////////
/// Ctc2SffThdConv - Convert CTC alarm threshold to SFF threshold value.
/// 
/// This function convert 32 bits CTC alarm threshod to be 16 bits SFF 
/// threshold value. Signed value will be added 0x8000 to change to unsigned 
/// value. So only unsigned value will be used for value comparison.
/// 
/// Parameters:
///	param almId alarm Id 
/// param val pointer to CTC 32bits threshold value
/// 
/// \return: converted sff value
////////////////////////////////////////////////////////////////////////////////
static
U16 Ctc2SffThdConv(U16 almId, U32 val)
    {
    MultiByte32 BULK * pVal = (MultiByte32 BULK *)&val;
   
	if ((almId == OamCtcAttrPowerMonTempAlmHigh)  ||
		(almId == OamCtcAttrPowerMonTempAlmLow)   ||
		(almId == OamCtcAttrPowerMonTempWarnHigh) ||
		(almId == OamCtcAttrPowerMonTempWarnLow ))
		{
	    if (TestBitsAny(pVal->warray[0], 0x8000))
	        {
            /* convert signed temperature to unsigned. */
	        return (pVal->warray[1] & 0x7FFF);
	        }
	    else
	        {
	        return (pVal->warray[1] + 0x8000);
	        }
		}
	
     
	return pVal->warray[1];
    }//Ctc2SffThdConv

////////////////////////////////////////////////////////////////////////////////
/// CtcOptDiagSetThd - Set optical diagnosis threshold value 
///                        
/// This function: sets optical diagnosis threshold value 
/// 
 // Parameters: 
/// \param src  pointer to var descriptors in request
/// 
/// \return:
///		None
////////////////////////////////////////////////////////////////////////////////
//extern
void CtcOptDiagSetThd(const OamCtcTlvPowerMonThd BULK * src)
    {
    U16 FAST almId = src->alarmId;
    CtcOptDiagThd BULK *val;
    
    val = &diagThd[OamCtcAlm2OptDiagAlm(almId)];
    val->ctcRaise = src->thdRaise;
    val->ctcClear = src->thdClear;
    val->sffRaise = Ctc2SffThdConv(almId, val->ctcRaise);
    val->sffClear = Ctc2SffThdConv(almId, val->ctcClear);
    }//CtcOptDiagSetThd

	

////////////////////////////////////////////////////////////////////////////////
/// CtcOptDiagGetThd - Get optical diagnosis threshold value from alarm Id
///                        
/// This function gets optical diagnosis threshold value from alarm Id
/// 
 // Parameters: 
/// \param almId Ctc Alarm Id
/// \param cont pointer to var containers in reply
/// 
/// \return:
///		None
////////////////////////////////////////////////////////////////////////////////
//extern
void CtcOptDiagGetThd(U16 almId, OamCtcTlvPowerMonThd BULK * cont)
{
    cont->thdRaise = diagThd[OamCtcAlm2OptDiagAlm(almId)].ctcRaise;
    cont->thdClear = diagThd[OamCtcAlm2OptDiagAlm(almId)].ctcClear;
}

////////////////////////////////////////////////////////////////////////////////
/// CtcOptDiagPoll-Poll the optical diagnosis.
/// 
/// This function polls the optical diagnosis, it compares the current value to 
/// ctc threshold value and push/pop the threshold alarm into message queue 
/// when meat the condition.It suggest to implement this polling in less than 
/// 500ms Timer, for optical diagnosis have 20 items and each poll only poll 
/// one optical diagnosis item.
/// 
/// Parameters: 
/// \param None
/// 
/// \return: 
///		None
////////////////////////////////////////////////////////////////////////////////
//extern
void CtcOptDiagPoll(void)
{
	U16 curVal = 0;
	CtcOptDiagThd * thd = &diagThd[diagInst];	

    if(FALSE == CtcAlmAdminStateGet(0, OptDiagAlm2OamCtcAlm(diagInst)))
    {
        goto opt_poll_exit;
    }

    U8 regAddr = OptDiagAlm2SffRegAddr(diagInst);
		
    switch (regAddr)
	{
		case Sff8472Temp:
			optical_temp(&curVal);
            break;
			
        case Sff8472Vcc:
            optical_vcc(&curVal);
            break;
			
        case Sff8472TxBias:
            optical_bias(&curVal);
	    	break;
			
        case Sff8472TxPower:
		    optical_txpower(&curVal);
            break;

		case Sff8472RxPower:
		    optical_rxpower(&curVal);
            break;

		default:
			break;
	}

	if ((((U32)curVal > thd->ctcRaise) && (!TestBitsAny(diagInst, 0x1))) ||
    	(((U32)curVal < thd->ctcRaise) && TestBitsAny(diagInst, 0x1)))
    {
    	CtcAlmSetCond (OptDiagAlm2OamCtcAlm(diagInst), 0x0, 
                       AlmInfoLen4, (U64)curVal);
        OsAstMsgQSet (OsAstAlmOntSff8472Thd, diagInst, 0);
    }


    if ((((U32)curVal < thd->ctcClear) && (!TestBitsAny(diagInst, 0x1))) ||
    		(((U32)curVal > thd->ctcClear) && TestBitsAny(diagInst, 0x1)))
    {
    	CtcAlmClrCond (OptDiagAlm2OamCtcAlm(diagInst), 0x0, 
                      AlmInfoLen4, (U64)curVal);            
        OsAstMsgQClr (OsAstAlmOntSff8472Thd, diagInst, 0);
    }

opt_poll_exit:    
    if (++diagInst >= CtcOptDiagNums)
    {
    	diagInst = CtcOptDiagRxPowerAlmHi;
    }
    
}//CtcOptDiagPoll


////////////////////////////////////////////////////////////////////////////////
/// CtcOptDiagInit - Optical diagnosis initialization
/// 
/// This function initialize optical diagnosis. All the threshold should be 
/// set to be the max value that can't raise any alarm if the threshold is not 
/// configure by the host.
/// 
/// Parameters: 
/// \param None
/// 
/// \return: 
///		None
////////////////////////////////////////////////////////////////////////////////
//extern
void CtcOptDiagInit(void)
	{
	U8 i;
	memset(diagThd, 0x0, sizeof(diagThd));
	for (i = 0; i < CtcOptDiagNums; i++)
		{			 
		if (!TestBitsAny(i, 0x1))
			{
			diagThd[i].sffRaise = 0xFFFF;	 
			if ((i == CtcOptDiagTempAlmHi) || (i == CtcOptDiagTempWrnHi))
				{
				diagThd[i].ctcRaise = OptDiagMaxSignedVal;
				diagThd[i].ctcClear = OptDiagMaxSignedVal;
				}
			else
				{
				diagThd[i].ctcRaise = OptDiagMaxUnsignedVal;
				diagThd[i].ctcClear = OptDiagMaxUnsignedVal;
				}
			}	
		else
			{		 
			if ((i == CtcOptDiagTempAlmLo) || (i == CtcOptDiagTempWrnLo))
				{
				diagThd[i].ctcRaise = OptDiagMinSignedVal;
				diagThd[i].ctcClear = OptDiagMinSignedVal;				
				}
			}			 
		}
    
	}


//end CtcOptDiag.c

