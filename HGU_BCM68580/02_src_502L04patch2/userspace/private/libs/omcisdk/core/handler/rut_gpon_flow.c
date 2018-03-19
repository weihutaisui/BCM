/***********************************************************************
 *
 *  Copyright (c) 2008 Broadcom
 *  All Rights Reserved
 *
<:label-BRCM:2012:proprietary:omcid

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

#if defined(DMP_X_ITU_ORG_GPON_1) 

#include "owsvc_api.h"
#include "me_handlers.h"

static CmsRet rutGpon_InsertTagOnSingleTaggedVidTpidDeiAction(UINT16 vlanTagOperOutputTpid,
                                                              OmciVlanTagRule_t *pVlanTagRule,
                                                              OmciPathFlow_t *pPathFlow)
{
    CmsRet ret = CMSRET_SUCCESS;

    /* Tags to remove = 0, Outer treatment prio = 15, Inner treatment prio = Px or 8 */

    /* this function determines the VID, TPID, and DEI values of a newly inserted tag */

    /* vid treatment */

    if(pVlanTagRule->treatment.vlanId <= OMCI_TREATMENT_VLANID_MAX)
    {
        OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_VID,
                             pVlanTagRule->treatment.vlanId, 0, 0);
    }
    else if(pVlanTagRule->treatment.vlanId == OMCI_TREATMENT_VLANID_COPY_FROM_INNER)
    {
        OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_COPY_VID_FROM_TAG, 0, 0, 1);
    }
    else if(pVlanTagRule->treatment.vlanId == OMCI_TREATMENT_VLANID_COPY_FROM_OUTER)
    {
        cmsLog_error("Cannot copy outer VID from single-tagged frame");

        ret = CMSRET_INVALID_ARGUMENTS;
        goto out;
    }
    else
    {
        cmsLog_error("Invalid inner.treatment.vlanId: <%d>",
                     pVlanTagRule->treatment.vlanId);

        ret = CMSRET_INVALID_ARGUMENTS;
        goto out;
    }

    /* tpid/dei treatment */

    switch(pVlanTagRule->treatment.tpid_de)
    {
        case OMCI_TREATMENT_TPID_DE_COPY_FROM_INNER:
            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_COPY_TPID_FROM_TAG, 0, 0, 1);
            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_COPY_DEI_FROM_TAG, 0, 0, 1);
            break;

        case OMCI_TREATMENT_TPID_OUTPUT_DE_COPY_FROM_INNER:
            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_TPID, vlanTagOperOutputTpid, 0, 0);
            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_COPY_DEI_FROM_TAG, 0, 0, 1);
            break;

        case OMCI_TREATMENT_TPID_DE_COPY_FROM_OUTER:
        case OMCI_TREATMENT_TPID_OUTPUT_DE_COPY_FROM_OUTER:
            cmsLog_error("Cannot copy outer TPID/DE from single-tagged frame: <%d>",
                         pVlanTagRule->treatment.tpid_de);
            ret = CMSRET_INVALID_ARGUMENTS;
            goto out;

        case OMCI_TREATMENT_TPID_8100_DE_X:
            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_TPID, C_TAG_TPID, 0, 0);
            /* FIXME: Can we really set DEI to 0 here ??? */
            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_DEI, 0, 0, 0);
            break;

        case OMCI_TREATMENT_TPID_OUTPUT_DE_0:
            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_TPID, vlanTagOperOutputTpid, 0, 0);
            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_DEI, 0, 0, 0);
            break;

        case OMCI_TREATMENT_TPID_OUTPUT_DE_1:
            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_TPID, vlanTagOperOutputTpid, 0, 0);
            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_DEI, 1, 0, 0);
            break;

        case OMCI_TREATMENT_TPID_DE_RESERVED:
        default:
            cmsLog_error("Invalid inner.treatment.tpid_de: <%d>",
                         pVlanTagRule->treatment.tpid_de);
            ret = CMSRET_INVALID_ARGUMENTS;
            goto out;
    }

out:
    return ret;
}

static CmsRet rutGpon_modifySingleTaggedVidTpidDeiAction(UINT16 vlanTagOperOutputTpid,
                                                         OmciExtVlanTagOper_t *pExtVlanTagOper,
                                                         OmciPathFlow_t *pPathFlow,
                                                         UINT8 toTag)
{
    CmsRet ret = CMSRET_SUCCESS;

    /* Tags to remove = 1, Outer treatment prio = 15, Inner treatment prio = Px or 8 */

    /* Tags to remove = 1, Outer treatment prio = Py, Inner treatment prio = Px or 8 */

    /* this function modifies the VID, TPID, and DEI values of an existing tag */

    /* vid treatment */

    if(pExtVlanTagOper->inner.treatment.vlanId <= OMCI_TREATMENT_VLANID_MAX)
    {
        OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_VID,
                             pExtVlanTagOper->inner.treatment.vlanId, toTag, 0);
    }
    else if(pExtVlanTagOper->inner.treatment.vlanId == OMCI_TREATMENT_VLANID_COPY_FROM_INNER)
    {
        /* keep the inner VID */
    }
    else if(pExtVlanTagOper->inner.treatment.vlanId == OMCI_TREATMENT_VLANID_COPY_FROM_OUTER)
    {
        cmsLog_error("Cannot copy outer VID from single-tagged frame");

        ret = CMSRET_INVALID_ARGUMENTS;
        goto out;
    }
    else
    {
        cmsLog_error("Invalid inner.treatment.vlanId: <%d>",
                     pExtVlanTagOper->inner.treatment.vlanId);

        ret = CMSRET_INVALID_ARGUMENTS;
        goto out;
    }

    /* tpid/dei treatment */

    switch(pExtVlanTagOper->inner.treatment.tpid_de)
    {
        case OMCI_TREATMENT_TPID_DE_COPY_FROM_INNER:
            /* keep both the inner TPID and DEI */
            break;

        case OMCI_TREATMENT_TPID_OUTPUT_DE_COPY_FROM_INNER:
            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_TPID, vlanTagOperOutputTpid, toTag, 0);
            /* keep the inner DEI */
            break;

        case OMCI_TREATMENT_TPID_DE_COPY_FROM_OUTER:
        case OMCI_TREATMENT_TPID_OUTPUT_DE_COPY_FROM_OUTER:
            cmsLog_error("Cannot copy outer TPID/DE from single-tagged frame: <%d>",
                         pExtVlanTagOper->inner.treatment.tpid_de);
            ret = CMSRET_INVALID_ARGUMENTS;
            goto out;

        case OMCI_TREATMENT_TPID_8100_DE_X:
            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_TPID, C_TAG_TPID, toTag, 0);
            /* keep the inner DEI */
            break;

        case OMCI_TREATMENT_TPID_OUTPUT_DE_0:
            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_TPID, vlanTagOperOutputTpid, toTag, 0);
            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_DEI, 0, toTag, 0);
            break;

        case OMCI_TREATMENT_TPID_OUTPUT_DE_1:
            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_TPID, vlanTagOperOutputTpid, toTag, 0);
            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_DEI, 1, toTag, 0);
            break;

        case OMCI_TREATMENT_TPID_DE_RESERVED:
        default:
            cmsLog_error("Invalid inner.treatment.tpid_de: <%d>",
                         pExtVlanTagOper->inner.treatment.tpid_de);
            ret = CMSRET_INVALID_ARGUMENTS;
            goto out;
    }

out:
    return ret;
}


static CmsRet rutGpon_InsertModifyTagOnSingleTagged(UINT16 vlanTagOperOutputTpid,
                                                    OmciExtVlanTagOper_t *pExtVlanTagOper,
                                                    OmciPathFlow_t *pPathFlow)
{
    CmsRet ret = CMSRET_SUCCESS;

    if(pExtVlanTagOper->inner.treatment.prio == OMCI_TREATMENT_PRIO_DONT_ADD)
    {
        /* Tags to remove = 1, Outer treatment prio = Py/8, Inner treatment prio = 15 */

        cmsLog_error("Cannot remove inner tag and add outer tag");

        ret = CMSRET_INVALID_ARGUMENTS;
        goto out;
    }
    else if((pExtVlanTagOper->inner.treatment.prio <= OMCI_TREATMENT_PRIO_MAX) ||
            (pExtVlanTagOper->inner.treatment.prio == OMCI_TREATMENT_PRIO_COPY_FROM_INNER) ||
            (pExtVlanTagOper->inner.treatment.prio == OMCI_TREATMENT_PRIO_COPY_FROM_OUTER) ||
            (pExtVlanTagOper->inner.treatment.prio == OMCI_TREATMENT_PRIO_DERIVE_FROM_DSCP))
    {
        /* Tags to remove = 1, Outer treatment prio = Py/8, Inner treatment prio = Px/8 */

        /* insert outer tag */
        OMCI_ADD_ETH_ACTION(pPathFlow, e_ETH_ACTION_PUSH_TAG);

        if(pExtVlanTagOper->outer.treatment.prio <= OMCI_TREATMENT_PRIO_MAX)
        {
            /* set outer pbits to Py */
            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_PBITS,
                                 pExtVlanTagOper->outer.treatment.prio, 0, 0);
        }
        else if (pExtVlanTagOper->outer.treatment.prio == OMCI_TREATMENT_PRIO_COPY_FROM_INNER)
        {
            /* copy pbits from inner tag */
            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_COPY_PBITS_FROM_TAG, 0, 0, 1);
        }
        else if (pExtVlanTagOper->outer.treatment.prio == OMCI_TREATMENT_PRIO_DERIVE_FROM_DSCP)
        {
            /* dscp to pbits from outer tag */
            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_XLATE_PBITS_FROM_DSCP, 0, 0, 0);
        }

        /* process outer VID, TPID, and DEI */
        ret = rutGpon_InsertTagOnSingleTaggedVidTpidDeiAction(vlanTagOperOutputTpid,
                                                              &pExtVlanTagOper->outer,
                                                              pPathFlow);
        if(ret != CMSRET_SUCCESS)
        {
            goto out;
        }

        /* modify existing tag */

        if(pExtVlanTagOper->inner.treatment.prio <= OMCI_TREATMENT_PRIO_MAX)
        {
            /* set inner pbits to Px */
            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_PBITS,
                                 pExtVlanTagOper->inner.treatment.prio, 1, 0);
        }
        else if (pExtVlanTagOper->inner.treatment.prio == OMCI_TREATMENT_PRIO_COPY_FROM_OUTER)
        {
            /* copy pbits from inner tag */
            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_COPY_PBITS_FROM_TAG, 0, 1, 0);
        }
        else if (pExtVlanTagOper->inner.treatment.prio == OMCI_TREATMENT_PRIO_DERIVE_FROM_DSCP)
        {
            /* dscp to pbits from inner tag */
            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_XLATE_PBITS_FROM_DSCP, 0, 1, 1);
        }

        /* process inner VID, TPID, and DEI */
        ret = rutGpon_modifySingleTaggedVidTpidDeiAction(vlanTagOperOutputTpid,
                                                         pExtVlanTagOper,
                                                         pPathFlow,
                                                         1);
        if(ret != CMSRET_SUCCESS)
        {
            goto out;
        }
    }
    else
    {
        cmsLog_error("Invalid inner.treatment.prio: <%d>",
                     pExtVlanTagOper->inner.treatment.prio);

        ret = CMSRET_INVALID_ARGUMENTS;
        goto out;
    }

out:
    return ret;
}

#ifdef SUPPORT_DOUBLE_TAGS
static CmsRet rutGpon_modifyInnerTaggedFlowActions
    (UINT16 vlanTagOperOutputTpid,
     OmciVlanTagRule_t *pExtVlanTagRule,
     OmciPathFlow_t *pPathFlow)
{
    CmsRet ret = CMSRET_SUCCESS;
    
    /* 1. inner of OmciPathFlow_t struct is inner of OmciExtVlanTagOper_t (e_VLAN_TAG1)
       2. outer of OmciPathFlow_t struct is outer of OmciExtVlanTagOper_t (e_VLAN_TAG0) */

    /* INNER pbits treatment */

    if (pExtVlanTagRule->treatment.prio <= OMCI_TREATMENT_PRIO_MAX)
    {
        OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_PBITS,
                             pExtVlanTagRule->treatment.prio, e_VLAN_TAG1, OMCI_DONT_CARE);
    }
    else if(pExtVlanTagRule->treatment.prio == OMCI_TREATMENT_PRIO_COPY_FROM_INNER)
    {
        /* keep pbits from inner tag */
    }
    else if(pExtVlanTagRule->treatment.prio == OMCI_TREATMENT_PRIO_DERIVE_FROM_DSCP)
    {
        pPathFlow->filter.ipv4.cmpOp = e_CMP_EQ;
        pPathFlow->filter.ipv4.type = e_IPV4_FILT_DSCP_1P;
        OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_XLATE_PBITS_FROM_DSCP, 0, e_VLAN_TAG1, OMCI_DONT_CARE);
    }
    else if(pExtVlanTagRule->treatment.prio == OMCI_TREATMENT_PRIO_COPY_FROM_OUTER)
    {
        OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_COPY_PBITS_FROM_TAG, 0, e_VLAN_TAG1, e_VLAN_TAG0);
    }
    else if (pExtVlanTagRule->treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD)
    {
        cmsLog_error("Invalid inner.treatment.prio: <%d>", pExtVlanTagRule->treatment.prio);
        ret = CMSRET_INVALID_ARGUMENTS;
        goto out;
    }

    /* INNER vid treatment */

    if(pExtVlanTagRule->treatment.vlanId <= OMCI_TREATMENT_VLANID_MAX)
    {
        OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_VID,
                             pExtVlanTagRule->treatment.vlanId, e_VLAN_TAG1, OMCI_DONT_CARE);
    }
    else if(pExtVlanTagRule->treatment.vlanId == OMCI_TREATMENT_VLANID_COPY_FROM_INNER)
    {
        /* keep the inner VID */
    }
    else if(pExtVlanTagRule->treatment.vlanId == OMCI_TREATMENT_VLANID_COPY_FROM_OUTER)
    {
        OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_COPY_VID_FROM_TAG, 0, e_VLAN_TAG1, e_VLAN_TAG0);
    }
    else
    {
        cmsLog_error("Invalid inner.treatment.vlanId: <%d>", pExtVlanTagRule->treatment.vlanId);
        ret = CMSRET_INVALID_ARGUMENTS;
        goto out;
    }

    /* INNER tpid/dei treatment */

    switch(pExtVlanTagRule->treatment.tpid_de)
    {
        case OMCI_TREATMENT_TPID_DE_COPY_FROM_INNER:
            /* keep both the inner TPID and DEI */
            break;

        case OMCI_TREATMENT_TPID_DE_COPY_FROM_OUTER:
            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_COPY_TPID_FROM_TAG, 0, e_VLAN_TAG1, e_VLAN_TAG0);
            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_COPY_DEI_FROM_TAG, 0, e_VLAN_TAG1, e_VLAN_TAG0);
            break;

        case OMCI_TREATMENT_TPID_OUTPUT_DE_COPY_FROM_INNER:
            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_TPID, vlanTagOperOutputTpid, e_VLAN_TAG1, OMCI_DONT_CARE);
            break;

        case OMCI_TREATMENT_TPID_OUTPUT_DE_COPY_FROM_OUTER:
            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_TPID, vlanTagOperOutputTpid, e_VLAN_TAG1, OMCI_DONT_CARE);
            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_COPY_DEI_FROM_TAG, 0, e_VLAN_TAG1, e_VLAN_TAG0);
            break;

        case OMCI_TREATMENT_TPID_8100_DE_X:
            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_TPID, C_TAG_TPID, e_VLAN_TAG1, OMCI_DONT_CARE);
            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_DEI, 0, e_VLAN_TAG1, OMCI_DONT_CARE);
            break;

        case OMCI_TREATMENT_TPID_OUTPUT_DE_0:
            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_TPID, vlanTagOperOutputTpid, e_VLAN_TAG1, OMCI_DONT_CARE);
            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_DEI, 0, e_VLAN_TAG1, OMCI_DONT_CARE);
            break;

        case OMCI_TREATMENT_TPID_OUTPUT_DE_1:
            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_TPID, vlanTagOperOutputTpid, e_VLAN_TAG1, OMCI_DONT_CARE);
            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_DEI, 1, e_VLAN_TAG1, OMCI_DONT_CARE);
            break;

        case OMCI_TREATMENT_TPID_DE_RESERVED:
        default:
            cmsLog_error("Invalid inner.treatment.tpid_de: <%d>",
                         pExtVlanTagRule->treatment.tpid_de);
            ret = CMSRET_INVALID_ARGUMENTS;
            goto out;
    }

out:
    return ret;
}

