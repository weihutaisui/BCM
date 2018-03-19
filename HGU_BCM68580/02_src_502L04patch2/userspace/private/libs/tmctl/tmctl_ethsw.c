/***********************************************************************
 *
 *  Copyright (c) 2010  Broadcom Corporation
 *  All Rights Reserved
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
 *
************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>
#include "os_defs.h"

#include "tmctl_ethsw.h"


/* ----------------------------------------------------------------------------
 * This function sets the Ethernet switch port scheduler.
 *
 * Parameters:
 *    ifname (IN) Linux interface name.
 *    portQcfg_p (IN) The port queue configurations.
 *
 * Return:
 *    tmctl_ret_e enum value
 * ----------------------------------------------------------------------------
 */
tmctl_ret_e tmctlEthSw_setPortSched(const char*       ifname,
                                    tmctl_portQcfg_t* portQcfg_p)
{
   tmctl_ret_e ret = TMCTL_SUCCESS;
   
#if defined(CHIP_63138) || defined(CHIP_63148) || defined(CHIP_4908) || defined(CHIP_63158)

   int i;
   int rc;
   int unit;
   bcm_port_t port;
   int schedMode = 0;
   int numSpQ    = 0;
   int weights[BCM_COS_COUNT] = {0};
   tmctl_queueCfg_t* qcfg_p;
   port_qos_sched_t  qs;
   uint32_t portmap = 0;
   
   tmctl_debug("Enter: ifname=%s numQueues=%d", ifname, portQcfg_p->numQueues);

   if (portQcfg_p->numQueues != BCM_COS_COUNT)
   {
      tmctl_error("number of configured queues (%d) is less than port queues (%d)",
                  portQcfg_p->numQueues, BCM_COS_COUNT);
      return TMCTL_ERROR;
   }

   for (i = 0; i < portQcfg_p->numQueues; i++)
   {
      qcfg_p = &(portQcfg_p->qcfg[i]);
      
      if (qcfg_p->qid < 0)
      {
         tmctl_error("queue #%d is not configured. qid=%d", i, qcfg_p->qid);
         ret = TMCTL_ERROR;
         break;
      }
      
      if (qcfg_p->schedMode == TMCTL_SCHED_SP)
         numSpQ++;
      else
         weights[i] = qcfg_p->weight;   
   }
   
   if (ret != TMCTL_SUCCESS)
      return ret;
   
   if (numSpQ == 0)
      schedMode = 1; /* WRR */
   else
      schedMode = 0; /* SP */
      
   if (numSpQ == portQcfg_p->numQueues)
      numSpQ = 0; /* all SP queues */
         
   /* get unit and port number from ifname */
   rc = bcm_enet_map_ifname_to_unit_portmap(ifname, &unit, &portmap);
   if (rc)
   {
      tmctl_error("bcm_enet_map_ifname_to_unit_port returns error %d", rc);
      return TMCTL_ERROR;
   }
   /* TMCTL must apply the same port setting to all the ports in LAG/Trunk group */
   port = -1; /* Start with -1 to increment with */
   while (portmap)
   {
      while( ! (portmap & (1<<(++port))) );
      portmap &= ~(1<<port); /* Reset the port we are execting below -- important */

      /* set port sched */
      memset(&qs, 0, sizeof(port_qos_sched_t));
      qs.sched_mode    = schedMode;
      qs.num_spq       = numSpQ;
      qs.wrr_type      = 0;   /* 0=WRR, 1=WDRR */
      qs.weights_upper = 0;   /* 0=lower, 1=upper */
      
      tmctl_debug("Calling bcm_cosq_sched_set: unit=%d port=%d schedMode=%d numSpQ=%d wt_lower=%d,%d,%d,%d",
                  unit, port, schedMode, numSpQ, weights[0], weights[1], weights[2], weights[3]);
      rc = bcm_cosq_sched_set_X(unit, port, &weights[0], &qs);
      if (rc)
      {
         tmctl_error("bcm_cosq_sched_set for lower weights returns error %d", rc);
         return TMCTL_ERROR;
      }
      
      qs.weights_upper = 1;   /* 0=lower, 1=upper */
      
      tmctl_debug("Calling bcm_cosq_sched_set: unit=%d port=%d schedMode=%d numSpQ=%d wt_upper=%d,%d,%d,%d",
                  unit, port, schedMode, numSpQ, weights[4], weights[5], weights[6], weights[7]);
      rc = bcm_cosq_sched_set_X(unit, port, &weights[0], &qs);
      if (rc)
      {
         tmctl_error("bcm_cosq_sched_set for upper weights returns error %d", rc);
         return TMCTL_ERROR;
      }
   }

#endif

   return ret;
       
}  /* End of tmctlEthSw_setPortSched() */


