/*
    <:copyright-BRCM:2015:DUAL/GPL:standard
    
       Copyright (c) 2015 Broadcom 
       All Rights Reserved
    
    Unless you and Broadcom execute a separate written software license
    agreement governing use of this software, this software is licensed
    to you under the terms of the GNU General Public License version 2
    (the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
    with the following added to such license:
    
       As a special exception, the copyright holders of this software give
       you permission to link this software with independent modules, and
       to copy and distribute the resulting executable under terms of your
       choice, provided that you also meet, for each linked independent
       module, the terms and conditions of the license of that module.
       An independent module is a module which is not derived from this
       software.  The special exception does not apply to any modifications
       of the software.
    
    Not withstanding the above, under no circumstances may you combine
    this software in any way with any other Broadcom software provided
    under a license other than the GPL, without Broadcom's express prior
    written consent.
    
:>
*/

#ifndef _RDD_IC_COMMON_H
#define _RDD_IC_COMMON_H

/****************************************************************************************
* Compose rule based ctx entry from rdd_ic_context_t
*
* \param[in]   index         rule index
* \param[in]   ctx           ic context to copy
* \param[out]  entry         rule based ctx entry
 *****************************************************************************************/
void rdd_ic_result_entry_compose(uint16_t index, const rdd_ic_context_t *ctx, uint8_t *entry);

/****************************************************************************************
* Compose rdd_ic_context_t entry from rule based ctx
*
* \param[in]  entry         rule based ctx entry to copy
* \param[out] index         rule index
* \param[out] ctx           ic context
 *****************************************************************************************/
void rdd_ic_result_entry_decompose(uint8_t *entry, uint16_t *index, rdd_ic_context_t *ctx);

#endif /* _RDD_IC_COMMON_H */
