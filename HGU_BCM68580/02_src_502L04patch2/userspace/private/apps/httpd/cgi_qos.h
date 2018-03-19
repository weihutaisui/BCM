/***********************************************************************
 *
 *  Copyright (c) 2004-2010  Broadcom Corporation
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

#ifndef __CGI_QOS_H__
#define __CGI_QOS_H__

#include <stdio.h>
#include <fcntl.h>

/** CGI function for QoS queue management.
 */
void cgiQosMgmt(char *query, FILE *fs);

/** CGI function for QoS classification.
 */
void cgiQosCls(char *path, FILE *fs);

/** CGI function for QoS queue.
 */
void cgiQosQueue(char *query, FILE *fs);




/** CGI function for QoS (ethernet) port shaping.
 */
void cgiQosPortShaping(char *query, FILE *fs);

/** Get html based info about available port shaping ports.
 */
void cgiGetQosPortShapingInfo(int argc, char **argv, char *varValue);
void cgiGetQosPortShapingInfo_igd(int argc, char **argv, char *varValue);
void cgiGetQosPortShapingInfo_dev2(int argc, char **argv, char *varValue);

/** Handle save/apply of QoS (ethernet) port shaping.
 */
void cgiQosPortShapingSavApply(char *query);
void cgiQosPortShapingSavApply_igd(char *query);
void cgiQosPortShapingSavApply_dev2(char *query);

#if defined(SUPPORT_DM_LEGACY98)
#define cgiQosPortShapingSavApply(q) cgiQosPortShapingSavApply_igd((q))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiQosPortShapingSavApply(q) cgiQosPortShapingSavApply_igd((q))
#elif defined(SUPPORT_DM_PURE181)
#define cgiQosPortShapingSavApply(q) cgiQosPortShapingSavApply_dev2((q))
#elif defined(SUPPORT_DM_DETECT)
#define cgiQosPortShapingSavApply(q) (cmsMdm_isDataModelDevice2() ? \
                                      cgiQosPortShapingSavApply_dev2((q)) : \
                                      cgiQosPortShapingSavApply_igd((q)))
#endif




/** CGI function for QoS policer.
 */
void cgiQosPolicer(char *query, FILE *fs);


/** Returns a list of queues that packets can be sent to after classification.
 *  Queues are identified by their MDM instance id, which goes
 *  into the classificationObj->classQueue field.
 */
void cgiGetQosQueueInfo(int argc, char **argv, char *varValue);
void cgiGetQosQueueInfo_igd(int argc, char **argv, char *varValue);
void cgiGetQosQueueInfo_dev2(int argc, char **argv, char *varValue);


/** Returns object instance values of all the existing QoS policers.
 */
void cgiGetQosPolicerInfo(int argc, char **argv, char *varValue);
void cgiGetQosPolicerInfo_igd(int argc, char **argv, char *varValue);
void cgiGetQosPolicerInfo_dev2(int argc, char **argv, char *varValue);


/** Print out the html page for the Policer page
 */
void cgiQosPolicerView(FILE *fs);


/** Traverse the MDM and dump out info about each Policer for WebUI.
 */
void cgiQosPolicerViewBody(FILE *fs);
void cgiQosPolicerViewBody_igd(FILE *fs);
void cgiQosPolicerViewBody_dev2(FILE *fs);

#if defined(SUPPORT_DM_LEGACY98)
#define cgiQosPolicerViewBody(w)       cgiQosPolicerViewBody_igd((w))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiQosPolicerViewBody(w)       cgiQosPolicerViewBody_igd((w))
#elif defined(SUPPORT_DM_PURE181)
#define cgiQosPolicerViewBody(w)       cgiQosPolicerViewBody_dev2((w))
#elif defined(SUPPORT_DM_DETECT)
#define cgiQosPolicerViewBody(w)       (cmsMdm_isDataModelDevice2() ? \
                                     cgiQosPolicerViewBody_dev2((w)) : \
                                     cgiQosPolicerViewBody_igd((w)))
#endif


/** Enable the policer
 */
void cgiQosPolicerEnable(char *query, FILE *fs);
void cgiQosPolicerEnable_igd(char *query, FILE *fs);
void cgiQosPolicerEnable_dev2(char *query, FILE *fs);

