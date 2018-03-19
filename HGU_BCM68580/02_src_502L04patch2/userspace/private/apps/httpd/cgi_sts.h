/***********************************************************************
 *
 *  Copyright (c) 2002-2010  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2011:proprietary:standard

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

/*
//  Filename:       cgi_sts.h
//  Author:         Peter T. Tran
//  Creation Date:  04/09/02
//
******************************************************************************
//  Description:
//      Define the CGI statistics functions.
//
*****************************************************************************/

#ifndef __CGI_STS_H__
#define __CGI_STS_H__

//~#include "bcmtypes.h"
#include "os_defs.h"
// mwang #include "bcmatmapi.h"

/********************** Global Types ****************************************/

// Ethernet & USB statistics
#define CGI_STS_IFC_ROWS     4
#define CGI_STS_ETH          0
#define CGI_STS_USB          1
#define CGI_STS_WIRELESS     2
#define GGI_STS_ETH1         3

// ATM statistics
#define CGI_STS_ATM_IFC_TYPES          3
#define CGI_STS_ATM_IFC_TYPE_ATM       0
#define CGI_STS_ATM_IFC_TYPE_AAL5      1
#define CGI_STS_ATM_IFC_TYPE_AAL0      2

#define CGI_STS_ATM_IFC_FIELDS         12
#define CGI_STS_ATM_IFC_FD_OCTET       0
#define CGI_STS_ATM_IFC_FD_OCTET_OUT   1
#define CGI_STS_ATM_IFC_FD_ERR         2
#define CGI_STS_ATM_IFC_FD_UNKNOWN     3
#define CGI_STS_ATM_IFC_FD_ERR_HEC     4
#define CGI_STS_ATM_IFC_FD_ERR_VCI     5
#define CGI_STS_ATM_IFC_FD_ERR_PORT    6
#define CGI_STS_ATM_IFC_FD_ERR_PTI     7
#define CGI_STS_ATM_IFC_FD_IDLE_CELL   8
#define CGI_STS_ATM_IFC_FD_ERR_TYPE    9
#define CGI_STS_ATM_IFC_FD_ERR_CRC     10
#define CGI_STS_ATM_IFC_FD_ERR_GFC     11

#define CGI_STS_AAL_IFC_FIELDS         8
#define CGI_STS_AAL_IFC_FD_OCTET_IN    0
#define CGI_STS_AAL_IFC_FD_OCTET_OUT   1
#define CGI_STS_AAL_IFC_FD_PKT_IN      2
#define CGI_STS_AAL_IFC_FD_PKT_OUT     3
#define CGI_STS_AAL_IFC_FD_ERR_IN      4
#define CGI_STS_AAL_IFC_FD_ERR_OUT     5
#define CGI_STS_AAL_IFC_FD_DISCARD_IN  6
#define CGI_STS_AAL_IFC_FD_DISCART_OUT 7

#define CGI_STS_ATM_VCC_TYPES          3
#define CGI_STS_ATM_VCC_TYPE_AAL5      0
#define CGI_STS_ATM_VCC_TYPE_AAL0_PCKT 1
#define CGI_STS_ATM_VCC_TYPE_AAL0_CELL 2

#define CGI_STS_VCC_AAL5_FIELDS        5
#define CGI_STS_VCC_AAL5_FD_ERR_CRC    0
#define CGI_STS_VCC_AAL5_FD_TIMEOUT    1
#define CGI_STS_VCC_AAL5_FD_SDU_SIZE   2
#define CGI_STS_VCC_AAL5_FD_ERR_PKT    3
#define CGI_STS_VCC_AAL5_FD_ERR_LEN    4

#define CGI_STS_VCC_AAL0_PKT_FIELDS        2
#define CGI_STS_VCC_AAL0_PKT_FD_TIMEOUT    0
#define CGI_STS_VCC_AAL0_PKT_FD_SDU_SIZE   1

#define CGI_STS_VCC_AAL0_CELL_FIELDS       1
#define CGI_STS_VCC_AAL0_CELL_FD_ERR_CRC   0

#define CGI_STS_ATM_CNTR_START      28

// ADSL statistics
#define CGI_STS_ADSL_LINE_TYPE_NONE        1
#define CGI_STS_ADSL_LINE_TYPE_FAST        2
#define CGI_STS_ADSL_LINE_TYPE_INTERLEAVE  3

