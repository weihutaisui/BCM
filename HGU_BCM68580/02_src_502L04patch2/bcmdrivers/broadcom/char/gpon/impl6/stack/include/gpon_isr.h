

/*
* <:copyright-BRCM:2016:proprietary:gpon
* 
*    Copyright (c) 2016 Broadcom 
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


#ifndef GPON_ISR_H_INCLUDED
#define GPON_ISR_H_INCLUDED

#include <linux/types.h>
#include "pon_sm_common.h"
#include "gpon_all.h"

typedef struct
{ 
    /* reserved */
    uint32_t r1                            :7;
    /* rogue_onu diff*/                        
    uint32_t rogue_onu_diff                :1;
    /* rogue_onu level */                      
    uint32_t rogue_onu_level               :1;
    /* overrun occured */
    uint32_t overrun_occured_irq           :1;
    /* DBA39 transmitted */
    uint32_t dba39_report_transmitted_irq  :2;
    /* DBA38 transmitted */
    uint32_t dba38_report_transmitted_irq  :2;
    /* DBA37 transmitted */
    uint32_t dba37_report_transmitted_irq  :2;
    /* DBA36 transmitted */
    uint32_t dba36_report_transmitted_irq  :2;
    /* DBA35 transmitted */
    uint32_t dba35_report_transmitted_irq  :2;
    /* DBA34 transmitted */
    uint32_t dba34_report_transmitted_irq  :2;
    /* DBA33 transmitted */
    uint32_t dba33_report_transmitted_irq  :2;
    /* DBA32 transmitted */
    uint32_t dba32_report_transmitted_irq  :2;
    /* DBA31 transmitted */
    uint32_t dba31_report_transmitted_irq  :2;
    /* DBA30 transmitted */
    uint32_t dba30_report_transmitted_irq  :2;
    /* DBA29 transmitted */
    uint32_t dba29_report_transmitted_irq  :2;
    /* DBA28 transmitted */
    uint32_t dba28_report_transmitted_irq  :2;    
    /* DBA27 transmitted */
    uint32_t dba27_report_transmitted_irq  :2;
    /* DBA26 transmitted */
    uint32_t dba26_report_transmitted_irq  :2;
    /* DBA25 transmitted */
    uint32_t dba25_report_transmitted_irq  :2;
    /* DBA24 transmitted */
    uint32_t dba24_report_transmitted_irq  :2;
    /* DBA23 transmitted */
    uint32_t dba23_report_transmitted_irq  :2;
    /* DBA22 transmitted */
    uint32_t dba22_report_transmitted_irq  :2;
    /* DBA21 transmitted */
    uint32_t dba21_report_transmitted_irq  :2;
    /* DBA20 transmitted */
    uint32_t dba20_report_transmitted_irq  :2;
    /* DBA19 transmitted */
    uint32_t dba19_report_transmitted_irq  :2;
    /* DBA18 transmitted */
    uint32_t dba18_report_transmitted_irq  :2;
    /* DBA17 transmitted */
    uint32_t dba17_report_transmitted_irq  :2;
    /* DBA16 transmitted */
    uint32_t dba16_report_transmitted_irq  :2;
    /* DBA15 transmitted */
    uint32_t dba15_report_transmitted_irq  :2;
    /* DBA14 transmitted */
    uint32_t dba14_report_transmitted_irq  :2;
    /* DBA13 transmitted */
    uint32_t dba13_report_transmitted_irq  :2;
    /* DBA12 transmitted */
    uint32_t dba12_report_transmitted_irq  :2;
    /* DBA11 transmitted */
    uint32_t dba11_report_transmitted_irq  :2;
    /* DBA10 transmitted */
    uint32_t dba10_report_transmitted_irq  :2;
    /* DBA9 transmitted */
    uint32_t dba9_report_transmitted_irq   :2;
    /* DBA8 transmitted */                 
    uint32_t dba8_report_transmitted_irq   :2;
    /* DBA7 transmitted */                 
    uint32_t dba7_report_transmitted_irq   :2;
    /* DBA6 transmitted */                 
    uint32_t dba6_report_transmitted_irq   :2;
    /* DBA5 transmitted */                 
    uint32_t dba5_report_transmitted_irq   :2;
    /* DBA4 transmitted */                 
    uint32_t dba4_report_transmitted_irq   :2;
    /* DBA3 transmitted */                 
    uint32_t dba3_report_transmitted_irq   :2;
    /* DBA2 transmitted */                 
    uint32_t dba2_report_transmitted_irq   :2;
    /* DBA1 transmitted */                 
    uint32_t dba1_report_transmitted_irq   :2;
    /* DBA0 transmitted */                 
    uint32_t dba0_report_transmitted_irq   :2;
    /* Ranging PLOAM transmitted */
    uint32_t ranging_ploam_transmitted_irq :1;    
    /* Idle PLOAM transmitted */
    uint32_t idle_ploam_transmitted_irq    :1;
    /* Urgent PLOAM transmitted */
    uint32_t urgent_ploam_transmitted_irq  :1;
    /* Normal PLOAM transmitted */         
    uint32_t normal_ploam_transmitted_irq  :1;
    /* Access FIFO error */                
    uint32_t access_fifo_error_irq         :1;
    /* TDM FIFO error */                   
    uint32_t tdm_fifo_error_irq            :1;   
}
GPON_TX_ISR;

