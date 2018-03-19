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


/*
* \file Utils.c
* \brief Utilities APIs
*
*/

#include "Utils.h"
#include "Oam.h"
#include "TkSdkInitApi.h"
#include "stdio.h"

/*******************************************************************************
* InitBufInfo
*/
void InitBufInfo (BufInfo *buf, U16 size, U8 *start)
    {
    buf->len   = size;
    buf->curr  = start;
    buf->start = start;
    //buf->onBufFullCall = NULL;
    }

BOOL BufSkip(BufInfo *buf, U16 len)
    {
    if((buf->start + buf->len) >= (buf->curr + len))
        {
        buf->curr += len;
        return TRUE;
        }

    return FALSE;
    }

U16 BufGetUsed(const BufInfo *buf)
    {
    return(U16) (buf->curr - buf->start);
    }

BOOL BufRead(BufInfo *buf, U8 *to, U16 len)
    {
    if((buf->start + buf->len) >= (buf->curr + len))
        {
        memcpy(to, buf->curr, len);
        buf->curr += len;
        return TRUE;
        }

    return FALSE;
    }

BOOL BufReadU8(BufInfo *buf, U8 *val)
    {
    return BufRead(buf, val, sizeof(*val));
    }

BOOL BufReadU16(BufInfo *buf, U16 *val)
    {
    return BufRead(buf, (U8 *) val, sizeof(*val));
    }

U16 BufGetRemainingSize(const BufInfo *buf)
    {
    return(U16) ((buf->start + buf->len) - buf->curr);
    }

BOOL BufWrite(BufInfo *buf, const U8 *from, U16 len)
    {
    if( (buf->start + buf->len) >= (buf->curr + len))
        {
        memcpy(buf->curr, from, len);
        buf->curr += len;
        return TRUE;
        }
    else
        {
        return FALSE;
#if 0
        if(buf->onBufFullCall != NULL)
            {
            return buf->onBufFullCall(buf, from, len);
            }
        else
            {
            return FALSE;
            }
#endif
        }
    }

BOOL BufWriteU16(BufInfo *buf, U16 val)
    {
    return BufWrite(buf, (U8 *) &val, sizeof(val));
    }

/*******************************************************************************
 * Miscellaneous Stuff
 ******************************************************************************/
void BufDump(char * title, U8 * buf, U16 len)
    {
    U16 i;
	U8  brk;

	if (title != NULL)
		printf("%s", title);
	printf("\nSz=%d\n", len);

/* BCA_EPON_SDK_CHANGES_BEGIN */
    if(len > 0)
		printf ("%02X", (U8)*buf);
/* BCA_EPON_SDK_CHANGES_BEGIN */

    for (i = 1; i < len; i++)
        {
        if (0==(i%16))     brk='\n';
        else if (0==(i%8)) brk='-';
		else brk=' ';
/* BCA_EPON_SDK_CHANGES_BEGIN */
        printf ("%c%02X", brk,(U8) *(char *)((U32)buf+i));
/* BCA_EPON_SDK_CHANGES_BEGIN */
        }
    printf("\n");
    }

void conditonBubbleSort(OamNewRuleCondition* pData,S32 count)
	{ 
	S32 i;
	S32 j;
	OamNewRuleCondition tmp;
	
	for(i = 0; i < count; i++)
		{
		for(j = 0; j < count; j++)
			{
			if(pData[j].field < pData[j-1].field)
				{
				memcpy(&tmp,&pData[j-1],sizeof(OamNewRuleCondition));
				memcpy(&pData[j-1],&pData[j],sizeof(OamNewRuleCondition));
				memcpy(&pData[j],&tmp,sizeof(OamNewRuleCondition));
				}
			
			}
		}
	return;
	} 


inline void Tk2BufU8(U8 * buf, U8 val)
    {
    *buf = val;
    }