static CmsRet rutGpon_modifyOuterTaggedFlowActions
    (UINT16 vlanTagOperOutputTpid,
     OmciVlanTagRule_t *pExtVlanTagRule,
     OmciPathFlow_t *pPathFlow)
{
    CmsRet ret = CMSRET_SUCCESS;

    /* 1. outer of OmciPathFlow_t struct is outer of OmciExtVlanTagOper_t (e_VLAN_TAG0)
       2. inner of OmciPathFlow_t struct is inner of OmciExtVlanTagOper_t (e_VLAN_TAG1) */

    /* OUTER pbits treatment */

    if (pExtVlanTagRule->treatment.prio <= OMCI_TREATMENT_PRIO_MAX)
    {
        OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_PBITS,
                             pExtVlanTagRule->treatment.prio, e_VLAN_TAG0, OMCI_DONT_CARE);
    }
    else if(pExtVlanTagRule->treatment.prio == OMCI_TREATMENT_PRIO_COPY_FROM_INNER)
    {
        OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_COPY_PBITS_FROM_TAG, 0, e_VLAN_TAG0, e_VLAN_TAG1);
    }
    else if(pExtVlanTagRule->treatment.prio == OMCI_TREATMENT_PRIO_DERIVE_FROM_DSCP)
    {
        pPathFlow->filter.ipv4.cmpOp = e_CMP_EQ;
        pPathFlow->filter.ipv4.type = e_IPV4_FILT_DSCP_1P;
        OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_XLATE_PBITS_FROM_DSCP, 0, e_VLAN_TAG0, OMCI_DONT_CARE);
    }
    else if(pExtVlanTagRule->treatment.prio == OMCI_TREATMENT_PRIO_COPY_FROM_OUTER)
    {
        /* keep pbits from outer tag */
    }
    else if (pExtVlanTagRule->treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD)
    {
        cmsLog_error("Invalid outer.treatment.prio: <%d>", pExtVlanTagRule->treatment.prio);
        ret = CMSRET_INVALID_ARGUMENTS;
        goto out;
    }

    /* OUTER vid treatment */

    if(pExtVlanTagRule->treatment.vlanId <= OMCI_TREATMENT_VLANID_MAX)
    {
        OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_VID,
                             pExtVlanTagRule->treatment.vlanId, e_VLAN_TAG0, OMCI_DONT_CARE);
    }
    else if(pExtVlanTagRule->treatment.vlanId == OMCI_TREATMENT_VLANID_COPY_FROM_INNER)
    {
        OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_COPY_VID_FROM_TAG, 0, e_VLAN_TAG0, e_VLAN_TAG1);
    }
    else if(pExtVlanTagRule->treatment.vlanId == OMCI_TREATMENT_VLANID_COPY_FROM_OUTER)
    {
        /* keep the outer VID */
    }
    else
    {
        cmsLog_error("Invalid outer.treatment.vlanId: <%d>", pExtVlanTagRule->treatment.vlanId);
        ret = CMSRET_INVALID_ARGUMENTS;
        goto out;
    }

    /* OUTER tpid/dei treatment */

    switch(pExtVlanTagRule->treatment.tpid_de)
    {
        case OMCI_TREATMENT_TPID_DE_COPY_FROM_INNER:
            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_COPY_TPID_FROM_TAG, 0, e_VLAN_TAG0, e_VLAN_TAG1);
            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_COPY_DEI_FROM_TAG, 0, e_VLAN_TAG0, e_VLAN_TAG1);
            break;

        case OMCI_TREATMENT_TPID_DE_COPY_FROM_OUTER:
            /* keep both the outer TPID and DEI */
            break;

        case OMCI_TREATMENT_TPID_OUTPUT_DE_COPY_FROM_INNER:
            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_TPID, vlanTagOperOutputTpid, e_VLAN_TAG0, OMCI_DONT_CARE);
            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_COPY_DEI_FROM_TAG, 0, e_VLAN_TAG0, e_VLAN_TAG1);
            break;

        case OMCI_TREATMENT_TPID_OUTPUT_DE_COPY_FROM_OUTER:
            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_TPID, vlanTagOperOutputTpid, e_VLAN_TAG0, OMCI_DONT_CARE);
            break;

        case OMCI_TREATMENT_TPID_8100_DE_X:
            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_TPID, C_TAG_TPID, e_VLAN_TAG0, OMCI_DONT_CARE);
            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_DEI, 0, e_VLAN_TAG0, OMCI_DONT_CARE);
            break;

        case OMCI_TREATMENT_TPID_OUTPUT_DE_0:
            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_TPID, vlanTagOperOutputTpid, e_VLAN_TAG0, OMCI_DONT_CARE);
            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_DEI, 0, e_VLAN_TAG0, OMCI_DONT_CARE);
            break;

        case OMCI_TREATMENT_TPID_OUTPUT_DE_1:
            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_TPID, vlanTagOperOutputTpid, e_VLAN_TAG0, OMCI_DONT_CARE);
            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_DEI, 1, e_VLAN_TAG0, OMCI_DONT_CARE);
            break;

        case OMCI_TREATMENT_TPID_DE_RESERVED:
        default:
            cmsLog_error("Invalid outer.treatment.tpid_de: <%d>",
                         pExtVlanTagRule->treatment.tpid_de);
            ret = CMSRET_INVALID_ARGUMENTS;
            goto out;
    }

out:
    return ret;
}

static CmsRet rutGpon_modifyDoubleTaggedFlowActions
    (UINT16 vlanTagOperOutputTpid,
     OmciVlanTagRule_t *pExtVlanInner,
     OmciVlanTagRule_t *pExtVlanOuter,
     OmciPathFlow_t *pPathFlow)
{
    CmsRet ret = CMSRET_SUCCESS;

    /* Tags to remove = 0, Outer treatment prio = 15, Inner treatment prio = 15 */

    /* Tags to remove = 1, Outer treatment prio = 15, Inner treatment prio = Px or 8, 9, 10 */

    /* Tags to remove = 1, Outer treatment prio = Px or 8, 9, 10, Inner treatment prio = 15 */

    /* Tags to remove = 2, Outer treatment prio = Px or 8, 9, 10, Inner treatment prio = Px or 8, 9, 10 */

    /* this function modifies the VID, TPID, and DEI values of an existing tag */

    /* double tagged frame has outer and inner:
            * outer uses e_VLAN_TAG0
            * inner uses e_VLAN_TAG1 */

    if (pExtVlanInner != NULL)
    {
        ret = rutGpon_modifyInnerTaggedFlowActions(vlanTagOperOutputTpid, pExtVlanInner, pPathFlow);
        if (ret != CMSRET_SUCCESS)
            goto out;
    }

    if (pExtVlanOuter != NULL)
    {
        ret = rutGpon_modifyOuterTaggedFlowActions(vlanTagOperOutputTpid, pExtVlanOuter, pPathFlow);
        if (ret != CMSRET_SUCCESS)
            goto out;
    }

out:
    return ret;
}


static CmsRet rutGpon_processDoubleTaggedFlowActions
    (UINT16 vlanTagOperOutputTpid,
     OmciExtVlanTagOper_t *pExtVlanTagOper,
     OmciPathFlow_t *pPathFlow)
{
    CmsRet ret = CMSRET_SUCCESS;

    /* Confusion as following:
           1. outer of OmciPathFlow_t struct is inner of OmciExtVlanTagOper_t (TAG0)
           2. inner of OmciPathFlow_t struct is outer of OmciExtVlanTagOper_t (TAG1) */

    if(pExtVlanTagOper->removeType == OMCI_TREATMENT_REMOVE_NONE)
    {
        /* Tags to remove = 0 */
        if(pExtVlanTagOper->outer.treatment.prio == OMCI_TREATMENT_PRIO_DONT_ADD &&
           pExtVlanTagOper->inner.treatment.prio == OMCI_TREATMENT_PRIO_DONT_ADD)
        {
            /* Tags to remove = 0, Outer treatment prio = 15, Inner treatment prio = 15 */
            // both outer and inner treatments are 15 ==> no actions
        }
        else if(pExtVlanTagOper->outer.treatment.prio <= OMCI_TREATMENT_PRIO_COPY_FROM_OUTER &&
                pExtVlanTagOper->inner.treatment.prio <= OMCI_TREATMENT_PRIO_COPY_FROM_OUTER)
        {
            /* Tags to remove = 0, Outer treatment prio = Px, Inner treatment prio = Px */
            ret = rutGpon_modifyDoubleTaggedFlowActions(vlanTagOperOutputTpid,
                                                                  &pExtVlanTagOper->inner,
                                                                  &pExtVlanTagOper->outer,
                                                                  pPathFlow);
            if(ret != CMSRET_SUCCESS)
            {
                goto out;
            }
        }
        else
        {
            cmsLog_error("Invalid inner.treatment.prio: <%d>, OR outer.treatment.prio: <%d>",
                         pExtVlanTagOper->inner.treatment.prio, pExtVlanTagOper->outer.treatment.prio);

            ret = CMSRET_INVALID_ARGUMENTS;
            goto out;
        }
    }
    else if(pExtVlanTagOper->removeType == OMCI_TREATMENT_REMOVE_OUTER)
    {
        /* Tags to remove = 1 */
        if(pExtVlanTagOper->outer.treatment.prio == OMCI_TREATMENT_PRIO_DONT_ADD &&
           pExtVlanTagOper->inner.treatment.prio == OMCI_TREATMENT_PRIO_DONT_ADD)
        {
            /* Tags to remove = 1, Outer treatment prio = 15, Inner treatment prio = 15 */

            /* remove existing tag */
            OMCI_ADD_ETH_ACTION(pPathFlow, e_ETH_ACTION_POP_TAG);
        }
        else if(pExtVlanTagOper->outer.treatment.prio == OMCI_TREATMENT_PRIO_DONT_ADD &&
           pExtVlanTagOper->inner.treatment.prio <= OMCI_TREATMENT_PRIO_COPY_FROM_OUTER)
        {
            /* Tags to remove = 1, Outer treatment prio = 15, Inner treatment prio = Px */
            ret = rutGpon_modifyOuterTaggedFlowActions(vlanTagOperOutputTpid,
                                                                  &pExtVlanTagOper->inner,
                                                                  pPathFlow);
            if(ret != CMSRET_SUCCESS)
            {
                goto out;
            }
        }
        else if(pExtVlanTagOper->inner.treatment.prio == OMCI_TREATMENT_PRIO_DONT_ADD &&
           pExtVlanTagOper->outer.treatment.prio <= OMCI_TREATMENT_PRIO_COPY_FROM_OUTER)
        {
            /* Tags to remove = 1, Outer treatment prio = Px, Inner treatment prio = 15 */
            ret = rutGpon_modifyOuterTaggedFlowActions(vlanTagOperOutputTpid,
                                                                  &pExtVlanTagOper->outer,
                                                                  pPathFlow);
            if(ret != CMSRET_SUCCESS)
            {
                goto out;
            }
        }
        else
        {
            /* Tags to remove = 1, Outer treatment prio = Px, Inner treatment prio = Px */
            cmsLog_error("Maximum number of tags can be supported is: 2");

            ret = CMSRET_INVALID_ARGUMENTS;
            goto out;
        }
    }
    else if(pExtVlanTagOper->removeType == OMCI_TREATMENT_REMOVE_BOTH)
    {
        /* Tags to remove = 2 */
        if(pExtVlanTagOper->outer.treatment.prio == OMCI_TREATMENT_PRIO_DONT_ADD &&
           pExtVlanTagOper->inner.treatment.prio == OMCI_TREATMENT_PRIO_DONT_ADD)
        {
            /* Tags to remove = 2, Outer treatment prio = 15, Inner treatment prio = 15 */

            /* remove outer tag */
            OMCI_ADD_ETH_ACTION(pPathFlow, e_ETH_ACTION_POP_TAG);
            /* remove inner tag */
            OMCI_ADD_ETH_ACTION(pPathFlow, e_ETH_ACTION_POP_TAG);
        }
        else if(pExtVlanTagOper->outer.treatment.prio == OMCI_TREATMENT_PRIO_DONT_ADD &&
           pExtVlanTagOper->inner.treatment.prio <= OMCI_TREATMENT_PRIO_COPY_FROM_OUTER)
        {
            /* Tags to remove = 2, Outer treatment prio = 15, Inner treatment prio = Px */
            ret = rutGpon_modifyOuterTaggedFlowActions(vlanTagOperOutputTpid,
                                                                  &pExtVlanTagOper->inner,
                                                                  pPathFlow);
            if(ret != CMSRET_SUCCESS)
            {
                goto out;
            }
            /* remove outer tag */
            OMCI_ADD_ETH_ACTION(pPathFlow, e_ETH_ACTION_POP_TAG);
        }
        else if(pExtVlanTagOper->inner.treatment.prio == OMCI_TREATMENT_PRIO_DONT_ADD &&
           pExtVlanTagOper->outer.treatment.prio <= OMCI_TREATMENT_PRIO_COPY_FROM_OUTER)
        {
            /* Tags to remove = 2, Outer treatment prio = Px, Inner treatment prio = 15 */
            ret = rutGpon_modifyOuterTaggedFlowActions(vlanTagOperOutputTpid,
                                                                  &pExtVlanTagOper->outer,
                                                                  pPathFlow);
            if(ret != CMSRET_SUCCESS)
            {
                goto out;
            }
            /* remove outer tag */
            OMCI_ADD_ETH_ACTION(pPathFlow, e_ETH_ACTION_POP_TAG);
        }
        else
        {
            /* Tags to remove = 2, Outer treatment prio = Px, Inner treatment prio = Px */
            ret = rutGpon_modifyDoubleTaggedFlowActions(vlanTagOperOutputTpid,
                                                                  &pExtVlanTagOper->inner,
                                                                  &pExtVlanTagOper->outer,
                                                                  pPathFlow);
            if(ret != CMSRET_SUCCESS)
            {
                goto out;
            }
        }
    }
    else
    {
        cmsLog_error("Invalid number of tags to remove: <%d>",
                     pExtVlanTagOper->removeType);

        ret = CMSRET_INVALID_ARGUMENTS;
        goto out;
    }

out:
    return ret;
}
#endif    // SUPPORT_DOUBLE_TAGS