/* ----------------------------------------------------------------------------
 * This function gets the Ethernet switch port shaper configuration.
 *
 * Parameters:
 *    ifname (IN) Linux interface name.
 *    qid (IN) Queue ID.
 *    shaper_p (OUT) The queue shaper configuration.
 *
 * Return:
 *    tmctl_ret_e enum value
 * ----------------------------------------------------------------------------
 */
tmctl_ret_e tmctlEthSw_getPortShaper(const char*      ifname,
                                     tmctl_shaper_t* shaper_p)
{
   tmctl_ret_e ret = TMCTL_SUCCESS;
   
#if defined(CHIP_63138) || defined(CHIP_63148) || defined(CHIP_4908) || defined(CHIP_63158)

   int        rc;
   int        unit;
   bcm_port_t port;
   uint32_t   shapingRate = 0;
   uint32_t   burstSize   = 0;

   tmctl_debug("Enter: ifname=%s", ifname);

   /* get unit and port number from ifname */
   rc = bcm_enet_map_ifname_to_unit_port(ifname, &unit, &port);
   if (rc)
   {
      tmctl_error("bcm_enet_map_ifname_to_unit_port returns error %d", rc);
      return TMCTL_ERROR;
   }
   
   memset(shaper_p, 0, sizeof(tmctl_shaper_t));
   
   tmctl_debug("Calling bcm_port_rate_egress_get_X: unit=%d port=%d",
               unit, port);
   rc = bcm_port_rate_egress_get_X(unit, port, &shapingRate, &burstSize, -1, NULL);
   if (rc)
   {
      tmctl_error("bcm_port_rate_egress_get_X returns error %d", rc);
      return TMCTL_ERROR;
   }

   tmctl_debug("Got: shapingRate=%d burstSize=%d", shapingRate, burstSize);

   shaper_p->shapingRate      = shapingRate;    /* in kbps */
   shaper_p->shapingBurstSize /* Bytes */ = (burstSize /* in kbits */ * 1000)/8;
   
#endif

   return ret;
   
}  /* End of tmctlEthSw_getPortShaper() */


/* ----------------------------------------------------------------------------
 * This function sets the Ethernet switch port shaper configuration.
 *
 * Parameters:
 *    ifname (IN) Linux interface name.
 *    shaper_p (IN) The port shaper configuration.
 *
 * Return:
 *    tmctl_ret_e enum value
 * ----------------------------------------------------------------------------
 */
tmctl_ret_e tmctlEthSw_setPortShaper(const char*     ifname,
                                     tmctl_shaper_t* shaper_p)
{
   tmctl_ret_e ret = TMCTL_SUCCESS;
   
#if defined(CHIP_63138) || defined(CHIP_63148) || defined(CHIP_4908) || defined(CHIP_63158)

   int        rc;
   int        unit;
   bcm_port_t port;
   uint32_t portmap = 0;
   
   tmctl_debug("Enter: ifname=%s minRate=%d shapingRate=%d burstSize=%d",
               ifname, shaper_p->minRate, shaper_p->shapingRate,
               shaper_p->shapingBurstSize);

   /* get unit and port number from ifname */
   rc = bcm_enet_map_ifname_to_unit_portmap(ifname, &unit, &portmap);
   if (rc)
   {
      tmctl_error("bcm_enet_map_ifname_to_unit_portmap returns error %d", rc);
      return TMCTL_ERROR;
   }
   
   /* TMCTL must apply the same port setting to all the ports in LAG/Trunk group */
   port = -1; /* Start with -1 to increment with */
   while (portmap)
   {
      while( ! (portmap & (1<<(++port))) );
      portmap &= ~(1<<port); /* Reset the port we are execting below -- important */

      {
         /* set port shaping */      
         tmctl_debug("Calling bcm_port_rate_egress_set_X: unit=%d port=%d ercLimit=%d ercBurst=%d",
                     unit, port, shaper_p->shapingRate, shaper_p->shapingBurstSize);
         rc = bcm_port_rate_egress_set_X(unit, port, shaper_p->shapingRate /* kbps */, (shaper_p->shapingBurstSize /* Bytes */ * 8)/1000,
                                         -1,    /* qid -1 denotes port shaping */
                                         0);    /* byte mode */
         if (rc)
         {
            tmctl_error("bcm_port_rate_egress_set_X returns error %d", rc);
            return TMCTL_ERROR;
         }
      }
   }
   
#endif

   return ret;
   
}  /* End of tmctlEthSw_setPortShaper() */