typedef enum
{
    txisr0_accirq_tdmirq_idx = 0,
    txisr0_uplmirq_nplmirq_idx,
    txisr0_rplmirq_iplmirq_idx,
    txisr0_db0irq_idx,
    txisr0_db1irq_idx,
    txisr0_db2irq_idx,
    txisr0_db3irq_idx,
    txisr0_db4irq_idx,
    txisr0_db5irq_idx,
    txisr0_db6irq_idx,
    txisr0_db7irq_idx,
    txisr0_db8irq_idx,
    txisr0_db9irq_idx,
    txisr0_db10irq_idx,
    txisr0_db11irq_idx,
    txisr0_db12irq_idx,
    txisr0_idx_num
}gpon_tx_txisr0_idxes;

typedef enum
{
    txisr1_db13irq_idx = 0,
    txisr1_db14irq_idx,
    txisr1_db15irq_idx,
    txisr1_db16irq_idx,
    txisr1_db17irq_idx,
    txisr1_db18irq_idx,
    txisr1_db19irq_idx,
    txisr1_db20irq_idx,
    txisr1_db21irq_idx,
    txisr1_db22irq_idx,
    txisr1_db23irq_idx,
    txisr1_db24irq_idx,
    txisr1_db25irq_idx,
    txisr1_db26irq_idx,
    txisr1_db27irq_idx,
    txisr1_db28irq_idx,
    txisr1_idx_num,
}gpon_tx_txisr1_idxes;

typedef enum
{
    txisr2_db29irq_idx = 0,
    txisr2_db30irq_idx,
    txisr2_db31irq_idx,
    txisr2_db32irq_idx,
    txisr2_db33irq_idx,
    txisr2_db34irq_idx,
    txisr2_db35irq_idx,
    txisr2_db36irq_idx,
    txisr2_db37irq_idx,
    txisr2_db38irq_idx,
    txisr2_db39irq_idx,
    txisr2_rouge_level_ovrirq_idx,
    txisr2_reserve_rouge_diff_idx,
    txisr2_idx_num,
}gpon_tx_txisr2_idxes;


/* Define the Operation state machine struct */
typedef struct
{
    bool rogue_state;
    BL_ROGUE_ONU_MODE_DTE rogue_type;
}
OPERATION_ROGUE_INDICATION_PARAMS_DTS;


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_isr                                                                 */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON - Interrupt handler                                                 */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*     PON_NO_ERROR - Success                                                 */
/*     PON_GENERAL_ERROR - General error                                      */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_isr(void *xi_ploam_message_ptr, 
    PON_INTERRUPTS_MASK_DTE xi_interrupt_vector);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   get_tx_pon_pending_interrupts                                            */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON Tx - Get pending Gpon Tx interrupts.                                */