static CmsRet rutGpon_processSingleTaggedFlowActions(UINT16 vlanTagOperOutputTpid,
                                                     OmciExtVlanTagOper_t *pExtVlanTagOper,
                                                     OmciPathFlow_t *pPathFlow)
{
    CmsRet ret = CMSRET_SUCCESS;

    if(pExtVlanTagOper->removeType == OMCI_TREATMENT_REMOVE_NONE)
    {
        /* Tags to remove = 0 */
        if(pExtVlanTagOper->outer.treatment.prio == OMCI_TREATMENT_PRIO_DONT_ADD)
        {
            /* Tags to remove = 0, Outer treatment prio = 15 */
            if(pExtVlanTagOper->inner.treatment.prio <= OMCI_TREATMENT_PRIO_MAX)
            {
                /* Tags to remove = 0, Outer treatment prio = 15, Inner treatment prio = Px */

                /* insert 1 tag using Px */

                OMCI_ADD_ETH_ACTION(pPathFlow, e_ETH_ACTION_PUSH_TAG);

                OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_PBITS,
                                     pExtVlanTagOper->inner.treatment.prio, 0, 0);

                ret = rutGpon_InsertTagOnSingleTaggedVidTpidDeiAction(vlanTagOperOutputTpid,
                                                                      &pExtVlanTagOper->inner,
                                                                      pPathFlow);
                if(ret != CMSRET_SUCCESS)
                {
                    goto out;
                }
            }
            else if(pExtVlanTagOper->inner.treatment.prio == OMCI_TREATMENT_PRIO_COPY_FROM_INNER)
            {
                /* Tags to remove = 0, Outer treatment prio = 15, Inner treatment prio = 8 */

                /* insert 1 tag, copying pbits from inner tag */

                OMCI_ADD_ETH_ACTION(pPathFlow, e_ETH_ACTION_PUSH_TAG);

                OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_COPY_PBITS_FROM_TAG, 0, 0, 1);

                ret = rutGpon_InsertTagOnSingleTaggedVidTpidDeiAction(vlanTagOperOutputTpid,
                                                                      &pExtVlanTagOper->inner,
                                                                      pPathFlow);
                if(ret != CMSRET_SUCCESS)
                {
                    goto out;
                }
            }
            else if(pExtVlanTagOper->inner.treatment.prio == OMCI_TREATMENT_PRIO_DERIVE_FROM_DSCP)
            {
                /* Tags to remove = 0, Outer treatment prio = 15, Inner treatment prio = 10 */

                /* insert 1 tag, dscp to pbits from outer tag */

                pPathFlow->filter.ipv4.cmpOp = e_CMP_EQ;
                pPathFlow->filter.ipv4.type = e_IPV4_FILT_DSCP_1P;
                OMCI_ADD_ETH_ACTION(pPathFlow, e_ETH_ACTION_PUSH_TAG);

                OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_XLATE_PBITS_FROM_DSCP, 0, 0, 0);

                ret = rutGpon_InsertTagOnSingleTaggedVidTpidDeiAction(vlanTagOperOutputTpid,
                                                                      &pExtVlanTagOper->inner,
                                                                      pPathFlow);
                if(ret != CMSRET_SUCCESS)
                {
                    goto out;
                }
            }
            else if(pExtVlanTagOper->inner.treatment.prio == OMCI_TREATMENT_PRIO_COPY_FROM_OUTER)
            {
                cmsLog_error("Cannot copy outer PBITS from single-tagged frame");

                ret = CMSRET_INVALID_ARGUMENTS;
                goto out;
            }
            else if(pExtVlanTagOper->inner.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD)
            {
                cmsLog_error("Invalid inner.treatment.prio: <%d>",
                             pExtVlanTagOper->inner.treatment.prio);

                ret = CMSRET_INVALID_ARGUMENTS;
                goto out;
            }
        }
        else if((pExtVlanTagOper->outer.treatment.prio <= OMCI_TREATMENT_PRIO_MAX) ||
                (pExtVlanTagOper->outer.treatment.prio == OMCI_TREATMENT_PRIO_COPY_FROM_INNER) ||
                (pExtVlanTagOper->outer.treatment.prio == OMCI_TREATMENT_PRIO_COPY_FROM_OUTER) ||
                (pExtVlanTagOper->outer.treatment.prio == OMCI_TREATMENT_PRIO_DERIVE_FROM_DSCP))
        {
            cmsLog_error("Double-tag insertion is not supported: <%d>",
                         pExtVlanTagOper->outer.treatment.prio);

            ret = CMSRET_METHOD_NOT_SUPPORTED;
            goto out;
        }
        else
        {
            cmsLog_error("Invalid outer.treatment.prio: <%d>",
                         pExtVlanTagOper->outer.treatment.prio);

            ret = CMSRET_INVALID_ARGUMENTS;
            goto out;
        }
    }
    else if(pExtVlanTagOper->removeType == OMCI_TREATMENT_REMOVE_OUTER)
    {
        /* Tags to remove = 1 */
        if(pExtVlanTagOper->outer.treatment.prio == OMCI_TREATMENT_PRIO_DONT_ADD)
        {
            /* Tags to remove = 1, Outer treatment prio = 15 */
            if(pExtVlanTagOper->inner.treatment.prio == OMCI_TREATMENT_PRIO_DONT_ADD)
            {
                /* Tags to remove = 1, Outer treatment prio = 15, Inner treatment prio = 15 */

                /* remove existing tag */
                OMCI_ADD_ETH_ACTION(pPathFlow, e_ETH_ACTION_POP_TAG);
            }
            else if(pExtVlanTagOper->inner.treatment.prio <= OMCI_TREATMENT_PRIO_MAX)
            {
                /* Tags to remove = 1, Outer treatment prio = 15, Inner treatment prio = Px */

                /* modify existing tag, set pbits */
                OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_PBITS,
                                     pExtVlanTagOper->inner.treatment.prio, 0, 0);

                ret = rutGpon_modifySingleTaggedVidTpidDeiAction(vlanTagOperOutputTpid,
                                                                 pExtVlanTagOper,
                                                                 pPathFlow,
                                                                 0);
                if(ret != CMSRET_SUCCESS)
                {
                    goto out;
                }
            }
            else if(pExtVlanTagOper->inner.treatment.prio == OMCI_TREATMENT_PRIO_COPY_FROM_INNER)
            {
                /* Tags to remove = 1, Outer treatment prio = 15, Inner treatment prio = 8 */

                /* modify existing tag, keep original pbits */
                ret = rutGpon_modifySingleTaggedVidTpidDeiAction(vlanTagOperOutputTpid,
                                                                 pExtVlanTagOper,
                                                                 pPathFlow,
                                                                 0);
                if(ret != CMSRET_SUCCESS)
                {
                    goto out;
                }
            }
            else if(pExtVlanTagOper->inner.treatment.prio == OMCI_TREATMENT_PRIO_DERIVE_FROM_DSCP)
            {
                /* Tags to remove = 1, Outer treatment prio = 15, Inner treatment prio = 10 */

                /* modify existing tag, dscp to pbits from outer tag */
                OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_XLATE_PBITS_FROM_DSCP,
                                     0, 0, 0);

                ret = rutGpon_modifySingleTaggedVidTpidDeiAction(vlanTagOperOutputTpid,
                                                                 pExtVlanTagOper,
                                                                 pPathFlow,
                                                                 0);
                if(ret != CMSRET_SUCCESS)
                {
                    goto out;
                }
            }
            else if(pExtVlanTagOper->inner.treatment.prio == OMCI_TREATMENT_PRIO_COPY_FROM_OUTER)
            {
                cmsLog_error("Cannot copy outer PBITS from single-tagged frame");

                ret = CMSRET_INVALID_ARGUMENTS;
                goto out;
            }
            else
            {
                cmsLog_error("Invalid inner.treatment.prio: <%d>",
                             pExtVlanTagOper->inner.treatment.prio);

                ret = CMSRET_INVALID_ARGUMENTS;
                goto out;
            }
        }
        else if((pExtVlanTagOper->outer.treatment.prio <= OMCI_TREATMENT_PRIO_MAX) ||
                (pExtVlanTagOper->outer.treatment.prio == OMCI_TREATMENT_PRIO_COPY_FROM_INNER) ||
                (pExtVlanTagOper->outer.treatment.prio == OMCI_TREATMENT_PRIO_DERIVE_FROM_DSCP))
        {
            /* Tags to remove = 1, Outer treatment prio = Py/8 */

            ret = rutGpon_InsertModifyTagOnSingleTagged(vlanTagOperOutputTpid,
                                                        pExtVlanTagOper,
                                                        pPathFlow);
            if(ret != CMSRET_SUCCESS)
            {
                goto out;
            }
        }
        else if(pExtVlanTagOper->outer.treatment.prio == OMCI_TREATMENT_PRIO_COPY_FROM_OUTER)
        {
            cmsLog_error("Cannot copy outer PBITS from single-tagged frame");

            ret = CMSRET_INVALID_ARGUMENTS;
            goto out;
        }
        else
        {
            cmsLog_error("Invalid outer.treatment.prio: <%d>",
                         pExtVlanTagOper->outer.treatment.prio);

            ret = CMSRET_INVALID_ARGUMENTS;
            goto out;
        }
    }
    else if(pExtVlanTagOper->removeType == OMCI_TREATMENT_REMOVE_BOTH)
    {
        cmsLog_error("Cannot remove 2 tags from single tagged frame");

        ret = CMSRET_INVALID_ARGUMENTS;
        goto out;
    }
    else
    {
        cmsLog_error("Invalid number of tags to remove: <%d>",
                     pExtVlanTagOper->removeType);

        ret = CMSRET_INVALID_ARGUMENTS;
        goto out;
    }

out:
    return ret;
}

#ifdef SUPPORT_DOUBLE_TAGS
static CmsRet rutGpon_processDoubleTaggedFlow(UINT16 vlanTagOperInputTpid,
                                              UINT16 vlanTagOperOutputTpid,
                                              OmciExtVlanTagOper_t *pExtVlanTagOper,
                                              OmciPathFlow_t *pPathFlow,
                                              UINT8 *pIsDefaultRule)
{
    CmsRet ret = CMSRET_SUCCESS;

    *pIsDefaultRule = 0;

    /* 1. outer of OmciPathFlow_t struct is outer of OmciExtVlanTagOper_t (e_VLAN_TAG0)
       2. inner of OmciPathFlow_t struct is inner of OmciExtVlanTagOper_t (e_VLAN_TAG1) */

    /* check if this is not the default rule */
    if(pExtVlanTagOper->inner.filter.prio == OMCI_FILTER_PRIO_DEFAULT &&
       pExtVlanTagOper->outer.filter.prio == OMCI_FILTER_PRIO_DEFAULT)
    {
        omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "Double-Tagged Flow: DEFAULT\n");

        /* default rule, do nothing */
        *pIsDefaultRule = 1;
    }
    else
    {
        omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "Double-Tagged Flow\n");

        /* pbits filtering */

        if(pExtVlanTagOper->inner.filter.prio <= OMCI_FILTER_PRIO_MAX)
        {
            /* create a pbits filter */
            pPathFlow->filter.vlan.inner.pbits = pExtVlanTagOper->inner.filter.prio;
        }
        else if(pExtVlanTagOper->inner.filter.prio != OMCI_FILTER_PRIO_NONE)
        {
            cmsLog_error("Invalid inner.filter.prio: <%d>",
                         pExtVlanTagOper->inner.filter.prio);

            ret = CMSRET_INVALID_ARGUMENTS;
            goto out;
        }

        if(pExtVlanTagOper->outer.filter.prio <= OMCI_FILTER_PRIO_MAX)
        {
            /* create a pbits filter */
            pPathFlow->filter.vlan.outer.pbits = pExtVlanTagOper->outer.filter.prio;
        }
        else if(pExtVlanTagOper->outer.filter.prio != OMCI_FILTER_PRIO_NONE)
        {
            cmsLog_error("Invalid outer.filter.prio: <%d>",
                         pExtVlanTagOper->outer.filter.prio);

            ret = CMSRET_INVALID_ARGUMENTS;
            goto out;
        }

        /* vid filtering */

        if(pExtVlanTagOper->inner.filter.vlanId <= OMCI_FILTER_VLANID_MAX)
        {
            /* create a vid filter */
            pPathFlow->filter.vlan.inner.vid = pExtVlanTagOper->inner.filter.vlanId;
        }
        else if(pExtVlanTagOper->inner.filter.vlanId != OMCI_FILTER_VLANID_NONE)
        {
            cmsLog_error("Invalid inner.filter.vlanId: <%d>",
                         pExtVlanTagOper->inner.filter.vlanId);

            ret = CMSRET_INVALID_ARGUMENTS;
            goto out;
        }

        if(pExtVlanTagOper->outer.filter.vlanId <= OMCI_FILTER_VLANID_MAX)
        {
            /* create a vid filter */
            pPathFlow->filter.vlan.outer.vid = pExtVlanTagOper->outer.filter.vlanId;
        }
        else if(pExtVlanTagOper->outer.filter.vlanId != OMCI_FILTER_VLANID_NONE)
        {
            cmsLog_error("Invalid outer.filter.vlanId: <%d>",
                         pExtVlanTagOper->outer.filter.vlanId);

            ret = CMSRET_INVALID_ARGUMENTS;
            goto out;
        }

        /* tpid/dei filtering */

        if(pExtVlanTagOper->inner.filter.tpid_de == OMCI_FILTER_TPID_8100_DE_X)
        {
            /* create tpid filter */
            pPathFlow->filter.vlan.inner.tpid = Q_TAG_TPID;
            /* create dei filter */
            pPathFlow->filter.vlan.inner.dei = 0;
        }
        else if(pExtVlanTagOper->inner.filter.tpid_de == OMCI_FILTER_TPID_INPUT_DE_X)
        {
            /* create tpid filter */
            pPathFlow->filter.vlan.inner.tpid = vlanTagOperInputTpid;
        }
        else if(pExtVlanTagOper->inner.filter.tpid_de == OMCI_FILTER_TPID_INPUT_DE_0)
        {
            /* create tpid filter */
            pPathFlow->filter.vlan.inner.tpid = vlanTagOperInputTpid;
            /* create dei filter */
            pPathFlow->filter.vlan.inner.dei = 0;
        }
        else if(pExtVlanTagOper->inner.filter.tpid_de == OMCI_FILTER_TPID_INPUT_DE_1)
        {
            /* create tpid filter */
            pPathFlow->filter.vlan.inner.tpid = vlanTagOperInputTpid;
            /* create dei filter */
            pPathFlow->filter.vlan.inner.dei = 1;
        }
        else if(pExtVlanTagOper->inner.filter.tpid_de != OMCI_FILTER_TPID_DE_NONE)
        {
            cmsLog_error("Invalid inner.filter.tpid_de: <%d>",
                         pExtVlanTagOper->inner.filter.tpid_de);

            ret = CMSRET_INVALID_ARGUMENTS;
            goto out;
        }

        if(pExtVlanTagOper->outer.filter.tpid_de == OMCI_FILTER_TPID_8100_DE_X)
        {
            /* create tpid filter */
            pPathFlow->filter.vlan.outer.tpid = Q_TAG_TPID;
            /* create dei filter */
            pPathFlow->filter.vlan.outer.dei = 0;
        }
        else if(pExtVlanTagOper->outer.filter.tpid_de == OMCI_FILTER_TPID_INPUT_DE_X)
        {
            /* create tpid filter */
            pPathFlow->filter.vlan.outer.tpid = vlanTagOperInputTpid;
        }
        else if(pExtVlanTagOper->outer.filter.tpid_de == OMCI_FILTER_TPID_INPUT_DE_0)
        {
            /* create tpid filter */
            pPathFlow->filter.vlan.outer.tpid = vlanTagOperInputTpid;
            /* create dei filter */
            pPathFlow->filter.vlan.outer.dei = 0;
        }
        else if(pExtVlanTagOper->outer.filter.tpid_de == OMCI_FILTER_TPID_INPUT_DE_1)
        {
            /* create tpid filter */
            pPathFlow->filter.vlan.outer.tpid = vlanTagOperInputTpid;
            /* create dei filter */
            pPathFlow->filter.vlan.outer.dei = 1;
        }
        else if(pExtVlanTagOper->outer.filter.tpid_de != OMCI_FILTER_TPID_DE_NONE)
        {
            cmsLog_error("Invalid outer.filter.tpid_de: <%d>",
                         pExtVlanTagOper->outer.filter.tpid_de);

            ret = CMSRET_INVALID_ARGUMENTS;
            goto out;
        }

        /* ethertype filtering */

        if(pExtVlanTagOper->etherType == OMCI_FILTER_ETHER_IPOE)
        {
            pPathFlow->filter.ethType.cmpOp = e_CMP_EQ;
            pPathFlow->filter.ethType.val = OMCI_FILTER_ETHER_0800;
        }
        else if(pExtVlanTagOper->etherType == OMCI_FILTER_ETHER_PPPOE)
        {
            pPathFlow->filter.ethType.cmpOp = e_CMP_EQ;
            pPathFlow->filter.ethType.val = OMCI_FILTER_ETHER_8863;
        }
        else if(pExtVlanTagOper->etherType == OMCI_FILTER_ETHER_ARP)
        {
            pPathFlow->filter.ethType.cmpOp = e_CMP_EQ;
            pPathFlow->filter.ethType.val = OMCI_FILTER_ETHER_0806;
        }
        else
        {
            if (pExtVlanTagOper->etherType == OMCI_FILTER_ETHER_NONE)
            {
                pPathFlow->filter.ethType.cmpOp = e_CMP_NEQ;
                pPathFlow->filter.ethType.val = OMCI_DONT_CARE;
            }
            else
            {
                cmsLog_error("Invalid etherType filter: <%d>",
                             pExtVlanTagOper->etherType);

                ret = CMSRET_INVALID_ARGUMENTS;
                goto out;
            }
        }

        /* 
         * if we reach here, all filters have been processed successfully.
         * Let's parse the treatments.
         */
        ret = rutGpon_processDoubleTaggedFlowActions(vlanTagOperOutputTpid,
                                                     pExtVlanTagOper,
                                                     pPathFlow);
        if(ret != CMSRET_SUCCESS)
        {
            goto out;
        }
    }

out:
    return ret;
}
#endif    // SUPPORT_DOUBLE_TAGS

