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

#include <boardparms.h>
#include "gpon_db.h"
#include "gpon_all.h"
#include "gpon_logger.h"
#include "gpon_general.h"
#include "wan_drv.h"
#ifdef CONFIG_BCM_GPON_TODD
#include "gpon_tod_common.h"
#endif

/* #define UNIT_TEST_GPON_INTERRUPTS */
extern GPON_DATABASE_DTS gs_gpon_database;

static void p_dump_ds_ploam(GPON_DS_PLOAM *ploam_ptr);
static PON_ERROR_DTE gpon_tx_set_interrupt_enable_register(GPON_TX_ISR gpon_tx_ier);


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
PON_ERROR_DTE gpon_are_rx_pon_interrupts_masked(bool  *xo_are_interrupts_masked)
{
    /* vector with all interrupts masked*/
    static const gpon_rx_irq_grxisr rx_ier_masked_vector = {};
    /* vector of currenlty enabled interrupts */
    gpon_rx_irq_grxier  rx_current_ier_vector = {};
    int drv_error;

    /* verify validity of parameters */ 
    if (xo_are_interrupts_masked == NULL)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, " output parameter is NULL ");
        return PON_ERROR_INVALID_PARAMETER ;
    }

    /* get current rx interrupt enable register status*/
    drv_error = ag_drv_gpon_rx_irq_grxier_get(&rx_current_ier_vector);
    if (drv_error > 0)
    {
        p_log (ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Failed read Rx interrupt enable register due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

    /* check whether all Rx interrupts are currently masked and return the result*/
    * xo_are_interrupts_masked = ! memcmp(& rx_current_ier_vector ,
        &rx_ier_masked_vector, sizeof(rx_current_ier_vector));

#ifdef UNIT_TEST_GPON_INTERRUPTS
    p_log (ge_onu_logger.sections.isr.debug.rx_event_id, 
        "gpon_are_rx_pon_interrupts_masked = %d, curr ier=0x%08x, ier=0x%08x",
        *xo_are_interrupts_masked, rx_current_ier_vector, rx_ier_masked_vector);
#endif 

    return PON_NO_ERROR;
}

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
PON_ERROR_DTE gpon_mask_rx_pon_interrupts(void)
{
    gpon_rx_irq_grxier grxier = {};
    int drv_error;
    
    /* set rx interrupt enable register to 0 */
    drv_error = ag_drv_gpon_rx_irq_grxier_set(&grxier);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Failed to mask Rx interrupts due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

#ifdef UNIT_TEST_GPON_INTERRUPTS
    printk(KERN_INFO "gpon_mask_rx_pon_interrupts");
#endif 

    return PON_NO_ERROR;
}


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
PON_ERROR_DTE gpon_unmask_rx_pon_interrupts(void)
{
    int drv_error;

#ifdef UNIT_TEST_GPON_INTERRUPTS
    p_log(ge_onu_logger.sections.isr.debug.rx_event_id, 
        "gpon_unmask_rx_pon_interrupts, ier=0x%08x", gs_gpon_database.rx_ier_vector);
#endif 
    
    /* Set rx interrupt enable register status, enable interrupts that should be handled by the stack */
    drv_error = ag_drv_gpon_rx_irq_grxier_set(&gs_gpon_database.rx_ier_vector);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Failed to unmask Rx interrupts due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }
    
    return PON_NO_ERROR;
}

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
PON_ERROR_DTE gpon_clear_rx_pon_interrupts(gpon_rx_irq_grxisr const *xi_gpon_rx_isr_vector)
{
    int drv_error;

#ifdef UNIT_TEST_GPON_INTERRUPTS
    p_log(ge_onu_logger.sections.isr.debug.rx_event_id, 
        "gpon_clear_rx_pon_interrupts isr=0x%08x, ier=0x%08x",
         xi_gpon_rx_isr_vector, gs_gpon_database.rx_ier_vector);
#endif 

    /* Clear the Rx interrupts, according to the received ISR vector */
    drv_error = ag_drv_gpon_rx_irq_grxisr_set(xi_gpon_rx_isr_vector);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Failed to clear Rx interrupts due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }

#ifdef UNIT_TEST_GPON_INTERRUPTS
//     ag_drv_gpon_rx_irq_grxitr_set(xi_gpon_rx_isr_vector);
#endif 
    
    return PON_NO_ERROR;
}


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
PON_ERROR_DTE get_rx_pon_pending_interrupts(gpon_rx_irq_grxisr *xo_gpon_rx_isr_vector)
{
    int drv_error;
    
    /* verify validity of parameters */ 
    if (xo_gpon_rx_isr_vector == NULL)
    {
        return PON_ERROR_INVALID_PARAMETER;
    }
      
    /* get the rx interrupt status register vector */
    memset(xo_gpon_rx_isr_vector, BDMF_FALSE, sizeof(gpon_rx_irq_grxisr));

    drv_error = ag_drv_gpon_rx_irq_grxisr_get(xo_gpon_rx_isr_vector);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Failed to read Rx interrupts due to driver error %d.", drv_error);
      return  PON_ERROR_DRIVER_ERROR ;
    }

    /* cross the status register vector with interrupts that should be handled by the stack,
       to receive the currently pending interrupts */
    *(uint32_t * ) xo_gpon_rx_isr_vector &=(*(uint32_t *) &gs_gpon_database.rx_ier_vector);

#ifdef UNIT_TEST_GPON_INTERRUPTS
    p_log(ge_onu_logger.sections.isr.debug.rx_event_id, 
        "get_rx_pon_pending_interrupts isr=0x%08x, ier=0x%08x",
        *xo_gpon_rx_isr_vector, gs_gpon_database.rx_ier_vector);
