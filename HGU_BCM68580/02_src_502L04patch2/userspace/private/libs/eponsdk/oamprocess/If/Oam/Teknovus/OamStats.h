/*
 *<:copyright-BRCM:2013:proprietary:epon 
 *
 *   Copyright (c) 2013 Broadcom 
 *   All Rights Reserved
 *
 * This program is the proprietary software of Broadcom and/or its
 * licensors, and may only be used, duplicated, modified or distributed pursuant
 * to the terms and conditions of a separate, written license agreement executed
 * between you and Broadcom (an "Authorized License").  Except as set forth in
 * an Authorized License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and Broadcom
 * expressly reserves all rights in and to the Software and all intellectual
 * property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 * NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 * BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1. This program, including its structure, sequence and organization,
 *    constitutes the valuable trade secrets of Broadcom, and you shall use
 *    all reasonable efforts to protect the confidentiality thereof, and to
 *    use this information only in connection with your use of Broadcom
 *    integrated circuit products.
 *
 * 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
 *    RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
 *    ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
 *    FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
 *    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
 *    TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
 *    PERFORMANCE OF THE SOFTWARE.
 *
 * 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
 *    ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
 *    INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
 *    WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
 *    IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
 *    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
 *    SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
 *    SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
 *    LIMITED REMEDY.
:>
 */ 
#if !defined(OamStats_h)
#define OamStats_h

////////////////////////////////////////////////////////////////////////////////
/// \file OamStats.h
/// \brief Support for OAM stats.
/// \author Lingyong Chen
/// \date September 27, 2010
///
////////////////////////////////////////////////////////////////////////////////

#if defined(__cplusplus)
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////
/// OamTekStatsGetEpon:  Get OAM stat for Epon
///
 // Parameters:
/// \param id   Identifies stat to get
/// \param cont OAM container into which value is to be returned
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OamTekStatsGetEpon (StatId id, OamVarContainer BULK* cont);


////////////////////////////////////////////////////////////////////////////////
/// OamTekStatsGetLink:  Get OAM per-link stat
///
 // Parameters:
/// \param link Link to query
/// \param id   Identifies stat to get
/// \param cont OAM container into which value is to be returned
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OamTekStatsGetLink (LinkIndex link,StatId id,
                                              OamVarContainer BULK* cont);


////////////////////////////////////////////////////////////////////////////////
/// OamTekStatsGetEth:  Get OAM per-port stat for Ethernet port
///
 // Parameters:
/// \param port port number
/// \param id   Identifies stat to get
/// \param cont OAM container into which value is to be returned
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OamTekStatsGetEth (TkOnuEthPort port,
                        StatId id,
                        OamVarContainer BULK* cont);

////////////////////////////////////////////////////////////////////////////////
/// OamTekStatsGetFifo:  Get OAM fifo stat for processor, epon and Uni
///
 // Parameters:
/// \param chan fifo channel
/// \param id   Identifies stat to get
/// \param cont OAM container into which value is to be returned
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OamTekStatsGetFifo(TkOnuEthPort port, 
                    StatId id, 
                    OamVarContainer BULK* cont);


////////////////////////////////////////////////////////////////////////////////
/// OamTekStatsGetEponThdVal:  Get OAM stat threshold for Epon
///
 // Parameters:
/// \param src  OAM descriptor into which value is to be requested
/// \param cont OAM container into which value is to be returned
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OamTekStatsGetEponThdVal (OamVarContainer BULK* src,
                            OamVarContainer BULK* cont);

////////////////////////////////////////////////////////////////////////////////
/// OamTekStatsGetLinkThdVal:  Get OAM per-link stat
///
 // Parameters:
/// \param link Link to query
/// \param src  OAM descriptor into which value is to be requested
/// \param cont OAM container into which value is to be returned
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OamTekStatsGetLinkThdVal (LinkIndex link,
                            OamVarContainer BULK* src,
                            OamVarContainer BULK* cont);

////////////////////////////////////////////////////////////////////////////////
/// OamTekStatsGetEthThdVal:  Get OAM stat threshold for Eth port
///
 // Parameters:
/// \param port Port to query
/// \param src   OAM container into which value is to be returned
/// \param cont OAM container into which value is to be returned
/// \return
////////////////////////////////////////////////////////////////////////////////
extern
void OamTekStatsGetEthThdVal(TkOnuEthPort port, 
                                     OamVarContainer BULK* src, 
                                     OamVarContainer BULK* cont);


////////////////////////////////////////////////////////////////////////////////
/// OamTekStatsSetEponThdVal:  Set OAM stat threshold for Epon
///
 // Parameters:
/// \param src  OAM container into which value is to be requested
/// \param cont OAM container into which value is to be returned
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OamTekStatsSetEponThdVal (OamVarContainer BULK* src,
                            OamVarContainer BULK* cont);

////////////////////////////////////////////////////////////////////////////////
/// OamTekStatsSetLinkThdVal:  Set OAM per-link stat
///
 // Parameters:
/// \param link Link to query
/// \param src  OAM container into which value is to be requested
/// \param cont OAM container into which value is to be returned
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OamTekStatsSetLinkThdVal (LinkIndex link,
                            OamVarContainer BULK* src,
                            OamVarContainer BULK* cont);

////////////////////////////////////////////////////////////////////////////////
/// OamTekStatsSetEthThdVal:  Set OAM per-port stat threshold for Ethernet port
///
 // Parameters:
/// \param port port number
/// \param src  OAM container into which value is to be requested
/// \param cont OAM container into which value is to be returned
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OamTekStatsSetEthThdVal (TkOnuEthPort port,
                           OamVarContainer BULK* src,
                           OamVarContainer BULK* cont);



////////////////////////////////////////////////////////////////////////////////
/// OamTekStatsClearAll:  Clear all ports(including PON) stat
///
 // Parameters:
/// \return
////////////////////////////////////////////////////////////////////////////////
extern
void OamTekStatsClearAll(void);

#if defined(__cplusplus)
}
#endif

#endif // end OamStats.h