#if defined(SUPPORT_DM_LEGACY98)
#define cgiQosPolicerEnable(q, f)       cgiQosPolicerEnable_igd((q), (f))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiQosPolicerEnable(q, f)       cgiQosPolicerEnable_igd((q), (f))
#elif defined(SUPPORT_DM_PURE181)
#define cgiQosPolicerEnable(q, f)       cgiQosPolicerEnable_dev2((q), (f))
#elif defined(SUPPORT_DM_DETECT)
#define cgiQosPolicerEnable(q, f)       (cmsMdm_isDataModelDevice2() ? \
                                     cgiQosPolicerEnable_dev2((q), (f)) : \
                                     cgiQosPolicerEnable_igd((q), (f)))
#endif


/** Remove the policer
 */
void cgiQosPolicerRemove(char *query, FILE *fs);
void cgiQosPolicerRemove_igd(char *query, FILE *fs);
void cgiQosPolicerRemove_dev2(char *query, FILE *fs);

#if defined(SUPPORT_DM_LEGACY98)
#define cgiQosPolicerRemove(q, f)       cgiQosPolicerRemove_igd((q), (f))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiQosPolicerRemove(q, f)       cgiQosPolicerRemove_igd((q), (f))
#elif defined(SUPPORT_DM_PURE181)
#define cgiQosPolicerRemove(q, f)       cgiQosPolicerRemove_dev2((q), (f))
#elif defined(SUPPORT_DM_DETECT)
#define cgiQosPolicerRemove(q, f)       (cmsMdm_isDataModelDevice2() ? \
                                     cgiQosPolicerRemove_dev2((q), (f)) : \
                                     cgiQosPolicerRemove_igd((q), (f)))
#endif


/** Returns names of all the QoS enabled interfaces.
 */
void cgiGetQosIntf(int argc, char **argv, char *varValue);
void cgiGetQosIntf_igd(int argc, char **argv, char *varValue);
void cgiGetQosIntf_dev2(int argc, char **argv, char *varValue);


/** Returns names of all the Queue interfaces their scheduler info.
 */
void cgiGetQueueIntf(int argc, char **argv, char *varValue);
void cgiGetQueueIntf_igd(int argc, char **argv, char *varValue);
void cgiGetQueueIntf_dev2(int argc, char **argv, char *varValue);


/** queue info helper function.
 */
void cgiGetIntfQueuePrec(const char *l2Ifname, UBOOL8 isWan, char *varValue);


/** Traverse through all the QoS queues matching l2Ifname and see which
 *  queue scheduling algorithms are in use.
 */
CmsRet cgiGetQosQueueSchedulerInfo(const char *l2Ifname,
                                       char *precArray, SINT32 *numSP,
                                       SINT32 *numWFQ, SINT32 *numWRR);

CmsRet cgiGetQosQueueSchedulerInfo_igd(const char *l2Ifname,
                                       char *precArray, SINT32 *numSP,
                                       SINT32 *numWFQ, SINT32 *numWRR);

CmsRet cgiGetQosQueueSchedulerInfo_dev2(const char *l2Ifname,
                                       char *precArray, SINT32 *numSP,
                                       SINT32 *numWFQ, SINT32 *numWRR);

#if defined(SUPPORT_DM_LEGACY98)
#define cgiGetQosQueueSchedulerInfo(i, p, s, w, r)  cgiGetQosQueueSchedulerInfo_igd((i), (p), (s), (w), (r))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiGetQosQueueSchedulerInfo(i, p, s, w, r)  cgiGetQosQueueSchedulerInfo_igd((i), (p), (s), (w), (r))
#elif defined(SUPPORT_DM_PURE181)
#define cgiGetQosQueueSchedulerInfo(i, p, s, w, r)  cgiGetQosQueueSchedulerInfo_dev2((i), (p), (s), (w), (r))
#elif defined(SUPPORT_DM_DETECT)
#define cgiGetQosQueueSchedulerInfo(i, p, s, w, r)  (cmsMdm_isDataModelDevice2() ? \
                                                        cgiGetQosQueueSchedulerInfo_dev2((i), (p), (s), (w), (r)) : \
                                                        cgiGetQosQueueSchedulerInfo_igd((i), (p), (s), (w), (r)))
#endif



/** Traverse the MDM and dump out info about each queue for WebUI.
 *
 */
void cgiQosQueueViewBody(FILE *fs);
void cgiQosQueueViewBody_igd(FILE *fs);
void cgiQosQueueViewBody_dev2(FILE *fs);