#endif 

     return PON_NO_ERROR;
}

       
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
PON_ERROR_DTE gpon_mask_tx_pon_interrupts(void)
{
    GPON_TX_ISR gpon_tx_ier = {};
    
    gpon_tx_set_interrupt_enable_register(gpon_tx_ier);

#ifdef UNIT_TEST_GPON_INTERRUPTS
   printk(KERN_INFO  "gpon_mask_tx_pon_interrupts");
#endif 

    return PON_NO_ERROR;
}


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
PON_ERROR_DTE gpon_mask_tx_pon_specific_interrupts(GPON_TX_ISR tx_ier_vector)
{
    /* Update the tx ier DB vector */
   (( uint32_t *)&gs_gpon_database.tx_ier_vector )[0] &=(~((uint32_t *) &tx_ier_vector)[0]);
   (( uint32_t *)&gs_gpon_database.tx_ier_vector )[1] &=(~((uint32_t *) &tx_ier_vector)[1]);
   (( uint32_t *)&gs_gpon_database.tx_ier_vector )[2] &=(~((uint32_t *) &tx_ier_vector)[2]);

    gpon_tx_set_interrupt_enable_register(gs_gpon_database.tx_ier_vector);

#ifdef UNIT_TEST_GPON_INTERRUPTS
   printk(KERN_INFO  "gpon_mask_tx_pon_interrupts");
#endif 

    return PON_NO_ERROR;
}

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
PON_ERROR_DTE gpon_unmask_tx_pon_specific_interrupts(GPON_TX_ISR tx_ier_vector)
{
    /* Update the tx ier DB vector */
   (( uint32_t *) &gs_gpon_database.tx_ier_vector )[0] |=((uint32_t *) &tx_ier_vector)[0];
   (( uint32_t *) &gs_gpon_database.tx_ier_vector )[1] |=((uint32_t *) &tx_ier_vector)[1];
   (( uint32_t *) &gs_gpon_database.tx_ier_vector )[2] |=((uint32_t *) &tx_ier_vector)[2];

    gpon_tx_set_interrupt_enable_register(gs_gpon_database.tx_ier_vector);

#ifdef UNIT_TEST_GPON_INTERRUPTS
   printk(KERN_INFO  "gpon_mask_tx_pon_interrupts");
#endif 

    return PON_NO_ERROR;
}

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
PON_ERROR_DTE gpon_unmask_tx_pon_interrupts(void)
{
#ifdef UNIT_TEST_GPON_INTERRUPTS
    uint32_t  *up;
#endif
    gpon_tx_set_interrupt_enable_register(gs_gpon_database.tx_ier_vector);

#ifdef UNIT_TEST_GPON_INTERRUPTS
    up = (uint32_t*) &gs_gpon_database.tx_ier_vector ;
   printk(KERN_INFO  
         "gpon_unmask_tx_pon_interrupts ier=0x%08x, 0x%08x, 0x%08x",
         up[0], up[1], up[2]);
#endif
    
    return PON_NO_ERROR;
}

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
PON_ERROR_DTE gpon_clear_tx_pon_interrupts(GPON_TX_ISR xi_gpon_tx_isr_vector)
{
#ifdef UNIT_TEST_GPON_INTERRUPTS
    static const GPON_TX_ISR tx_ier_vector = {};
   printk(KERN_INFO  
        "gpon_clear_tx_pon_interrupts isr=0x%08x, 0x%08x, 0x%08x, ier=0x%08x, 0x%08x, 0x%08x",
        ((uint32_t *)&xi_gpon_tx_isr_vector)[0], ((uint32_t *)&xi_gpon_tx_isr_vector)[1], ((uint32_t*)&xi_gpon_tx_isr_vector)[2],
        ((uint32_t *)&gs_gpon_database.tx_ier_vector)[0], ((uint32_t*)&gs_gpon_database.tx_ier_vector)[1], 
        ((uint32_t*)&gs_gpon_database.tx_ier_vector)[2]);
#endif 
   
#ifdef _BYTE_ORDER_LITTLE_ENDIAN_
    ag_drv_gpon_tx_interrupt_controller_txisr0_set(txisr0_accirq_tdmirq_idx, 
        (xi_gpon_tx_isr_vector.tdm_fifo_error_irq&1<<1) | (xi_gpon_tx_isr_vector.access_fifo_error_irq&1));          
    ag_drv_gpon_tx_interrupt_controller_txisr0_set(txisr0_uplmirq_nplmirq_idx, 
        (xi_gpon_tx_isr_vector.normal_ploam_transmitted_irq&1<<1) | (xi_gpon_tx_isr_vector.urgent_ploam_transmitted_irq&1));          
    ag_drv_gpon_tx_interrupt_controller_txisr0_set(txisr0_rplmirq_iplmirq_idx, 
        (xi_gpon_tx_isr_vector.idle_ploam_transmitted_irq&1<<1) | (xi_gpon_tx_isr_vector.ranging_ploam_transmitted_irq&1)); 
#else
    ag_drv_gpon_tx_interrupt_controller_txisr0_set(txisr0_accirq_tdmirq_idx, 
        (xi_gpon_tx_isr_vector.tdm_fifo_error_irq&1) | (xi_gpon_tx_isr_vector.access_fifo_error_irq&1<<1));          
    ag_drv_gpon_tx_interrupt_controller_txisr0_set(txisr0_uplmirq_nplmirq_idx, 
        (xi_gpon_tx_isr_vector.normal_ploam_transmitted_irq&1) | (xi_gpon_tx_isr_vector.urgent_ploam_transmitted_irq&1<<1));          
    ag_drv_gpon_tx_interrupt_controller_txisr0_set(txisr0_rplmirq_iplmirq_idx, 
        (xi_gpon_tx_isr_vector.idle_ploam_transmitted_irq&1) | (xi_gpon_tx_isr_vector.ranging_ploam_transmitted_irq&1<<1)); 
#endif              
    ag_drv_gpon_tx_interrupt_controller_txisr0_set(txisr0_db0irq_idx, xi_gpon_tx_isr_vector.dba0_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txisr0_set(txisr0_db1irq_idx, xi_gpon_tx_isr_vector.dba1_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txisr0_set(txisr0_db2irq_idx, xi_gpon_tx_isr_vector.dba2_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txisr0_set(txisr0_db3irq_idx, xi_gpon_tx_isr_vector.dba3_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txisr0_set(txisr0_db4irq_idx, xi_gpon_tx_isr_vector.dba4_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txisr0_set(txisr0_db5irq_idx, xi_gpon_tx_isr_vector.dba5_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txisr0_set(txisr0_db6irq_idx, xi_gpon_tx_isr_vector.dba6_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txisr0_set(txisr0_db7irq_idx, xi_gpon_tx_isr_vector.dba7_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txisr0_set(txisr0_db8irq_idx, xi_gpon_tx_isr_vector.dba8_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txisr0_set(txisr0_db9irq_idx, xi_gpon_tx_isr_vector.dba9_report_transmitted_irq);                      
    ag_drv_gpon_tx_interrupt_controller_txisr0_set(txisr0_db10irq_idx, xi_gpon_tx_isr_vector.dba10_report_transmitted_irq);                    
    ag_drv_gpon_tx_interrupt_controller_txisr0_set(txisr0_db11irq_idx, xi_gpon_tx_isr_vector.dba11_report_transmitted_irq);                   
    ag_drv_gpon_tx_interrupt_controller_txisr0_set(txisr0_db12irq_idx, xi_gpon_tx_isr_vector.dba12_report_transmitted_irq);          

    ag_drv_gpon_tx_interrupt_controller_txisr1_set(txisr1_db13irq_idx, xi_gpon_tx_isr_vector.dba13_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txisr1_set(txisr1_db14irq_idx, xi_gpon_tx_isr_vector.dba14_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txisr1_set(txisr1_db15irq_idx, xi_gpon_tx_isr_vector.dba15_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txisr1_set(txisr1_db16irq_idx, xi_gpon_tx_isr_vector.dba16_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txisr1_set(txisr1_db17irq_idx, xi_gpon_tx_isr_vector.dba17_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txisr1_set(txisr1_db18irq_idx, xi_gpon_tx_isr_vector.dba18_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txisr1_set(txisr1_db19irq_idx, xi_gpon_tx_isr_vector.dba19_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txisr1_set(txisr1_db20irq_idx, xi_gpon_tx_isr_vector.dba20_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txisr1_set(txisr1_db21irq_idx, xi_gpon_tx_isr_vector.dba21_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txisr1_set(txisr1_db22irq_idx, xi_gpon_tx_isr_vector.dba22_report_transmitted_irq);                      
    ag_drv_gpon_tx_interrupt_controller_txisr1_set(txisr1_db23irq_idx, xi_gpon_tx_isr_vector.dba23_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txisr1_set(txisr1_db24irq_idx, xi_gpon_tx_isr_vector.dba24_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txisr1_set(txisr1_db25irq_idx, xi_gpon_tx_isr_vector.dba25_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txisr1_set(txisr1_db26irq_idx, xi_gpon_tx_isr_vector.dba26_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txisr1_set(txisr1_db27irq_idx, xi_gpon_tx_isr_vector.dba27_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txisr1_set(txisr1_db28irq_idx, xi_gpon_tx_isr_vector.dba28_report_transmitted_irq);                                          
   
    ag_drv_gpon_tx_interrupt_controller_txisr2_set(txisr2_db29irq_idx, xi_gpon_tx_isr_vector.dba29_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txisr2_set(txisr2_db30irq_idx, xi_gpon_tx_isr_vector.dba30_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txisr2_set(txisr2_db31irq_idx, xi_gpon_tx_isr_vector.dba31_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txisr2_set(txisr2_db32irq_idx, xi_gpon_tx_isr_vector.dba32_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txisr2_set(txisr2_db33irq_idx, xi_gpon_tx_isr_vector.dba33_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txisr2_set(txisr2_db34irq_idx, xi_gpon_tx_isr_vector.dba34_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txisr2_set(txisr2_db35irq_idx, xi_gpon_tx_isr_vector.dba35_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txisr2_set(txisr2_db36irq_idx, xi_gpon_tx_isr_vector.dba36_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txisr2_set(txisr2_db37irq_idx, xi_gpon_tx_isr_vector.dba37_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txisr2_set(txisr2_db38irq_idx, xi_gpon_tx_isr_vector.dba38_report_transmitted_irq);                      
    ag_drv_gpon_tx_interrupt_controller_txisr2_set(txisr2_db39irq_idx, xi_gpon_tx_isr_vector.dba39_report_transmitted_irq);                                          
#ifdef _BYTE_ORDER_LITTLE_ENDIAN_
    ag_drv_gpon_tx_interrupt_controller_txisr2_set(txisr2_rouge_level_ovrirq_idx, 
        (xi_gpon_tx_isr_vector.overrun_occured_irq&1<<1) | (xi_gpon_tx_isr_vector.rogue_onu_level&1));                                          
    ag_drv_gpon_tx_interrupt_controller_txisr2_set(txisr2_reserve_rouge_diff_idx, xi_gpon_tx_isr_vector.rogue_onu_diff&1<<1);
#else
    ag_drv_gpon_tx_interrupt_controller_txisr2_set(txisr2_rouge_level_ovrirq_idx, 
        (xi_gpon_tx_isr_vector.overrun_occured_irq&1) | (xi_gpon_tx_isr_vector.rogue_onu_level&1<<1));                                          
    ag_drv_gpon_tx_interrupt_controller_txisr2_set(txisr2_reserve_rouge_diff_idx, xi_gpon_tx_isr_vector.rogue_onu_diff&1);
#endif                                          

#ifdef UNIT_TEST_GPON_INTERRUPTS
    gpon_tx_generate_interrupt_test_register(tx_ier_vector);
#endif 

    return PON_NO_ERROR;
}
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
PON_ERROR_DTE gpon_clear_specific_tx_pon_interrupts(GPON_TX_ISR xi_gpon_tx_isr_vector)
{
    /* result of GPON driver APIs */
    PON_ERROR_DTE  gpon_error = PON_NO_ERROR;

    /* clear the tx interrupts, according to the recieved ISR vector */
    gpon_error = gpon_clear_tx_pon_interrupts(xi_gpon_tx_isr_vector);
    if (gpon_error != PON_NO_ERROR )
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Unable to clear gpon tx interrupts, error = %d", gpon_error);
        return gpon_error;
    }

    return PON_NO_ERROR;
}


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
PON_ERROR_DTE gpon_cfg_rogue_onu_interrupts (PON_ROGUE_ONU_TX_INT_CMD cmd, int rogue_onu_diff, 
   int rogue_onu_level)
{
   GPON_TX_ISR interupts_vector = {};
   int rc;

   interupts_vector.rogue_onu_diff = rogue_onu_diff;
   interupts_vector.rogue_onu_level = rogue_onu_level;
   
   switch (cmd)
   {
     case ROGUE_ONU_TX_INT_CLEAR:
       rc = gpon_clear_specific_tx_pon_interrupts(interupts_vector);
       break;

     case ROGUE_ONU_TX_INT_MASK:
       rc = gpon_mask_tx_pon_specific_interrupts(interupts_vector);
       break;

     case ROGUE_ONU_TX_INT_UNMASK:
       rc = gpon_unmask_tx_pon_specific_interrupts(interupts_vector);
       break;

     default:
       p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
          "Illegal Rogue ONU interrupts configuration operation cmd=%d", cmd);
       rc = PON_ERROR_DRIVER_ERROR;
   }

   return rc;
}


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
PON_ERROR_DTE get_tx_pon_pending_interrupts(GPON_TX_ISR *xo_gpon_tx_isr)
{
    int drv_error;
    uint8_t txisr0[txisr0_idx_num] = {};
    uint8_t txisr1[txisr1_idx_num] = {};
    uint8_t txisr2[txisr2_idx_num] = {};
    int i = 0;

    if (xo_gpon_tx_isr == NULL)
    {
        return PON_ERROR_INVALID_PARAMETER;
    }
  
    /* get the interrupt status register vector */
    for(i = 0; i < txisr0_idx_num; i++)
    {
        drv_error = ag_drv_gpon_tx_interrupt_controller_txisr0_get(i, &txisr0[i]);
        if (drv_error > 0)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                 "Failed to read txisr0 index %d due to driver error %d", i, drv_error);
            return PON_ERROR_DRIVER_ERROR;
        }

        drv_error = ag_drv_gpon_tx_interrupt_controller_txisr1_get(i, &txisr1[i]);
        if (drv_error > 0)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                 "Failed to read txisr1 index %d due to driver error %d", i, drv_error);
            return PON_ERROR_DRIVER_ERROR;
        }

        if (i < txisr2_idx_num)
        {
            drv_error = ag_drv_gpon_tx_interrupt_controller_txisr2_get(i, &txisr2[i]);
            if (drv_error > 0)
            {
                p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                     "Failed to read txisr2 index %d due to driver error %d", i, drv_error);
                return  PON_ERROR_DRIVER_ERROR ;
            }
        }
    }

    memset(xo_gpon_tx_isr, BDMF_FALSE, sizeof(GPON_TX_ISR));

#ifdef _BYTE_ORDER_LITTLE_ENDIAN_
    xo_gpon_tx_isr->overrun_occured_irq = (txisr2[txisr2_rouge_level_ovrirq_idx]&2)>>1;                   
    xo_gpon_tx_isr->rogue_onu_level = txisr2[txisr2_rouge_level_ovrirq_idx]&1;                   
    xo_gpon_tx_isr->rogue_onu_diff = (txisr2[txisr2_reserve_rouge_diff_idx]&2)>>1;                           
#else
    xo_gpon_tx_isr->overrun_occured_irq = txisr2[txisr2_rouge_level_ovrirq_idx]&1;                   
    xo_gpon_tx_isr->rogue_onu_level = (txisr2[txisr2_rouge_level_ovrirq_idx]&2)>>1;                   
    xo_gpon_tx_isr->rogue_onu_diff = txisr2[txisr2_reserve_rouge_diff_idx]&1;                           
#endif
    xo_gpon_tx_isr->dba39_report_transmitted_irq = txisr2[txisr2_db39irq_idx];                
    xo_gpon_tx_isr->dba38_report_transmitted_irq = txisr2[txisr2_db38irq_idx];                
    xo_gpon_tx_isr->dba37_report_transmitted_irq = txisr2[txisr2_db37irq_idx];                
    xo_gpon_tx_isr->dba36_report_transmitted_irq = txisr2[txisr2_db36irq_idx];                
    xo_gpon_tx_isr->dba35_report_transmitted_irq = txisr2[txisr2_db35irq_idx];                
    xo_gpon_tx_isr->dba34_report_transmitted_irq = txisr2[txisr2_db34irq_idx];                
    xo_gpon_tx_isr->dba33_report_transmitted_irq = txisr2[txisr2_db33irq_idx];                
    xo_gpon_tx_isr->dba32_report_transmitted_irq = txisr2[txisr2_db32irq_idx];                
    xo_gpon_tx_isr->dba31_report_transmitted_irq = txisr2[txisr2_db31irq_idx];                
    xo_gpon_tx_isr->dba30_report_transmitted_irq = txisr2[txisr2_db30irq_idx];                
    xo_gpon_tx_isr->dba29_report_transmitted_irq = txisr2[txisr2_db29irq_idx];  
                                                                                               
    xo_gpon_tx_isr->dba13_report_transmitted_irq = txisr1[txisr1_db13irq_idx];       
    xo_gpon_tx_isr->dba14_report_transmitted_irq = txisr1[txisr1_db14irq_idx];       
    xo_gpon_tx_isr->dba15_report_transmitted_irq = txisr1[txisr1_db15irq_idx];       
    xo_gpon_tx_isr->dba16_report_transmitted_irq = txisr1[txisr1_db16irq_idx];       
    xo_gpon_tx_isr->dba17_report_transmitted_irq = txisr1[txisr1_db17irq_idx];       
    xo_gpon_tx_isr->dba18_report_transmitted_irq = txisr1[txisr1_db18irq_idx];       
    xo_gpon_tx_isr->dba19_report_transmitted_irq = txisr1[txisr1_db19irq_idx];       
    xo_gpon_tx_isr->dba20_report_transmitted_irq = txisr1[txisr1_db20irq_idx];       
    xo_gpon_tx_isr->dba21_report_transmitted_irq = txisr1[txisr1_db21irq_idx];       
    xo_gpon_tx_isr->dba22_report_transmitted_irq = txisr1[txisr1_db22irq_idx];       
    xo_gpon_tx_isr->dba23_report_transmitted_irq = txisr1[txisr1_db23irq_idx];       
    xo_gpon_tx_isr->dba24_report_transmitted_irq = txisr1[txisr1_db24irq_idx];       
    xo_gpon_tx_isr->dba25_report_transmitted_irq = txisr1[txisr1_db25irq_idx];       
    xo_gpon_tx_isr->dba26_report_transmitted_irq = txisr1[txisr1_db26irq_idx];       
    xo_gpon_tx_isr->dba27_report_transmitted_irq = txisr1[txisr1_db27irq_idx];       
    xo_gpon_tx_isr->dba28_report_transmitted_irq = txisr1[txisr1_db28irq_idx];       

    xo_gpon_tx_isr->dba0_report_transmitted_irq = txisr0[txisr0_db0irq_idx];             
    xo_gpon_tx_isr->dba1_report_transmitted_irq = txisr0[txisr0_db1irq_idx];             
    xo_gpon_tx_isr->dba2_report_transmitted_irq = txisr0[txisr0_db2irq_idx];             
    xo_gpon_tx_isr->dba3_report_transmitted_irq = txisr0[txisr0_db3irq_idx];             
    xo_gpon_tx_isr->dba4_report_transmitted_irq = txisr0[txisr0_db4irq_idx];             
    xo_gpon_tx_isr->dba5_report_transmitted_irq = txisr0[txisr0_db5irq_idx];             
    xo_gpon_tx_isr->dba6_report_transmitted_irq = txisr0[txisr0_db6irq_idx];             
    xo_gpon_tx_isr->dba7_report_transmitted_irq = txisr0[txisr0_db7irq_idx];             
    xo_gpon_tx_isr->dba8_report_transmitted_irq = txisr0[txisr0_db8irq_idx];             
    xo_gpon_tx_isr->dba9_report_transmitted_irq = txisr0[txisr0_db9irq_idx];             
    xo_gpon_tx_isr->dba10_report_transmitted_irq = txisr0[txisr0_db10irq_idx];                    
    xo_gpon_tx_isr->dba11_report_transmitted_irq = txisr0[txisr0_db11irq_idx];                    
    xo_gpon_tx_isr->dba12_report_transmitted_irq = txisr0[txisr0_db12irq_idx];  
#ifdef _BYTE_ORDER_LITTLE_ENDIAN_
    xo_gpon_tx_isr->ranging_ploam_transmitted_irq = txisr0[txisr0_rplmirq_iplmirq_idx]&1;
    xo_gpon_tx_isr->idle_ploam_transmitted_irq = (txisr0[txisr0_rplmirq_iplmirq_idx]&2)>>1;                         
    xo_gpon_tx_isr->urgent_ploam_transmitted_irq = txisr0[txisr0_uplmirq_nplmirq_idx]&1;                 
    xo_gpon_tx_isr->normal_ploam_transmitted_irq = (txisr0[txisr0_uplmirq_nplmirq_idx]&2)>>1;                                  
    xo_gpon_tx_isr->access_fifo_error_irq = txisr0[txisr0_accirq_tdmirq_idx]&1;
    xo_gpon_tx_isr->tdm_fifo_error_irq = (txisr0[txisr0_accirq_tdmirq_idx]&2)>>1;        
#else
    xo_gpon_tx_isr->ranging_ploam_transmitted_irq = (txisr0[txisr0_rplmirq_iplmirq_idx]&2)>>1;
    xo_gpon_tx_isr->idle_ploam_transmitted_irq = txisr0[txisr0_rplmirq_iplmirq_idx]&1;                         
    xo_gpon_tx_isr->urgent_ploam_transmitted_irq = (txisr0[txisr0_uplmirq_nplmirq_idx]&2)>>1;                 
    xo_gpon_tx_isr->normal_ploam_transmitted_irq = txisr0[txisr0_uplmirq_nplmirq_idx]&1;                                  
    xo_gpon_tx_isr->access_fifo_error_irq = (txisr0[txisr0_accirq_tdmirq_idx]&2)>>1;
    xo_gpon_tx_isr->tdm_fifo_error_irq = txisr0[txisr0_accirq_tdmirq_idx]&1;        
#endif 

    /* Cross status register vector with interrupts that should be handled by the stack, to receive the currently pending interrupts */
    ((uint32_t *)xo_gpon_tx_isr)[0] &= ((uint32_t *)&gs_gpon_database.tx_ier_vector)[0];
    ((uint32_t *)xo_gpon_tx_isr)[1] &= ((uint32_t *)&gs_gpon_database.tx_ier_vector)[1];
    ((uint32_t *)xo_gpon_tx_isr)[2] &= ((uint32_t *)&gs_gpon_database.tx_ier_vector)[2];

#ifdef UNIT_TEST_GPON_INTERRUPTS
   printk(KERN_INFO  
        "\nget_tx_pon_pending_interrupts  isr=0x%08x, 0x%08x, 0x%08x, ier=0x%08x, 0x%08x, 0x%08x\n",
        ((uint32_t*)xo_gpon_tx_isr)[0], ((uint32_t*)xo_gpon_tx_isr)[1], ((uint32_t*)xo_gpon_tx_isr)[2], 
        ((uint32_t*)&gs_gpon_database.tx_ier_vector)[0], ((uint32_t*)&gs_gpon_database.tx_ier_vector)[1], 
        ((uint32_t*)&gs_gpon_database.tx_ier_vector)[2]);
#endif
     
    return PON_NO_ERROR;
}


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
    PON_INTERRUPTS_MASK_DTE xi_interrupt_vector)
{
    bdmf_boolean lof_state;
    bdmf_boolean fec_state;
    bdmf_boolean lcdg_state;
    bdmf_error_t bdmf_error;
    uint8_t bit_align;
    PON_ERROR_DTE gpon_error;
    OPERATION_SM_PARAMS_DTS operation_sm_params;
    gpon_rx_irq_grxisr gpon_rx_isr_vector = {};
    GPON_TX_ISR gpon_tx_isr_vector = {};
    PON_OAM_ALARM_STATUS_DTE alarm_status;
    OPERATION_ROGUE_INDICATION_PARAMS_DTS rogue_param;
    GPON_DS_PLOAM *_xi_ploam_message_ptr = NULL;

    if (xi_ploam_message_ptr != NULL)
    {
    	_xi_ploam_message_ptr = (GPON_DS_PLOAM *)xi_ploam_message_ptr;
    }

    /* Varify that the stack is active */
    if (gs_gpon_database.gs_sw_state != GPON_STATE_INITIALIZED)
    {
        return PON_ERROR_INVALID_STATE;
    }

    /* Link activated? */
    if (gs_gpon_database.link_parameters.operation_state_machine.link_state != LINK_STATE_ACTIVE)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Link not active yet!");
        return PON_GENERAL_ERROR;
    }

    /* Check if Ploam received in the Runner */
    if (xi_interrupt_vector.ploam_received_interrupt == BDMF_TRUE)
    {
        operation_sm_params.ploam_message_ptr = _xi_ploam_message_ptr;
        /* Get the ploam from the Runner Device driver(via Configuration handler callback ?? ) */
        /* Inegration point - need to get the PLOAM from the Runner device driver */
        /* Clear the ploam received interrupt - in the Interrupt controller */
        /* Analysing the received ploam */
        p_dump_ds_ploam(operation_sm_params.ploam_message_ptr);

        switch(operation_sm_params.ploam_message_ptr->message_id)
        {
        case CE_DS_NO_MESSAGE_MESSAGE_ID:
            break;

        case CE_DS_US_OVERHEAD_MESSAGE_ID:
            p_operation_state_machine(CS_OPERATION_EVENT_US_OVERHEAD_MESSAGE, &operation_sm_params);
            break;
        case CE_DS_ASSIGN_ONU_ID_MESSAGE_ID:
            p_operation_state_machine(CS_OPERATION_EVENT_ASSIGN_ONU_ID_MESSAGE, &operation_sm_params);
            break;

        case CE_DS_RANGING_TIME_MESSAGE_ID:
            p_operation_state_machine(CS_OPERATION_EVENT_RANGING_TIME_MESSAGE, &operation_sm_params);
            break;

        case CE_DS_DEACTIVATE_ONU_ID_MESSAGE_ID:
            p_operation_state_machine(CS_OPERATION_EVENT_DEACTIVATE_ONU_ID_MESSAGE, &operation_sm_params);
            break;

        case CE_DS_DISABLE_SERIAL_NUMBER_MESSAGE_ID:

            /* Check if the flag is disable | enable */
            if (operation_sm_params.ploam_message_ptr->message.disable_serial_number.control == CE_SERIAL_NUMBER_UNICAST_DISABLE)
            {
                p_operation_state_machine(CS_OPERATION_EVENT_DISABLE_SERIAL_NUMBER_DISABLE_MESSAGE, &operation_sm_params);
            }
            else if ((operation_sm_params.ploam_message_ptr->message.disable_serial_number.control == CE_SERIAL_NUMBER_UNICAST_ENABLE) ||
                (operation_sm_params.ploam_message_ptr->message.disable_serial_number.control == CE_SERIAL_NUMBER_MULTICAST_ENABLE))
            {
                p_operation_state_machine(CS_OPERATION_EVENT_DISABLE_SERIAL_NUMBER_ENABLE_MESSAGE, &operation_sm_params);
            }
            break;

        case CE_DS_POPUP_MESSAGE_ID:
            /* Check if the onu - id is broadcast | direct */
            if (operation_sm_params.ploam_message_ptr->onu_id == gs_gpon_database.onu_parameters.broadcast_default_value)
            {
                p_operation_state_machine(CS_OPERATION_EVENT_BROADCAST_POPUP_REQUEST, &operation_sm_params);
            }
            else if (operation_sm_params.ploam_message_ptr->onu_id == gs_gpon_database.onu_parameters.onu_id)
            {
                p_operation_state_machine(CS_OPERATION_EVENT_DIRECTED_POPUP_REQUEST, &operation_sm_params);
            }
            break;

        case CE_DS_CHANGE_POWER_LEVEL_MESSAGE_ID:
            p_operation_state_machine(CS_OPERATION_EVENT_CHANGE_POWER_LEVEL_MESSAGE, &operation_sm_params);
            break;

        case CE_DS_REQUEST_PASSWORD_MESSAGE_ID:
            p_operation_state_machine(CS_OPERATION_EVENT_REQUEST_PASSWORD, &operation_sm_params);
            break;

        case CE_DS_ASSIGN_ALLOC_ID_MESSAGE_ID:
            operation_sm_params.ploam_message_ptr->message.assign_alloc_id.alloc_16 = 
                ntohs(operation_sm_params.ploam_message_ptr->message.assign_alloc_id.alloc_16);
            p_log(ge_onu_logger.sections.stack.downstream_ploam.assign_alloc_id_message_id, 
                "CE_DS_ASSIGN_ALLOC_ID_MESSAGE_ID - Alloc ID %d", 
                operation_sm_params.ploam_message_ptr->message.assign_alloc_id.alloc_id.alloc_id);

            p_operation_state_machine(CS_OPERATION_EVENT_ASSIGN_ALLOC_ID_MESSAGE, &operation_sm_params);
            break;

        case CE_DS_REQUEST_KEY_MESSAGE_ID:
            p_operation_state_machine(CS_OPERATION_EVENT_REQUEST_KEY_MESSAGE, &operation_sm_params);
            break;

        case CE_DS_CONFIGURE_PORT_ID_MESSAGE_ID: 
            operation_sm_params.ploam_message_ptr->message.configure_port_id.port_id_16 = 
                ntohs(operation_sm_params.ploam_message_ptr->message.configure_port_id.port_id_16);
            p_log(ge_onu_logger.sections.stack.downstream_ploam.configure_port_id_message_id, 
                "CE_DS_CONFIGURE_PORT_ID_MESSAGE_ID - OMCI port %d", 
                operation_sm_params.ploam_message_ptr->message.configure_port_id.port_id.port_id);

            p_operation_state_machine(CS_OPERATION_EVENT_CONFIGURE_PORT_ID_MESSAGE, &operation_sm_params);
            break;

        case CE_DS_ENCRYPTED_PORT_ID_MESSAGE_ID:
            operation_sm_params.ploam_message_ptr->message.encrypted_port_id_or_vpi.vpi_16 = 
                ntohs(operation_sm_params.ploam_message_ptr->message.encrypted_port_id_or_vpi.vpi_16);
            operation_sm_params.ploam_message_ptr->message.encrypted_port_id_or_vpi.port_id_16 = 
                ntohs(operation_sm_params.ploam_message_ptr->message.encrypted_port_id_or_vpi.port_id_16);
            p_log(ge_onu_logger.sections.stack.downstream_ploam.encrypted_vpi_port_message_id, 
                "CE_DS_ENCRYPTED_PORT_ID_MESSAGE_ID - VPI id %d port %d", 
                operation_sm_params.ploam_message_ptr->message.encrypted_port_id_or_vpi.vpi_id.vpi_id, 
                operation_sm_params.ploam_message_ptr->message.encrypted_port_id_or_vpi.port_id.port_id);

            p_operation_state_machine(CS_OPERATION_EVENT_ENCRYPT_PORT_ID_MESSAGE, &operation_sm_params);
            break;

        case CE_DS_PHYSICAL_EQUIPMENT_ERROR_MESSAGE_ID:
            p_operation_state_machine(CS_OPERATION_EVENT_PHYSICAL_EQUIPMENT_ERROR_MESSAGE, &operation_sm_params);
            break;

        case CE_DS_PST_MESSAGE_ID:
            p_operation_state_machine(CS_OPERATION_EVENT_PST_MESSAGE, &operation_sm_params);
            break;

        case CE_DS_BER_INTERVAL_MESSAGE_ID:
            p_operation_state_machine(CS_OPERATION_EVENT_BER_INTERVAL_MESSAGE, &operation_sm_params);
            break;

        case CE_DS_KEY_SWITCHING_TIME_MESSAGE_ID:
            p_operation_state_machine(CS_OPERATION_EVENT_KEY_SWITCHING_TIME_MESSAGE, &operation_sm_params);
            break;

        case CE_DS_EXTENDED_BURST_LENGTH_MESSAGE_ID :
            p_operation_state_machine(CS_OPERATION_EVENT_EXTENDED_BURST_LENGTH_MESSAGE, &operation_sm_params);
            break;

        case CE_DS_SERIAL_NUMBER_MASK_MESSAGE_ID :
            p_operation_state_machine(CS_OPERATION_EVENT_SERIAL_NUMBER_MASK_MESSAGE, &operation_sm_params);
            break;

        case CE_DS_SLEEP_ALLOW_MESSAGE_ID:
            p_operation_state_machine(CS_OPERATION_EVENT_SLEEP_ALLOW_MESSAGE, &operation_sm_params);
            break;

        case CE_DS_PON_ID_MESSAGE_ID:
            p_operation_state_machine(CS_OPERATION_EVENT_PON_ID_MESSAGE, &operation_sm_params);
            break;

        case CE_DS_SWIFT_POPUP_MESSAGE_ID:
            p_operation_state_machine(CS_OPERATION_EVENT_SWIFT_POPUP_MESSAGE, &operation_sm_params);
            break;

        case CE_DS_RANGING_ADJUSTMENT_MESSAGE_ID:
            p_operation_state_machine(CS_OPERATION_EVENT_RANGING_ADJUSTMENT_MESSAGE, &operation_sm_params);
            break;        
        
        default :
            /* Send user indication "MEM" - unknown ploam */
            gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_OAM ,
                GPON_INDICATION_MEM, CE_OAM_ALARM_STATUS_ON,
                *(operation_sm_params.ploam_message_ptr));
            if (gpon_error != PON_NO_ERROR)
            {
                p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                    "Unable to send oam indication: error %d !", gpon_error);
            }
            p_log(ge_onu_logger.sections.stack.upstream_ploam.unknown_message_id,
                "Unknown PLOAM !");

            /* count the unknown messege */
            gs_gpon_database.unknown_ploam.non_standard_rx_msg++;
            break;
        }
    }

    /* Check Rx interrupts */
    if (xi_interrupt_vector.rxpon_interrupt == BDMF_TRUE)
    {
#ifdef UNIT_TEST_GPON_INTERRUPTS
        p_log(ge_onu_logger.sections.isr.debug.rx_event_id, "Gpon RX interrupt\n");
#endif /* UNIT_TEST_GPON_INTERRUPTS */

        /* Get currently pending rxpon interrupts */
        gpon_error =  get_rx_pon_pending_interrupts(&gpon_rx_isr_vector);
        if (gpon_error != PON_NO_ERROR )
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
                "Unable to get vector of pending interrupts, error =%d", gpon_error);
            return gpon_error;
        }
       
        /* Clear the pending rxpon interrupts */
        gpon_error = gpon_clear_rx_pon_interrupts(&gpon_rx_isr_vector);
        if (gpon_error != PON_NO_ERROR )
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to clear gpon rx interrupts, error=%d", gpon_error);    
          return  gpon_error;
        }

        /* Handling GPON RX interrups */

        /* LCDG/LOF state change IRQ */
        if (gpon_rx_isr_vector.lcdgchng || gpon_rx_isr_vector.lofchng)
        {
            /* Check LOF & LCDG */
            bdmf_error = ag_drv_gpon_rx_general_config_rcvr_status_get(&lof_state, &fec_state,
               &lcdg_state, &bit_align);
            if (bdmf_error < 0)
            {
                p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                    "Unable to get LCDG & LOF status due to Driver error %d", bdmf_error);
                return PON_ERROR_DRIVER_ERROR;
            }

            /* Set the Operation SM according to HW */
            operation_sm_params.lof_state = lof_state;
            operation_sm_params.lcdg_state = lcdg_state;

            /* HW_WA_2 - If LCDG status change need to set timer and disable LCDG interrupt */
            if (gpon_rx_isr_vector.lcdgchng == BDMF_TRUE)
            {
                if (lcdg_state) 
                {
                    p_log(ge_onu_logger.sections.isr.debug.rx_event_id, 
                        "Gpon LCDG state change interrupt - LCDG alarm ON");
                } 
                else
                {
                    p_log(ge_onu_logger.sections.isr.debug.rx_event_id, 
                        "Gpon LCDG state change interrupt - LCDG alarm Cleared");
                }

                /* Send LCDG status indication */
                alarm_status = (operation_sm_params.lcdg_state == BDMF_TRUE) ? 
                    CE_OAM_ALARM_STATUS_ON : CE_OAM_ALARM_STATUS_OFF;
                gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_OAM , 
                    PON_INDICATION_LCDG , alarm_status);
                if (gpon_error != PON_NO_ERROR)
                {
                    p_log(ge_onu_logger.sections.stack.debug.sw_errors_id,
                        "Unable to send oam indication: error %d !", gpon_error);
                }

                /* prevent enabling the LCDG interrupt at the end of the ISR*/
                gs_gpon_database.rx_ier_vector.lcdgchng = BDMF_FALSE;
                

                /* Start LCDG timer */
                bdmf_error = bdmf_timer_start(&gs_gpon_database.os_resources.lcdg_timer_id, 
                    GPON_MILISEC_TO_TICKS(gs_gpon_database.onu_parameters.lcdg_timer_timeout));
                if (bdmf_error != BDMF_ERR_OK)
                {
                    p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, "Unable to start LCDG timer");
                }
                else
                {
                    p_log(ge_onu_logger.sections.stack.debug.timers_id, "Start LCDG timer");
                }
            }
            if (gpon_rx_isr_vector.lofchng)
            {
                if (lof_state == BDMF_TRUE)
                {
                    p_log(ge_onu_logger.sections.isr.debug.rx_event_id, 
                        "Gpon LOF state change interrupt - LOF alarm ON");
                    /* LOF alarms set->Detect */
                    p_operation_state_machine(CS_OPERATION_EVENT_DETECT_DS_LOS_OR_LOF, &operation_sm_params);
                }
                else
                {
                    if (gs_gpon_database.link_parameters.operation_state_machine.activation_state == OPERATION_STATE_O5 )
                    {
                        /* LOF alarms set ->Pop-Up [O6] */
                        p_operation_state_machine(CS_OPERATION_EVENT_DETECT_DS_LOS_OR_LOF, &operation_sm_params);
                    }

                    p_operation_state_machine(CS_OPERATION_EVENT_CLEAR_DS_LOS_OR_LOF, &operation_sm_params);
                }

                pon_serdes_lof_fixup_irq(lof_state);
            }
        }

        /* Access FIFO Full IRQ */
        if (gpon_rx_isr_vector.accfifo)
        {
            p_log(ge_onu_logger.sections.isr.debug.rx_event_id, "Gpon Access fifo full interrupt !!!");
        }

        /* AES Error IRQ */
        if (gpon_rx_isr_vector.aes)
        {
            p_log(ge_onu_logger.sections.isr.debug.rx_event_id, "Gpon AES Error interrupt !!!");
        }

        /* Rx FIFO Full IRQ */
        if (gpon_rx_isr_vector.rxfifo)
        {
            p_log(ge_onu_logger.sections.isr.debug.rx_event_id, "Gpon RX Fifo full interrupt !!!");
        }

        /* FEC status change IRQ */
        if (gpon_rx_isr_vector.fecchng)
        {
            p_log(ge_onu_logger.sections.isr.debug.rx_event_id, "FEC status change interrupt !!!");
        }

#ifdef CONFIG_BCM_GPON_TODD
        /* ToD update IRQ */
        if (gpon_rx_isr_vector.tod)
        {
            p_log(ge_onu_logger.sections.isr.debug.rx_event_id, "ToD update interrupt !!!");
            tod_handle_tod_update_interrupt();
        }
#endif 

        /* TDM FIFO Full IRQ */
        if (gpon_rx_isr_vector.tdmfifo)
        {
            p_log(ge_onu_logger.sections.isr.debug.rx_event_id, "TDM FIFO Full interrupt !!!");
        }
    }

    /* Check Tx interrupts */
    if (xi_interrupt_vector.txpon_interrupt == BDMF_TRUE )
    {
#ifdef UNIT_TEST_GPON_INTERRUPTS
       printk(KERN_INFO  "Gpon TX interrupt\n");
#endif 

        /* Get currently pending txpon interrupts */
        gpon_error =  get_tx_pon_pending_interrupts(& gpon_tx_isr_vector);
        if (gpon_error != PON_NO_ERROR )
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                "Unable to get gpon tx pending interrupts, error = %d", gpon_error);
            return gpon_error;
        }

        /* Clear the pending txpon interrupts */
        gpon_error = gpon_clear_tx_pon_interrupts(gpon_tx_isr_vector);
        if (gpon_error != PON_NO_ERROR )
        {
          p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
              "Unable to clear gpon tx interrupts, error = %d", gpon_error);
          return  gpon_error;
        }

        /* Handling GPON TX interrups */

        /* overrun occured */
        if (gpon_tx_isr_vector.overrun_occured_irq)
        {
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, 
                "Gpon TX: Gpon TX overrun occured in  access !!!");
        }

        /* Ranging PLOAM transmitted */
        if (gpon_tx_isr_vector.ranging_ploam_transmitted_irq)
        {
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, 
                "Gpon TX: Ranging PLOAM transmitted interrupt !!!");

            switch(gs_gpon_database.link_parameters.operation_state_machine.activation_state )
            {
            case OPERATION_STATE_O3:
                p_operation_state_machine(CS_OPERATION_EVENT_SERIAL_NUMBER_REQUEST, &operation_sm_params);
                break;

            case OPERATION_STATE_O4:
                p_operation_state_machine(CS_OPERATION_EVENT_RANGING_REQUEST, &operation_sm_params);
                break;

            default:
                p_log(ge_onu_logger.sections.isr.debug.tx_event_id, "Gpon TX: Unpredictible request arrived !!!");
                break;
            }
         }

        /* Urgent/Normal/Idle PLOAM transmitted */
        if ((gpon_tx_isr_vector.urgent_ploam_transmitted_irq) ||
            (gpon_tx_isr_vector.normal_ploam_transmitted_irq) ||
            (gpon_tx_isr_vector.idle_ploam_transmitted_irq))
        {
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, 
                "Gpon TX: PLOAM transmitted interrupt isr=0x%08x, 0x%08x, 0x%08x, ier=0x%08x, 0x%08x, 0x%08x", 
                ((uint32_t*)&gpon_tx_isr_vector)[0], ((uint32_t*)&gpon_tx_isr_vector)[1], 
                ((uint32_t*)&gpon_tx_isr_vector)[2], ((uint32_t*)&gs_gpon_database.tx_ier_vector)[0],
                ((uint32_t*)&gs_gpon_database.tx_ier_vector)[1], ((uint32_t*)&gs_gpon_database.tx_ier_vector)[2]);
 

            /* fetch ploam message from urgent, normal or base level queue. */
            gpon_error = f_gpon_txpon_fetch_ploam_message(NORMAL_PLOAM_CELL);
            if (gpon_error != PON_NO_ERROR)
                return gpon_error;
        }

         /* Access FIFO error */
        if (gpon_tx_isr_vector.access_fifo_error_irq)
        {
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, 
                "Gpon TX: Access Fifo error interrupt !!!");
        }

        /* TDM FIFO error */
        if (gpon_tx_isr_vector.tdm_fifo_error_irq)
        {
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, 
                "Gpon TX: TDM Fifo error interrupt !!!");
        }

        /* rogue_onu level */
        if (gpon_tx_isr_vector.rogue_onu_level)
        {
            rogue_param.rogue_state = BDMF_TRUE;
            rogue_param.rogue_type = CE_BL_TX_FAULT_ROGUE_MODE;
            /* Send ROGUE level status indication */

            gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_USER_INDICATION, 
                PON_INDICATION_ROGUE_ONU, rogue_param);
            if (gpon_error != PON_NO_ERROR )
            {
                p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                    "Unable to send user indication: error %d !", gpon_error);
            }

            /* disable the rogue interrupt */
            gs_gpon_database.tx_ier_vector.rogue_onu_level = BDMF_FALSE;
            
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, "Gpon TX: rogue_onu interrupt !!!");
        }

        /* rogue_onu diff */
        if (gpon_tx_isr_vector.rogue_onu_diff)
        {
            rogue_param.rogue_state = BDMF_TRUE;
            rogue_param.rogue_type = CE_BL_TX_MONITOR_ROGUE_MODE;
            /* Send ROGUE diff status indication */

            gpon_error = f_gpon_send_message_to_pon_task(PON_EVENT_USER_INDICATION, 
                PON_INDICATION_ROGUE_ONU, rogue_param);
            if (gpon_error != PON_NO_ERROR)
            {
                p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                    "Unable to send user indication: error %d !", gpon_error);
            }
            
            /* disable the rogue interrupt*/
            gs_gpon_database.tx_ier_vector.rogue_onu_diff = BDMF_FALSE;

            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, "Gpon TX: rogue_onu interrupt !!!");
        }

        /* DBA0 transmitted */
        if (gpon_tx_isr_vector.dba0_report_transmitted_irq )
        {
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, 
                "Gpon TX: Gpon TX DBA 0 report interrupt !!!");
        }

        /* DBA1 transmitted */
        if (gpon_tx_isr_vector.dba1_report_transmitted_irq )
        {
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, "Gpon TX: Gpon TX DBA 1 report interrupt !!!");
        }

        /* DBA2 transmitted */
        if (gpon_tx_isr_vector.dba2_report_transmitted_irq )
        {
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, 
                "Gpon TX: Gpon TX DBA 2 report interrupt !!!");
        }

        /* DBA3 transmitted */
        if (gpon_tx_isr_vector.dba3_report_transmitted_irq )
        {
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, 
                "Gpon TX: Gpon TX DBA 3 report interrupt !!!");
        }

        /* DBA4 transmitted */
        if (gpon_tx_isr_vector.dba4_report_transmitted_irq )
        {
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, 
                "Gpon TX: Gpon TX DBA 4 report interrupt !!!");
        }

        /* DBA5 transmitted */
        if (gpon_tx_isr_vector.dba5_report_transmitted_irq )
        {
            /* Log */
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, 
                "Gpon TX: Gpon TX DBA 5 report interrupt !!!");
        }

        /* DBA6 transmitted */
        if (gpon_tx_isr_vector.dba6_report_transmitted_irq )
        {
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, 
                "Gpon TX: Gpon TX DBA 6 report interrupt !!!");
        }

        /* DBA7 transmitted */
        if (gpon_tx_isr_vector.dba7_report_transmitted_irq )
        {
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, 
                "Gpon TX: Gpon TX DBA 7 report interrupt !!!");
        }

        /* DBA8 transmitted */
        if (gpon_tx_isr_vector.dba8_report_transmitted_irq )
        {
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, 
                "Gpon TX: Gpon TX DBA 8 report interrupt !!!");
        }

        /* DBA9 transmitted */
        if (gpon_tx_isr_vector.dba9_report_transmitted_irq )
        {
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, 
                "Gpon TX: Gpon TX DBA 9 report interrupt !!!");
        }

        /* DBA10 transmitted */
        if (gpon_tx_isr_vector.dba10_report_transmitted_irq )
        {
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, 
                "Gpon TX: Gpon TX DBA 10 report interrupt !!!");
        }

        /* DBA11 transmitted */
        if (gpon_tx_isr_vector.dba11_report_transmitted_irq )
        {
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, 
                "Gpon TX: Gpon TX DBA 11 report interrupt !!!");
        }

        /* DBA12 transmitted */
        if (gpon_tx_isr_vector.dba12_report_transmitted_irq )
        {
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, 
               "Gpon TX: Gpon TX DBA 12 report interrupt !!!");
        }

        /* DBA13 transmitted */
        if (gpon_tx_isr_vector.dba13_report_transmitted_irq )
        {
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, 
                "Gpon TX: Gpon TX DBA 13 report interrupt !!!");
        }

        /* DBA14 transmitted */
        if (gpon_tx_isr_vector.dba14_report_transmitted_irq )
        {
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, 
                "Gpon TX: Gpon TX DBA 14 report interrupt !!!");
        }

        /* DBA15 transmitted */
        if (gpon_tx_isr_vector.dba15_report_transmitted_irq )
        {
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, 
                "Gpon TX: Gpon TX DBA 15 report interrupt !!!");
        }

        /* DBA16 transmitted */
        if (gpon_tx_isr_vector.dba16_report_transmitted_irq )
        {
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, 
                "Gpon TX: Gpon TX DBA 16 report interrupt !!!");
        }

        /* DBA17 transmitted */
        if (gpon_tx_isr_vector.dba17_report_transmitted_irq )
        {
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, 
                "Gpon TX: Gpon TX DBA 17 report interrupt !!!");
        }

        /* DBA18 transmitted */
        if (gpon_tx_isr_vector.dba18_report_transmitted_irq )
        {
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, 
               "Gpon TX: Gpon TX DBA 18 report interrupt !!!");
        }

        /* DBA19 transmitted */
        if (gpon_tx_isr_vector.dba19_report_transmitted_irq )
        {
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, 
                "Gpon TX: Gpon TX DBA 19 report interrupt !!!");
        }

        /* DBA20 transmitted */
        if (gpon_tx_isr_vector.dba20_report_transmitted_irq )
        {
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, 
                "Gpon TX: Gpon TX DBA 20 report interrupt !!!");
        }

        /* DBA21 transmitted */
        if (gpon_tx_isr_vector.dba21_report_transmitted_irq )
        {
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, 
                "Gpon TX: Gpon TX DBA 21 report interrupt !!!");
        }

        /* DBA22 transmitted */
        if (gpon_tx_isr_vector.dba22_report_transmitted_irq )
        {
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, 
                "Gpon TX: Gpon TX DBA 22 report interrupt !!!");
        }

        /* DBA23 transmitted */
        if (gpon_tx_isr_vector.dba23_report_transmitted_irq )
        {
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, 
                "Gpon TX: Gpon TX DBA 23 report interrupt !!!");
        }

        /* DBA24 transmitted */
        if (gpon_tx_isr_vector.dba24_report_transmitted_irq )
        {
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, 
                "Gpon TX: Gpon TX DBA 24 report interrupt !!!");
        }

        /* DBA25 transmitted */
        if (gpon_tx_isr_vector.dba25_report_transmitted_irq )
        {
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, 
                "Gpon TX: Gpon TX DBA 25 report interrupt !!!");
        }

        /* DBA26 transmitted */
        if (gpon_tx_isr_vector.dba26_report_transmitted_irq )
        {
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, 
                "Gpon TX: Gpon TX DBA 26 report interrupt !!!");
        }

        /* DBA27 transmitted */
        if (gpon_tx_isr_vector.dba27_report_transmitted_irq )
        {
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, 
                "Gpon TX: Gpon TX DBA 27 report interrupt !!!");
        }

        /* DBA28 transmitted */
        if (gpon_tx_isr_vector.dba28_report_transmitted_irq )
        {
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, 
                "Gpon TX: Gpon TX DBA 28 report interrupt !!!");
        }

        /* DBA29 transmitted */
        if (gpon_tx_isr_vector.dba29_report_transmitted_irq )
        {
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, 
                "Gpon TX: Gpon TX DBA 29 report interrupt !!!");
        }

        /* DBA30 transmitted */
        if (gpon_tx_isr_vector.dba30_report_transmitted_irq )
        {
            /* Log */
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, 
                "Gpon TX: Gpon TX DBA 30 report interrupt !!!");
        }

        /* DBA31 transmitted */
        if (gpon_tx_isr_vector.dba31_report_transmitted_irq )
        {
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, 
                "Gpon TX: Gpon TX DBA 31 report interrupt !!!");
        }

        /* DBA32 transmitted */
        if (gpon_tx_isr_vector.dba32_report_transmitted_irq )
        {
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, 
                "Gpon TX: Gpon TX DBA 32 report interrupt !!!");
        }

        /* DBA33 transmitted */
        if (gpon_tx_isr_vector.dba33_report_transmitted_irq )
        {
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, 
                "Gpon TX: Gpon TX DBA 33 report interrupt !!!");
        }

        /* DBA34 transmitted */
        if (gpon_tx_isr_vector.dba34_report_transmitted_irq )
        {
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, 
                "Gpon TX: Gpon TX DBA 34 report interrupt !!!");
        }

        /* DBA35 transmitted */
        if (gpon_tx_isr_vector.dba35_report_transmitted_irq )
        {
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, 
                "Gpon TX: Gpon TX DBA 35 report interrupt !!!");
        }

        /* DBA36 transmitted */
        if (gpon_tx_isr_vector.dba36_report_transmitted_irq )
        {
            /* Log */
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, 
                "Gpon TX: Gpon TX DBA 36 report interrupt !!!");
        }

        /* DBA37 transmitted */
        if (gpon_tx_isr_vector.dba37_report_transmitted_irq )
        {
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, 
                "Gpon TX: Gpon TX DBA 37 report interrupt !!!");
        }

        /* DBA38 transmitted */
        if (gpon_tx_isr_vector.dba38_report_transmitted_irq )
        {
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, 
                "Gpon TX: Gpon TX DBA 38 report interrupt !!!");
        }

        /* DBA39 transmitted */
        if (gpon_tx_isr_vector.dba39_report_transmitted_irq )
        {
            p_log(ge_onu_logger.sections.isr.debug.tx_event_id, 
                "Gpon TX: Gpon TX DBA 39 report interrupt !!!");
        }
    }

    return PON_NO_ERROR;
}



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
PON_ERROR_DTE gpon_int_lcdg_enable(void)
{
    int drv_error;

    gs_gpon_database.rx_ier_vector.lcdgchng = BDMF_TRUE;

    drv_error = ag_drv_gpon_rx_irq_grxier_set(&gs_gpon_database.rx_ier_vector);
    if (drv_error > 0)
	{
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Failed to set Rx interrupts due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
	}

    return PON_NO_ERROR;
}


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
PON_ERROR_DTE gpon_int_lcdg_disable(void)
{
    int drv_error;

    gs_gpon_database.rx_ier_vector.lcdgchng = BDMF_FALSE;

    drv_error = ag_drv_gpon_rx_irq_grxier_set(&gs_gpon_database.rx_ier_vector);
    if (drv_error > 0)
	{
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Failed to set Rx interrupts due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
	}

    return PON_NO_ERROR;
}