static CmsRet rutGpon_processSingleTaggedFlow(UINT16 vlanTagOperInputTpid,
                                              UINT16 vlanTagOperOutputTpid,
                                              OmciExtVlanTagOper_t *pExtVlanTagOper,
                                              OmciPathFlow_t *pPathFlow,
                                              UINT8 *pIsDefaultRule)
{
    CmsRet ret = CMSRET_SUCCESS;

    *pIsDefaultRule = 0;

    /* check if this is not the default rule */
    if(pExtVlanTagOper->inner.filter.prio == OMCI_FILTER_PRIO_DEFAULT)
    {
        omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "Single-Tagged Flow: DEFAULT\n");

        /* default rule, do nothing */
        *pIsDefaultRule = 1;
    }
    else
    {
        omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "Single-Tagged Flow\n");

        /* pbits filtering */

        if(pExtVlanTagOper->inner.filter.prio <= OMCI_FILTER_PRIO_MAX)
        {
            /* create a pbits filter */
            pPathFlow->filter.vlan.inner.pbits = pExtVlanTagOper->inner.filter.prio;
        }
        else if(pExtVlanTagOper->inner.filter.prio != OMCI_FILTER_PRIO_NONE)
        {
            cmsLog_error("Invalid inner.filter.prio: <%d>",
                         pExtVlanTagOper->inner.filter.prio);

            ret = CMSRET_INVALID_ARGUMENTS;
            goto out;
        }

        /* vid filtering */

        if(pExtVlanTagOper->inner.filter.vlanId <= OMCI_FILTER_VLANID_MAX)
        {
            /* create a vid filter */
            pPathFlow->filter.vlan.inner.vid = pExtVlanTagOper->inner.filter.vlanId;
        }
        else if(pExtVlanTagOper->inner.filter.vlanId != OMCI_FILTER_VLANID_NONE)
        {
            cmsLog_error("Invalid inner.filter.vlanId: <%d>",
                         pExtVlanTagOper->inner.filter.vlanId);

            ret = CMSRET_INVALID_ARGUMENTS;
            goto out;
        }

        /* tpid/dei filtering */

        if(pExtVlanTagOper->inner.filter.tpid_de == OMCI_FILTER_TPID_8100_DE_X)
        {
            /* create tpid filter */
            pPathFlow->filter.vlan.inner.tpid = Q_TAG_TPID;
        }
        else if(pExtVlanTagOper->inner.filter.tpid_de == OMCI_FILTER_TPID_INPUT_DE_X)
        {
            /* create tpid filter */
            pPathFlow->filter.vlan.inner.tpid = vlanTagOperInputTpid;
        }
        else if(pExtVlanTagOper->inner.filter.tpid_de == OMCI_FILTER_TPID_INPUT_DE_0)
        {
            /* create tpid filter */
            pPathFlow->filter.vlan.inner.tpid = vlanTagOperInputTpid;
            /* create dei filter */
            pPathFlow->filter.vlan.inner.dei = 0;
        }
        else if(pExtVlanTagOper->inner.filter.tpid_de == OMCI_FILTER_TPID_INPUT_DE_1)
        {
            /* create tpid filter */
            pPathFlow->filter.vlan.inner.tpid = vlanTagOperInputTpid;
            /* create dei filter */
            pPathFlow->filter.vlan.inner.dei = 1;
        }
        else if(pExtVlanTagOper->inner.filter.tpid_de != OMCI_FILTER_TPID_DE_NONE)
        {
            cmsLog_error("Invalid inner.filter.tpid_de: <%d>",
                         pExtVlanTagOper->inner.filter.tpid_de);

            ret = CMSRET_INVALID_ARGUMENTS;
            goto out;
        }

        /* ethertype filtering */

        if(pExtVlanTagOper->etherType == OMCI_FILTER_ETHER_IPOE)
        {
            pPathFlow->filter.ethType.cmpOp = e_CMP_EQ;
            pPathFlow->filter.ethType.val = OMCI_FILTER_ETHER_0800;
        }
        else if(pExtVlanTagOper->etherType == OMCI_FILTER_ETHER_PPPOE)
        {
            pPathFlow->filter.ethType.cmpOp = e_CMP_EQ;
            pPathFlow->filter.ethType.val = OMCI_FILTER_ETHER_8863;
        }
        else if(pExtVlanTagOper->etherType == OMCI_FILTER_ETHER_ARP)
        {
            pPathFlow->filter.ethType.cmpOp = e_CMP_EQ;
            pPathFlow->filter.ethType.val = OMCI_FILTER_ETHER_0806;
        }
        else
        {
            if (pExtVlanTagOper->etherType == OMCI_FILTER_ETHER_NONE)
            {
                pPathFlow->filter.ethType.cmpOp = e_CMP_NEQ;
                pPathFlow->filter.ethType.val = OMCI_DONT_CARE;
            }
            else
            {
                cmsLog_error("Invalid etherType filter: <%d>",
                             pExtVlanTagOper->etherType);

                ret = CMSRET_INVALID_ARGUMENTS;
                goto out;
            }
        }

        /* 
         * if we reach here, all filters have been processed successfully.
         * Let's parse the treatments.
         */
        ret = rutGpon_processSingleTaggedFlowActions(vlanTagOperOutputTpid,
                                                     pExtVlanTagOper,
                                                     pPathFlow);
        if(ret != CMSRET_SUCCESS)
        {
            goto out;
        }
    }

out:
    return ret;
}

#ifdef SUPPORT_DOUBLE_TAGS
static CmsRet rutGpon_processUntaggedFlow(UINT16 vlanTagOperOutputTpid,
                                          OmciExtVlanTagOper_t *pExtVlanTagOper,
                                          OmciPathFlow_t *pPathFlow,
                                          UINT8 *pIsDefaultRule)
{
    CmsRet ret = CMSRET_SUCCESS;

    *pIsDefaultRule = 0;

    if(pExtVlanTagOper->etherType == OMCI_FILTER_ETHER_IPOE)
    {
        pPathFlow->filter.ethType.cmpOp = e_CMP_EQ;
        pPathFlow->filter.ethType.val = OMCI_FILTER_ETHER_0800;
    }
    else if(pExtVlanTagOper->etherType == OMCI_FILTER_ETHER_PPPOE)
    {
        pPathFlow->filter.ethType.cmpOp = e_CMP_EQ;
        pPathFlow->filter.ethType.val = OMCI_FILTER_ETHER_8863;
    }
    else if(pExtVlanTagOper->etherType == OMCI_FILTER_ETHER_ARP)
    {
        pPathFlow->filter.ethType.cmpOp = e_CMP_EQ;
        pPathFlow->filter.ethType.val = OMCI_FILTER_ETHER_0806;
    }
    else
    {
        if (pExtVlanTagOper->etherType == OMCI_FILTER_ETHER_NONE)
        {
            pPathFlow->filter.ethType.val = OMCI_DONT_CARE;
        }
        else
        {
            cmsLog_error("Invalid etherType filter: <%d>",
                         pExtVlanTagOper->etherType);

            ret = CMSRET_INVALID_ARGUMENTS;
            goto out;
        }
    }

    if(pExtVlanTagOper->outer.treatment.prio == OMCI_TREATMENT_PRIO_DONT_ADD)
    {
        //988 amd2: etherType is valid criterion of no-tag rule
        if(pExtVlanTagOper->inner.treatment.prio == OMCI_TREATMENT_PRIO_DONT_ADD &&
           pExtVlanTagOper->etherType == 0)
        {
            omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "Untagged Flow: DEFAULT\n");

            *pIsDefaultRule = 1;
            /* default rule, do nothing */
        }
        else
        {
            omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "Untagged Flow\n");

            /* insert 1 VLAN tag */
            if(pExtVlanTagOper->inner.treatment.prio <= OMCI_TREATMENT_PRIO_MAX)
            {
                if(pExtVlanTagOper->inner.treatment.vlanId <= OMCI_TREATMENT_VLANID_MAX)
                {
                    omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "Insert Tag\n");

                    OMCI_ADD_ETH_ACTION(pPathFlow, e_ETH_ACTION_PUSH_TAG);

                    switch(pExtVlanTagOper->inner.treatment.tpid_de)
                    {
                        case OMCI_TREATMENT_TPID_8100_DE_X:
                            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_TPID, C_TAG_TPID, 0, 0);
                            /* FIXME: Can we really set DEI to 0 here ??? */
                            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_DEI, 0, 0, 0);
                            break;

                        case OMCI_TREATMENT_TPID_OUTPUT_DE_COPY_FROM_INNER: /* allow it for un-tagged rule, iop with ALU */
                        case OMCI_TREATMENT_TPID_OUTPUT_DE_0:
                            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_TPID,
                                                 vlanTagOperOutputTpid, 0, 0);
                            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_DEI, 0, 0, 0);
                            break;

                        case OMCI_TREATMENT_TPID_OUTPUT_DE_1:
                            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_TPID,
                                                 vlanTagOperOutputTpid, 0, 0);
                            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_DEI, 1, 0, 0);
                            break;

                        default:
                            cmsLog_error("Must provide TPID and DEI values on tag insertion <%d>",
                                         pExtVlanTagOper->inner.treatment.tpid_de);

                            ret = CMSRET_INVALID_ARGUMENTS;
                            goto out;
                    }

                    OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_PBITS,
                                         pExtVlanTagOper->inner.treatment.prio, 0, 0);

                    OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_VID,
                                         pExtVlanTagOper->inner.treatment.vlanId, 0, 0);
                }
                else
                {
                    cmsLog_error("inner.treatment.vlanId must be a constant: <%d>",
                                 pExtVlanTagOper->inner.treatment.vlanId);

                    ret = CMSRET_INVALID_ARGUMENTS;
                    goto out;
                }
            }
            else if (pExtVlanTagOper->inner.treatment.prio == OMCI_TREATMENT_PRIO_DERIVE_FROM_DSCP)
            {
                omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "Untagged Flow\n");
    
                pPathFlow->filter.ipv4.cmpOp = e_CMP_EQ;
                pPathFlow->filter.ipv4.type = e_IPV4_FILT_DSCP_1P;
                OMCI_ADD_ETH_ACTION(pPathFlow, e_ETH_ACTION_PUSH_TAG);

                OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_XLATE_PBITS_FROM_DSCP, 0, 0, 0);

                switch(pExtVlanTagOper->inner.treatment.tpid_de)
                {
                    case OMCI_TREATMENT_TPID_8100_DE_X:
                        OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_TPID, C_TAG_TPID, 0, 0);
                        /* FIXME: Can we really set DEI to 0 here ??? */
                        OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_DEI, 0, 0, 0);
                        break;

                    case OMCI_TREATMENT_TPID_OUTPUT_DE_COPY_FROM_INNER: /* allow it for un-tagged rule, iop with ALU */
                    case OMCI_TREATMENT_TPID_OUTPUT_DE_0:
                        OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_TPID,
                                             vlanTagOperOutputTpid, 0, 0);
                        OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_DEI, 0, 0, 0);
                        break;

                    case OMCI_TREATMENT_TPID_OUTPUT_DE_1:
                        OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_TPID,
                                             vlanTagOperOutputTpid, 0, 0);
                        OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_DEI, 1, 0, 0);
                        break;

                    default:
                        cmsLog_error("Must provide TPID and DEI values on tag insertion <%d>",
                                     pExtVlanTagOper->inner.treatment.tpid_de);
                        ret = CMSRET_INVALID_ARGUMENTS;
                            goto out;
                }
                OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_VID,
                                     pExtVlanTagOper->inner.treatment.vlanId, 0, 0);

                if(ret != CMSRET_SUCCESS)
                {
                    goto out;
                }

            }

            else if (pExtVlanTagOper->inner.treatment.prio == OMCI_TREATMENT_PRIO_DONT_ADD)
            {
                /* don't insert tag, do nothing */
            }
            else
            {
                cmsLog_error("inner.treatment.prio must be a constant: <%d>",
                             pExtVlanTagOper->inner.treatment.prio);

                ret = CMSRET_INVALID_ARGUMENTS;
                goto out;
            }
        }
    }
    else
    {
        if(pExtVlanTagOper->outer.treatment.prio <= OMCI_TREATMENT_PRIO_COPY_FROM_OUTER &&
           pExtVlanTagOper->inner.treatment.prio <= OMCI_TREATMENT_PRIO_COPY_FROM_OUTER)
        {
            omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "Insert Inner Tag\n");
            OMCI_ADD_ETH_ACTION(pPathFlow, e_ETH_ACTION_PUSH_TAG);
            omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "Insert Outer Tag\n");
            OMCI_ADD_ETH_ACTION(pPathFlow, e_ETH_ACTION_PUSH_TAG);

            ret = rutGpon_modifyDoubleTaggedFlowActions(vlanTagOperOutputTpid,
                                                                  &pExtVlanTagOper->inner,
                                                                  &pExtVlanTagOper->outer,
                                                                  pPathFlow);
            if(ret != CMSRET_SUCCESS)
            {
                goto out;
            }
        }
        else
        {		
            cmsLog_error("Vlan tagging operation is not supported");
            ret = CMSRET_METHOD_NOT_SUPPORTED;
            goto out;
        }
    }

out:
    return ret;
}
#else    // SUPPORT_DOUBLE_TAGS
static CmsRet rutGpon_processUntaggedFlow(UINT16 vlanTagOperOutputTpid,
                                          OmciExtVlanTagOper_t *pExtVlanTagOper,
                                          OmciPathFlow_t *pPathFlow,
                                          UINT8 *pIsDefaultRule)
{
    CmsRet ret = CMSRET_SUCCESS;

    *pIsDefaultRule = 0;

    if(pExtVlanTagOper->outer.treatment.prio == OMCI_TREATMENT_PRIO_DONT_ADD)
    {
        //988: etherType is valid criterion of no-tag rule
        if(pExtVlanTagOper->inner.treatment.prio == OMCI_TREATMENT_PRIO_DONT_ADD &&
           pExtVlanTagOper->etherType == 0)
        {
            omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "Untagged Flow: DEFAULT\n");

            *pIsDefaultRule = 1;
        }
        else
        {
            omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "Untagged Flow\n");

            if(pExtVlanTagOper->etherType == OMCI_FILTER_ETHER_IPOE)
            {
                pPathFlow->filter.ethType.cmpOp = e_CMP_EQ;
                pPathFlow->filter.ethType.val = OMCI_FILTER_ETHER_0800;
            }
            else if(pExtVlanTagOper->etherType == OMCI_FILTER_ETHER_PPPOE)
            {
                pPathFlow->filter.ethType.cmpOp = e_CMP_EQ;
                pPathFlow->filter.ethType.val = OMCI_FILTER_ETHER_8863;
            }
            else if(pExtVlanTagOper->etherType == OMCI_FILTER_ETHER_ARP)
            {
                pPathFlow->filter.ethType.cmpOp = e_CMP_EQ;
                pPathFlow->filter.ethType.val = OMCI_FILTER_ETHER_0806;
            }
            else
            {
                if (pExtVlanTagOper->etherType == OMCI_FILTER_ETHER_NONE)
                {
                    pPathFlow->filter.ethType.val = OMCI_DONT_CARE;
                }
                else
                {
                    cmsLog_error("Invalid etherType filter: <%d>",
                                 pExtVlanTagOper->etherType);

                    ret = CMSRET_INVALID_ARGUMENTS;
                    goto out;
                }
            }

            /* insert 1 VLAN tag */
            if(pExtVlanTagOper->inner.treatment.prio <= OMCI_TREATMENT_PRIO_MAX)
            {
                if(pExtVlanTagOper->inner.treatment.vlanId <= OMCI_TREATMENT_VLANID_MAX)
                {
                    omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "Insert Tag\n");

                    OMCI_ADD_ETH_ACTION(pPathFlow, e_ETH_ACTION_PUSH_TAG);

                    switch(pExtVlanTagOper->inner.treatment.tpid_de)
                    {
                        case OMCI_TREATMENT_TPID_8100_DE_X:
                            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_TPID, C_TAG_TPID, 0, 0);
                            /* FIXME: Can we really set DEI to 0 here ??? */
                            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_DEI, 0, 0, 0);
                            break;

                        case OMCI_TREATMENT_TPID_OUTPUT_DE_COPY_FROM_INNER: /* allow it for un-tagged rule, iop with ALU */
                        case OMCI_TREATMENT_TPID_OUTPUT_DE_0:
                            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_TPID,
                                                 vlanTagOperOutputTpid, 0, 0);
                            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_DEI, 0, 0, 0);
                            break;

                        case OMCI_TREATMENT_TPID_OUTPUT_DE_1:
                            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_TPID,
                                                 vlanTagOperOutputTpid, 0, 0);
                            OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_DEI, 1, 0, 0);
                            break;

                        default:
                            cmsLog_error("Must provide TPID and DEI values on tag insertion <%d>",
                                         pExtVlanTagOper->inner.treatment.tpid_de);

                            ret = CMSRET_INVALID_ARGUMENTS;
                            goto out;
                    }

                    OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_PBITS,
                                         pExtVlanTagOper->inner.treatment.prio, 0, 0);

                    OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_VID,
                                         pExtVlanTagOper->inner.treatment.vlanId, 0, 0);
                }
                else
                {
                    cmsLog_error("inner.treatment.vlanId must be a constant: <%d>",
                                 pExtVlanTagOper->inner.treatment.vlanId);

                    ret = CMSRET_INVALID_ARGUMENTS;
                    goto out;
                }
            }
            else if (pExtVlanTagOper->inner.treatment.prio == OMCI_TREATMENT_PRIO_DERIVE_FROM_DSCP)
            {
                omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "Untagged Flow\n");
    
                if(pExtVlanTagOper->etherType == OMCI_FILTER_ETHER_IPOE)
                {
                    pPathFlow->filter.ethType.cmpOp = e_CMP_EQ;
                    pPathFlow->filter.ethType.val = OMCI_FILTER_ETHER_0800;
                }
                else if(pExtVlanTagOper->etherType == OMCI_FILTER_ETHER_PPPOE)
                {
                    pPathFlow->filter.ethType.cmpOp = e_CMP_EQ;
                    pPathFlow->filter.ethType.val = OMCI_FILTER_ETHER_8863;
                }
                else if(pExtVlanTagOper->etherType == OMCI_FILTER_ETHER_ARP)
                {
                    pPathFlow->filter.ethType.cmpOp = e_CMP_EQ;
                    pPathFlow->filter.ethType.val = OMCI_FILTER_ETHER_0806;
                }
                else
                {
                    if (pExtVlanTagOper->etherType == OMCI_FILTER_ETHER_NONE)
                    {
                        pPathFlow->filter.ethType.cmpOp = e_CMP_NEQ;
                        pPathFlow->filter.ethType.val = OMCI_DONT_CARE;
                    }
                    else
                    {
                        cmsLog_error("Invalid etherType filter: <%d>",
                                     pExtVlanTagOper->etherType);

                        ret = CMSRET_INVALID_ARGUMENTS;
                        goto out;
                    }
                }
                pPathFlow->filter.ipv4.cmpOp = e_CMP_EQ;
                pPathFlow->filter.ipv4.type = e_IPV4_FILT_DSCP_1P;
                OMCI_ADD_ETH_ACTION(pPathFlow, e_ETH_ACTION_PUSH_TAG);

                OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_XLATE_PBITS_FROM_DSCP, 0, 0, 0);

                switch(pExtVlanTagOper->inner.treatment.tpid_de)
                {
                    case OMCI_TREATMENT_TPID_8100_DE_X:
                        OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_TPID, C_TAG_TPID, 0, 0);
                        /* FIXME: Can we really set DEI to 0 here ??? */
                        OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_DEI, 0, 0, 0);
                        break;

                    case OMCI_TREATMENT_TPID_OUTPUT_DE_COPY_FROM_INNER: /* allow it for un-tagged rule, iop with ALU */
                    case OMCI_TREATMENT_TPID_OUTPUT_DE_0:
                        OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_TPID,
                                             vlanTagOperOutputTpid, 0, 0);
                        OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_DEI, 0, 0, 0);
                        break;

                    case OMCI_TREATMENT_TPID_OUTPUT_DE_1:
                        OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_TPID,
                                             vlanTagOperOutputTpid, 0, 0);
                        OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_DEI, 1, 0, 0);
                        break;

                    default:
                        cmsLog_error("Must provide TPID and DEI values on tag insertion <%d>",
                                     pExtVlanTagOper->inner.treatment.tpid_de);
                        ret = CMSRET_INVALID_ARGUMENTS;
                            goto out;
                }
                OMCI_ADD_VLAN_ACTION(pPathFlow, e_VLAN_ACTION_SET_VID,
                                     pExtVlanTagOper->inner.treatment.vlanId, 0, 0);

                if(ret != CMSRET_SUCCESS)
                {
                    goto out;
                }

            }

            else if (pExtVlanTagOper->inner.treatment.prio == OMCI_TREATMENT_PRIO_DONT_ADD)
            {
                /* don't insert tag, do nothing */
            }
            else
            {
                cmsLog_error("inner.treatment.prio must be a constant: <%d>",
                             pExtVlanTagOper->inner.treatment.prio);

                ret = CMSRET_INVALID_ARGUMENTS;
                goto out;
            }
        }
    }
    else
    {
        /* insert 2 VLAN tags */
        cmsLog_error("Double-tag insertion is not supported");

        ret = CMSRET_METHOD_NOT_SUPPORTED;
        goto out;
    }