#define CGI_STS_ADSL_LINE_ENTRY            0
#define CGI_STS_ADSL_LINE_CODING           0
#define CGI_STS_ADSL_LINE_TYPE             1

#define CGI_STS_ADSL_PHYS_ENTRY            1
#define CGI_STS_ADSL_ATUC_PHYS_ENTRY       8
#define CGI_STS_ADSL_PHYS_SNR_MGN          0
#define CGI_STS_ADSL_PHYS_ATN              1
#define CGI_STS_ADSL_PHYS_STATUS           2
#define CGI_STS_ADSL_PHYS_OUT_PWR          3
#define CGI_STS_ADSL_PHYS_ATT_RATE         4
#define CGI_STS_ADSL_PHYS_LINK_POWER       5

#define CGI_STS_ADSL_CHAN_ENTRY            2
#define CGI_STS_ADSL_CHAN_INTL_DELAY       0
#define CGI_STS_ADSL_CHAN_CURR_TX_RATE     1
#define CGI_STS_ADSL_CHAN_PREV_TX_RATE     2
#define CGI_STS_ADSL_CHAN_CRC_BLK_LEN      3

#define CGI_STS_ADSL_PERF_CNTR_ENTRY       3
#define CGI_STS_ADSL_PERF_CNTR_LOFS        0
#define CGI_STS_ADSL_PERF_CNTR_LOSS        1
#define CGI_STS_ADSL_PERF_CNTR_LOLS        2
#define CGI_STS_ADSL_PERF_CNTR_LPRS        3
#define CGI_STS_ADSL_PERF_CNTR_ESS         4
#define CGI_STS_ADSL_PERF_CNTR_INITS       5

#define CGI_STS_ADSL_CHAN_CNTR_ENTRY       4
#define CGI_STS_ADSL_CHAN_CNTR_RX_BLK      0
#define CGI_STS_ADSL_CHAN_CNTR_TX_BLK      1
#define CGI_STS_ADSL_CHAN_CNTR_CRR_BLK     2
#define CGI_STS_ADSL_CHAN_CNTR_ERR_BLK     3
#define CGI_STS_ADSL_CHAN_CNTR_RX_BYTE     4
#define CGI_STS_ADSL_CHAN_CNTR_TX_BYTE     5
#define CGI_STS_ADSL_CHAN_CNTR_CRR_BYTE    6
#define CGI_STS_ADSL_CHAN_CNTR_ERR_BYTE    7

#define CGI_STS_ADSL_CNCT_INFO_ENTRY       5
#define CGI_STS_ADSL_CNCT_INFO_CH_TYPE     0
#define CGI_STS_ADSL_CNCT_INFO_MD_TYPE     1
#define CGI_STS_ADSL_CNCT_INFO_CODING      2
#define CGI_STS_ADSL_CNCT_INFO_RX_K        3
#define CGI_STS_ADSL_CNCT_INFO_RX_S        4
#define CGI_STS_ADSL_CNCT_INFO_RX_R        5
#define CGI_STS_ADSL_CNCT_INFO_RX_D        6
#define CGI_STS_ADSL_CNCT_INFO_TX_K        7
#define CGI_STS_ADSL_CNCT_INFO_TX_S        8
#define CGI_STS_ADSL_CNCT_INFO_TX_R        9
#define CGI_STS_ADSL_CNCT_INFO_TX_D        10
#define CGI_STS_ADSL_CNCT_DATA_D_D         11
#define CGI_STS_ADSL_CNCT_DATA_D_U         12


#define CGI_STS_ADSL_CNCT_DATA_ENTRY       6
#define CGI_STS_ADSL_CNCT_DATA_RX_RS       0
#define CGI_STS_ADSL_CNCT_DATA_RX_RS_COR   1
#define CGI_STS_ADSL_CNCT_DATA_RX_RS_UNCOR 2
#define CGI_STS_ADSL_CNCT_DATA_RX_SF       3
#define CGI_STS_ADSL_CNCT_DATA_RX_SF_ERR   4
#define CGI_STS_ADSL_CNCT_DATA_TX_RS       5
#define CGI_STS_ADSL_CNCT_DATA_TX_RS_COR   6
#define CGI_STS_ADSL_CNCT_DATA_TX_RS_UNCOR 7
#define CGI_STS_ADSL_CNCT_DATA_TX_SF       8
#define CGI_STS_ADSL_CNCT_DATA_TX_SF_ERR   9

