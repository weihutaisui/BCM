/******************************************************************
 * $Id: mocad.h 17719 2010-05-16 10:18:33Z amirm $
 *
 * Project:      MoCA Driver
 *
 * Description:  This file is used to define parameters passed between the emulator and mocad
 *
 * Copyright:    (c) Broadcom 2011 , Confidential and Proprietary
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
 ******************************************************************/

#ifndef __MOCAD_H__
#define __MOCAD_H__

#ifdef __cplusplus
extern "C" {
#endif

#define   NODE_MODE_AUTO            0
#define   NODE_MODE_NC              1
#define   NODE_MODE_NN              2
#define   NODE_MODE_SS_NC           3
#define   NODE_MODE_SS_NN           4
#define   NODE_MODE_CONTINUOUS_TX   5
#define   NODE_MODE_CONTINUOUS_RX   6
#define   NODE_MODE_LAST            7

#define   TCM_SINGLE_TONE           0
#define   TCM_NORM_PROBE1           1
#define   TCM_CW                    2
#define   TCM_BAND                  3

#define   BW_50                     0
#define   BW_100                    1

#define   MMP_INIT                        0  // Normal MoCA start mode.
#define   MMP_CONTINUOUS_TX_PROBE_I_MODE  1  // Continues transmission of probe I. MoCA functionality not available.
#define   MMP_CONTINUOUS_RX_MODE          2  // Continues RX mode, LO disabled. MoCA functionality not available.
#define   MMP_EXTERNAL_CONTROL_MODE       3  // Control by MoTek/BBS/Host. MoCA functionality not available.
#define   MMP_CONTINUOUS_RX_LO_ON_MODE    4  // Continuous RX mode, LO enabled


struct mocad_args
{
	unsigned int sn;    // Simulated node (how many nodes exists)
	unsigned int sc;    // Single channel
	unsigned int tlb;   // Taboo left bitmask
	unsigned int trb;   // Taboo tight bitmask
	unsigned int pns;   // Preliminary Network Search
	unsigned int pfm;   // Preliminary NS frequency bitmask
	unsigned int lof;   // LOF parameter - Last Operating Frequency
	unsigned int tpc;   // TPC enable
	signed   int rf;    // RF type
	unsigned int tsc;   // Taboo fixed mask start
	unsigned int tbm;   // Taboo fixed channel bitmask 
	unsigned int frm;   // Frequency bitmask
	unsigned int lpbk;  // Loopback mode enable
	unsigned int wait;  // Wait for mocactl start command before starting
	unsigned int nc_mode; // NC/NN/AUTO
	unsigned int ofdma;   //ofdma enable
	unsigned int cn;    // channel number
	char chip[32];

};


// This struct must be kept in sync with firmware's version (Init_Config_T)
struct mocad_pre_init_config
{
   // input fields: written by host, read by firmware
   unsigned int phyclock;
   unsigned int cpuclock;
   unsigned int hostMacAddress[2];       // Host MAC address
   unsigned int chipId;                  // Allow FW to distinguish between 6802 and 6803.

   // output fields: written by firmware, read by host
   unsigned int ciroffset;               // Offset of CIR data, in lab-mode
   unsigned int iqoffset;                // Offset of IQ data
   unsigned int snroffset;               // Offset of SNR data

   // input fields: written by host, read by firmware
   unsigned int rmon_hz;
   unsigned int rmon_vt;
   unsigned int random_seed[8]; // 8x32=256bits

   struct _capabilities_
   {
      union
      {
         struct
         {
#if (__BYTE_ORDER != __LITTLE_ENDIAN)
            unsigned int reserved          : 21;// [31..11] Reserved
            unsigned int turbo_en          : 1; // [10]0-Default, 1-turbo enable.
            unsigned int brcm_seed_bonding : 1; // [9] 0-Default, 1-Use of primary seed on the secondary channel.
            unsigned int useExtPram        : 1; // [8] Configuration whether the entire PRAM of 6802C0 should be used in single
            unsigned int psm_analog        : 1; // [7] PSM ANALOG 1- enable, 0- disable
           	unsigned int psm_pll           : 1; // [6] PSM PLL 1- enable, 0- disable
            unsigned int psm_3451          : 1; // [5] PSM 3451 1- enable, 0- disable
            unsigned int bcm3450rev        : 1;  //[4] Revision of the 3450(0) or 3451(1)
            unsigned int limit_traffic     : 1; // [3] Low-Traffic-Max-Power-Save mode
            unsigned int bonded            : 1; // [2] 1-bonded mode, 0 non-bonded
            unsigned int remote_management : 2; // [0..1] 0-no 680x remote management, 1-remote management over Ethernet, 2-management over moca or Ethernet
#else
            unsigned int remote_management : 2; // [0..1] 0-no 680x remote management, 1-remote management over Ethernet, 2-management over moca or Ethernet
            unsigned int bonded            : 1; // [2] 1-bonded mode, 0 non-bonded
            unsigned int limit_traffic     : 1; // [3] Low-Traffic-Max-Power-Save mode
            unsigned int bcm3450rev        : 1;  //[4] Revision of the 3450(0) or 3451(1)
            unsigned int psm_3451          : 1; // [5] PSM 3451 1- enable, 0- disable
            unsigned int psm_pll           : 1; // [6] PSM PLL 1- enable, 0- disable
            unsigned int psm_analog        : 1; // [7] PSM ANALOG 1- enable, 0- disable
            unsigned int useExtPram        : 1; // [8] Configuration whether the entire PRAM of 6802C0 should be used in single
            unsigned int brcm_seed_bonding : 1; // [9] 0-Default, 1-Use of primary seed on the secondary channel.
            unsigned int turbo_en          : 1; // [10]0-Default, 1-turbo enable.
            unsigned int reserved          : 21;// [31..11] Reserved
#endif
         } ;
         unsigned int val32;
      };
   } capabilities;

   unsigned int valid;                   // 1: structure contains valid data, otherwise data is invalid.  This must be the last field of this struct
};

#ifndef STANDALONE
int mocad_main(struct mocad_args *args);
#endif

#ifdef __cplusplus
}
#endif

#endif