out:
    return ret;
}
#endif    // SUPPORT_DOUBLE_TAGS

static CmsRet rutGpon_parseExtendedVlanOper(UINT16 vlanTagOperInputTpid,
                                            UINT16 vlanTagOperOutputTpid,
                                            OmciExtVlanTagOper_t *pExtVlanTagOper,
                                            OmciPathFlow_t *pPathFlow,
                                            UINT8 *pIsDefaultRule)
{
    CmsRet ret = CMSRET_SUCCESS;

    pPathFlow->filter.vlan.nbrOfTags = pExtVlanTagOper->filterType;

    switch (pPathFlow->filter.vlan.nbrOfTags)
    {
        case OMCI_FILTER_TYPE_UNTAG:
            /* un-tagged frame rule */
            ret = rutGpon_processUntaggedFlow(vlanTagOperOutputTpid,
                                              pExtVlanTagOper,
                                              pPathFlow,
                                              pIsDefaultRule);
            break;

        case OMCI_FILTER_TYPE_SINGLE_TAG:
            /* single-tagged frame rule */
            /* Create default tpid filter, will be overwritten later if an explicit
               TPID filter is specified */

            ret = rutGpon_processSingleTaggedFlow(vlanTagOperInputTpid,
                                                  vlanTagOperOutputTpid,
                                                  pExtVlanTagOper,
                                                  pPathFlow,
                                                  pIsDefaultRule);
            break;

        case OMCI_FILTER_TYPE_DOUBLE_TAG:
            /* double-tagged frame rule */
#ifdef SUPPORT_DOUBLE_TAGS
            pPathFlow->filter.vlan.nbrOfTags = OMCI_FILTER_TYPE_DOUBLE_TAG;

            ret = rutGpon_processDoubleTaggedFlow(vlanTagOperInputTpid,
                                                  vlanTagOperOutputTpid,
                                                  pExtVlanTagOper,
                                                  pPathFlow,
                                                  pIsDefaultRule);
#else    // SUPPORT_DOUBLE_TAGS
            cmsLog_error("Double-tagged rules are not supported");
            ret = CMSRET_METHOD_NOT_SUPPORTED;
#endif    // SUPPORT_DOUBLE_TAGS
            break;
        default:
            cmsLog_error("Double-tagged rules are not supported");
            ret = CMSRET_METHOD_NOT_SUPPORTED;
            break;
    }

    return ret;
}

#ifdef OMCI_DEBUG_SUPPORT
static void rutGpon_printPathVlanFilter(OmciFlowFilterVlanVal_t *pVlanTag)
{
    if(OMCI_IS_DONT_CARE(pVlanTag->tpid))
    {
        omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "tpid <X>, ");
    }
    else
    {
        omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "tpid <0x%04X>, ", pVlanTag->tpid);
    }

    if(OMCI_IS_DONT_CARE(pVlanTag->pbits))
    {
        omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "pbits<X>, ");
    }
    else
    {
        omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "pbits<%d>, ", pVlanTag->pbits);
    }

    if(OMCI_IS_DONT_CARE(pVlanTag->dei))
    {
        omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "dei<X>, ");
    }
    else
    {
        omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "dei<%d>, ", pVlanTag->dei);
    }

    if(OMCI_IS_DONT_CARE(pVlanTag->vid))
    {
        omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "vid<X>\n");
    }
    else
    {
        omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "vid<%d>\n", pVlanTag->vid);
    }
}

static char *omciEthActionName[] = OMCI_ETH_ACTION_NAME();
static char *omciVlanActionName[] = OMCI_VLAN_ACTION_NAME();


static void rutGpon_printPathFlow(OmciPathFlow_t *pPathFlow)
{
    OmciVlanAction_t *vlanAction;
    int i;

    omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "******************** Path Flow ********************\n");

    if(OMCI_IS_DONT_CARE(pPathFlow->filter.vlan.nbrOfTags))
    {
        omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "VLAN Tag Filters  : <X>\n");
    }
    else
    {
        omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "VLAN Tag Filters  : <%d>\n", pPathFlow->filter.vlan.nbrOfTags);
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "Outer VLAN Filter : ");
    rutGpon_printPathVlanFilter(&pPathFlow->filter.vlan.outer);

    omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "Inner VLAN Filter : ");
    rutGpon_printPathVlanFilter(&pPathFlow->filter.vlan.inner);

    if(OMCI_IS_DONT_CARE(pPathFlow->filter.ethType.val))
    {
        omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "EtherType Filter  : <X>\n");
    }
    else
    {
        omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "EtherType Filter  : val <0x%04X> cmpOp <%d>\n",
                       pPathFlow->filter.ethType.val, pPathFlow->filter.ethType.cmpOp);
    }

    for(i=0; i<pPathFlow->action.ethActionIx; ++i)
    {
        omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "ETH Action[%d]  : type<%d><%s>\n",
                       i, pPathFlow->action.ethAction[i],
                       omciEthActionName[pPathFlow->action.ethAction[i]]);
    }

    for(i=0; i<pPathFlow->action.vlanActionIx; ++i)
    {
        vlanAction = &pPathFlow->action.vlanAction[i];

        omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "VLAN Action[%d] : type<%d><%s>, val<%d>/<0x%04X>, toTag<%d>, fromTag<%d>\n",
                       i, vlanAction->type, omciVlanActionName[vlanAction->type],
                       vlanAction->val, vlanAction->val,
                       vlanAction->toTag, vlanAction->fromTag);
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "***************************************************\n");
}

static void rutGpon_printVlanTagOper(OmciExtVlanTagOper_t *pExtVlanTagOper)
{
    omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "Extended VLAN Tag Operation\n");
    omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "Filters\n");
    omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "\t+ Outer: PRIO <%d>, VID <%d>, TPID_DE <%d>\n", pExtVlanTagOper->outer.filter.prio,
                   pExtVlanTagOper->outer.filter.vlanId, pExtVlanTagOper->outer.filter.tpid_de);
    omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "\t+ Inner: PRIO <%d>, VID <%d>, TPID_DE <%d>\n", pExtVlanTagOper->inner.filter.prio,
                   pExtVlanTagOper->inner.filter.vlanId, pExtVlanTagOper->inner.filter.tpid_de);
    omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "\t+ EtherType <%d>\n", pExtVlanTagOper->etherType);
    omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "Treatments\n");
    omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "\t+ Tags to Remove <%d>\n", pExtVlanTagOper->removeType);
    omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "\t+ Outer: PRIO <%d>, VID <%d>, TPID_DE <%d>\n", pExtVlanTagOper->outer.treatment.prio,
                   pExtVlanTagOper->outer.treatment.vlanId, pExtVlanTagOper->outer.treatment.tpid_de);
    omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "\t+ Inner: PRIO <%d>, VID <%d>, TPID_DE <%d>\n", pExtVlanTagOper->inner.treatment.prio,
                   pExtVlanTagOper->inner.treatment.vlanId, pExtVlanTagOper->inner.treatment.tpid_de);
}
#endif /* OMCI_DEBUG_SUPPORT */

#ifndef SUPPORT_DOUBLE_TAGS
static int rutGpon_searchEthAction(OmciPathFlow_t *pPathFlow, BpaEthActionType_t type)
{
    int i;

    for(i=0; i<pPathFlow->action.ethActionIx; ++i)
    {
        if(pPathFlow->action.ethAction[i] == type)
        {
#if 0
            omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "Found ETH Action: type<%d><%s>\n", pPathFlow->action.ethAction[i],
                           omciEthActionName[pPathFlow->action.ethAction[i]]);
#endif
            return 1;
        }
    }

    return 0;
}
#endif    // SUPPORT_DOUBLE_TAGS

static OmciVlanAction_t *rutGpon_searchVlanAction(OmciPathFlow_t *pPathFlow,
                                                  BpaVlanActionType_t type,
                                                  UINT8 toTag,
                                                  UINT8 fromTag)
{
    OmciVlanAction_t *vlanAction;
    int i;

    for(i=0; i<pPathFlow->action.vlanActionIx; ++i)
    {
        vlanAction = &pPathFlow->action.vlanAction[i];

        if(vlanAction->type == type)
        {
            if((OMCI_IS_DONT_CARE(toTag) || vlanAction->toTag == toTag) &&
               (OMCI_IS_DONT_CARE(fromTag) || vlanAction->fromTag == fromTag))
            {
#if 0
                omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "Found VLAN Action: type<%d><%s>, val<%d>/<0x%04X>, toTag<%d>, fromTag<%d>\n",
                               vlanAction->type, omciVlanActionName[vlanAction->type],
                               vlanAction->val, vlanAction->val,
                               vlanAction->toTag, vlanAction->fromTag);
#endif
                return vlanAction;
            }
        }
    }

    return NULL;
}

#ifdef SUPPORT_DOUBLE_TAGS
static UINT32 rutGpon_getNumberOfPushTag(OmciPathFlow_t *pPathFlow)
{
    UINT32 i = 0, num = 0;

    for (i=0; i < pPathFlow->action.ethActionIx; i++)
    {
        if (pPathFlow->action.ethAction[i] == e_ETH_ACTION_PUSH_TAG)
        {
            num++;
        }
    }

    return num;
}
#endif    // SUPPORT_DOUBLE_TAGS

static CmsRet rutGpon_createPbitsFilter(OmciPathFlow_t *pPathFlow, UINT8 pbits)
{
    CmsRet ret = CMSRET_SUCCESS;
    OmciVlanAction_t *vlanAction;

    /* FIXME: Dual-tag filtering NOT supported */

    /* The e_VLAN_ACTION_COPY_PBITS_FROM_TAG action can only be created when the pbits value
       of a single tagged frames is copied into the pbits on a newly inserted outer tag.
       Since the pbits filter will exist for the original tag, there is nothing else needed. */

    /* Search for existing action. We only care if the action applies to
       the outer tag, which is seen by the Mapper and the VLAN Filter objects */
    if((vlanAction = rutGpon_searchVlanAction(pPathFlow,
                                              e_VLAN_ACTION_SET_PBITS,
                                              e_VLAN_TAG0, OMCI_DONT_CARE)) != NULL)
    {
        /* SWBCACPE-9532
           If action in Extende Vlan ME is e_VLAN_ACTION_SET_PBITS but inner priority is
           DON"T CARE then do not match pbits with 802.1p mapper ME. */
        if(!OMCI_IS_DONT_CARE(pPathFlow->filter.vlan.inner.pbits) && vlanAction->val != pbits)
        {
            omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "MISMATCH: pbits <%d>, e_VLAN_ACTION_SET_PBITS <%d>\n",
                           pbits, vlanAction->val);

            ret = CMSRET_REQUEST_DENIED;
            goto out;
        }
    }
    else if((vlanAction = rutGpon_searchVlanAction(pPathFlow,
                                              e_VLAN_ACTION_XLATE_PBITS_FROM_DSCP,
                                              e_VLAN_TAG0, OMCI_DONT_CARE)) != NULL)
    {
        pPathFlow->filter.ipv4.val = pbits;
    }
    else
    {
        /* There are NO actions that change the PBITS in the flow. We now need to check if
           there is a pbits filter that matches the path PBITS */

        if(!OMCI_IS_DONT_CARE(pPathFlow->filter.vlan.inner.pbits))
        {
            if(pPathFlow->filter.vlan.inner.pbits != pbits)
            {
                omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "MISMATCH: pbits <%d>, filter.vlan.inner.pbits <%d>\n",
                               pbits, pPathFlow->filter.vlan.inner.pbits);

                ret = CMSRET_REQUEST_DENIED;
                goto out;
            }
        }
        else
        {
            /* The flow does not have a PBITS filter, let's create it */

            if(OMCI_IS_DONT_CARE(pPathFlow->filter.vlan.nbrOfTags) ||
               pPathFlow->filter.vlan.nbrOfTags == OMCI_FILTER_TYPE_UNTAG)
                pPathFlow->filter.vlan.nbrOfTags = OMCI_FILTER_TYPE_SINGLE_TAG;

#ifdef SUPPORT_DOUBLE_TAGS
            if (pPathFlow->filter.vlan.nbrOfTags == OMCI_FILTER_TYPE_DOUBLE_TAG)
                pPathFlow->filter.vlan.outer.pbits = pbits;
            else
                pPathFlow->filter.vlan.inner.pbits = pbits;
#else    // SUPPORT_DOUBLE_TAGS
            pPathFlow->filter.vlan.inner.pbits = pbits;
#endif    // SUPPORT_DOUBLE_TAGS
        }
    }

out:
    return ret;
}

static CmsRet rutGpon_createVidFilter(OmciPathFlow_t *pPathFlow, UINT16 vid)
{
    CmsRet ret = CMSRET_SUCCESS;
    OmciVlanAction_t *vlanAction;

    /* FIXME: Dual-tag filtering NOT supported */

    /* The e_VLAN_ACTION_COPY_VID_FROM_TAG action can only be created when the vid value
       of a single tagged frames is copied into the vid on a newly inserted outer tag.
       Since the vid filter will exist for the original tag, there is nothing else needed. */

    /* Search for existing action. We only care if the action applies to
       the outer tag, which is seen by the Mapper and the VLAN Filter objects */
    if((vlanAction = rutGpon_searchVlanAction(pPathFlow,
                                              e_VLAN_ACTION_SET_VID,
                                              0, OMCI_DONT_CARE)) != NULL)
    {
        /* SWBCACPE-9532
           If action in Extende Vlan ME is e_VLAN_ACTION_SET_VID but inner vlanId is
           DON"T CARE then do not match vid with Vlan Filter ME. */
        if(!OMCI_IS_DONT_CARE(pPathFlow->filter.vlan.inner.vid) && vlanAction->val != vid)
        {
            omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "MISMATCH: vid <%d>, e_VLAN_ACTION_SET_VID <%d>\n",
                           vid, vlanAction->val);

            ret = CMSRET_REQUEST_DENIED;
            goto out;
        }
    }
    else
    {
        /* There are NO actions that change the VID in the flow. We now need to check if
           there is a vid filter that matches the path VID */

        if(!OMCI_IS_DONT_CARE(pPathFlow->filter.vlan.inner.vid))
        {
            if(pPathFlow->filter.vlan.inner.vid != vid)
            {
                omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "MISMATCH: vid <%d>, filter.vlan.inner.vid <%d>\n",
                               vid, pPathFlow->filter.vlan.inner.vid);

                ret = CMSRET_REQUEST_DENIED;
                goto out;
            }
        }
        else
        {
            /* The flow does not have a VID filter, let's create it */

            if(OMCI_IS_DONT_CARE(pPathFlow->filter.vlan.nbrOfTags) ||
               pPathFlow->filter.vlan.nbrOfTags == OMCI_FILTER_TYPE_UNTAG)
            {
                pPathFlow->filter.vlan.nbrOfTags = OMCI_FILTER_TYPE_SINGLE_TAG;
            }

#ifdef SUPPORT_DOUBLE_TAGS
            if (pPathFlow->filter.vlan.nbrOfTags == OMCI_FILTER_TYPE_DOUBLE_TAG)
                pPathFlow->filter.vlan.outer.vid = vid;
            else
                pPathFlow->filter.vlan.inner.vid = vid;
#else    // SUPPORT_DOUBLE_TAGS
            pPathFlow->filter.vlan.inner.vid = vid;
#endif    // SUPPORT_DOUBLE_TAGS
        }
    }