#define CGI_STS_ATM_CNCT_DATA_ENTRY        7
#define CGI_STS_VDSL_PERF_DATA_ATM_ENTRY   CGI_STS_ATM_CNCT_DATA_ENTRY
#define CGI_STS_ATM_CNCT_DATA_RX_HEC       0
#define CGI_STS_ATM_CNCT_DATA_RX_OCD       1
#define CGI_STS_ATM_CNCT_DATA_RX_LCD       2
#define CGI_STS_ATM_CNCT_DATA_RX_ES        3
#define CGI_STS_ATM_CNCT_DATA_TX_HEC       4
#define CGI_STS_ATM_CNCT_DATA_TX_OCD       5
#define CGI_STS_ATM_CNCT_DATA_TX_LCD       6
#define CGI_STS_ATM_CNCT_DATA_TX_ES        7
#define CGI_STS_ATM_CNCT_DATA_RX_CELL_TOT  8
#define CGI_STS_ATM_CNCT_DATA_RX_CELL_DATA 9  
#define CGI_STS_ATM_CNCT_DATA_RX_BIT_ERRS  10
#define CGI_STS_ATM_CNCT_DATA_TX_CELL_TOT  11
#define CGI_STS_ATM_CNCT_DATA_TX_CELL_DATA 12  
#define CGI_STS_ATM_CNCT_DATA_TX_BIT_ERRS  13
/* VDSL only */
#define CGI_STS_ATM_CNCT_DATA_RX_CELL_DROP 14  
#define CGI_STS_ATM_CNCT_DATA_TX_CELL_DROP 15  

#define CGI_STS_ATM_CNCT_DATA_BEARER_COUNT    (CGI_STS_ATM_CNCT_DATA_TX_CELL_DROP + 1)

#define CGI_STS_VOICE_DATA_ENTRY           8
#ifdef ENDPT_6345
/* Decoder */
#define CGI_STS_VOICE_DATA_PHT             0
#define CGI_STS_VOICE_DATA_PKTCNT          1
#define CGI_STS_VOICE_DATA_ADDTAILCNT      2
#define CGI_STS_VOICE_DATA_RDRCNT          3
#define CGI_STS_VOICE_DATA_ORUNCNT         4
#define CGI_STS_VOICE_DATA_DUPCNT          5
#define CGI_STS_VOICE_DATA_OORCNT          6
#define CGI_STS_VOICE_DATA_TOOLTCNT        7
#define CGI_STS_VOICE_DATA_CANTDCDCNT      8
#define CGI_STS_VOICE_DATA_AJCURCNT        9
#define CGI_STS_VOICE_DATA_AJCDELCNT       10
#define CGI_STS_VOICE_DATA_AJCREPCNT       11
#define CGI_STS_VOICE_DATA_AJCRESCNT       12
#define CGI_STS_VOICE_DATA_AJCPJCNT        13

/* Encoder */
#define CGI_STS_VOICE_DATA_INACTCNT        14
#define CGI_STS_VOICE_DATA_ACTCNT          15
#define CGI_STS_VOICE_DATA_SIDCNT          16
#define CGI_STS_VOICE_DATA_TRCNT           17

#else

#define CGI_STS_VOICE_DATA_PHT             0    /* Peak holding time */
#define CGI_STS_VOICE_DATA_MHT             1    /* Minimum holding time */
#define CGI_STS_VOICE_DATA_THT             2    /* Target holding time */
#define CGI_STS_VOICE_DATA_IFC             3    /* Inactive frame count */
#define CGI_STS_VOICE_DATA_AFC             4    /* Active frame count */
#define CGI_STS_VOICE_DATA_DOC             5    /* Decoder overrun count */
#define CGI_STS_VOICE_DATA_DUC             6    /* Decoder underrun count */
#define CGI_STS_VOICE_DATA_LFC             7    /* Lost frame count */
#define CGI_STS_VOICE_DATA_EOC             8    /* Encoder overrun count */
#endif

