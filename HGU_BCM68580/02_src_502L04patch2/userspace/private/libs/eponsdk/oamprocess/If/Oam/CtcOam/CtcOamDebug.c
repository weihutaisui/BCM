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
/// \file CtcOamDebug.c 
/// \brief China Telecom debug module
/// \author Jason Armstrong
/// \author Joshua Melcon
/// \date March 2, 2006
///
/// \todo
/// Finish this header.
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "TkDebug.h"
#include "Teknovus.h"
#include "Oam.h"
#include "CtcOam.h"
#include "CtcOnuUtils.h"
#include "CtcOamDebug.h"
#include "Mpcp.h"
#include "LinkTable.h"

CtcDebugLevel ctcDebugAreas[NumCtcDebugAreas]=
    {
        {TkDebugOff,"OAM discovery"},
        {TkDebugOff,"Post discovery heartbeat"},
        {TkDebugOff,"Key exchange"},
        {TkDebugOff,"FPGA MDIO commands\n"},
        {TkDebugOff,"CTC DBA debug"},
        {TkDebugOff,"CTC host IF debug"}
    };

////////////////////////////////////////////////////////////////////////////////
/// CtcDebugPrintOui - Print Organizationally Unique Identifier
///
/// This function parses and prints the contents of a Organizationally Unique
/// Identifier.
/// \param oui    Organizationally Unique Identifier
///
/// \return
/// Nothing
////////////////////////////////////////////////////////////////////////////////
void CtcDebugPrintOui (const IeeeOui BULK *oui, U8 indent)
    {
    while (indent-- != 0)
        {
        printf ("\t");
        }
    printf ("OUI: %02X-%02X-%02X\n",
        (int)oui->byte[0],
        (int)oui->byte[1],
        (int)oui->byte[2]);
    } // CtcDebugPrintOui

////////////////////////////////////////////////////////////////////////////////
/// CtcDebugPrintChurningOam: Prints a OamCtcChurningPdu to the CLI in a human
/// readable format.
///
/// \param *msg message to print.
/// 
/// \return No return value.
////////////////////////////////////////////////////////////////////////////////
void CtcDebugPrintChurningOam(const OamCtcChurningPdu BULK *msg)
    {
    BOOL msgIs10g = TRUE;
    OamCtcChurning10GPdu *msg10g = (OamCtcChurning10GPdu *)msg;

    if ((msg10g->key.key[1].parts.pPart == 0) &&
        (msg10g->key.key[2].parts.pPart == 0))
        {
        msgIs10g = FALSE;
        }

    printf("CTC Churning PDU:\n"); 
    printf("\tExtension %d\n",(int)msg->ext);
    printf("\tChurning code %d\n",(int)msg->opcode);
    switch(msg->opcode)
        {
        case ChurningKeyRequest:
            printf("\tIn use key index %02X\n",(int)msg->keyIndex); 
            break;
        case ChurningNewKey:
            printf("\tNew key index %02X\n",(int)msg->keyIndex); 
            if (!msgIs10g)
                {
                printf("\tKey %02X-%02X-%02X\n"    ,(int)msg->key.byte[0]
                                            ,(int)msg->key.byte[1]
                                            ,(int)msg->key.byte[2]);
                }
            else
                {

                printf("\tKey1 %02X-%02X-%02X\n",(int)msg10g->key.key[0].byte[0]
                                            ,(int)msg10g->key.key[0].byte[1]
                                            ,(int)msg10g->key.key[0].byte[2]);
                printf("\tKey2 %02X-%02X-%02X\n",(int)msg10g->key.key[1].byte[0]
                                            ,(int)msg10g->key.key[1].byte[1]
                                            ,(int)msg10g->key.key[1].byte[2]);
                printf("\tKey3 %02X-%02X-%02X\n",(int)msg10g->key.key[2].byte[0]
                                            ,(int)msg10g->key.key[2].byte[1]
                                            ,(int)msg10g->key.key[2].byte[2]);
                }
            break;
        default:
            printf("\tUnexpected opcode!\n"); 
            break;
        }

    }
////////////////////////////////////////////////////////////////////////////////
/// CtcDebugPrintInfoTvl - Print a CTC information TVL
///
/// This function parses and dumps the contents of a China Telecom Information
/// TLV.
/// \param tlv Information TVL
///
/// \return
/// Nothing
////////////////////////////////////////////////////////////////////////////////
void CtcDebugPrintInfoTvl (const OamCtcInfoTlvHeader BULK *tlv)
    {
    U8 FAST extCount;


    printf ("CTC Info TLV:\n");
    printf ("\tOAM Type: %02X\n", (int)tlv->type);
    printf ("\tLength: %d\n", (int)tlv->length);
    CtcDebugPrintOui (&tlv->oui, 1);
    printf ("\tSupport: %d %s\n",
        (int)tlv->support, tlv->support ? "yes" : "no");
    printf ("\tVersion: %d\n", (int)tlv->version);
    
    extCount = OamCtcInfoTlvGetExtCount (tlv);
    printf ("\textCount = %d\n", (int)extCount);
    if (extCount != 0)
        {
        U8 FAST i;

        printf ("\tExtended Data:\n");
        for (i = 0; i < extCount; ++i)
            {
            CtcDebugPrintOui (&(tlv->ext[i].oui), 2);
            printf ("\t\tVersion: %d\n", (int)tlv->ext[i].version);
            }
        }
    } // CtcDebugPrintInfoTvl