/*                                                                            */
/* Abstract:                                                                  */
/*   Returns a vector of currently pending Gpon Tx interrupts.                */
/*                                                                            */
/* Input:                                                                     */                                                                                          
/*                                                                            */         
/* Output:                                                                    */
/*                                                                            */
/*  xo_gpon_tx_isr_vector - vector of currently pending Gpon Tx interrupts.   */ 
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*     PON_NO_ERROR - Success                                                 */
/*     PON_GENERAL_ERROR - General error                                      */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE get_tx_pon_pending_interrupts(GPON_TX_ISR *xo_gpon_tx_isr);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_cfg_rogue_onu_interrupts                                            */
/*                                                                            */
/* Abstract:                                                                  */
/*  Clear/Mask/Unmask interrupts related to Rogue ONU detection               */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*  cmd - Clear/Mask/Unmask                                                   */
/*  rogue_onu_diff - if non-zero clear Rogue ONU Diff interrupt.              */
/*  rogue_onu_level - if non-zero clear Rogue ONU Level interrupt.            */
/* Output:                                                                    */
/*                                                                            */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*     PON_NO_ERROR - Success                                                 */
/*     PON_GENERAL_ERROR - General error                                      */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE  gpon_cfg_rogue_onu_interrupts (PON_ROGUE_ONU_TX_INT_CMD cmd, 
   int rogue_onu_diff, int rogue_onu_level);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_clear_specific_tx_pon_interrupts                                    */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*  GPON -  Clears Specific Gpon Tx interrupts                                */
/*                                                                            */
/* Abstract:                                                                  */
/*  Clears Gpon Tx interrupts according to received interrupt status          */
/*  register vector and keep the previous status.                             */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*  xi_gpon_tx_isr_vector - vector of Gpon Tx interrupts to clear.            */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */                                                                           
/*   PON_ERROR_DTE - Return code                                              */
/*     PON_NO_ERROR - Success                                                 */
/*     PON_GENERAL_ERROR - General error                                      */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_clear_specific_tx_pon_interrupts(GPON_TX_ISR xi_gpon_tx_isr_vector);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_clear_tx_pon_interrupts                                             */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON Tx - Clears GPON Tx interrupts                                      */
/*                                                                            */
/* Abstract:                                                                  */
/*   Clears Gpon Tx interrupts according to received interrupt status register*/
/*                                                                            */
/* Input:                                                                     */                                                                                          
/*                                                                            */
/*  xi_gpon_tx_isr_vector - vector of Gpon Tx interrupts to clear.            */          
/*                                                                            */
/* Output:                                                                    */
/*   PON_ERROR_DTE - Return code                                              */
/*     PON_NO_ERROR - Success                                                 */
/*     PON_GENERAL_ERROR - General error                                      */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_clear_tx_pon_interrupts(GPON_TX_ISR xi_gpon_tx_isr_vector);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_unmask_tx_pon_interrupts                                            */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON Tx - Unmasks all Gpon Tx interrupts                                 */
/*                                                                            */
/* Input:                                                                     */                                                                                          
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*     PON_NO_ERROR - Success                                                 */
/*     PON_GENERAL_ERROR - General error                                      */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_unmask_tx_pon_interrupts(void);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_unmask_rx_pon_interrupts                                            */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON Rx - Unmasks Gpon Rx Interrupts                                     */
/*                                                                            */
/* Abstract:                                                                  */
/*   GPON Rx - Unmasks all Gpon Rx interrupts that are handled by the stack.  */
/*                                                                            */
/* Input:                                                                     */                                                                                          
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*     PON_NO_ERROR - Success                                                 */
/*     PON_GENERAL_ERROR - General error                                      */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_unmask_rx_pon_interrupts(void);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_unmask_tx_pon_specific_interrupts                                   */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON Tx - Unmasks specific Gpon Tx interrupts.                           */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*     PON_NO_ERROR - Success                                                 */
/*     PON_GENERAL_ERROR - General error                                      */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_unmask_tx_pon_specific_interrupts(GPON_TX_ISR tx_ier_vector);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_mask_tx_pon_specific_interrupts                                     */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON Tx - Masks specific Gpon Tx interrupts.                             */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*     PON_NO_ERROR - Success                                                 */
/*     PON_GENERAL_ERROR - General error                                      */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_mask_tx_pon_specific_interrupts(GPON_TX_ISR tx_ier_vector);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_mask_tx_pon_interrupts                                              */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON Tx - Masks all Gpon Tx interrupts.                                  */
/*                                                                            */
/* Input:                                                                     */                                                                     
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*     PON_NO_ERROR - Success                                                 */
/*     PON_GENERAL_ERROR - General error                                      */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_mask_tx_pon_interrupts(void);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   get_rx_pon_pending_interrupts                                            */
/*                                                                            */
/* Title:                                                                     */
/*  GPON Rx - Get PON Pending interrups                                       */
/*                                                                            */
/*                                                                            */
/* Abstract:                                                                  */
/*  Returns a vector of currently pending Gpon Rx interrupts.                 */
/*                                                                            */
/* Input:                                                                     */                                                                                          
/*                                                                            */         
/* Output:                                                                    */
/*                                                                            */
/*  xo_gpon_rx_isr_vector - vector of currently pending Gpon Rx interrupts.   */ 
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*     PON_NO_ERROR - Success                                                 */
/*     PON_GENERAL_ERROR - General error                                      */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE get_rx_pon_pending_interrupts(gpon_rx_irq_grxisr *xo_gpon_rx_isr_vector);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_clear_rx_pon_interrupts                                             */
/*                                                                            */
/* Title:                                                                     */
/*  GPON - Clears Gpon Rx Interrupts                                          */
/*                                                                            */
/* Abstract:                                                                  */
/*  GPON - Clears Gpon Rx interrupts according to received  interrupt         */
/*  status register vector.                                                   */
/*                                                                            */
/* Input:                                                                     */                                                                                          
/*                                                                            */
/*  xi_gpon_rx_isr_vector - vector of Gpon Rx interrupts to clear.            */          
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*     PON_NO_ERROR - Success                                                 */
/*     PON_GENERAL_ERROR - General error                                      */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_clear_rx_pon_interrupts(gpon_rx_irq_grxisr const *xi_gpon_rx_isr_vector);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_mask_rx_pon_interrupts                                              */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   Rx GPON -  Masks all Gpon Rx interrupts.                                 */
/*                                                                            */
/* Input:                                                                     */                                                                     
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*     PON_NO_ERROR - Success                                                 */
/*     PON_GENERAL_ERROR - General error                                      */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_mask_rx_pon_interrupts(void);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_are_rx_pon_interrupts_masked                                        */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*  GPON -  Are Rx interrupts masked?.                                        */
/*                                                                            */
/* Abstract:                                                                  */
/*   Checks whether all Gpon Rx interrupts are currently masked.              */
/*                                                                            */
/* Input:                                                                     */                                                                     
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*  xo_are_interrupts_masked - TRUE if all RX Pon interrupts are currently    */
/*                             masked and FALSE otherwise.                    */ 
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*     PON_NO_ERROR - Success                                                 */
/*     PON_GENERAL_ERROR - General error                                      */
/******************************************************************************/
PON_ERROR_DTE gpon_are_rx_pon_interrupts_masked(bool *xo_are_interrupts_masked);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_show_rx_irs                                                         */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   BL GPON - Show Rx interrupt registers: Enable, Status                    */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function shows Rx interrupt registers: Enable, Status               */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*    None                                                                    */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_show_rx_irs(void);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_int_lof_disable                                                     */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON - Disable interrupt: LOF                                            */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function disables interrupt: LOF                                    */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*    None                                                                    */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_int_lof_disable(void);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_int_lcdg_disable                                                    */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON - Disable interrupt: LCGD                                           */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function disables interrupt: LCGD                                   */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*    None                                                                    */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_int_lcdg_disable(void);