/* ----------------------------------------------------------------------------
 * This function gets the Ethernet switch queue shaper configuration.
 *
 * Parameters:
 *    ifname (IN) Linux interface name.
 *    qid (IN) Queue ID.
 *    shaper_p (OUT) The queue shaper configuration.
 *
 * Return:
 *    tmctl_ret_e enum value
 * ----------------------------------------------------------------------------
 */
tmctl_ret_e tmctlEthSw_getQueueShaper(const char*     ifname,
                                      int             qid,
                                      tmctl_shaper_t* shaper_p)
{
   tmctl_ret_e ret = TMCTL_SUCCESS;
   
#if defined(CHIP_63138) || defined(CHIP_63148) || defined(CHIP_4908) || defined(CHIP_63158)

   int        rc;
   int        unit;
   bcm_port_t port;
   uint32_t   shapingRate = 0;
   uint32_t   burstSize   = 0;

   tmctl_debug("Enter: ifname=%s qid=%d", ifname, qid);

   /* get unit and port number from ifname */
   rc = bcm_enet_map_ifname_to_unit_port(ifname, &unit, &port);
   if (rc)
   {
      tmctl_error("bcm_enet_map_ifname_to_unit_port returns error %d", rc);
      return TMCTL_ERROR;
   }
   
   memset(shaper_p, 0, sizeof(tmctl_shaper_t));
   
   tmctl_debug("Calling bcm_port_rate_egress_get_X: unit=%d port=%d qid=%d",
               unit, port, qid);
               
   rc = bcm_port_rate_egress_get_X(unit, port, &shapingRate, &burstSize, qid, NULL);
   if (rc)
   {
      tmctl_error("bcm_port_rate_egress_get_X returns error %d", rc);
      return TMCTL_ERROR;
   }

   tmctl_debug("Got: shapingRate=%d burstSize=%d", shapingRate, burstSize);

   shaper_p->shapingRate      = shapingRate;    /* in kbps */
   shaper_p->shapingBurstSize /* Bytes */ = (burstSize /* in kbits */ * 1000)/8;
   
#endif

   return ret;
   
}  /* End of tmctlEthSw_getQueueShaper() */


/* ----------------------------------------------------------------------------
 * This function sets the Ethernet switch queue shaper configuration.
 *
 * Parameters:
 *    ifname (IN) Linux interface name.
 *    qid (IN) Queue ID.
 *    shaper_p (IN) The queue shaper configuration.
 *
 * Return:
 *    tmctl_ret_e enum value
 * ----------------------------------------------------------------------------
 */
tmctl_ret_e tmctlEthSw_setQueueShaper(const char*     ifname,
                                      int             qid,
                                      tmctl_shaper_t* shaper_p)
{
   tmctl_ret_e ret = TMCTL_SUCCESS;
   
#if defined(CHIP_63138) || defined(CHIP_63148) || defined(CHIP_4908) || defined(CHIP_63158)

   int        rc;
   int        unit;
   bcm_port_t port;
   uint32_t portmap = 0;

   tmctl_debug("Enter: ifname=%s qid=%d minRate=%d shapingRate=%d burstSize=%d",
               ifname, qid, shaper_p->minRate, shaper_p->shapingRate,
               shaper_p->shapingBurstSize);

   /* get unit and port number from ifname */
   rc = bcm_enet_map_ifname_to_unit_portmap(ifname, &unit, &portmap);
   if (rc)
   {
      tmctl_error("bcm_enet_map_ifname_to_unit_port returns error %d", rc);
      return TMCTL_ERROR;
   }
   /* TMCTL must apply the same port setting to all the ports in LAG/Trunk group */   
   port = -1; /* Start with -1 to increment with */
   while (portmap)
   {
      while( ! (portmap & (1<<(++port))) );
      portmap &= ~(1<<port); /* Reset the port we are execting below -- important */

      {

         tmctl_debug("Calling bcm_port_rate_egress_set_X: unit=%d port=%d ercLimit=%d ercBurst=%d qid=%d",
                     unit, port, shaper_p->shapingRate, shaper_p->shapingBurstSize /* kb */, qid);
         rc = bcm_port_rate_egress_set_X(unit, port, shaper_p->shapingRate/*Kbps*/,(shaper_p->shapingBurstSize /* Bytes */ * 8)/1000,
                                         qid,
                                         0);    /* byte mode */
         if (rc)
         {
            tmctl_error("bcm_port_rate_egress_set_X returns error %d", rc);
            return TMCTL_ERROR;
         }
      }
   }
   
#endif

   return ret;
   
}  /* End of tmctlEthSw_setQueueShaper() */