out:
    return ret;
}

static CmsRet rutGpon_createDeiFilter(OmciPathFlow_t *pPathFlow, UINT8 dei)
{
    CmsRet ret = CMSRET_SUCCESS;
    OmciVlanAction_t *vlanAction;

    /* FIXME: Dual-tag filtering NOT supported */

    /* The e_VLAN_ACTION_COPY_DEI_FROM_TAG action can only be created when the dei value
       of a single tagged frames is copied into the dei on a newly inserted outer tag.
       Since the dei filter will exist for the original tag, there is nothing else needed. */

    /* Search for existing action. We only care if the action applies to
       the outer tag, which is seen by the Mapper and the VLAN Filter objects */
    if((vlanAction = rutGpon_searchVlanAction(pPathFlow,
                                              e_VLAN_ACTION_SET_DEI,
                                              0, OMCI_DONT_CARE)) != NULL)
    {
        if(vlanAction->val != dei)
        {
            omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "MISMATCH: dei <%d>, e_VLAN_ACTION_SET_DEI <%d>\n",
                           dei, vlanAction->val);

            ret = CMSRET_REQUEST_DENIED;
            goto out;
        }
    }
    else
    {
        /* There are NO actions that change the DEI in the flow. We now need to check if
           there is a dei filter that matches the path DEI */

        if(!OMCI_IS_DONT_CARE(pPathFlow->filter.vlan.inner.dei))
        {
            if(pPathFlow->filter.vlan.inner.dei != dei)
            {
                omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "MISMATCH: dei <%d>, filter.vlan.inner.dei <%d>\n",
                               dei, pPathFlow->filter.vlan.inner.dei);

                ret = CMSRET_REQUEST_DENIED;
                goto out;
            }
        }
        else
        {
            /* The flow does not have a DEI filter, let's create it */

            if(OMCI_IS_DONT_CARE(pPathFlow->filter.vlan.nbrOfTags) ||
               pPathFlow->filter.vlan.nbrOfTags == OMCI_FILTER_TYPE_UNTAG)
            {
                pPathFlow->filter.vlan.nbrOfTags = OMCI_FILTER_TYPE_SINGLE_TAG;
            }

#ifdef SUPPORT_DOUBLE_TAGS
            if (pPathFlow->filter.vlan.nbrOfTags == OMCI_FILTER_TYPE_DOUBLE_TAG)
                pPathFlow->filter.vlan.outer.dei = dei;
            else
                pPathFlow->filter.vlan.inner.dei = dei;
#else    // SUPPORT_DOUBLE_TAGS
            pPathFlow->filter.vlan.inner.dei = dei;
#endif    // SUPPORT_DOUBLE_TAGS
        }
    }

out:
    return ret;
}

#ifdef SUPPORT_DOUBLE_TAGS
static CmsRet rutGpon_createDsFlow(OmciExtVlanTagOper_t *pExtVlanTagOper,
                                   OmciPathFlow_t *pPathFlowUs,
                                   OmciPathFlow_t *pPathFlowDs,
                                   UINT8 vlanTagOperDsMode,
                                   UINT8 isDefaultRule)
{
    CmsRet ret = CMSRET_SUCCESS;
    OmciVlanAction_t *vlanAction = NULL;
    OmciFlowFilterVlanVal_t *pInnerFilterVlanVal = NULL, *pOuterFilterVlanVal = NULL;
    UINT32 nbrPushTags = rutGpon_getNumberOfPushTag(pPathFlowUs);

    if(vlanTagOperDsMode == 1)
    {
        if(isDefaultRule)
        {
            pPathFlowDs->filter.vlan.nbrOfTags = OMCI_FILTER_TYPE_UNTAG;
        }

        goto out;
    }
    else if(vlanTagOperDsMode != 0)
    {
        cmsLog_error("Invalid Extended VLAN Tagging Operation Downstream Mode: <%d>",
                     vlanTagOperDsMode);

        ret = CMSRET_INVALID_ARGUMENTS;
    }

    /*
     * Filters
     */

    pPathFlowDs->filter.vlan.nbrOfTags = (pPathFlowUs->filter.vlan.nbrOfTags + nbrPushTags);

    /* turn actions into filters */

    /* double tagged frame has outer and inner:
            * outer uses e_VLAN_TAG0
            * inner uses e_VLAN_TAG1
        single tagged frame has inner only and uses e_VLAN_TAG0 */

    if(pPathFlowDs->filter.vlan.nbrOfTags == OMCI_FILTER_TYPE_DOUBLE_TAG)
    {
        pOuterFilterVlanVal = &pPathFlowDs->filter.vlan.outer;
        pInnerFilterVlanVal = &pPathFlowDs->filter.vlan.inner;
    }
    else if(pPathFlowDs->filter.vlan.nbrOfTags == OMCI_FILTER_TYPE_SINGLE_TAG)
    {
        pOuterFilterVlanVal = &pPathFlowDs->filter.vlan.inner;
    }
    else if(pPathFlowDs->filter.vlan.nbrOfTags != OMCI_FILTER_TYPE_UNTAG)
    {
        cmsLog_error("DS Flow has <%d> VLAN Tag filters!",
                     pPathFlowDs->filter.vlan.nbrOfTags);

        ret = CMSRET_INTERNAL_ERROR;
        goto out;
    }

    if(pPathFlowDs->filter.vlan.nbrOfTags == OMCI_FILTER_TYPE_DOUBLE_TAG)
    {
        /* tpid */
        if((vlanAction = rutGpon_searchVlanAction(pPathFlowUs,
                                                  e_VLAN_ACTION_SET_TPID,
                                                  e_VLAN_TAG1, OMCI_DONT_CARE)) != NULL)
        {
            pInnerFilterVlanVal->tpid = vlanAction->val;
        }
        else if ((vlanAction = rutGpon_searchVlanAction(pPathFlowUs,
                                                  e_VLAN_ACTION_COPY_TPID_FROM_TAG,
                                                  e_VLAN_TAG1, e_VLAN_TAG0)) != NULL)
        {
            pInnerFilterVlanVal->tpid = vlanAction->val;
        }
        else
        {
            // Default value should be OMCI_DONT_CARE to accept any TPID value
            pInnerFilterVlanVal->tpid = OMCI_DONT_CARE;
        }

        /* pbits */
        if ((vlanAction = rutGpon_searchVlanAction(pPathFlowUs,
                                                  e_VLAN_ACTION_SET_PBITS,
                                                  e_VLAN_TAG1, OMCI_DONT_CARE)) != NULL)
        {
            pInnerFilterVlanVal->pbits = vlanAction->val;
        }
        else if ((vlanAction = rutGpon_searchVlanAction(pPathFlowUs,
                                                  e_VLAN_ACTION_COPY_PBITS_FROM_TAG,
                                                  e_VLAN_TAG1, e_VLAN_TAG0)) != NULL)
        {
            pInnerFilterVlanVal->pbits = vlanAction->val;
        }
        else if ((vlanAction = rutGpon_searchVlanAction(pPathFlowUs,
                                                  e_VLAN_ACTION_XLATE_PBITS_FROM_DSCP,
                                                  e_VLAN_TAG1, OMCI_DONT_CARE)) != NULL)
        {
            //pInnerFilterVlanVal->ipv4.cmpOp = e_CMP_EQ;
            //pInnerFilterVlanVal->ipv4.type = e_IPV4_FILT_DSCP_1P;
            //pInnerFilterVlanVal->pbits = vlanAction->val;
        }

        /* dei */
        if ((vlanAction = rutGpon_searchVlanAction(pPathFlowUs,
                                                  e_VLAN_ACTION_SET_DEI,
                                                  e_VLAN_TAG1, OMCI_DONT_CARE)) != NULL)
        {
            pInnerFilterVlanVal->dei = vlanAction->val;
        }
        else if ((vlanAction = rutGpon_searchVlanAction(pPathFlowUs,
                                                  e_VLAN_ACTION_COPY_DEI_FROM_TAG,
                                                  e_VLAN_TAG1, e_VLAN_TAG0)) != NULL)
        {
            pInnerFilterVlanVal->dei = vlanAction->val;
        }

        /* vlan id */
        if ((vlanAction = rutGpon_searchVlanAction(pPathFlowUs,
                                                  e_VLAN_ACTION_SET_VID,
                                                  e_VLAN_TAG1, OMCI_DONT_CARE)) != NULL)
        {
            pInnerFilterVlanVal->vid = vlanAction->val;
        }
        else if ((vlanAction = rutGpon_searchVlanAction(pPathFlowUs,
                                                  e_VLAN_ACTION_COPY_VID_FROM_TAG,
                                                  e_VLAN_TAG1, e_VLAN_TAG1)) != NULL)
        {
            pInnerFilterVlanVal->vid = vlanAction->val;
        }
    }

    if (pPathFlowDs->filter.vlan.nbrOfTags == OMCI_FILTER_TYPE_DOUBLE_TAG ||
        pPathFlowDs->filter.vlan.nbrOfTags == OMCI_FILTER_TYPE_SINGLE_TAG)
    {
        /* tpid */
        if((vlanAction = rutGpon_searchVlanAction(pPathFlowUs,
                                                  e_VLAN_ACTION_SET_TPID,
                                                  e_VLAN_TAG0, OMCI_DONT_CARE)) != NULL)
        {
            pOuterFilterVlanVal->tpid = vlanAction->val;
        }
        else if ((vlanAction = rutGpon_searchVlanAction(pPathFlowUs,
                                                  e_VLAN_ACTION_COPY_TPID_FROM_TAG,
                                                  e_VLAN_TAG0, e_VLAN_TAG1)) != NULL)
        {
            pOuterFilterVlanVal->tpid = vlanAction->val;
        }
        else
        {
            // Default value should be OMCI_DONT_CARE to accept any TPID value
            pOuterFilterVlanVal->tpid = OMCI_DONT_CARE;
        }

        /* pbits */
        if ((vlanAction = rutGpon_searchVlanAction(pPathFlowUs,
                                                  e_VLAN_ACTION_SET_PBITS,
                                                  e_VLAN_TAG0, OMCI_DONT_CARE)) != NULL)
        {
            pOuterFilterVlanVal->pbits = vlanAction->val;
        }
        else if ((vlanAction = rutGpon_searchVlanAction(pPathFlowUs,
                                                  e_VLAN_ACTION_COPY_PBITS_FROM_TAG,
                                                  e_VLAN_TAG0, e_VLAN_TAG1)) != NULL)
        {
            pOuterFilterVlanVal->pbits = vlanAction->val;
        }
        else if ((vlanAction = rutGpon_searchVlanAction(pPathFlowUs,
                                                  e_VLAN_ACTION_XLATE_PBITS_FROM_DSCP,
                                                  e_VLAN_TAG0, OMCI_DONT_CARE)) != NULL)
        {
            //pOuterFilterVlanVal->ipv4.cmpOp = e_CMP_EQ;
            //pOuterFilterVlanVal->ipv4.type = e_IPV4_FILT_DSCP_1P;
            //pOuterFilterVlanVal->pbits = vlanAction->val;
        }

        /* dei */
        if ((vlanAction = rutGpon_searchVlanAction(pPathFlowUs,
                                                  e_VLAN_ACTION_SET_DEI,
                                                  e_VLAN_TAG0, OMCI_DONT_CARE)) != NULL)
        {
            pOuterFilterVlanVal->dei = vlanAction->val;
        }
        else if ((vlanAction = rutGpon_searchVlanAction(pPathFlowUs,
                                                  e_VLAN_ACTION_COPY_DEI_FROM_TAG,
                                                  e_VLAN_TAG0, e_VLAN_TAG1)) != NULL)
        {
            pOuterFilterVlanVal->dei = vlanAction->val;
        }

        /* vlan id */
        if ((vlanAction = rutGpon_searchVlanAction(pPathFlowUs,
                                                  e_VLAN_ACTION_SET_VID,
                                                  e_VLAN_TAG0, OMCI_DONT_CARE)) != NULL)
        {
            pOuterFilterVlanVal->vid = vlanAction->val;
        }
        else if ((vlanAction = rutGpon_searchVlanAction(pPathFlowUs,
                                                  e_VLAN_ACTION_COPY_VID_FROM_TAG,
                                                  e_VLAN_TAG0, e_VLAN_TAG1)) != NULL)
        {
            pOuterFilterVlanVal->vid = vlanAction->val;
        }
    }

    /* copy ethertype filter */
    if (pPathFlowUs->filter.ethType.cmpOp != e_CMP_NEQ)
    {
        /* copy ethertype filter */
        pPathFlowDs->filter.ethType = pPathFlowUs->filter.ethType;
    }

    /*
     * Actions
     */

    if (nbrPushTags)
    {
        UINT32 i = 0;
        for (i = 0; i < nbrPushTags; i++)
        {
            OMCI_ADD_ETH_ACTION(pPathFlowDs, e_ETH_ACTION_POP_TAG);
        }

	if (pPathFlowDs->filter.vlan.nbrOfTags == OMCI_FILTER_TYPE_DOUBLE_TAG &&
            pPathFlowUs->filter.vlan.nbrOfTags == OMCI_FILTER_TYPE_SINGLE_TAG)
        {
            /* Tags pop: Outer Tag Remove, Inner Tag Replacement */
            /* turn filters into actions */

            if (!OMCI_IS_DONT_CARE(pPathFlowUs->filter.vlan.inner.tpid))
            {
                OMCI_ADD_VLAN_ACTION(pPathFlowDs, e_VLAN_ACTION_SET_TPID,
                                     pPathFlowUs->filter.vlan.inner.tpid, e_VLAN_TAG1, OMCI_DONT_CARE);
            }
            if (!OMCI_IS_DONT_CARE(pPathFlowUs->filter.vlan.inner.pbits))
            {
                OMCI_ADD_VLAN_ACTION(pPathFlowDs, e_VLAN_ACTION_SET_PBITS,
                            	     pPathFlowUs->filter.vlan.inner.pbits, e_VLAN_TAG1, OMCI_DONT_CARE);
            }
            if (!OMCI_IS_DONT_CARE(pPathFlowUs->filter.vlan.inner.dei))
            {
                OMCI_ADD_VLAN_ACTION(pPathFlowDs, e_VLAN_ACTION_SET_DEI,
                                     pPathFlowUs->filter.vlan.inner.dei, e_VLAN_TAG1, OMCI_DONT_CARE);
            }
            if (!OMCI_IS_DONT_CARE(pPathFlowUs->filter.vlan.inner.vid))
            {
                OMCI_ADD_VLAN_ACTION(pPathFlowDs, e_VLAN_ACTION_SET_VID,
                                     pPathFlowUs->filter.vlan.inner.vid, e_VLAN_TAG1, OMCI_DONT_CARE);
            }
        }
    }
    else
    {
        /* double tagged frame has outer and inner:
                * outer uses e_VLAN_TAG0
                * inner uses e_VLAN_TAG1
            single tagged frame has inner only and uses e_VLAN_TAG0 */

        if (pPathFlowDs->filter.vlan.nbrOfTags == OMCI_FILTER_TYPE_DOUBLE_TAG &&
            pPathFlowUs->filter.vlan.nbrOfTags == OMCI_FILTER_TYPE_DOUBLE_TAG)
        {
            /* No tags pushed: Outer Tag Replacement */
            /* turn filters into actions */

            if (pExtVlanTagOper->outer.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD)
            {
                // there is outer action upstream ==> turn outer filters into actions for outer tag tag0
                if (!OMCI_IS_DONT_CARE(pPathFlowUs->filter.vlan.outer.tpid))
                {
                    OMCI_ADD_VLAN_ACTION(pPathFlowDs, e_VLAN_ACTION_SET_TPID,
                                         pPathFlowUs->filter.vlan.outer.tpid, e_VLAN_TAG0, OMCI_DONT_CARE);
                }
                if (!OMCI_IS_DONT_CARE(pPathFlowUs->filter.vlan.outer.pbits))
                {
                    OMCI_ADD_VLAN_ACTION(pPathFlowDs, e_VLAN_ACTION_SET_PBITS,
                                         pPathFlowUs->filter.vlan.outer.pbits, e_VLAN_TAG0, OMCI_DONT_CARE);
                }
                if (!OMCI_IS_DONT_CARE(pPathFlowUs->filter.vlan.outer.dei))
                {
                    OMCI_ADD_VLAN_ACTION(pPathFlowDs, e_VLAN_ACTION_SET_DEI,
                                         pPathFlowUs->filter.vlan.outer.dei, e_VLAN_TAG0, OMCI_DONT_CARE);
                }
                if (!OMCI_IS_DONT_CARE(pPathFlowUs->filter.vlan.outer.vid))
                {
                    OMCI_ADD_VLAN_ACTION(pPathFlowDs, e_VLAN_ACTION_SET_VID,
                                         pPathFlowUs->filter.vlan.outer.vid, e_VLAN_TAG0, OMCI_DONT_CARE);
                }

                if (pExtVlanTagOper->inner.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD)
                {
                    // there are both outer and inner actions upstream
                    // ==> turn inner filters into actions for inner tag tag1
                    if (!OMCI_IS_DONT_CARE(pPathFlowUs->filter.vlan.inner.tpid))
                    {
                        OMCI_ADD_VLAN_ACTION(pPathFlowDs, e_VLAN_ACTION_SET_TPID,
                                             pPathFlowUs->filter.vlan.inner.tpid, e_VLAN_TAG1, OMCI_DONT_CARE);
                    }
                    if (!OMCI_IS_DONT_CARE(pPathFlowUs->filter.vlan.inner.pbits))
                    {
                        OMCI_ADD_VLAN_ACTION(pPathFlowDs, e_VLAN_ACTION_SET_PBITS,
                                    	     pPathFlowUs->filter.vlan.inner.pbits, e_VLAN_TAG1, OMCI_DONT_CARE);
                    }
                    if (!OMCI_IS_DONT_CARE(pPathFlowUs->filter.vlan.inner.dei))
                    {
                        OMCI_ADD_VLAN_ACTION(pPathFlowDs, e_VLAN_ACTION_SET_DEI,
                                             pPathFlowUs->filter.vlan.inner.dei, e_VLAN_TAG1, OMCI_DONT_CARE);
                    }
                    if (!OMCI_IS_DONT_CARE(pPathFlowUs->filter.vlan.inner.vid))
                    {
                        OMCI_ADD_VLAN_ACTION(pPathFlowDs, e_VLAN_ACTION_SET_VID,
                                             pPathFlowUs->filter.vlan.inner.vid, e_VLAN_TAG1, OMCI_DONT_CARE);
                    }
                }
            }
            else
            {
                if (pExtVlanTagOper->inner.treatment.prio != OMCI_TREATMENT_PRIO_DONT_ADD)
                {
                    // there is inner action upstream without outer action uptream
                    // G.988 implementations: if upstream has inner without outer treatment then
                    //      inner treatment is applied for outer tag. 
                    //      ==> outstream: turn OUTER filters into actions for outer tag tag0.
                    if (!OMCI_IS_DONT_CARE(pPathFlowUs->filter.vlan.outer.tpid))
                    {
                        OMCI_ADD_VLAN_ACTION(pPathFlowDs, e_VLAN_ACTION_SET_TPID,
                                             pPathFlowUs->filter.vlan.outer.tpid, e_VLAN_TAG0, OMCI_DONT_CARE);
                    }
                    if (!OMCI_IS_DONT_CARE(pPathFlowUs->filter.vlan.outer.pbits))
                    {
                        OMCI_ADD_VLAN_ACTION(pPathFlowDs, e_VLAN_ACTION_SET_PBITS,
                                    	     pPathFlowUs->filter.vlan.outer.pbits, e_VLAN_TAG0, OMCI_DONT_CARE);
                    }
                    if (!OMCI_IS_DONT_CARE(pPathFlowUs->filter.vlan.outer.dei))
                    {
                        OMCI_ADD_VLAN_ACTION(pPathFlowDs, e_VLAN_ACTION_SET_DEI,
                                             pPathFlowUs->filter.vlan.outer.dei, e_VLAN_TAG0, OMCI_DONT_CARE);
                    }
                    if (!OMCI_IS_DONT_CARE(pPathFlowUs->filter.vlan.outer.vid))
                    {
                        OMCI_ADD_VLAN_ACTION(pPathFlowDs, e_VLAN_ACTION_SET_VID,
                                             pPathFlowUs->filter.vlan.outer.vid, e_VLAN_TAG0, OMCI_DONT_CARE);
                    }
                }
            }
        }
	else if (pPathFlowDs->filter.vlan.nbrOfTags == OMCI_FILTER_TYPE_SINGLE_TAG &&
                 pPathFlowUs->filter.vlan.nbrOfTags == OMCI_FILTER_TYPE_SINGLE_TAG)
        {
            /* No tags pushed: Inner Tag Replacement */
            /* turn filters into actions */

            if (!OMCI_IS_DONT_CARE(pPathFlowUs->filter.vlan.inner.tpid))
            {
                OMCI_ADD_VLAN_ACTION(pPathFlowDs, e_VLAN_ACTION_SET_TPID,
                                     pPathFlowUs->filter.vlan.inner.tpid, e_VLAN_TAG0, OMCI_DONT_CARE);
            }
            if (!OMCI_IS_DONT_CARE(pPathFlowUs->filter.vlan.inner.pbits))
            {
                OMCI_ADD_VLAN_ACTION(pPathFlowDs, e_VLAN_ACTION_SET_PBITS,
                            	     pPathFlowUs->filter.vlan.inner.pbits, e_VLAN_TAG0, OMCI_DONT_CARE);
            }
            if (!OMCI_IS_DONT_CARE(pPathFlowUs->filter.vlan.inner.dei))
            {
                OMCI_ADD_VLAN_ACTION(pPathFlowDs, e_VLAN_ACTION_SET_DEI,
                                     pPathFlowUs->filter.vlan.inner.dei, e_VLAN_TAG0, OMCI_DONT_CARE);
            }
            if (!OMCI_IS_DONT_CARE(pPathFlowUs->filter.vlan.inner.vid))
            {
                OMCI_ADD_VLAN_ACTION(pPathFlowDs, e_VLAN_ACTION_SET_VID,
                                     pPathFlowUs->filter.vlan.inner.vid, e_VLAN_TAG0, OMCI_DONT_CARE);
            }
        }
    }

out:
    return ret;
}
#else    // SUPPORT_DOUBLE_TAGS
static CmsRet rutGpon_createDsFlow(OmciExtVlanTagOper_t *pExtVlanTagOper,
                                   OmciPathFlow_t *pPathFlowUs,
                                   OmciPathFlow_t *pPathFlowDs,
                                   UINT8 vlanTagOperDsMode,
                                   UINT8 isDefaultRule)
{
    CmsRet ret = CMSRET_SUCCESS;
    OmciVlanAction_t *vlanAction = NULL;
    OmciFlowFilterVlanVal_t *pFilterVlanVal = NULL;
    UINT32 hasTagInsertion = rutGpon_searchEthAction(pPathFlowUs, e_ETH_ACTION_PUSH_TAG);

    if(vlanTagOperDsMode == 1)
    {
        if(isDefaultRule)
        {
            pPathFlowDs->filter.vlan.nbrOfTags = OMCI_FILTER_TYPE_UNTAG;
        }

        goto out;
    }
    else if(vlanTagOperDsMode != 0)
    {
        cmsLog_error("Invalid Extended VLAN Tagging Operation Downstream Mode: <%d>",
                     vlanTagOperDsMode);

        ret = CMSRET_INVALID_ARGUMENTS;
    }

    /*
     * Filters
     */

    pPathFlowDs->filter.vlan.nbrOfTags = (pPathFlowUs->filter.vlan.nbrOfTags +
                                          ((hasTagInsertion) ? 1 : 0));

    /* turn actions into filters */

    if(pPathFlowDs->filter.vlan.nbrOfTags == 2)
    {
        pFilterVlanVal = &pPathFlowDs->filter.vlan.outer;
    }
    else if(pPathFlowDs->filter.vlan.nbrOfTags == 1)
    {
        pFilterVlanVal = &pPathFlowDs->filter.vlan.inner;
    }
    else if(pPathFlowDs->filter.vlan.nbrOfTags != 0)
    {
        cmsLog_error("DS Flow has <%d> VLAN Tag filters!",
                     pPathFlowDs->filter.vlan.nbrOfTags);

        ret = CMSRET_INTERNAL_ERROR;
        goto out;
    }

    if((vlanAction = rutGpon_searchVlanAction(pPathFlowUs,
                                              e_VLAN_ACTION_SET_TPID,
                                              0, OMCI_DONT_CARE)) != NULL)
    {
        pFilterVlanVal->tpid = vlanAction->val;
    }
    else
    {
        if(pFilterVlanVal != NULL)
        {
            /* This is a single-tagged frame, but there are no actions to set the TPID.
               Let's add the default TPID as the filter */
            pFilterVlanVal->tpid = OMCI_DONT_CARE;
        }
    }

    if((vlanAction = rutGpon_searchVlanAction(pPathFlowUs,
                                              e_VLAN_ACTION_SET_PBITS,
                                              0, OMCI_DONT_CARE)) != NULL)
    {
        pFilterVlanVal->pbits = vlanAction->val;
    }

    if((vlanAction = rutGpon_searchVlanAction(pPathFlowUs,
                                              e_VLAN_ACTION_SET_DEI,
                                              0, OMCI_DONT_CARE)) != NULL)
    {
        pFilterVlanVal->dei = vlanAction->val;
    }

    if((vlanAction = rutGpon_searchVlanAction(pPathFlowUs,
                                              e_VLAN_ACTION_SET_VID,
                                              0, OMCI_DONT_CARE)) != NULL)
    {
        pFilterVlanVal->vid = vlanAction->val;
    }

    if(hasTagInsertion && pPathFlowUs->filter.vlan.nbrOfTags == 1)
    {
        /* US frame is single-tagged, and a tag was pushed: we also need
           to set the inner tag filters */
        pPathFlowDs->filter.vlan.inner = pPathFlowUs->filter.vlan.inner;
    }


    /* copy ethertype filter */

    if(pPathFlowUs->filter.ethType.cmpOp != e_CMP_NEQ)
    {
        /* copy ethertype filter */
        pPathFlowDs->filter.ethType = pPathFlowUs->filter.ethType;
    }

    /*
     * Actions
     */

    if(hasTagInsertion)
    {
        OMCI_ADD_ETH_ACTION(pPathFlowDs, e_ETH_ACTION_POP_TAG);

        /* Tags pop: Outer Tag Remove, Inner Tag Replacement */
        /* turn filters into actions */

        if (!OMCI_IS_DONT_CARE(pPathFlowUs->filter.vlan.inner.tpid))
        {
            OMCI_ADD_VLAN_ACTION(pPathFlowDs, e_VLAN_ACTION_SET_TPID,
                                 pPathFlowUs->filter.vlan.inner.tpid, e_VLAN_TAG1, OMCI_DONT_CARE);
        }
        if (!OMCI_IS_DONT_CARE(pPathFlowUs->filter.vlan.inner.pbits))
        {
            OMCI_ADD_VLAN_ACTION(pPathFlowDs, e_VLAN_ACTION_SET_PBITS,
                        	     pPathFlowUs->filter.vlan.inner.pbits, e_VLAN_TAG1, OMCI_DONT_CARE);
        }
        if (!OMCI_IS_DONT_CARE(pPathFlowUs->filter.vlan.inner.dei))
        {
            OMCI_ADD_VLAN_ACTION(pPathFlowDs, e_VLAN_ACTION_SET_DEI,
                                 pPathFlowUs->filter.vlan.inner.dei, e_VLAN_TAG1, OMCI_DONT_CARE);
        }
        if (!OMCI_IS_DONT_CARE(pPathFlowUs->filter.vlan.inner.vid))
        {
            OMCI_ADD_VLAN_ACTION(pPathFlowDs, e_VLAN_ACTION_SET_VID,
                                 pPathFlowUs->filter.vlan.inner.vid, e_VLAN_TAG1, OMCI_DONT_CARE);
        }
    }
    else if(pPathFlowUs->filter.vlan.nbrOfTags == 1)
    {
        /* US frame is single-tagged, no tags pushed: Tag replacement */

        /* turn filters into actions */

        if(!OMCI_IS_DONT_CARE(pPathFlowUs->filter.vlan.inner.tpid))
        {
            OMCI_ADD_VLAN_ACTION(pPathFlowDs, e_VLAN_ACTION_SET_TPID,
                                 pPathFlowUs->filter.vlan.inner.tpid, 0, 0);
        }
        if(!OMCI_IS_DONT_CARE(pPathFlowUs->filter.vlan.inner.pbits))
        {
       	    if((rutGpon_searchVlanAction(pPathFlowUs,
    					 e_VLAN_ACTION_COPY_PBITS_FROM_TAG,
                                         0, 1) == NULL) &&
               (rutGpon_searchVlanAction(pPathFlowUs,
    	                                 e_VLAN_ACTION_SET_PBITS,
                                         0, 0) == NULL))
            {
                pFilterVlanVal->pbits = pPathFlowUs->filter.vlan.inner.pbits;
            }
            else
            { 
                OMCI_ADD_VLAN_ACTION(pPathFlowDs, e_VLAN_ACTION_SET_PBITS,
                        	     pPathFlowUs->filter.vlan.inner.pbits, 0, 0);
            }
        }
        if(!OMCI_IS_DONT_CARE(pPathFlowUs->filter.vlan.inner.dei))
        {
            OMCI_ADD_VLAN_ACTION(pPathFlowDs, e_VLAN_ACTION_SET_DEI,
                                 pPathFlowUs->filter.vlan.inner.dei, 0, 0);
        }
        if(!OMCI_IS_DONT_CARE(pPathFlowUs->filter.vlan.inner.vid))
        {
            OMCI_ADD_VLAN_ACTION(pPathFlowDs, e_VLAN_ACTION_SET_VID,
                                 pPathFlowUs->filter.vlan.inner.vid, 0, 0);
        }
    }

out:
    return ret;
}
#endif    // SUPPORT_DOUBLE_TAGS