#if defined(SUPPORT_DM_LEGACY98)
#define cgiQosQueueViewBody(w)       cgiQosQueueViewBody_igd((w))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiQosQueueViewBody(w)       cgiQosQueueViewBody_igd((w))
#elif defined(SUPPORT_DM_PURE181)
#define cgiQosQueueViewBody(w)       cgiQosQueueViewBody_dev2((w))
#elif defined(SUPPORT_DM_DETECT)
#define cgiQosQueueViewBody(w)       (cmsMdm_isDataModelDevice2() ? \
                                     cgiQosQueueViewBody_dev2((w)) : \
                                     cgiQosQueueViewBody_igd((w)))
#endif



#ifdef BRCM_WLAN
/** Traverse the MDM and dump out info about each wlan queue for WebUI.
 *
 */
void cgiQosWlQueueViewBody(FILE *fs);
void cgiQosWlQueueViewBody_igd(FILE *fs);
void cgiQosWlQueueViewBody_dev2(FILE *fs);

#if defined(SUPPORT_DM_LEGACY98)
#define cgiQosWlQueueViewBody(w)       cgiQosWlQueueViewBody_igd((w))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiQosWlQueueViewBody(w)       cgiQosWlQueueViewBody_igd((w))
#elif defined(SUPPORT_DM_PURE181)
#define cgiQosWlQueueViewBody(w)       cgiQosWlQueueViewBody_dev2((w))
#elif defined(SUPPORT_DM_DETECT)
#define cgiQosWlQueueViewBody(w)       (cmsMdm_isDataModelDevice2() ? \
                                     cgiQosWlQueueViewBody_dev2((w)) : \
                                     cgiQosWlQueueViewBody_igd((w)))
#endif

#endif


/** Enable all queues which have been checked on webui
 *
 */
void cgiQosQueueEnable(char *query, FILE *fs);
void cgiQosQueueEnable_igd(char *query, FILE *fs);
void cgiQosQueueEnable_dev2(char *query, FILE *fs);

#if defined(SUPPORT_DM_LEGACY98)
#define cgiQosQueueEnable(q, f)       cgiQosQueueEnable_igd((q), (f))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiQosQueueEnable(q, f)       cgiQosQueueEnable_igd((q), (f))
#elif defined(SUPPORT_DM_PURE181)
#define cgiQosQueueEnable(q, f)       cgiQosQueueEnable_dev2((q), (f))
#elif defined(SUPPORT_DM_DETECT)
#define cgiQosQueueEnable(q, f)       (cmsMdm_isDataModelDevice2() ? \
                                     cgiQosQueueEnable_dev2((q), (f)) : \
                                     cgiQosQueueEnable_igd((q), (f)))
#endif



/** Delete all queues which have been checked on webui
 *
 */
void cgiQosQueueRemove(char *query, FILE *fs);
void cgiQosQueueRemove_igd(char *query, FILE *fs);
void cgiQosQueueRemove_dev2(char *query, FILE *fs);

#if defined(SUPPORT_DM_LEGACY98)
#define cgiQosQueueRemove(q, f)       cgiQosQueueRemove_igd((q), (f))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiQosQueueRemove(q, f)       cgiQosQueueRemove_igd((q), (f))
#elif defined(SUPPORT_DM_PURE181)
#define cgiQosQueueRemove(q, f)       cgiQosQueueRemove_dev2((q), (f))
#elif defined(SUPPORT_DM_DETECT)
#define cgiQosQueueRemove(q, f)       (cmsMdm_isDataModelDevice2() ? \
                                     cgiQosQueueRemove_dev2((q), (f)) : \
                                     cgiQosQueueRemove_igd((q), (f)))
#endif




/** Traverse the MDM and dump out info about each classifier entry for WebUI.
 */
void cgiQosClsViewBody(FILE *fs);
void cgiQosClsViewBody_igd(FILE *fs);
void cgiQosClsViewBody_dev2(FILE *fs);

#if defined(SUPPORT_DM_LEGACY98)
#define cgiQosClsViewBody(w)       cgiQosClsViewBody_igd((w))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiQosClsViewBody(w)       cgiQosClsViewBody_igd((w))
#elif defined(SUPPORT_DM_PURE181)
#define cgiQosClsViewBody(w)       cgiQosClsViewBody_dev2((w))
#elif defined(SUPPORT_DM_DETECT)
#define cgiQosClsViewBody(w)       (cmsMdm_isDataModelDevice2() ? \
                                     cgiQosClsViewBody_dev2((w)) : \
                                     cgiQosClsViewBody_igd((w)))