#define CGI_STS_ADSL_PERF_DATA_ENTRY       9
#define CGI_STS_ADSL_PERF_DATA_ES          0
#define CGI_STS_ADSL_PERF_DATA_SES         1
#define CGI_STS_ADSL_PERF_DATA_UAS         2
#define CGI_STS_ADSL_PERF_DATA_ES_TX       3
#define CGI_STS_ADSL_PERF_DATA_SES_TX      4
#define CGI_STS_ADSL_PERF_DATA_UAS_TX      5

#define CGI_STS_VDSL_CURR_15MIN_PERF_DATA_ES          0
#define CGI_STS_VDSL_CURR_15MIN_PERF_DATA_SES         1
#define CGI_STS_VDSL_CURR_15MIN_PERF_DATA_UAS         2
#define CGI_STS_VDSL_CURR_15MIN_PERF_DATA_ES_TX       3
#define CGI_STS_VDSL_CURR_15MIN_PERF_DATA_SES_TX      4
#define CGI_STS_VDSL_CURR_15MIN_PERF_DATA_UAS_TX      5

#define CGI_STS_VDSL_PREV_15MIN_PERF_DATA_ES          6
#define CGI_STS_VDSL_PREV_15MIN_PERF_DATA_SES         7
#define CGI_STS_VDSL_PREV_15MIN_PERF_DATA_UAS         8
#define CGI_STS_VDSL_PREV_15MIN_PERF_DATA_ES_TX       9
#define CGI_STS_VDSL_PREV_15MIN_PERF_DATA_SES_TX     10
#define CGI_STS_VDSL_PREV_15MIN_PERF_DATA_UAS_TX     11

#define CGI_STS_VDSL_CURR_1DAY_PERF_DATA_ES          12
#define CGI_STS_VDSL_CURR_1DAY_PERF_DATA_SES         13
#define CGI_STS_VDSL_CURR_1DAY_PERF_DATA_UAS         14
#define CGI_STS_VDSL_CURR_1DAY_PERF_DATA_ES_TX       15
#define CGI_STS_VDSL_CURR_1DAY_PERF_DATA_SES_TX      16
#define CGI_STS_VDSL_CURR_1DAY_PERF_DATA_UAS_TX      17

#define CGI_STS_VDSL_PREV_1DAY_PERF_DATA_ES          18
#define CGI_STS_VDSL_PREV_1DAY_PERF_DATA_SES         19
#define CGI_STS_VDSL_PREV_1DAY_PERF_DATA_UAS         20
#define CGI_STS_VDSL_PREV_1DAY_PERF_DATA_ES_TX       21
#define CGI_STS_VDSL_PREV_1DAY_PERF_DATA_SES_TX      22
#define CGI_STS_VDSL_PREV_1DAY_PERF_DATA_UAS_TX      23

#define CGI_STS_VDSL_CURR_15MIN_ANOMALY_DATA_BO_CV              32
#define CGI_STS_VDSL_CURR_15MIN_ANOMALY_DATA_BO_FEC             33
#define CGI_STS_VDSL_CURR_15MIN_ANOMALY_DATA_BO_CV_TX           34
#define CGI_STS_VDSL_CURR_15MIN_ANOMALY_DATA_BO_FEC_TX          35

#define CGI_STS_VDSL_CURR_15MIN_ANOMALY_DATA_B1_CV              36
#define CGI_STS_VDSL_CURR_15MIN_ANOMALY_DATA_B1_FEC             37
#define CGI_STS_VDSL_CURR_15MIN_ANOMALY_DATA_B1_CV_TX           38
#define CGI_STS_VDSL_CURR_15MIN_ANOMALY_DATA_B1_FEC_TX          39

#define CGI_STS_VDSL_PREV_15MIN_ANOMALY_DATA_BO_CV              40
#define CGI_STS_VDSL_PREV_15MIN_ANOMALY_DATA_BO_FEC             41
#define CGI_STS_VDSL_PREV_15MIN_ANOMALY_DATA_BO_CV_TX           42
#define CGI_STS_VDSL_PREV_15MIN_ANOMALY_DATA_BO_FEC_TX          43

