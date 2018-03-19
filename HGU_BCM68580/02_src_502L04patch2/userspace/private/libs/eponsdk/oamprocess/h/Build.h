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


#if !defined(Build_h)
#define Build_h
////////////////////////////////////////////////////////////////////////////////
/// \file Build.h
/// \brief Contains definitions specific to the build
///
///  In this file we define options that are specific for the build
///
////////////////////////////////////////////////////////////////////////////////

#if defined(__cplusplus)
extern "C" {
#endif

#define  LINKS_32                         1

// IOP
#define  CTC_PROXY                        0
#define  REPORT_AUTODETECT                1

// OAM 
#define  CTC_OAM_DEBUG                    0
#define  CTC_ALARM                        1 
#define  DPOE_DEMARC			0
#define  OAM_FULLSUPPORT                  0
#define  OAM_RSTP                         1
#define  OAM_NTT_AS                       0
#define  OAM_POLICER                      0 
#define  OAM_SHAPER                       0 
#define  OAM_SWITCH                       0
#define  OAM_VERSION                      320
#define  REPORTING_FULLSUPPORT            0

// FEATURES
#define  ARP_REPLICATION                  0
#define  FEC_SUPPORT                      0
#define  IPV6_SUPPORT                     0
#define  LACP_PASSTHROUGH                 0
#define  LINK_LOOPBACK_SUPPORT            0
#define  REGISTER_NACK_BACKOFF_SUPPORT    1
#define  STATS_COLLECT                    1
#define  EMC_UNI                          0
#define  OAM_PORTLABEL_COUNT              16
#define  APP2_SUPPORT
#define  STATS_THRESHOLD                  0 
#define  RSTP                             0 
#define  LOOP_DETECT                      0
#define  MCAST                            0
#define  INCLUDE_PPPOE                    0
#define  GLOBAL_DN_SHAPER         1

// CLI
#define  OAM_CLI                          0 


// TO BE FINISHED -- 0 Means it is NOT finished
#define  ALARM_TO_BE_FINISHED             0
#define  ETH_FRAME_UTIL_TO_BE_FINISHED    0
#define  OAM_IEEE_TO_BE_FINISHED          0
#define  OAM_TEKNOVUS_TO_BE_FINISHED      0
#define  OAM_UTIL_TO_BE_FINISHED          0

// TO DO
// Open this macro for EASW-1513 [10G ONU] Set CTC VLAN in Translation mode with
// one translationi returns Action Bad Parameters error
// #define  CTC_VLAN_TRANSLATION_A0A1        0

// ASIC SPECIFIC
#define  HARDCODED_PERSONALITY            1
#define  TK4701                           1
#define  PERSUNI10GDEFAULT                1


// PROFILING
// List here the files you may want to pay attention to one day. All you need
// to do is replace the .c by a _c and assign it a unique number. If the
// filename is not unique, come up with a different name.
// The name you choose will have to be used in the actual file you consider
// For more information please refer to Profiling.h
#define OamIeee_c            4

#if defined(__cplusplus)
}
#endif

#endif // Build.h