inline void Tk2BufU16(U8 * buf, U16 val)
    {
	union {
		U8   arryU8[2];
		U16  arryU16;
	}tmpVal;

	tmpVal.arryU16 = htons(val);

	memcpy(buf,&(tmpVal.arryU8[0]),sizeof(U16));

	return ;
    }

inline void Tk2BufU32(U8 * buf, U32 val)
    {
	union{
		U8  arryU8[4];
		U16 arryU16[2];
		U32 arryU32;
	}tmpVal;

	tmpVal.arryU32 = htonl(val);

	memcpy(buf,&(tmpVal.arryU8[0]),sizeof(U32));
    }


/*******************************************************************************
 * OAM Handling Stuff
 ******************************************************************************/

#define IsErrorResponse(x)  ((x) > 128)

/*******************************************************************************
* InitBufInfo
*/
BOOL AddOamTlv(BufInfo *bufInfo,
			   OamVarBranch branch,
			   U16 leaf,
			   U8 len,
			   const U8 *value)
	{
	BOOL ret = FALSE;
	OamVarContainer *var = (OamVarContainer *) bufInfo->curr;
	
	// non-zero length requires a value
	if((len != 0) && (value == NULL))
		{
		return ret;
		}
	
	if((len <= 0x80) &&
		((bufInfo->start + bufInfo->len) >
		  (bufInfo->curr + len + sizeof(OamVarContainer))
		))
		{
		var->branch = branch;
		var->leaf = htons(leaf);
		if(len == 0)
			{
			var->length = 0x80;
			}
		else
			{
			var->length = len & 0x7F;	// 0x80 is encoded as 0
			memcpy(var->value, value, len); //lint !e419 !e669 !e670
			}

		// move current pointer. Note that OamVarContainer includes an extra
		// byte that is subtracted out.
		bufInfo->curr += (sizeof(OamVarContainer) - 1) + len;
		ret = TRUE;
		}
	
	return ret;
	}

BOOL AddSetRespDesc(BufInfo *bufInfo,
   OamVarBranch branch,
   U16 leaf,
   OamVarErrorCode ackVal)
{
    OamSetRespDesc *var = (OamSetRespDesc *) bufInfo->curr;
    var->branch = branch;
    var->leaf = htons(leaf);
    var->ackVal = ackVal;
    // move current pointer.
    bufInfo->curr += sizeof(OamSetRespDesc);
    return TRUE;
}

BOOL FormatBranchLeaf(BufInfo *bufInfo, OamVarBranch branch, OamAttrLeaf leaf)
		{
		BOOL ret = FALSE;
		OamVarContainer *var = (OamVarContainer *) bufInfo->curr;
		U8 len = sizeof(OamVarBranch) + sizeof(OamAttrLeaf);
		if(((bufInfo->start + bufInfo->len) > (bufInfo->curr + len)))
			{
			var->branch = branch;
			var->leaf = htons(leaf);
	
			// move current pointer.
			bufInfo->curr += len;
			ret = TRUE;
			}
	
		return ret;
		}

void CloseVarMsg(BufInfo *pBufInfo) // Io: Buffer len, start and currentpos
    {
    (void) BufWriteU16(pBufInfo, 0);
    }



BOOL OamAddAttrLeaf(BufInfo *bufInfo, U16 leaf)
	{
	return FormatBranchLeaf(bufInfo, OamBranchAttribute, (OamAttrLeaf) leaf);
	}