/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_int_lcdg_enable                                                     */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON - Enable interrupt: LCGD                                            */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function enables interrupt: LCGD                                    */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/*    None                                                                    */
/*                                                                            */
/* Output:                                                                    */
/*                                                                            */
/*   PON_ERROR_DTE - Return code                                              */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_int_lcdg_enable(void);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_tx_clear_interrupt_status_register                                  */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON Tx - Clear Interrupt status register                                */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function clear the interrupt status register.                       */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_tx_clear_interrupt_status_register(void);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_tx_generate_interrupt_test_register                                 */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON Tx - Generate Interrupt test register                               */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function generate interrupt in the interrupt test register          */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_tx_generate_interrupt_test_register(GPON_TX_ISR gpon_tx_itr);


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_rx_clear_interrupt_status_reg                                       */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON Rx - Clear ISR                                                      */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function clear the interrupt status register                        */
/*                                                                            */
/******************************************************************************/
PON_ERROR_DTE gpon_rx_clear_interrupt_status_reg(void);


/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/* Calculate OMCI CRC                                                         */
/*                                                                            */
/******************************************************************************/
uint32_t gpon_calc_omci_crc(uint8_t *buffer, uint32_t length);


/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/* Prepare table for OMCI CRC caclulations                                    */
/*                                                                            */
/******************************************************************************/
void gpon_init_crc32_table_for_omci(void);

#endif