#define CGI_STS_VDSL_PREV_15MIN_ANOMALY_DATA_B1_CV              44
#define CGI_STS_VDSL_PREV_15MIN_ANOMALY_DATA_B1_FEC             45
#define CGI_STS_VDSL_PREV_15MIN_ANOMALY_DATA_B1_CV_TX           46
#define CGI_STS_VDSL_PREV_15MIN_ANOMALY_DATA_B1_FEC_TX          47

#define CGI_STS_VDSL_CURR_1DAY_ANOMALY_DATA_BO_CV               48
#define CGI_STS_VDSL_CURR_1DAY_ANOMALY_DATA_BO_FEC              49
#define CGI_STS_VDSL_CURR_1DAY_ANOMALY_DATA_BO_CV_TX            50
#define CGI_STS_VDSL_CURR_1DAY_ANOMALY_DATA_BO_FEC_TX           51

#define CGI_STS_VDSL_CURR_1DAY_ANOMALY_DATA_B1_CV               52
#define CGI_STS_VDSL_CURR_1DAY_ANOMALY_DATA_B1_FEC              53
#define CGI_STS_VDSL_CURR_1DAY_ANOMALY_DATA_B1_CV_TX            54
#define CGI_STS_VDSL_CURR_1DAY_ANOMALY_DATA_B1_FEC_TX           55

#define CGI_STS_VDSL_PREV_1DAY_ANOMALY_DATA_BO_CV               56
#define CGI_STS_VDSL_PREV_1DAY_ANOMALY_DATA_BO_FEC              57
#define CGI_STS_VDSL_PREV_1DAY_ANOMALY_DATA_BO_CV_TX            58
#define CGI_STS_VDSL_PREV_1DAY_ANOMALY_DATA_BO_FEC_TX           59

#define CGI_STS_VDSL_PREV_1DAY_ANOMALY_DATA_B1_CV               60
#define CGI_STS_VDSL_PREV_1DAY_ANOMALY_DATA_B1_FEC              61
#define CGI_STS_VDSL_PREV_1DAY_ANOMALY_DATA_B1_CV_TX            62
#define CGI_STS_VDSL_PREV_1DAY_ANOMALY_DATA_B1_FEC_TX           63


#define CGI_STS_VDSL_CURR_15MIN_PERF_DATA_ATM_RX_HEC          0
#define CGI_STS_VDSL_CURR_15MIN_PERF_DATA_ATM_RX_OCD          1
#define CGI_STS_VDSL_CURR_15MIN_PERF_DATA_ATM_RX_LCD          2
#define CGI_STS_VDSL_CURR_15MIN_PERF_DATA_ATM_RX_ES           3
#define CGI_STS_VDSL_CURR_15MIN_PERF_DATA_ATM_TX_HEC          4
#define CGI_STS_VDSL_CURR_15MIN_PERF_DATA_ATM_TX_OCD          5
#define CGI_STS_VDSL_CURR_15MIN_PERF_DATA_ATM_TX_LCD          6
#define CGI_STS_VDSL_CURR_15MIN_PERF_DATA_ATM_TX_ES           7
#define CGI_STS_VDSL_CURR_15MIN_PERF_DATA_ATM_RX_CELL_TOT     8
#define CGI_STS_VDSL_CURR_15MIN_PERF_DATA_ATM_RX_CELL_DATA    9
#define CGI_STS_VDSL_CURR_15MIN_PERF_DATA_ATM_RX_BIT_ERRS    10
#define CGI_STS_VDSL_CURR_15MIN_PERF_DATA_ATM_TX_CELL_TOT    11
#define CGI_STS_VDSL_CURR_15MIN_PERF_DATA_ATM_TX_CELL_DATA   12
#define CGI_STS_VDSL_CURR_15MIN_PERF_DATA_ATM_TX_BIT_ERRS    13