// lyf@???
BOOL GetNextOamVar(BufInfo *pBufInfo, tGenOamVar *pOamVar, U8 *tlvRet)
    {
    BOOL more = TRUE;
    tGenOamVar *temp = (tGenOamVar *) pBufInfo->curr;
    *tlvRet = RcFail;   // not found by default
    if(BufGetRemainingSize(pBufInfo) != 0)
        {
        switch(*(pBufInfo->curr))
            {
            // TODO: does not support OamBranchPackage at this stage
            //case OamBranchPackage:
            //    break;

            case OamBranchAttribute:
			case OamBranchAction:
            case OamCtcBranchExtAttribute:
			case OamCtcBranchExtAction:
			case OamCtcBranchObjInst:
#if BCA_EPON_SDK_CHANGES
			case OamCtcBranchObjInst21:
#endif			
            case OamBranchNameBinding:
                {
                pOamVar->Branch = temp->Branch;
				// TODO: I'm not sure about the SWAP???
                pOamVar->Leaf  = TkSwap16(temp->Leaf);
                pOamVar->Width = temp->Width;
				pBufInfo->curr = (pBufInfo->curr + sizeof(tGenOamVar)) - sizeof(U8 *);

                // Set the data pointer to NULL if we receive an error response
                if(IsErrorResponse(temp->Width))
                    {
                    pOamVar->pValue = NULL;
                    *tlvRet = RcOnuReturnedErr;
                    }
                else
                    {
                    if(pOamVar->Width == 0)
                        {
                        pOamVar->Width = 0x80;
                        }
                    else if(pOamVar->Width == 0x80)
                        {
                        pOamVar->Width = 0;
                        }
                    else
                        {
                        // nothing
                        }

                    pOamVar->pValue = pBufInfo->curr;
                    pBufInfo->curr += pOamVar->Width;
                    *tlvRet = RcOk;  // Found it, no error
                    }
                }
                break;

            case OamBranchTermination:
                more = FALSE;
                break;

            default:
                more = FALSE;
            }
        }
    else
        {
        more = FALSE;
        pOamVar->Width = 0;
        }

    return more;
    }

BOOL GetEventTlv(BufInfo *pBufInfo, OamEventTlv **pOamEventTlv,int *tlvError)
	{
    OamEventTlv *temp;
	BOOL ret = TRUE;
	
	if(NULL == pBufInfo ||
		NULL == tlvError)
		{
		return FALSE;
		}

	temp = (OamEventTlv *) pBufInfo->curr;
	
    if(BufGetRemainingSize(pBufInfo) != 0)
        {
        switch(*(pBufInfo->curr))
            {
            case OamEventErrSymbolPeriod:
    		case OamEventErrFrameCount:
    		case OamEventErrFramePeriod:
    		case OamEventErrFrameSecSummary:
    		case OamEventErrVendor:
    		case OamEventErrVendorOld:
				{
				if((pBufInfo->curr + temp->length + sizeof(OamEventTlv))>(pBufInfo->start+pBufInfo->len))
					{
					*tlvError = ERROR;
					ret = FALSE;
					}
				else
					{
					*tlvError = OK;
					*pOamEventTlv = temp;
					pBufInfo->curr += sizeof(OamEventTlv) + temp->length; 
					ret = TRUE;
					}
				}
				break;
			case OamEventEndOfTlvMarker:
				*tlvError = OK;
				ret = FALSE;
				break;
			default:
				*tlvError = ERROR;
				ret = FALSE;
				break;
            }
        }
    else
        {
        ret = FALSE;
        *tlvError = OK;
        }
    return ret;
    }

U8 SearchBranchLeaf(void *oamResp, OamVarBranch branch, U16 leaf, tGenOamVar *pOamVar)
    {
    BufInfo     bufInfo;
    tGenOamVar  var;
    U8         ret = RcFail;
    U8         tlvRet = RcOk;

    InitBufInfo(&bufInfo, 1500, (U8 *) oamResp);
    while(GetNextOamVar(&bufInfo, &var, &tlvRet))
        {
         if((var.Branch == branch) && (var.Leaf == leaf))
            {
            if(tlvRet == RcOk)
                {
                *pOamVar = var;
                ret = RcOk;
                }
            break;
            }
        }

    return ret;
    }

void RuleDebug(U8 *buf,U32 len)
	{
	S32 i;
	for (i = 0; i < len; i++)
		{
			printf("%02x ", buf[i]);
			if ((i + 1) % 16 == 0)
				printf("\n");
		}
	printf("\n");
	
	}