#endif



/** Add a new classification entry.
 */
void cgiQosClsAdd(char *query, FILE *fs);
void cgiQosClsAdd_igd(char *query, FILE *fs);
void cgiQosClsAdd_dev2(char *query, FILE *fs);

#if defined(SUPPORT_DM_LEGACY98)
#define cgiQosClsAdd(q, f)       cgiQosClsAdd_igd((q), (f))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiQosClsAdd(q, f)       cgiQosClsAdd_igd((q), (f))
#elif defined(SUPPORT_DM_PURE181)
#define cgiQosClsAdd(q, f)       cgiQosClsAdd_dev2((q), (f))
#elif defined(SUPPORT_DM_DETECT)
#define cgiQosClsAdd(q, f)       (cmsMdm_isDataModelDevice2() ? \
                                     cgiQosClsAdd_dev2((q), (f)) : \
                                     cgiQosClsAdd_igd((q), (f)))
#endif



/** Enable the specified classification entries.
 */
void cgiQosClsEnable(char *query, FILE *fs);
void cgiQosClsEnable_igd(char *query, FILE *fs);
void cgiQosClsEnable_dev2(char *query, FILE *fs);

#if defined(SUPPORT_DM_LEGACY98)
#define cgiQosClsEnable(q, f)       cgiQosClsEnable_igd((q), (f))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiQosClsEnable(q, f)       cgiQosClsEnable_igd((q), (f))
#elif defined(SUPPORT_DM_PURE181)
#define cgiQosClsEnable(q, f)       cgiQosClsEnable_dev2((q), (f))
#elif defined(SUPPORT_DM_DETECT)
#define cgiQosClsEnable(q, f)       (cmsMdm_isDataModelDevice2() ? \
                                     cgiQosClsEnable_dev2((q), (f)) : \
                                     cgiQosClsEnable_igd((q), (f)))
#endif



/** Delete the specified classification entries.
 */
void cgiQosClsRemove(char *query, FILE *fs);
void cgiQosClsRemove_igd(char *query, FILE *fs);
void cgiQosClsRemove_dev2(char *query, FILE *fs);

#if defined(SUPPORT_DM_LEGACY98)
#define cgiQosClsRemove(q, f)       cgiQosClsRemove_igd((q), (f))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiQosClsRemove(q, f)       cgiQosClsRemove_igd((q), (f))
#elif defined(SUPPORT_DM_PURE181)
#define cgiQosClsRemove(q, f)       cgiQosClsRemove_dev2((q), (f))
#elif defined(SUPPORT_DM_DETECT)
#define cgiQosClsRemove(q, f)       (cmsMdm_isDataModelDevice2() ? \
                                     cgiQosClsRemove_dev2((q), (f)) : \
                                     cgiQosClsRemove_igd((q), (f)))
#endif



/** Returns classification orders of all the existing classification rules.
 */
void cgiGetQosClsRulsOrder(int argc, char **argv, char *varValue);
void cgiGetQosClsRulsOrder_igd(int argc, char **argv, char *varValue);
void cgiGetQosClsRulsOrder_dev2(int argc, char **argv, char *varValue);


/** Returns the value of queue management enable.
 */
void cgiGetQosMgmtEnbl(int argc, char **argv, char *varValue);
void cgiGetQosMgmtEnbl_igd(int argc, char **argv, char *varValue);
void cgiGetQosMgmtEnbl_dev2(int argc, char **argv, char *varValue);


/** Returns the default DSCP mark for queue management.
 */
void cgiGetQosMgmtDefMark(int argc, char **argv, char *varValue);
void cgiGetQosMgmtDefMark_igd(int argc, char **argv, char *varValue);
void cgiGetQosMgmtDefMark_dev2(int argc, char **argv, char *varValue);


/** Returns the default queue for queue management.
 */
void cgiGetQosMgmtDefQueue(int argc, char **argv, char *varValue);
void cgiGetQosMgmtDefQueue_igd(int argc, char **argv, char *varValue);
void cgiGetQosMgmtDefQueue_dev2(int argc, char **argv, char *varValue);


/** Returns DSCP name from the mark value.
 */
char *cgiQosDscpMarkToName(UINT8 mark);

/** Returns supported precedence levels
 */
void cgiGetPrecedence(int argc __attribute__((unused)), char **argv, char *varValue);

#endif // __CGI_QOS_H__