#define CGI_STS_VDSL_PERF_DATA_ATM_RX_HEC          0
#define CGI_STS_VDSL_PERF_DATA_ATM_RX_OCD          1
#define CGI_STS_VDSL_PERF_DATA_ATM_RX_LCD          2
#define CGI_STS_VDSL_PERF_DATA_ATM_RX_ES           3
#define CGI_STS_VDSL_PERF_DATA_ATM_TX_HEC          4
#define CGI_STS_VDSL_PERF_DATA_ATM_TX_OCD          5
#define CGI_STS_VDSL_PERF_DATA_ATM_TX_LCD          6
#define CGI_STS_VDSL_PERF_DATA_ATM_TX_ES           7
#define CGI_STS_VDSL_PERF_DATA_ATM_RX_CELL_TOT     8
#define CGI_STS_VDSL_PERF_DATA_ATM_RX_CELL_DATA    9
#define CGI_STS_VDSL_PERF_DATA_ATM_RX_BIT_ERRS    10
#define CGI_STS_VDSL_PERF_DATA_ATM_TX_CELL_TOT    11
#define CGI_STS_VDSL_PERF_DATA_ATM_TX_CELL_DATA   12
#define CGI_STS_VDSL_PERF_DATA_ATM_TX_BIT_ERRS    13
#define CGI_STS_VDSL_PERF_DATA_ATM_RX_UTOPIA_OVERFLOW 14
#define CGI_STS_VDSL_PERF_DATA_ATM_TX_UTOPIA_OVERFLOW 15


#define CGI_STS_VDSL_CURR_15MIN_PERF_DATA_COUNT    (CGI_STS_VDSL_PERF_DATA_ATM_TX_UTOPIA_OVERFLOW + 1)
#define CGI_STS_VDSL_PREV_15MIN_PERF_DATA_COUNT    (CGI_STS_VDSL_CURR_15MIN_PERF_DATA_COUNT*2)
#define CGI_STS_VDSL_CURR_1DAY_PERF_DATA_COUNT     (CGI_STS_VDSL_CURR_15MIN_PERF_DATA_COUNT*3)
#define CGI_STS_VDSL_PREV_1DAY_PERF_DATA_COUNT     (CGI_STS_VDSL_CURR_15MIN_PERF_DATA_COUNT*4)

#define CGI_STS_VDSL_CURR_15MIN_PERF_DATA_OFFSET   (0)
#define CGI_STS_VDSL_PREV_15MIN_PERF_DATA_OFFSET   (CGI_STS_VDSL_CURR_15MIN_PERF_DATA_COUNT*1)
#define CGI_STS_VDSL_CURR_1DAY_PERF_DATA_OFFSET    (CGI_STS_VDSL_CURR_15MIN_PERF_DATA_COUNT*2)
#define CGI_STS_VDSL_PREV_1DAY_PERF_DATA_OFFSET    (CGI_STS_VDSL_CURR_15MIN_PERF_DATA_COUNT*3)

#define CGI_STS_VDSL_ATM_CNCT_DATA_BEARER_COUNT    (CGI_STS_VDSL_PREV_1DAY_PERF_DATA_COUNT)


// adsl2 parameters
#define CGI_STS_ADSL2_CNCT_DATA_ENTRY      10
#define CGI_STS_ADSL2_CNCT_DATA_MSGC_R     0
#define CGI_STS_ADSL2_CNCT_DATA_MSGC_X     1
#define CGI_STS_ADSL2_CNCT_DATA_B_R        2
#define CGI_STS_ADSL2_CNCT_DATA_B_X        3
#define CGI_STS_ADSL2_CNCT_DATA_M_R        4 
#define CGI_STS_ADSL2_CNCT_DATA_M_X        5
#define CGI_STS_ADSL2_CNCT_DATA_T_R        6   
#define CGI_STS_ADSL2_CNCT_DATA_T_X        7
#define CGI_STS_ADSL2_CNCT_DATA_R_R        8
#define CGI_STS_ADSL2_CNCT_DATA_R_X        9
#define CGI_STS_ADSL2_CNCT_DATA_S_R        10
#define CGI_STS_ADSL2_CNCT_DATA_S_X        11
#define CGI_STS_ADSL2_CNCT_DATA_L_R        12
#define CGI_STS_ADSL2_CNCT_DATA_L_X        13
#define CGI_STS_ADSL2_CNCT_DATA_D_R        14
#define CGI_STS_ADSL2_CNCT_DATA_D_X        15
#define CGI_STS_ADSL2_CNCT_DATA_D_D        16
#define CGI_STS_ADSL2_CNCT_DATA_D_U        17