static void rutGpon_initPathFlow(OmciPathFlow_t *pPathFlow)
{
    memset(pPathFlow, (UINT8)(OMCI_DONT_CARE), sizeof(OmciPathFlow_t));
    pPathFlow->action.ethActionIx = 0;
    pPathFlow->action.vlanActionIx = 0;
}

CmsRet rutGpon_processVlanFilter(UINT8 vlanTciFilterFwdOper,
                                 OmciVlanTciFilter_t *pVlanTciFilter,
                                 OmciPathFlow_t *pPathFlowUs)
{
    CmsRet ret = CMSRET_SUCCESS;

    /*
     * Vlan Tagging Filter
     */

    if(pVlanTciFilter == NULL)
    {
        omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "===> Vlan Tagging Filter NOT specified\n");
        goto out;
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "===> Processing Vlan Tagging Filter: fwdOper <0x%02X>, pbits<%d>, dei<%d>, vid<%d>...\n",
                   vlanTciFilterFwdOper, pVlanTciFilter->pbits, pVlanTciFilter->cfi, pVlanTciFilter->vlanId);
        
    if(OMCI_TCI_FILTER_IS_DONT_CARE(pVlanTciFilter))
    {
        /* the default TCI Filter is used to create the default downstream filter */

        omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "Default Vlan Tagging Filter\n");

        /* FIXME: We only support the modes that DROP on misses, so let's not create
           the US default flow for now */
        pPathFlowUs->filter.vlan.nbrOfTags = OMCI_DONT_CARE;
    }
    else
    {
        /* IMPORTANT: MAC lerning will only be supported in the RG implementation, which
           was not available at the time this code was written. */

        switch(vlanTciFilterFwdOper)
        {
            case 0x03:
            case 0x04:
            case 0x0F:
            case 0x10:
            case 0x16:
            case 0x17:
            case 0x1C:
                /* Tagged: Hit[EQ, VID], Miss[DROP]; Untagged: BRIDGE */
            case 0x1D:
                /* Tagged: Hit[EQ, VID], Miss[DROP]; Untagged: DROP */

                /* FIXME: Ignoring Untagged flow filtering for now */

                ret = rutGpon_createVidFilter(pPathFlowUs, pVlanTciFilter->vlanId);
                if(ret != CMSRET_SUCCESS)
                {
                    omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "MISMATCH: vlanTciFilter Oper <0x%02X>\n", vlanTciFilterFwdOper);
                    goto out;
                }

                break;

            case 0x07:
            case 0x08:
            case 0x11:
            case 0x12:
            case 0x18:
            case 0x19:
            case 0x1E:
                /* Tagged: Hit[EQ, PBITS], Miss[DROP]; Untagged: BRIDGE */
            case 0x1F:
                /* Tagged: Hit[EQ, PBITS], Miss[DROP]; Untagged: DROP */

                /* FIXME: Ignoring Untagged flow filtering for now */

                ret = rutGpon_createPbitsFilter(pPathFlowUs, pVlanTciFilter->pbits);
                if(ret != CMSRET_SUCCESS)
                {
                    omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "MISMATCH: vlanTciFilter Oper <0x%02X>\n", vlanTciFilterFwdOper);
                    goto out;
                }

                break;

            case 0x0B:
            case 0x0C:
            case 0x13:
            case 0x14:
            case 0x1A:
            case 0x1B:
            case 0x20:
                /* Tagged: Hit[EQ, TCI], Miss[DROP]; Untagged: BRIDGE */
            case 0x21:
                /* Tagged: Hit[EQ, TCI], Miss[DROP]; Untagged: DROP */

                /* FIXME: Ignoring Untagged flow filtering for now */

                ret = rutGpon_createPbitsFilter(pPathFlowUs, pVlanTciFilter->pbits);
                if(ret != CMSRET_SUCCESS)
                {
                    omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "MISMATCH: vlanTciFilter Oper <0x%02X>\n", vlanTciFilterFwdOper);
                    goto out;
                }

                ret = rutGpon_createDeiFilter(pPathFlowUs, pVlanTciFilter->cfi);
                if(ret != CMSRET_SUCCESS)
                {
                    omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "MISMATCH: vlanTciFilter Oper <0x%02X>\n", vlanTciFilterFwdOper);
                    goto out;
                }

                ret = rutGpon_createVidFilter(pPathFlowUs, pVlanTciFilter->vlanId);
                if(ret != CMSRET_SUCCESS)
                {
                    omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "MISMATCH: vlanTciFilter Oper <0x%02X>\n", vlanTciFilterFwdOper);
                    goto out;
                }

                break;

            case 0x00:
                /* Tagged: BRIDGE; Untagged: BRIDGE */
                if(OMCI_IS_DONT_CARE(pPathFlowUs->filter.vlan.nbrOfTags))
                    pPathFlowUs->filter.vlan.nbrOfTags = OMCI_FILTER_TYPE_UNTAG;
                break;

            case 0x01:
                /* Tagged: DROP; Untagged: BRIDGE */
                if(OMCI_IS_DONT_CARE(pPathFlowUs->filter.vlan.nbrOfTags))
                    pPathFlowUs->filter.vlan.nbrOfTags = OMCI_FILTER_TYPE_UNTAG;
                pPathFlowUs->filter.ethType.cmpOp = e_CMP_NEQ;
                pPathFlowUs->filter.ethType.val = Q_TAG_TPID;
                break;

            case 0x15:
            case 0x02:
                /* Tagged: BRIDGE; Untagged: DROP */
                if(OMCI_IS_DONT_CARE(pPathFlowUs->filter.vlan.nbrOfTags))
                    pPathFlowUs->filter.vlan.nbrOfTags = OMCI_FILTER_TYPE_SINGLE_TAG;
                break;

            default:
                if(vlanTciFilterFwdOper < 0x0F)
                {
                    cmsLog_error("Vlan Tagging Filter operation NOT supported: <%d>",
                                 vlanTciFilterFwdOper);

                    ret = CMSRET_METHOD_NOT_SUPPORTED;
                }
                else
                {
                    cmsLog_error("Invalid Vlan Tagging Filter operation: <%d>",
                                 vlanTciFilterFwdOper);

                    ret = CMSRET_INVALID_ARGUMENTS;
                }

                goto out;
        }
    }