/******************************************************************************/
/*                                                                            */
/* Name:                                                                      */
/*                                                                            */
/*   gpon_int_lof_enable                                                      */
/*                                                                            */
/* Title:                                                                     */
/*                                                                            */
/*   GPON - Enable interrupt: LOF                                             */
/*                                                                            */
/* Abstract:                                                                  */
/*                                                                            */
/*   This function enables interrupt: LOF                                     */
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
PON_ERROR_DTE gpon_int_lof_enable(void)
{
    int drv_error;

    gs_gpon_database.rx_ier_vector.lofchng = BDMF_TRUE;

    drv_error = ag_drv_gpon_rx_irq_grxier_set(&gs_gpon_database.rx_ier_vector);
    if (drv_error > 0)
	{
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Failed to set Rx interrupts due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
	}

    return PON_NO_ERROR;
}


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
PON_ERROR_DTE gpon_int_lof_disable(void)
{
    int drv_error;

    gs_gpon_database.rx_ier_vector.lofchng = BDMF_FALSE;

    drv_error = ag_drv_gpon_rx_irq_grxier_set(&gs_gpon_database.rx_ier_vector);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Failed to set Rx interrupts due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }
    
    return PON_NO_ERROR;
}


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
PON_ERROR_DTE gpon_show_rx_irs(void)
{
    int drv_error;
    gpon_rx_irq_grxier grxier;
    gpon_rx_irq_grxisr grxisr;

    p_log( ge_onu_logger.sections.isr.debug.rx_event_id,
    "__PwM> gs_gpon_database.rx_ier_vector: 0x%08x", gs_gpon_database.rx_ier_vector);        

    drv_error = ag_drv_gpon_rx_irq_grxier_get(&grxier);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Failed to read Rx interrupts enable due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }
    
    p_log( ge_onu_logger.sections.isr.debug.rx_event_id,
        "__PwM> interrupt_enable_register: 0x%08x", grxier);        
    
    drv_error = ag_drv_gpon_rx_irq_grxisr_get(&grxisr);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Failed to read Rx interrupts status due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }
    
    p_log( ge_onu_logger.sections.isr.debug.rx_event_id,
        "__PwM> interrupt_status_register: 0x%08x", grxisr);        

    return PON_NO_ERROR;
}

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
PON_ERROR_DTE gpon_tx_generate_interrupt_test_register(GPON_TX_ISR gpon_tx_itr)
{
    ag_drv_gpon_tx_interrupt_controller_txitr0_set(txisr0_accirq_tdmirq_idx*2, gpon_tx_itr.tdm_fifo_error_irq);          
    ag_drv_gpon_tx_interrupt_controller_txitr0_set(txisr0_accirq_tdmirq_idx*2+1, gpon_tx_itr.access_fifo_error_irq);          
    ag_drv_gpon_tx_interrupt_controller_txitr0_set(txisr0_uplmirq_nplmirq_idx*2, gpon_tx_itr.normal_ploam_transmitted_irq);          
    ag_drv_gpon_tx_interrupt_controller_txitr0_set(txisr0_uplmirq_nplmirq_idx*2+1, gpon_tx_itr.urgent_ploam_transmitted_irq);             
    ag_drv_gpon_tx_interrupt_controller_txitr0_set(txisr0_rplmirq_iplmirq_idx*2, gpon_tx_itr.idle_ploam_transmitted_irq);               
    ag_drv_gpon_tx_interrupt_controller_txitr0_set(txisr0_rplmirq_iplmirq_idx*2+1, gpon_tx_itr.ranging_ploam_transmitted_irq);                          
    ag_drv_gpon_tx_interrupt_controller_txitr0_set(txisr0_db0irq_idx*2, gpon_tx_itr.dba0_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txitr0_set(txisr0_db1irq_idx*2, gpon_tx_itr.dba1_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txitr0_set(txisr0_db2irq_idx*2, gpon_tx_itr.dba2_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txitr0_set(txisr0_db3irq_idx*2, gpon_tx_itr.dba3_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txitr0_set(txisr0_db4irq_idx*2, gpon_tx_itr.dba4_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txitr0_set(txisr0_db5irq_idx*2, gpon_tx_itr.dba5_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txitr0_set(txisr0_db6irq_idx*2, gpon_tx_itr.dba6_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txitr0_set(txisr0_db7irq_idx*2, gpon_tx_itr.dba7_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txitr0_set(txisr0_db8irq_idx*2, gpon_tx_itr.dba8_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txitr0_set(txisr0_db9irq_idx*2, gpon_tx_itr.dba9_report_transmitted_irq);                      
    ag_drv_gpon_tx_interrupt_controller_txitr0_set(txisr0_db10irq_idx*2, gpon_tx_itr.dba10_report_transmitted_irq);                    
    ag_drv_gpon_tx_interrupt_controller_txitr0_set(txisr0_db11irq_idx*2, gpon_tx_itr.dba11_report_transmitted_irq);                   
    ag_drv_gpon_tx_interrupt_controller_txitr0_set(txisr0_db12irq_idx*2, gpon_tx_itr.dba12_report_transmitted_irq);          

    ag_drv_gpon_tx_interrupt_controller_txitr1_set(txisr1_db13irq_idx*2, gpon_tx_itr.dba13_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txitr1_set(txisr1_db14irq_idx*2, gpon_tx_itr.dba14_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txitr1_set(txisr1_db15irq_idx*2, gpon_tx_itr.dba15_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txitr1_set(txisr1_db16irq_idx*2, gpon_tx_itr.dba16_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txitr1_set(txisr1_db17irq_idx*2, gpon_tx_itr.dba17_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txitr1_set(txisr1_db18irq_idx*2, gpon_tx_itr.dba18_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txitr1_set(txisr1_db19irq_idx*2, gpon_tx_itr.dba19_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txitr1_set(txisr1_db20irq_idx*2, gpon_tx_itr.dba20_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txitr1_set(txisr1_db21irq_idx*2, gpon_tx_itr.dba21_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txitr1_set(txisr1_db22irq_idx*2, gpon_tx_itr.dba22_report_transmitted_irq);                      
    ag_drv_gpon_tx_interrupt_controller_txitr1_set(txisr1_db23irq_idx*2, gpon_tx_itr.dba23_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txitr1_set(txisr1_db24irq_idx*2, gpon_tx_itr.dba24_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txitr1_set(txisr1_db25irq_idx*2, gpon_tx_itr.dba25_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txitr1_set(txisr1_db26irq_idx*2, gpon_tx_itr.dba26_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txitr1_set(txisr1_db27irq_idx*2, gpon_tx_itr.dba27_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txitr1_set(txisr1_db28irq_idx*2, gpon_tx_itr.dba28_report_transmitted_irq);                                          

    ag_drv_gpon_tx_interrupt_controller_txitr2_set(txisr2_db29irq_idx*2, gpon_tx_itr.dba29_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txitr2_set(txisr2_db30irq_idx*2, gpon_tx_itr.dba30_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txitr2_set(txisr2_db31irq_idx*2, gpon_tx_itr.dba31_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txitr2_set(txisr2_db32irq_idx*2, gpon_tx_itr.dba32_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txitr2_set(txisr2_db33irq_idx*2, gpon_tx_itr.dba33_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txitr2_set(txisr2_db34irq_idx*2, gpon_tx_itr.dba34_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txitr2_set(txisr2_db35irq_idx*2, gpon_tx_itr.dba35_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txitr2_set(txisr2_db36irq_idx*2, gpon_tx_itr.dba36_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txitr2_set(txisr2_db37irq_idx*2, gpon_tx_itr.dba37_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txitr2_set(txisr2_db38irq_idx*2, gpon_tx_itr.dba38_report_transmitted_irq);                      
    ag_drv_gpon_tx_interrupt_controller_txitr2_set(txisr2_db39irq_idx*2, gpon_tx_itr.dba39_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txitr2_set(txisr2_rouge_level_ovrirq_idx*2, gpon_tx_itr.overrun_occured_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txitr2_set(txisr2_rouge_level_ovrirq_idx*2+1, gpon_tx_itr.rogue_onu_level);                                          
    ag_drv_gpon_tx_interrupt_controller_txitr2_set(txisr2_reserve_rouge_diff_idx*2, gpon_tx_itr.rogue_onu_diff);                                          

    return PON_NO_ERROR;
}


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
static PON_ERROR_DTE gpon_tx_set_interrupt_enable_register(GPON_TX_ISR gpon_tx_ier)
{
    ag_drv_gpon_tx_interrupt_controller_txier0_set(txisr0_accirq_tdmirq_idx*2, gpon_tx_ier.tdm_fifo_error_irq);          
    ag_drv_gpon_tx_interrupt_controller_txier0_set(txisr0_accirq_tdmirq_idx*2+1, gpon_tx_ier.access_fifo_error_irq);          
    ag_drv_gpon_tx_interrupt_controller_txier0_set(txisr0_uplmirq_nplmirq_idx*2, gpon_tx_ier.normal_ploam_transmitted_irq);          
    ag_drv_gpon_tx_interrupt_controller_txier0_set(txisr0_uplmirq_nplmirq_idx*2+1, gpon_tx_ier.urgent_ploam_transmitted_irq);             
    ag_drv_gpon_tx_interrupt_controller_txier0_set(txisr0_rplmirq_iplmirq_idx*2, gpon_tx_ier.idle_ploam_transmitted_irq);               
    ag_drv_gpon_tx_interrupt_controller_txier0_set(txisr0_rplmirq_iplmirq_idx*2+1, gpon_tx_ier.ranging_ploam_transmitted_irq);                          
    ag_drv_gpon_tx_interrupt_controller_txier0_set(txisr0_db0irq_idx*2, gpon_tx_ier.dba0_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txier0_set(txisr0_db1irq_idx*2, gpon_tx_ier.dba1_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txier0_set(txisr0_db2irq_idx*2, gpon_tx_ier.dba2_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txier0_set(txisr0_db3irq_idx*2, gpon_tx_ier.dba3_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txier0_set(txisr0_db4irq_idx*2, gpon_tx_ier.dba4_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txier0_set(txisr0_db5irq_idx*2, gpon_tx_ier.dba5_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txier0_set(txisr0_db6irq_idx*2, gpon_tx_ier.dba6_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txier0_set(txisr0_db7irq_idx*2, gpon_tx_ier.dba7_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txier0_set(txisr0_db8irq_idx*2, gpon_tx_ier.dba8_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txier0_set(txisr0_db9irq_idx*2, gpon_tx_ier.dba9_report_transmitted_irq);                      
    ag_drv_gpon_tx_interrupt_controller_txier0_set(txisr0_db10irq_idx*2, gpon_tx_ier.dba10_report_transmitted_irq);                    
    ag_drv_gpon_tx_interrupt_controller_txier0_set(txisr0_db11irq_idx*2, gpon_tx_ier.dba11_report_transmitted_irq);                   
    ag_drv_gpon_tx_interrupt_controller_txier0_set(txisr0_db12irq_idx*2, gpon_tx_ier.dba12_report_transmitted_irq);          
                                            
    ag_drv_gpon_tx_interrupt_controller_txier1_set(txisr1_db13irq_idx*2, gpon_tx_ier.dba13_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txier1_set(txisr1_db14irq_idx*2, gpon_tx_ier.dba14_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txier1_set(txisr1_db15irq_idx*2, gpon_tx_ier.dba15_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txier1_set(txisr1_db16irq_idx*2, gpon_tx_ier.dba16_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txier1_set(txisr1_db17irq_idx*2, gpon_tx_ier.dba17_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txier1_set(txisr1_db18irq_idx*2, gpon_tx_ier.dba18_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txier1_set(txisr1_db19irq_idx*2, gpon_tx_ier.dba19_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txier1_set(txisr1_db20irq_idx*2, gpon_tx_ier.dba20_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txier1_set(txisr1_db21irq_idx*2, gpon_tx_ier.dba21_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txier1_set(txisr1_db22irq_idx*2, gpon_tx_ier.dba22_report_transmitted_irq);                      
    ag_drv_gpon_tx_interrupt_controller_txier1_set(txisr1_db23irq_idx*2, gpon_tx_ier.dba23_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txier1_set(txisr1_db24irq_idx*2, gpon_tx_ier.dba24_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txier1_set(txisr1_db25irq_idx*2, gpon_tx_ier.dba25_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txier1_set(txisr1_db26irq_idx*2, gpon_tx_ier.dba26_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txier1_set(txisr1_db27irq_idx*2, gpon_tx_ier.dba27_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txier1_set(txisr1_db28irq_idx*2, gpon_tx_ier.dba28_report_transmitted_irq);                                          
                                            
    ag_drv_gpon_tx_interrupt_controller_txier2_set(txisr2_db29irq_idx*2, gpon_tx_ier.dba29_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txier2_set(txisr2_db30irq_idx*2, gpon_tx_ier.dba30_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txier2_set(txisr2_db31irq_idx*2, gpon_tx_ier.dba31_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txier2_set(txisr2_db32irq_idx*2, gpon_tx_ier.dba32_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txier2_set(txisr2_db33irq_idx*2, gpon_tx_ier.dba33_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txier2_set(txisr2_db34irq_idx*2, gpon_tx_ier.dba34_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txier2_set(txisr2_db35irq_idx*2, gpon_tx_ier.dba35_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txier2_set(txisr2_db36irq_idx*2, gpon_tx_ier.dba36_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txier2_set(txisr2_db37irq_idx*2, gpon_tx_ier.dba37_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txier2_set(txisr2_db38irq_idx*2, gpon_tx_ier.dba38_report_transmitted_irq);                      
    ag_drv_gpon_tx_interrupt_controller_txier2_set(txisr2_db39irq_idx*2, gpon_tx_ier.dba39_report_transmitted_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txier2_set(txisr2_rouge_level_ovrirq_idx*2, gpon_tx_ier.overrun_occured_irq);                                          
    ag_drv_gpon_tx_interrupt_controller_txier2_set(txisr2_rouge_level_ovrirq_idx*2+1, gpon_tx_ier.rogue_onu_level);                                          
    ag_drv_gpon_tx_interrupt_controller_txier2_set(txisr2_reserve_rouge_diff_idx*2, gpon_tx_ier.rogue_onu_diff);                                          

    return PON_NO_ERROR;
}

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
PON_ERROR_DTE gpon_tx_clear_interrupt_status_register(void)
{
    int drv_error;
    int i;

    /* get the interrupt status register vector */
    for (i=0; i < txisr0_idx_num; i++) 
    {
        drv_error = ag_drv_gpon_tx_interrupt_controller_txisr0_set(i, 0x3);
        if (drv_error > 0)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                 "Failed to read txisr0 index %d due to driver error %d", i, drv_error);
            return PON_ERROR_DRIVER_ERROR;
        }

        drv_error = ag_drv_gpon_tx_interrupt_controller_txisr1_set(i, 0x3);
        if (drv_error > 0)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                 "Failed to read txisr1 index %d due to driver error %d", i, drv_error);
            return PON_ERROR_DRIVER_ERROR;
        }


        drv_error = ag_drv_gpon_tx_interrupt_controller_txisr2_set(i, 0x3);
        if (drv_error > 0)
        {
            p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
                 "Failed to read txisr2 index %d due to driver error %d", i, drv_error);
            return PON_ERROR_DRIVER_ERROR;
        }
    }

    return PON_NO_ERROR;
}

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
PON_ERROR_DTE gpon_rx_clear_interrupt_status_reg(void)
{
    gpon_rx_irq_grxisr gpon_rx_isr;
    int drv_error;

    gpon_rx_isr.rxfifo = HIGH;
    gpon_rx_isr.tdmfifo = HIGH;
    gpon_rx_isr.aes = HIGH;
    gpon_rx_isr.accfifo = HIGH;
    gpon_rx_isr.lofchng = HIGH;
    gpon_rx_isr.lcdgchng = HIGH;
    gpon_rx_isr.fecchng = HIGH;
    gpon_rx_isr.tod = HIGH;
    gpon_rx_isr.fwi = HIGH;

    drv_error = ag_drv_gpon_rx_irq_grxisr_set(&gpon_rx_isr);
    if (drv_error > 0)
    {
        p_log(ge_onu_logger.sections.stack.debug.sw_errors_id, 
            "Failed to clear Rx interrupts due to driver error %d.", drv_error);
        return PON_ERROR_DRIVER_ERROR;
    }
    
    return PON_NO_ERROR;
}


static void p_dump_ds_ploam(GPON_DS_PLOAM *ploam_ptr)
{
    p_log(ge_onu_logger.sections.stack.downstream_ploam.print_ploam, 
        "Received Ploam: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x ",
        ploam_ptr->onu_id,ploam_ptr->message_id,
        ploam_ptr ->message.unknown_message[0], ploam_ptr ->message.unknown_message[1],
        ploam_ptr ->message.unknown_message[2], ploam_ptr ->message.unknown_message[3],
        ploam_ptr ->message.unknown_message[4], ploam_ptr ->message.unknown_message[5],
        ploam_ptr ->message.unknown_message[6], ploam_ptr ->message.unknown_message[7],
        ploam_ptr ->message.unknown_message[8], ploam_ptr ->message.unknown_message[9]);

    switch(ploam_ptr->message_id)
    {
    case CE_DS_NO_MESSAGE_MESSAGE_ID:
        break;

    case CE_DS_US_OVERHEAD_MESSAGE_ID:
        p_log(ge_onu_logger.sections.stack.downstream_ploam.upstream_overhead_message_id, 
            "DS PLOAM received: US_OVERHEAD\n"
            "  number_of_guard_bits: %u, \n"
            "  number_of_type_1_preamble_bits: %u, \n"
            "  number_of_type_2_preamble_bits: %u, \n",
            ploam_ptr->message.upstream_overhead.number_of_guard_bits,
            ploam_ptr->message.upstream_overhead.number_of_type_1_preamble_bits,
            ploam_ptr->message.upstream_overhead.number_of_type_2_preamble_bits);
        p_log(ge_onu_logger.sections.stack.downstream_ploam.upstream_overhead_message_id, 
            "  pattern_of_type_3_preamble_bits: 0x%02X, \n"
            "  delimiter: 0x%02X%02X%02X, \n",
            ploam_ptr->message.upstream_overhead.pattern_of_type_3_preamble_bits,
            ploam_ptr->message.upstream_overhead.delimiter[0],
            ploam_ptr->message.upstream_overhead.delimiter[1],
            ploam_ptr->message.upstream_overhead.delimiter[2]);
            p_log(ge_onu_logger.sections.stack.downstream_ploam.upstream_overhead_message_id, "  options: \n"
            "    pre_equalization_status = %s, \n"
            "    serial_number_mask_status = %s, \n"
            "    extra_serial_number_transmissions = %d, \n"
            "    power_level_mode = %d,\n",
            (ploam_ptr->message.upstream_overhead.options.pre_equalization_status == CE_PRE_EQUALIZATION_DELAY_DONT_USE ? "DONT-USE" : "USE" ),
            (ploam_ptr->message.upstream_overhead.options.serial_number_mask_status == CE_SERIAL_NUMBER_MASK_DISABLED ? "DISABLED" : "ENABLED" ),
            ploam_ptr->message.upstream_overhead.options.extra_serial_number_transmissions,
            ploam_ptr->message.upstream_overhead.options.power_level_mode);
        p_log(ge_onu_logger.sections.stack.downstream_ploam.upstream_overhead_message_id, "  preassigned_equalization_delay[0]: %d,\n"
            "  preassigned_equalization_delay[1]: %d,\n",
            ploam_ptr->message.upstream_overhead.preassigned_equalization_delay[0],
            ploam_ptr->message.upstream_overhead.preassigned_equalization_delay[1]);
            break;

    case CE_DS_SERIAL_NUMBER_MASK_MESSAGE_ID:
        p_log(ge_onu_logger.sections.stack.downstream_ploam.serial_number_mask_message_id, 
            "DS PLOAM received: SERIAL_NUMBER_MASK\n"
            " number_of_valid_bits: %d",
            ploam_ptr->message.serial_number_mask.number_of_valid_bits);
        p_log(ge_onu_logger.sections.stack.downstream_ploam.serial_number_mask_message_id,
           " serial_number: 0x%02X%02X%02X%02X%02X%02X%02X%02X",
           ploam_ptr->message.serial_number_mask.serial_number[0],
           ploam_ptr->message.serial_number_mask.serial_number[1],
           ploam_ptr->message.serial_number_mask.serial_number[2],
           ploam_ptr->message.serial_number_mask.serial_number[3],
           ploam_ptr->message.serial_number_mask.serial_number[4],
           ploam_ptr->message.serial_number_mask.serial_number[5],
           ploam_ptr->message.serial_number_mask.serial_number[6],
           ploam_ptr->message.serial_number_mask.serial_number[7]);
        break;

    case CE_DS_ASSIGN_ONU_ID_MESSAGE_ID:
        p_log(ge_onu_logger.sections.stack.downstream_ploam.assign_onu_id_message_id, 
            "DS PLOAM received: ASSIGN_ONU_ID\n"
            " assigned_onu_id: %d", ploam_ptr->message.assign_onu_id.assigned_onu_id);
        p_log(ge_onu_logger.sections.stack.downstream_ploam.assign_onu_id_message_id,
            " serial_number: 0x%02X%02X%02X%02X%02X%02X%02X%02X",
            ploam_ptr->message.assign_onu_id.serial_number[0],
            ploam_ptr->message.assign_onu_id.serial_number[1],
            ploam_ptr->message.assign_onu_id.serial_number[2],
            ploam_ptr->message.assign_onu_id.serial_number[3],
            ploam_ptr->message.assign_onu_id.serial_number[4],
            ploam_ptr->message.assign_onu_id.serial_number[5],
            ploam_ptr->message.assign_onu_id.serial_number[6],
            ploam_ptr->message.assign_onu_id.serial_number[7]);
        break;

    case CE_DS_RANGING_TIME_MESSAGE_ID:
        p_log(ge_onu_logger.sections.stack.downstream_ploam.ranging_time_message_id, 
            "DS PLOAM received: RANGING_TIME\n"
            " options: \n"
            " path: %s \n"
            " delay: 0x%02X%02X%02X%02X",
            (ploam_ptr->message.ranging_time.options.path == CE_PATH_MAIN ? "MAIN" : "PROTECTION"),
            ploam_ptr->message.ranging_time.delay[0],
            ploam_ptr->message.ranging_time.delay[1],
            ploam_ptr->message.ranging_time.delay[2],
            ploam_ptr->message.ranging_time.delay[3]);
        break;

    case CE_DS_DEACTIVATE_ONU_ID_MESSAGE_ID:
        p_log(ge_onu_logger.sections.stack.downstream_ploam.deactivate_onu_id_message_id, 
            "DS PLOAM received: DEACTIVATE_ONU_ID");
        break;

    case CE_DS_DISABLE_SERIAL_NUMBER_MESSAGE_ID:
        p_log(ge_onu_logger.sections.stack.downstream_ploam.disable_serial_number_message_id, 
            "DS PLOAM received: DISABLE_SERIAL_NUMBER\n"
             " control: 0x%02X (%s)",
             ploam_ptr->message.disable_serial_number.control,
             (ploam_ptr->message.disable_serial_number.control == 0x00 ? "ENABLE" :
             (ploam_ptr->message.disable_serial_number.control == 0x0F ? "ENABLE ALL" :
             (ploam_ptr->message.disable_serial_number.control == 0xFF ? "DISABLE" : "UNKNOWN"))));
        p_log(ge_onu_logger.sections.stack.downstream_ploam.disable_serial_number_message_id,
            " serial_number: 0x%02X%02X%02X%02X%02X%02X%02X%02X",
            ploam_ptr->message.disable_serial_number.serial_number[0],
            ploam_ptr->message.disable_serial_number.serial_number[1],
            ploam_ptr->message.disable_serial_number.serial_number[2],
            ploam_ptr->message.disable_serial_number.serial_number[3],
            ploam_ptr->message.disable_serial_number.serial_number[4],
            ploam_ptr->message.disable_serial_number.serial_number[5],
            ploam_ptr->message.disable_serial_number.serial_number[6],
            ploam_ptr->message.disable_serial_number.serial_number[7]);
        break;

    case CE_DS_POPUP_MESSAGE_ID:
        p_log(ge_onu_logger.sections.stack.downstream_ploam.popup_message_id, 
            "DS PLOAM received: POPUP" );
        break;
    case CE_DS_CHANGE_POWER_LEVEL_MESSAGE_ID:
        p_log(ge_onu_logger.sections.stack.downstream_ploam.cpl_message_id, 
            "DS PLOAM received: CHANGE_POWER_LEVEL\n"
            "  indication: %d",
            ploam_ptr->message.change_power_level.indication);
        break;
    case CE_DS_REQUEST_PASSWORD_MESSAGE_ID:
        p_log(ge_onu_logger.sections.stack.downstream_ploam.request_password_message_id, 
            "DS PLOAM CE_DS_REQUEST_PASSWORD_MESSAGE_ID received: Request password arrived");
        break;
    case CE_DS_ASSIGN_ALLOC_ID_MESSAGE_ID:
        p_log(ge_onu_logger.sections.stack.downstream_ploam.assign_alloc_id_message_id, 
            "DS PLOAM CE_DS_ASSIGN_ALLOC_ID_MESSAGE_ID received: Assign alloc id" );
        break;
    case CE_DS_REQUEST_KEY_MESSAGE_ID:
        p_log(ge_onu_logger.sections.stack.downstream_ploam.request_key_message_id, 
            "DS PLOAM CE_DS_REQUEST_KEY_MESSAGE_ID received: REQUEST_KEY" );
        break;
    case CE_DS_CONFIGURE_PORT_ID_MESSAGE_ID:
        p_log(ge_onu_logger.sections.stack.downstream_ploam.configure_port_id_message_id, 
            "DS PLOAM CE_DS_CONFIGURE_PORT_ID_MESSAGE_ID received: Configure port id" );
        break;
    case CE_DS_ENCRYPTED_PORT_ID_MESSAGE_ID:
        p_log(ge_onu_logger.sections.stack.downstream_ploam.encrypted_vpi_port_message_id, 
            "DS PLOAM CE_DS_ENCRYPTED_PORT_ID_MESSAGE_ID received: Encrypred port id" );
        break;
    case CE_DS_PHYSICAL_EQUIPMENT_ERROR_MESSAGE_ID:
        p_log(ge_onu_logger.sections.stack.downstream_ploam.pee_message_id, 
            "DS PLOAM CE_DS_PHYSICAL_EQUIPMENT_ERROR_MESSAGE_ID received: PEE error message arrived" );
        break;
    case CE_DS_PST_MESSAGE_ID:
        p_log(ge_onu_logger.sections.stack.downstream_ploam.pst_message_id, 
            "DS PLOAM CE_DS_PST_MESSAGE_ID received: Not implemented yet" );
        break;
    case CE_DS_BER_INTERVAL_MESSAGE_ID:
        p_log(ge_onu_logger.sections.stack.downstream_ploam.ber_interval_message_id, 
            "DS PLOAM CE_DS_BER_INTERVAL_MESSAGE_ID received: Ber Interval message" );
        break;
    case CE_DS_KEY_SWITCHING_TIME_MESSAGE_ID :
        p_log(ge_onu_logger.sections.stack.downstream_ploam.key_switching_time_id, 
            "DS PLOAM CE_DS_KEY_SWITCHING_TIME_MESSAGE_ID received: AES: Key switching time msg arrived" );
        break;
    case CE_DS_EXTENDED_BURST_LENGTH_MESSAGE_ID :
        p_log(ge_onu_logger.sections.stack.downstream_ploam.extended_burst_id, 
            "DS PLOAM CE_DS_EXTENDED_BURST_LENGTH_MESSAGE_ID received: Extended burst msg arrived" );
        break;
    case CE_DS_SLEEP_ALLOW_MESSAGE_ID : 
		p_log(ge_onu_logger.sections.stack.downstream_ploam.sleep_allow_message_id,
            "DS PLOAM received: SLEEP_ALLOW( %s )",(ploam_ptr->message.sleep_allow.sleep_allow == CE_SLEEP_ALLOW_ON ) ? "ON" : "OFF");
        break;
    case CE_DS_SWIFT_POPUP_MESSAGE_ID:
        p_log(ge_onu_logger.sections.stack.downstream_ploam.swift_popup_message_id, 
            "DS PLOAM CE_DS_SWIFT_POPUP_MESSAGE_ID received");
        break;
    case CE_DS_RANGING_ADJUSTMENT_MESSAGE_ID:
        p_log(ge_onu_logger.sections.stack.downstream_ploam.ranging_adjustment_message_id, 
            "DS PLOAM CE_DS_RANGING_ADJUSTMENT_MESSAGE_ID received: %s the current EqD. EqD Delay: 0x%02X%02X%02X%02X",
            ploam_ptr->message.ranging_adjustment.decrease_flag == 0 ? "Increase" : "Decrease", 
            ploam_ptr->message.ranging_adjustment.eqd_delay[0], ploam_ptr->message.ranging_adjustment.eqd_delay[1],
            ploam_ptr->message.ranging_adjustment.eqd_delay[2], ploam_ptr->message.ranging_adjustment.eqd_delay[3]);
        break;
    case CE_DS_PON_ID_MESSAGE_ID:
        p_log(ge_onu_logger.sections.stack.downstream_ploam.pon_id_message_id, 
            "DS PLOAM CE_DS_RANGING_ADJUSTMENT_MESSAGE_ID received:\n PON Type: %s. Budget class: %s. PON Identifier: 0x%02X%02X%02X%02X%02X%02X%02X Tx Optical Level: 0x%02X%02X", 
            ploam_ptr->message.pon_id.pon_id_type.tol_report == DS_PON_ID_TYPE_REPORT_OLT ? "TOL report OLT's power level" : "TOL report RE's power level",
            ploam_ptr->message.pon_id.pon_id_type.budget_class == DS_PON_ID_TYPE_BUDGET_CLASS_A ? "A" : ploam_ptr->message.pon_id.pon_id_type.budget_class == DS_PON_ID_TYPE_BUDGET_CLASS_B ? "B" :
            ploam_ptr->message.pon_id.pon_id_type.budget_class == DS_PON_ID_TYPE_BUDGET_CLASS_B_PLUS ? "B+" : ploam_ptr->message.pon_id.pon_id_type.budget_class == DS_PON_ID_TYPE_BUDGET_CLASS_C ? "C" :
            ploam_ptr->message.pon_id.pon_id_type.budget_class == DS_PON_ID_TYPE_BUDGET_CLASS_C_PLUS ? "C+" : "Unknown",
            ploam_ptr->message.pon_id.pon_identifier[0], ploam_ptr->message.pon_id.pon_identifier[1], ploam_ptr->message.pon_id.pon_identifier[2],
            ploam_ptr->message.pon_id.pon_identifier[3], ploam_ptr->message.pon_id.pon_identifier[4], ploam_ptr->message.pon_id.pon_identifier[5],
            ploam_ptr->message.pon_id.pon_identifier[6], ploam_ptr->message.pon_id.tx_optical_level[0], ploam_ptr->message.pon_id.tx_optical_level[1]);
		break;

    default :
        p_log(ge_onu_logger.sections.stack.downstream_ploam.general_id, "DS PLOAM Unknown ploam ID: %d ", ploam_ptr->message_id);
        break;
    }
}


#define OMCI_CRC32_POLYNOMIAL 0x04c11db7L /* Standard CRC-32 polynomial */
static uint32_t localCrcTable[256]; /* Table of 8-bit remainders */

void gpon_init_crc32_table_for_omci(void)
{
  uint16_t i;
  uint16_t j;
  uint32_t crcAccum;

  for(i=0; i<256; ++i) {
    crcAccum = (uint32_t)(i << 24);

    for(j=0; j<8; ++j) {
      if ( crcAccum & 0x80000000L ){
        crcAccum = (crcAccum << 1) ^ OMCI_CRC32_POLYNOMIAL;
      }
      else {
        crcAccum = (crcAccum << 1);
      }
    }

    localCrcTable[i] = crcAccum;
  }
}

uint32_t getCrc32(uint32_t crcAccum, uint8_t *pBuf, uint32_t size) 
{
  uint32_t i;
  uint32_t j;

  for (j=0; j<size; j++) {
    i = ((int) (crcAccum >> 24) ^ *pBuf++) & 0xFF;
    crcAccum = (crcAccum << 8) ^ localCrcTable[i];
  }

  crcAccum = ~crcAccum;
  return crcAccum;
}



uint32_t gpon_calc_omci_crc(uint8_t *buffer, uint32_t length)
{
  return getCrc32(~0, buffer, length);
}