// vdsl ptm parameters
#define CGI_STS_VDSL_PERF_DATA_PTM_ENTRY               11
#define CGI_STS_VDSL_PERF_DATA_PTM_RX_CV_P0             0
#define CGI_STS_VDSL_PERF_DATA_PTM_RX_CV_P1             1
#define CGI_STS_VDSL_PERF_DATA_PTM_RX_UTOPIA_OVERFLOW   2
#define CGI_STS_VDSL_PERF_DATA_PTM_TX_UTOPIA_OVERFLOW   3

#define CGI_STS_VDSL_CURR_15MIN_PERF_PTM_DATA_COUNT    (CGI_STS_VDSL_PERF_DATA_PTM_TX_UTOPIA_OVERFLOW + 1)
#define CGI_STS_VDSL_PREV_15MIN_PERF_PTM_DATA_COUNT    (CGI_STS_VDSL_CURR_15MIN_PERF_PTM_DATA_COUNT*2)
#define CGI_STS_VDSL_CURR_1DAY_PERF_PTM_DATA_COUNT     (CGI_STS_VDSL_CURR_15MIN_PERF_PTM_DATA_COUNT*3)
#define CGI_STS_VDSL_PREV_1DAY_PERF_PTM_DATA_COUNT     (CGI_STS_VDSL_CURR_15MIN_PERF_PTM_DATA_COUNT*4)

#define CGI_STS_VDSL_CURR_15MIN_PERF_PTM_DATA_OFFSET   (0)
#define CGI_STS_VDSL_PREV_15MIN_PERF_PTM_DATA_OFFSET   (CGI_STS_VDSL_CURR_15MIN_PERF_PTM_DATA_COUNT*1)
#define CGI_STS_VDSL_CURR_1DAY_PERF_PTM_DATA_OFFSET    (CGI_STS_VDSL_CURR_15MIN_PERF_PTM_DATA_COUNT*2)
#define CGI_STS_VDSL_PREV_1DAY_PERF_PTM_DATA_OFFSET    (CGI_STS_VDSL_CURR_15MIN_PERF_PTM_DATA_COUNT*3)

#define CGI_STS_VDSL_PTM_CNCT_DATA_BEARER_COUNT    (CGI_STS_VDSL_PREV_1DAY_PERF_PTM_DATA_COUNT)

/********************** Function Declaration ********************************/

void bcmStsIfcUpdate(void);
void bcmStsIfcGet(int ifc, int type, char *val);
void bcmStsIfcReset(char *devname);
void bcmStsWanReset(char *devname);
void bcmStsAtmUpdate(int reset);
void bcmStsAtmIfc(int ifc, int type, char *val);
void bcmStsAtmVcc(int ifc, int type, char *val);
void bcmStsAtmVccGet(UINT16 port, UINT16 vpi, UINT16 vci, int reset);
void bcmStsAtmIfcGet(int port, int reset);
char *strBcmStsAtmIfc(int type, int field, char *val);
char *strBcmStsAtmVcc(int type, int field, char *val);
void bcmStsAdslUpdate(void);
void bcmStsAdslUpdateLinkDown(void);
void bcmStsVdslUpdate(void);
void bcmStsVdsl(int entry, int field, char *val);
void bcmStsVdslUpdateLinkDown(void);
void bcmStsAdslLine(int field, char *val);
void bcmStsAdslPhys(int field, char *val);
void bcmStsAdslAtucPhys(int field, char *val);
void bcmStsAdslInterleave(int entry, int field, char *val);
void bcmStsAdslFast(int entry, int field, char *val);
void bcmStsAdslCntInfo(int field, char *val);
void bcmStsAdslDataInfo(int field, char *val);
void bcmStsAtmDataInfo(int field, char *val);
void bcmStsAdsl(int entry, int field, char *val);
void bcmStsAdsl2DataInfo(int field, char *val);


/** Dump LAN side statistics */
void cgiGetStsIfc_igd(char *varValue);
void cgiGetStsIfc_dev2(char *varValue);

void cgiStsWanBody_dev2(char *query __attribute__((unused)), FILE *fs);
void cgiResetStsWan_dev2(void);