out:
    return ret;
}

CmsRet rutGpon_processTagOperations(UINT16 vlanTagOperInputTpid,
                                    UINT16 vlanTagOperOutputTpid,
                                    UINT8  vlanTagOperDsMode,
                                    OmciExtVlanTagOper_t *pExtVlanTagOper,
                                    OmciPathFlow_t *pPathFlowUs,
                                    OmciPathFlow_t *pPathFlowDs)
{
    CmsRet ret = CMSRET_SUCCESS;
    UINT8 isDefaultRule;

    /*
     * Extended VLAN Tagging Operation
     */

    if(pExtVlanTagOper == NULL)
    {
        omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "===> Extended VLAN Tagging Operation NOT specified\n");
        goto out;
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "===> Processing Extended VLAN Tagging Operation...\n");

#ifdef OMCI_DEBUG_SUPPORT
    rutGpon_printVlanTagOper(pExtVlanTagOper);
#endif

    /* Extract flow information from Extended VLAN operations entry */

    ret = rutGpon_parseExtendedVlanOper(vlanTagOperInputTpid,
                                        vlanTagOperOutputTpid,
                                        pExtVlanTagOper,
                                        pPathFlowUs,
                                        &isDefaultRule);
    if(ret != CMSRET_SUCCESS)
    {
        goto out;
    }

    ret = rutGpon_createDsFlow(pExtVlanTagOper,
                               pPathFlowUs,
                               pPathFlowDs,
                               vlanTagOperDsMode,
                               isDefaultRule);
    if(ret != CMSRET_SUCCESS)
    {
        goto out;
    }

out:
    return ret;
}

CmsRet rutGpon_processPbitsMapper(UINT8 mapperPbits,
                                  OmciPathFlow_t *pPathFlowUs)
{
    CmsRet ret = CMSRET_SUCCESS;

    /*
     * Mapper PBITS
     */

    if(OMCI_IS_DONT_CARE(mapperPbits))
    {
        omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "===> Mapper PBITS NOT specified\n");
        goto out;
    }

    omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "===> Processing Mapper PBITS <%d>...\n", mapperPbits);

    if(mapperPbits > OMCI_FILTER_PRIO_MAX)
    {
        cmsLog_error("Invalid Mapper PBITS: <%d>", mapperPbits);

        ret = CMSRET_INTERNAL_ERROR;
        goto out;
    }

    /* check for actions and filters that may already change the Mapper pbits... */
    ret = rutGpon_createPbitsFilter(pPathFlowUs, mapperPbits);
    if(ret != CMSRET_SUCCESS)
    {
        goto out;
    }

out:
    return ret;
}

/*
 * ASSUMPTIONS: Default behavior for FCB misses is DROP
 *
 * @ pExtVlanTagOper: Must be set to NULL if the path does not have an
 *                 Extended Vlan Tagging Operation object
 * 
 * @ pVlanTciFilter: Must be set to NULL if the path does not have a
 *                   Vlan Tagging Filter object
 *
 * @ mapperPbits: Must be set to OMCI_DONT_CARE is the path does not have a
 *                802.1p Mapper Service Profile object
 *
 * Returns:
 *    CMSRET_REQUEST_DENIED       : flow could not be created
 *    CMSRET_METHOD_NOT_SUPPORTED : filter/action valid, but not supported
 *    CMSRET_INVALID_ARGUMENTS    : Error: Invalid filter/action value
 *    CMSRET_INTERNAL_ERROR       : Fatal error, exit omcid
 */
CmsRet rutGpon_extractPathFlows(UINT16 vlanTagOperInputTpid,
                                UINT16 vlanTagOperOutputTpid,
                                UINT8  vlanTagOperDsMode,
                                OmciExtVlanTagOper_t *pExtVlanTagOper,

                                UINT8 vlanTciFilterFwdOper,
                                OmciVlanTciFilter_t *pVlanTciFilter,

                                UINT8 mapperPbits,

                                OmciPathFlow_t *pPathFlowUs,
                                OmciPathFlow_t *pPathFlowDs)
{
    CmsRet ret = CMSRET_SUCCESS;
#ifdef OMCI_DEBUG_SUPPORT
    static int flowCounter = 0;
    omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "\n\n%s: ******************** Flow <%d> ********************\n",
                   __FUNCTION__, flowCounter++);
#endif

    /* initialize flows */
    rutGpon_initPathFlow(pPathFlowUs);
    rutGpon_initPathFlow(pPathFlowDs);

    if(pExtVlanTagOper == NULL && OMCI_IS_DONT_CARE(mapperPbits) && pVlanTciFilter == NULL )
    {
        /* Nothing */
        omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "===> No Filters, do nothing!\n");
        goto out;
    }
    else if(pExtVlanTagOper == NULL && OMCI_IS_DONT_CARE(mapperPbits) && pVlanTciFilter != NULL )
    {
        /* VLAN Filter Only */

        ret = rutGpon_processVlanFilter(vlanTciFilterFwdOper,
                                        pVlanTciFilter,
                                        pPathFlowUs);
        if(ret != CMSRET_SUCCESS)
        {
            goto out;
        }

        if(OMCI_TCI_FILTER_IS_DONT_CARE(pVlanTciFilter))
        {
            /* create default downstream flow */
            pPathFlowDs->filter.vlan.nbrOfTags = OMCI_FILTER_TYPE_UNTAG;
        }
    }
    else if(pExtVlanTagOper == NULL && !OMCI_IS_DONT_CARE(mapperPbits) && pVlanTciFilter == NULL )
    {
        /* 802.1p Mapper Only */

        pPathFlowUs->filter.vlan.nbrOfTags = OMCI_FILTER_TYPE_SINGLE_TAG;

        ret = rutGpon_processPbitsMapper(mapperPbits,
                                         pPathFlowUs);
        if(ret != CMSRET_SUCCESS)
        {
            goto out;
        }

        /* create default downstream flow */
        pPathFlowDs->filter.vlan.nbrOfTags = OMCI_FILTER_TYPE_UNTAG;
    }
    else if(pExtVlanTagOper == NULL && !OMCI_IS_DONT_CARE(mapperPbits) && pVlanTciFilter != NULL )
    {
        /* 802.1p Mapper + VLAN Filter */

        ret = rutGpon_processPbitsMapper(mapperPbits,
                                         pPathFlowUs);
        if(ret != CMSRET_SUCCESS)
        {
            goto out;
        }

        ret = rutGpon_processVlanFilter(vlanTciFilterFwdOper,
                                        pVlanTciFilter,
                                        pPathFlowUs);
        if(ret != CMSRET_SUCCESS)
        {
            goto out;
        }

        if(OMCI_TCI_FILTER_IS_DONT_CARE(pVlanTciFilter))
        {
            /* create default downstream flow */
            pPathFlowDs->filter.vlan.nbrOfTags = OMCI_FILTER_TYPE_UNTAG;
        }
    }
    else if(pExtVlanTagOper != NULL && OMCI_IS_DONT_CARE(mapperPbits) && pVlanTciFilter == NULL )
    {
        /* VLAN Tagging Operations Only */

        /* FIXME: The default rule will create US and DS flows with fwd info ONLY... */

        ret = rutGpon_processTagOperations(vlanTagOperInputTpid,
                                           vlanTagOperOutputTpid,
                                           vlanTagOperDsMode,
                                           pExtVlanTagOper,
                                           pPathFlowUs,
                                           pPathFlowDs);
        if(ret != CMSRET_SUCCESS)
        {
            goto out;
        }
    }
    else if(pExtVlanTagOper != NULL && OMCI_IS_DONT_CARE(mapperPbits) && pVlanTciFilter != NULL )
    {
        /* VLAN Tagging Operations + VLAN Filter */

        if(OMCI_TCI_FILTER_IS_DONT_CARE(pVlanTciFilter))
        {
            omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "MISMATCH: Default VLAN Filter + VLAN Tagging Operations\n");

            ret = CMSRET_REQUEST_DENIED;
            goto out;
        }

        ret = rutGpon_processTagOperations(vlanTagOperInputTpid,
                                           vlanTagOperOutputTpid,
                                           vlanTagOperDsMode,
                                           pExtVlanTagOper,
                                           pPathFlowUs,
                                           pPathFlowDs);
        if(ret != CMSRET_SUCCESS)
        {
            goto out;
        }

        ret = rutGpon_processVlanFilter(vlanTciFilterFwdOper,
                                        pVlanTciFilter,
                                        pPathFlowUs);
        if(ret != CMSRET_SUCCESS)
        {
            goto out;
        }
    }
    else if(pExtVlanTagOper != NULL && !OMCI_IS_DONT_CARE(mapperPbits) && pVlanTciFilter == NULL )
    {
        /* VLAN Tagging Operations + 802.1p Mapper */

        ret = rutGpon_processTagOperations(vlanTagOperInputTpid,
                                           vlanTagOperOutputTpid,
                                           vlanTagOperDsMode,
                                           pExtVlanTagOper,
                                           pPathFlowUs,
                                           pPathFlowDs);
        if(ret != CMSRET_SUCCESS)
        {
            goto out;
        }

        ret = rutGpon_processPbitsMapper(mapperPbits,
                                         pPathFlowUs);
        if(ret != CMSRET_SUCCESS)
        {
            goto out;
        }
    }
    else if(pExtVlanTagOper != NULL && !OMCI_IS_DONT_CARE(mapperPbits) && pVlanTciFilter != NULL )
    {
        /* VLAN Tagging Operations + 802.1p Mapper + VLAN Filter */

        if(OMCI_TCI_FILTER_IS_DONT_CARE(pVlanTciFilter))
        {
            omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "MISMATCH: Default VLAN Filter + VLAN Tagging Operations + 802.1p Mapper\n");

            ret = CMSRET_REQUEST_DENIED;
            goto out;
        }

        ret = rutGpon_processTagOperations(vlanTagOperInputTpid,
                                           vlanTagOperOutputTpid,
                                           vlanTagOperDsMode,
                                           pExtVlanTagOper,
                                           pPathFlowUs,
                                           pPathFlowDs);
        if(ret != CMSRET_SUCCESS)
        {
            goto out;
        }

        ret = rutGpon_processPbitsMapper(mapperPbits,
                                         pPathFlowUs);
        if(ret != CMSRET_SUCCESS)
        {
            goto out;
        }

        ret = rutGpon_processVlanFilter(vlanTciFilterFwdOper,
                                        pVlanTciFilter,
                                        pPathFlowUs);
        if(ret != CMSRET_SUCCESS)
        {
            goto out;
        }
    }
    else
    {
        cmsLog_error("Invalid Operation: pExtVlanTagOper 0x%08X, mapperPbits %d, pVlanTciFilter 0x%08X",
                     (UINT32)pExtVlanTagOper, mapperPbits, (UINT32)pVlanTciFilter);

        ret = CMSRET_INTERNAL_ERROR;
        goto out;
    }

out:
    if(ret == CMSRET_SUCCESS)
    {
#ifdef OMCI_DEBUG_SUPPORT
        omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "*** US Flow ***\n");
        rutGpon_printPathFlow(pPathFlowUs);

        omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "*** DS Flow ***\n");
        rutGpon_printPathFlow(pPathFlowDs);
#endif /* OMCI_DEBUG_SUPPORT */
    }

    return ret;
}

#if defined(CONFIG_BCM_GPON_802_1Q_ENABLED)
CmsRet rutGpon_extractDsPathFlows(UINT16 vlanTagOperInputTpid,
                                  UINT16 vlanTagOperOutputTpid,
                                  UINT8  vlanTagOperDsMode,
                                  OmciExtVlanTagOper_t *pExtVlanTagOper,

                                  UINT8 vlanTciFilterFwdOper,
                                  OmciVlanTciFilter_t *pVlanTciFilter,

                                  OmciPathFlow_t *pPathFlowUs,
                                  OmciPathFlow_t *pPathFlowDs)
{
    CmsRet ret = CMSRET_SUCCESS;

    /* initialize flows */
    rutGpon_initPathFlow(pPathFlowUs);
    rutGpon_initPathFlow(pPathFlowDs);

    if(pExtVlanTagOper == NULL && pVlanTciFilter == NULL )
    {
        /* Nothing */
        omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "===> No Filters, do nothing!\n");
        goto out;
    }
    else if(pExtVlanTagOper == NULL && pVlanTciFilter != NULL )
    {
        /* VLAN Filter Only */

        ret = rutGpon_processVlanFilter(vlanTciFilterFwdOper,
                                        pVlanTciFilter,
                                        pPathFlowDs);
        if(ret != CMSRET_SUCCESS)
        {
            goto out;
        }
    }
    else if(pExtVlanTagOper != NULL && pVlanTciFilter == NULL )
    {
        /* VLAN Tagging Operations */

        ret = rutGpon_processTagOperations(vlanTagOperInputTpid,
                                           vlanTagOperOutputTpid,
                                           vlanTagOperDsMode,
                                           pExtVlanTagOper,
                                           pPathFlowUs,
                                           pPathFlowDs);
        if(ret != CMSRET_SUCCESS)
        {
            goto out;
        }

    }
    else if(pExtVlanTagOper != NULL && pVlanTciFilter != NULL )
    {
        /* VLAN Tagging Operations + 802.1p Mapper + VLAN Filter */

        if(OMCI_TCI_FILTER_IS_DONT_CARE(pVlanTciFilter))
        {
            omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "MISMATCH: Default VLAN Filter + VLAN Tagging Operations \n");

            ret = CMSRET_REQUEST_DENIED;
            goto out;
        }

        ret = rutGpon_processTagOperations(vlanTagOperInputTpid,
                                           vlanTagOperOutputTpid,
                                           vlanTagOperDsMode,
                                           pExtVlanTagOper,
                                           pPathFlowUs,
                                           pPathFlowDs);
        if(ret != CMSRET_SUCCESS)
        {
            goto out;
        }

        ret = rutGpon_processVlanFilter(vlanTciFilterFwdOper,
                                        pVlanTciFilter,
                                        pPathFlowDs);
        if(ret != CMSRET_SUCCESS)
        {
            goto out;
        }
    }
    else
    {
        cmsLog_error("Invalid Operation: pExtVlanTagOper 0x%08X, pVlanTciFilter 0x%08lX",
                     (UINT32)pExtVlanTagOper, (UINT32)pVlanTciFilter);

        ret = CMSRET_INTERNAL_ERROR;
        goto out;
    }

out:
    if(ret == CMSRET_SUCCESS)
    {
        omciDebugPrint(OMCI_DEBUG_MODULE_FLOW, "*** DS Flow ***\n");
        rutGpon_printPathFlow(pPathFlowDs);
    }

    return ret;
}
#endif /* CONFIG_BCM_GPON_802_1Q_ENABLED */

#endif /* DMP_X_ITU_ORG_GPON_1 */