/* ----------------------------------------------------------------------------
 * This function gets port TM parameters (capabilities) from Ethernet Switch
 * driver.
 *
 * Parameters:
 *    ifname (IN) Linux interface name.
 *    tmParms_p (OUT) Structure to return port TM parameters.
 *
 * Return:
 *    tmctl_ret_e enum value
 * ----------------------------------------------------------------------------
 */
tmctl_ret_e tmctlEthSw_getPortTmParms(const char*          ifname,
                                      tmctl_portTmParms_t* tmParms_p)
{
   tmctl_ret_e ret = TMCTL_SUCCESS;
   
#if defined(CHIP_63138) || defined(CHIP_63148) || defined(CHIP_4908) || defined(CHIP_63158) 

   int        rc;
   int        unit;
   bcm_port_t port;
   int               weights[BCM_COS_COUNT] = {0};
   port_qos_sched_t  qs;
   
   tmctl_debug("Enter: ifname=%s", ifname);

   /* get unit and port number from ifname */
   rc = bcm_enet_map_ifname_to_unit_port(ifname, &unit, &port);
   if (rc)
   {
      tmctl_error("bcm_enet_map_ifname_to_unit_port returns error %d", rc);
      return TMCTL_ERROR;
   }

   memset(&qs, 0, sizeof(port_qos_sched_t));   
   
   rc = bcm_cosq_sched_get_X(unit, port, &weights[0], &qs);
   if (rc)
   {
      tmctl_error("bcm_cosq_sched_get_X returns error %d", rc);
      return TMCTL_ERROR;
   }

   tmParms_p->schedCaps = 0;

   if (qs.port_qos_caps & QOS_SCHED_SP_CAP)
      tmParms_p->schedCaps |= TMCTL_SP_CAPABLE;
   if (qs.port_qos_caps & QOS_SCHED_WRR_CAP)
      tmParms_p->schedCaps |= TMCTL_WRR_CAPABLE;
   if (qs.port_qos_caps & QOS_SCHED_WDR_CAP)
      tmParms_p->schedCaps |= TMCTL_WDRR_CAPABLE;
   if (qs.port_qos_caps & QOS_SCHED_COMBO)
      tmParms_p->schedCaps |= TMCTL_SP_WRR_CAPABLE;

   tmParms_p->maxQueues   = qs.max_egress_q;
   tmParms_p->maxSpQueues = qs.max_egress_spq;
   
   if (qs.port_qos_caps & QOS_QUEUE_SHAPER_CAP)
      tmParms_p->queueShaper = TRUE;
   else
      tmParms_p->queueShaper = FALSE;
      
   if (qs.port_qos_caps & QOS_PORT_SHAPER_CAP)
      tmParms_p->portShaper = TRUE;
   else
      tmParms_p->portShaper = FALSE;

   tmParms_p->cfgFlags = 0;
   
   tmctl_debug("schedType=0x%x maxQueues=%d maxSpQueues=%d queueShaper=%d portShaper=%d",
               tmParms_p->schedCaps, tmParms_p->maxQueues, tmParms_p->maxSpQueues,
               tmParms_p->queueShaper, tmParms_p->portShaper);
   
#endif

   return ret;
      
}  /* End of tmctlEthSw_getPortTmParms() */