void cgiWriteWanSts(FILE *fs, const char *interfaceDisplayStr, const char *serviceStr,
                      UINT32 rxTotalByte, UINT32 rxTotalPacket, 
                      UINT32 rxTotalErr, UINT32 rxTotalDrop,
                      UINT32 rxMulticastByte, UINT32 rxMulticastPacket,
                      UINT32 rxUnicastPacket, UINT32 rxBroadcastPacket,
                      UINT32 txTotalByte, UINT32 txTotalPacket, 
                      UINT32 txTotalErr, UINT32 txTotalDrop,
                      UINT32 txMulticastByte, UINT32 txMulticastPacket,
                      UINT32 txUnicastPacket, UINT32 txBroadcastPacket);
                      
/** Clear LAN side statistics */
void cgiResetStsIfc_igd(void);
void cgiResetStsIfc_dev2(void);

/** Dump Wifi LAN side statistics */
void cgiGetStsWifiIfc(char **pp);
void cgiGetStsWifiIfc_igd(char **pp);
void cgiGetStsWifiIfc_dev2(char **pp);

#if defined(SUPPORT_DM_LEGACY98)
#define cgiGetStsWifiIfc(pp)         cgiGetStsWifiIfc_igd((pp))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiGetStsWifiIfc(pp)         cgiGetStsWifiIfc_igd((pp))
#elif defined(SUPPORT_DM_PURE181)
#define cgiGetStsWifiIfc(pp)         cgiGetStsWifiIfc_dev2((pp))
#elif defined(SUPPORT_DM_DETECT)
#define cgiGetStsWifiIfc(pp)         (cmsMdm_isDataModelDevice2() ? \
                                      cgiGetStsWifiIfc_dev2((pp)) : \
                                      cgiGetStsWifiIfc_igd((pp)))
#endif


/** Reset statistics on all LAN side instances of Wifi interface */
void cgiResetStsWifiLan(void);
void cgiResetStsWifiLan_igd(void);
void cgiResetStsWifiLan_dev2(void);

#if defined(SUPPORT_DM_LEGACY98)
#define cgiResetStsWifiLan()       cgiResetStsWifiLan_igd()
#elif defined(SUPPORT_DM_HYBRID)
#define cgiResetStsWifiLan()       cgiResetStsWifiLan_igd()
#elif defined(SUPPORT_DM_PURE181)
#define cgiResetStsWifiLan()       cgiResetStsWifiLan_dev2()
#elif defined(SUPPORT_DM_DETECT)
#define cgiResetStsWifiLan()       (cmsMdm_isDataModelDevice2() ? \
                                     cgiResetStsWifiLan_dev2() : \
                                     cgiResetStsWifiLan_igd())
#endif




/** This function displays Device info WAN status
 */
void cgiDeviceInfoWanStatus(FILE *fs);


/** Traverse appropriate data model to display Device Info WAN status.
 *
 */
void cgiDeviceInfoWanStatusBody(FILE *fs);
void cgiDeviceInfoWanStatusBody_igd(FILE *fs);
void cgiDeviceInfoWanStatusBody_dev2(FILE *fs);

#if defined(SUPPORT_DM_LEGACY98)
#define cgiDeviceInfoWanStatusBody(f)   cgiDeviceInfoWanStatusBody_igd((f))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiDeviceInfoWanStatusBody(f)   cgiDeviceInfoWanStatusBody_igd((f))
#elif defined(SUPPORT_DM_PURE181)
#define cgiDeviceInfoWanStatusBody(f)   cgiDeviceInfoWanStatusBody_dev2((f))
#elif defined(SUPPORT_DM_DETECT)
#define cgiDeviceInfoWanStatusBody(f)         (cmsMdm_isDataModelDevice2() ? \
                                      cgiDeviceInfoWanStatusBody_dev2((f)) : \
                                      cgiDeviceInfoWanStatusBody_igd((f)))
#endif


/** helper function for cgiDeviceInfoWanStatusBody
 */
void cgiDisplayWanStatusEntry(FILE *fs, char *srvcStr,
             char *ifcStr, char *protoStr, SINT32 vlanId,
             UBOOL8 ipv6Enabled,
             UBOOL8 igmpEnabled, UBOOL8 igmpSourceEnabled,
             UBOOL8 natEnabled, UBOOL8 fwEnabled,
             char *status, char *ipAddr,
             UBOOL8 mldEnabled, UBOOL8 mldSourceEnabled, 
             char *ipv6status,char *ipv6Addr);

#endif  /* __CGI_STS_H__ */

