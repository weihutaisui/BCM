/**********************************************************************************
** <:copyright-BRCM:2017:proprietary:standard
** 
**    Copyright (c) 2017 Broadcom 
**    All Rights Reserved
** 
**  This program is the proprietary software of Broadcom and/or its
**  licensors, and may only be used, duplicated, modified or distributed pursuant
**  to the terms and conditions of a separate, written license agreement executed
**  between you and Broadcom (an "Authorized License").  Except as set forth in
**  an Authorized License, Broadcom grants no license (express or implied), right
**  to use, or waiver of any kind with respect to the Software, and Broadcom
**  expressly reserves all rights in and to the Software and all intellectual
**  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
**  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
**  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
** 
**  Except as expressly set forth in the Authorized License,
** 
**  1. This program, including its structure, sequence and organization,
**     constitutes the valuable trade secrets of Broadcom, and you shall use
**     all reasonable efforts to protect the confidentiality thereof, and to
**     use this information only in connection with your use of Broadcom
**     integrated circuit products.
** 
**  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
**     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
**     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
**     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
**     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
**     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
**     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
**     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
**     PERFORMANCE OF THE SOFTWARE.
** 
**  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
**     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
**     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
**     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
**     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
**     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
**     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
**     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
**     LIMITED REMEDY.
** :>
***********************************************************************************/

/* ---- Include Files ---------------------------------------------------- */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <bcm_local_kernel_include/linux/sockios.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <bcm_local_kernel_include/linux/if_arp.h>
#include <bcmtypes.h>
#include <bcmatmapi.h>
#include <atmapidrv.h>

#include "atmMibHelper.h"
#include "atmMibDefs.h"
#include "ifMib.h"
#include "syscall.h"
#include "ifcdefs.h"
#include "dbapi.h"
#include "cpeDslMibHelper.h"

/* ---- Public Variables ------------------------------------------------- */
/* ---- Private Constants and Types -------------------------------------- */

#define ATMMIB_STATUS_OK         0
#define ATMMIB_STATUS_ERR        1

#if defined(BUILD_SNMP_ATM_MIB) || defined(CPE_DSL_MIB)
/* ---- Private Variables ------------------------------------------------ */
static oid defAtmTrafficDescriptor[ATM_TRAFDESC_OIDLEN]  = { 1, 3, 6, 1, 2, 1, 37, 1, 1, 2 }   ;
ATM_INFO_LIST atmList;

/* ---- Private Function Prototypes -------------------------------------- */
static int     getConfVccCount( void );
static void    trafDescCopyDriverData( ATM_TRAFDESC_TABLE* tableRow, ATM_TRAFFIC_DESCR_PARM_ENTRY* driverTableRow );
static void    trafDescSetDefValues( ATM_TRAFDESC_TABLE* tableRow );
static void    trafDescMapType( ATM_TRAFDESC_TABLE* tableRow, ATM_TRAFFIC_DESCR_PARM_ENTRY* driverTableRow );
static void    vclCopyDriverData( ATM_VCL_TABLE* tableRow, PATM_VCC_ADDR pAddrs, ATM_VCC_CFG* vccCfg );
static void    aal5vccCopyDriverData( AAL5_VCC_TABLE* tableRow, PATM_VCC_ADDR pAddrs, ATM_VCC_STATS* stats );
static void    aal5vccSetDefValues( AAL5_VCC_TABLE* tableRow );
static int     mapAalType( UINT32 driverAalType );

/* ---- Functions -------------------------------------------------------- */
int     getVccAddrs( int numAddrs, PATM_VCC_ADDR pAddrs );
int     getAal5CpcsIfIndex( void );
int     getAtmCellIfIndex( void );
void    vclSetDefValues( ATM_VCL_TABLE* tableRow );

//#define SNMP_DEBUG_FLAG

void atmAddVclToList(ATM_VCL_TABLE* p)
{
#ifndef CPE_DSL_MIB
  if (atmList.head == NULL) {
    atmList.head = p;
    atmList.tail = pIfNew;
  }
  else {
    atmList.tail->next = pIfNew;
    atmList.tail = pIfNew;
  }
#else
  ATM_VCL_TABLE *ptr, *ptrNext, *ptrHead;

  /* add to list, order is sorted by vpi/vci */
  if (atmList.head == NULL) {
    atmList.head = p;
    atmList.tail = p;
  }
  else {
    ptrHead = ptr = atmList.head;
    ptrNext = ptr->next;
    while (ptr != NULL) {
      if ((ptrNext) &&
          ((p->atmVclVpi > ptrNext->atmVclVpi) ||
           ((p->atmVclVpi == ptrNext->atmVclVpi) && (p->atmVclVci > ptrNext->atmVclVci)))) {
        /* move on */
        ptrHead = ptr;
        ptr = ptr->next;
        if (ptr != NULL)
          ptrNext = ptr->next;
        else
          ptrNext = NULL;
      }
      else {
        if ((ptr->atmVclVpi > p->atmVclVpi) ||
            ((ptr->atmVclVpi == p->atmVclVpi) && (ptr->atmVclVci > p->atmVclVci))) {
          p->next = ptr;
          atmList.head = p;
          break;
        } /* head */
        else if ((ptrNext == NULL) && 
                 ((ptr->atmVclVpi < p->atmVclVpi) || 
                  ((ptr->atmVclVpi == p->atmVclVpi) && (ptr->atmVclVci < p->atmVclVci)))) {
          ptr->next = p;
          atmList.tail = p;
          break;
        } /* end */
        else {
          p->next = ptrNext;
          ptr->next = p;
          break;
        } /* middle */
      } 
    } /* while */
  } /* list != NULL */
#endif /* CPE_DSL_MIB */
  atmList.count++;
}

/* Get the number of ATM interfaces */
int atmMibHelper_getAtmIfCount( void )
{
#if ( ATM_INTERFACE_COUNT != 1 )
#error Currently only one ATM interface supported by the ATM MIB
#endif
   return( ATM_INTERFACE_COUNT );
}

int atmMibHelper_getTotalVccCount( void )
{
   return ( getConfVccCount() );
}

/* Get the (first) ATM cell interface index */
int getAtmCellIfIndex( void )
{
   /* This should be the index for the ATM cell layer in the ifTable */
  return( ifGetIfIndexByName(ATM_CELL_LAYER_IFNAME));
}

/* Get the (first) AAL5 CPCS interface index */
int getAal5CpcsIfIndex( void )
{
   /* This should be the index for the AAL5 CPCS layer in the ifTable */
  return (ifGetIfIndexByName(AAL5_CPCS_LAYER_IFNAME));
}

/* Get the number of configured VCCs */
int getConfVccCount( void )
{
   UINT32 ulNumAddrs = 0;
   UINT32 ulInterfaceId = 0;
   UINT32 ulAddrsReturned = 0;
   UINT32 i = 0;

   /* Small portion taken from GetVccAddrsToUse in atmctl.c */
   for( i = 0; i < PHY_NUM_PORTS; i++ )
   {
      ulInterfaceId = (UINT32) i;
      BcmAtm_GetInterfaceId( (UINT8) i, &ulInterfaceId );
      BcmAtm_GetVccAddrs( ulInterfaceId, NULL, 0, &ulAddrsReturned );
      ulNumAddrs += ulAddrsReturned;
   }

   return( ulNumAddrs );
}
#endif /*  #if defined(BUILD_SNMP_ATM_MIB) || defined(CPE_DSL_MIB) */

#ifdef BUILD_SNMP_ATM_MIB
/* Get the number of traffic indexes */
int atmMibHelper_getTrafDescIndexCount( void )
{
   int ulTdtSize = 0;

   BcmAtm_GetTrafficDescrTableSize( (UINT32*)&ulTdtSize );

   return( ulTdtSize );
}


#ifdef ATMIFCONFTBL
/* Fill out the ATM interface configuration table */
void  atmMibHelper_fillAtmIfConfTable( ATM_INTERFACE_CONF_TABLE* pTable )
{
   /* Assuming only one ATM interface (ATM_INTERFACE_COUNT = 1) */

   ATM_INTERFACE_CONF_TABLE* p = pTable;

   p->pTableBeginning = pTable;
   p->ifIndex = getAtmCellIfIndex();

   p->atmInterfaceMaxVpcs                           = ATM_INTF_MAX_VPCS;
   p->atmInterfaceMaxVccs                           = ATM_INTF_MAX_VCCS;
   p->atmInterfaceConfVpcs                          = ATM_INTF_CONF_VPCS;
   p->atmInterfaceConfVccs                          = getConfVccCount();
   p->atmInterfaceMaxActiveVpiBits                  = ATM_INTF_MAX_ACTIVE_VPI_BITS;
   p->atmInterfaceMaxActiveVciBits                  = ATM_INTF_MAX_ACTIVE_VCI_BITS;
   p->atmInterfaceIlmiVpi                           = ATM_INTF_ILMI_VPI;
#ifdef BUILD_SNMP_AUTOCONFIG
   p->atmInterfaceIlmiVci                           = ATM_INTF_ILMI_VCI;
#else
   p->atmInterfaceIlmiVci                           = ATM_INTF_ILMI_VCI_UNSUPPORTED;
#endif
   p->atmInterfaceAddressType                       = ATM_INTF_ADDRTYPE_OTHER;
   cmsUtl_strncpy( p->atmInterfaceAdminAddress, "", strlen("") );
   cmsUtl_strncpy( p->atmInterfaceMyNeighborIpAddress, "", strlen("") );
   cmsUtl_strncpy( p->atmInterfaceMyNeighborIfName, "", strlen("") );
   p->atmInterfaceCurrentMaxVpiBits                 = ATM_INTF_CURRENT_MAX_VPI_BITS;
   p->atmInterfaceCurrentMaxVciBits                 = ATM_INTF_CURRENT_MAX_VCI_BITS;
   cmsUtl_strncpy( p->atmInterfaceSubscrAddress, "", strlen("") );

   p->next = NULL;
}
#endif /* ATMIFCONFTBL */


#ifdef ATMIFTCTBL
/* Fill out the ATM interface TC table */
void  atmMibHelper_fillAtmIfTCTable( ATM_INTERFACE_TC_TABLE* pTable )
{
   /* Assuming only one ATM interface (ATM_INTERFACE_COUNT = 1) */

   ATM_INTERFACE_TC_TABLE* p = pTable;
   p->pTableBeginning = pTable;
   p->ifIndex = getAtmCellIfIndex();

   p->atmInterfaceOCDEvents      = ATM_INTF_TC_OCD             ;
   p->atmInterfaceTCAlarmState   = ATM_INTF_TC_ALMST_NOALARM   ;

   p->next = NULL;
}
#endif /* ATMIFTCTBL */


#ifdef ATMTRAFFDESCTBL
/* Fill out the ATM interface traffic descriptor table */
int   atmMibHelper_fillAtmTrafDescTable( ATM_TRAFDESC_TABLE* pTable )
{
   ATM_TRAFDESC_TABLE* p = pTable;
   int nRet = 0;
   int i;
   UINT32 ulTdtSize = 0;
   int errStatus = ATMMIB_STATUS_ERR;
   ATM_TRAFFIC_DESCR_PARM_ENTRY* pTdt = NULL;
   ATM_TRAFFIC_DESCR_PARM_ENTRY* pTdte = NULL;

   /* Small portion taken from TdteHandler and TdteHandlerShow in atmctl.c */
   BcmAtm_GetTrafficDescrTableSize( &ulTdtSize );
   pTdt = (PATM_TRAFFIC_DESCR_PARM_ENTRY) malloc( ulTdtSize * sizeof(ATM_TRAFFIC_DESCR_PARM_ENTRY) );
   if( pTdt )
   {
      pTdt->ulStructureId = ID_ATM_TRAFFIC_DESCR_PARM_ENTRY;
      nRet = (int) BcmAtm_GetTrafficDescrTable( pTdt, ulTdtSize );
      if( nRet == 0 )
      {
         pTdte = pTdt;
         for ( i = 1; i <= ulTdtSize; i++, pTdte++ )
         {
            p->pTableBeginning = pTable;
            trafDescCopyDriverData( p, pTdte );
            if ( i == ulTdtSize )
            {
               /* the last row */
               p->next = NULL;
               errStatus = ATMMIB_STATUS_OK;
            }
            else
            {
            p->next  = (p+1);
            p++;
         }
         }
      }
      else
      {
         ATMMIBLOG_ERROR(("atmMibHelper_fillAtmTrafDescTable: Could not get traffic description table"));
      }
   }
   else
   {
      ATMMIBLOG_ERROR(("atmMibHelper_fillAtmTrafDescTable: Could not allocate memory for the traffic description table"));
   }

   free( pTdt );

   return( errStatus );
}
#endif /* ATMTRAFFDESCTBL */
#endif /* BUILD_SNMP_ATM_MIB */

#if defined(BUILD_SNMP_ATM_MIB) || defined(CPE_DSL_MIB)
#ifdef ATMVCLTBL
int atmMibHelper_deleteVclTable()
{
  ATM_VCL_TABLE* p;

  p = atmList.head;
     
  if (atmList.count != 0) {
    while (p) {
      free(p);
      p = p->next;
    }
    atmList.head = NULL;
  }
    return 0;
}

/* Fill out the ATM VCL table */
int atmMibHelper_fillAtmVclTable()
{
   /* Assuming only one ATM interface (ATM_INTERFACE_COUNT = 1) */
   int errStatus = ATMMIB_STATUS_ERR;
   int nRet = 0;
   PATM_VCC_ADDR pAddrs = NULL;
   ATM_VCL_TABLE* p;
   int vccCount = 0;
   ATM_VCC_CFG Cfg;
   int i;
   PATM_VCC_ADDR freePointer;

   atmMibHelper_deleteVclTable();

   vccCount = getConfVccCount();
   
   if ( vccCount > 0 )
   {
      /* Allocate memory for an ATM_VCC_ADDR stuct for each configured VCC. */
      pAddrs = (PATM_VCC_ADDR)malloc( vccCount * (sizeof(ATM_VCC_ADDR) + sizeof(UINT8)) );
      freePointer = pAddrs;
      if ( pAddrs )
      {
        if ( getVccAddrs( vccCount, pAddrs ) == 0 ) {
          for ( i = 1; i <= vccCount; i++, pAddrs++ ) {
            memset( &Cfg, 0x00, sizeof(Cfg) );
            Cfg.ulStructureId = ID_ATM_VCC_CFG;
            
            nRet = (int) BcmAtm_GetVccCfg( pAddrs, &Cfg );
            if ( nRet == 0 )
              {
                p = (ATM_VCL_TABLE*)calloc(1, sizeof(ATM_VCL_TABLE));
                p->pTableBeginning = p;
                p->next = NULL;
                p->ifIndex = getAtmCellIfIndex();
                vclCopyDriverData( p, pAddrs, &Cfg );
                atmAddVclToList(p);
                errStatus = ATMMIB_STATUS_OK;
              } /* if */
            else
              {
                ATMMIBLOG_ERROR(("atmMibHelper_fillAtmVclTable: Could not get VCC configuration"));
                break;
              }
          } /* for all vccs */
        } /* if getVccAddrs */
        else
          {
            ATMMIBLOG_ERROR(("atmMibHelper_fillAtmVclTable: Could not get VCC address"));
          }
        free( freePointer );
      } /* if pAddrs */
      else
        {
          ATMMIBLOG_ERROR(("atmMibHelper_fillAtmVclTable: Could not allocate memory"));
        }
   } /* vccCount */
#ifdef SNMP_DEBUG_FLAG
   if ((snmpDebug.mib & SNMP_DEBUG_LEVEL_MIB_ATM) == SNMP_DEBUG_LEVEL_MIB_ATM) {
     p = atmList.head;
     
     if (atmList.count != 0) {
       printf("************---atmList.count %d, head %x, p %x, next %x---************\n",
              atmList.count,(int)atmList.head, (int)p, (int)p->next);
       while (p) {
         printf("vpi/vci %d/%d\n",p->atmVclVpi,p->atmVclVci);
         p = p->next;
       }
     }
   } /* debug enable */
#endif
   return( errStatus );
}

int vclEntryExists(int vpi, int vci) {
  ATM_VCL_TABLE* p;

  if ( atmMibHelper_fillAtmVclTable() != 0 ) { 
    return 0;
  }
  p = atmList.head;
  if (atmList.count != 0) {
    while (p) {
      if ((p->atmVclVpi == vpi) && (p->atmVclVci == vci))
        return 1;
      p = p->next;
    }
  }
  return 0;
}


#endif /* ATMVCLTBL */
#endif /* defined(BUILD_SNMP_ATM_MIB) || defined(CPE_DSL_MIB) */

#ifdef BUILD_SNMP_ATM_MIB
#ifdef AAL5VCCTBL
/* Fill out the AAL5 VCC table */
int atmMibHelper_fillAal5VccTable( AAL5_VCC_TABLE* pTable )
{
   /* Assuming only one AAL5 CPCS interface */

   int errStatus = ATMMIB_STATUS_ERR;
   int nRet = 0;
   PATM_VCC_ADDR pAddrs = NULL;
   AAL5_VCC_TABLE* p = pTable;
   int vccCount = 0;
   ATM_VCC_STATS Stats;
   int i;
   PATM_VCC_ADDR freePointer;


   vccCount = getConfVccCount();

   if ( vccCount > 0 )
   {
      /* Allocate memory for an ATM_VCC_ADDR stuct for each configured VCC. */
      pAddrs = (PATM_VCC_ADDR)malloc( vccCount * (sizeof(ATM_VCC_ADDR) + sizeof(UINT8)) );
      freePointer = pAddrs;
      if ( pAddrs )
      {
         if ( getVccAddrs( vccCount, pAddrs ) == 0 )
         {
            for ( i = 1; i <= vccCount; i++, pAddrs++ )
            {
               memset( &Stats, 0x00, sizeof(Stats) );
               Stats.ulStructureId = ID_ATM_VCC_STATS;

               nRet = (int) BcmAtm_GetVccStatistics( pAddrs, &Stats, 0 );
               if ( nRet == 0 )
               {
                  p->pTableBeginning = pTable;
                  p->aal5ifIndex = getAal5CpcsIfIndex();
		            aal5vccCopyDriverData( p, pAddrs, &Stats );
                  if ( i == vccCount )
                  {
                     /* The last row */
                     p->next = NULL;
                     errStatus = ATMMIB_STATUS_OK;
                  }
                  else
                  {
                  p->next = (p+1);
                  p++;
               }
               }
               else
               {
                  ATMMIBLOG_ERROR(("atmMibHelper_fillAal5VccTable: Could not get AAL5 VCC statistics"));
                  break;
               }
            }
         }
         else
         {
            ATMMIBLOG_ERROR(("atmMibHelper_fillAtmVclTable: Could not get VCC address"));
      }

      free( freePointer );
   }
   else
   {
         ATMMIBLOG_ERROR(("atmMibHelper_fillAtmVclTable: Could not allocate memory"));
      }
   }

   return( errStatus );
}
#endif /* AAL5VCCTBL */



#ifdef ATMTRAFFDESCTBL
/* Map the traffic descriptor type from a value provided by the ATM driver (0-15)
** to a oid value for the traffic descriptor type (e.g. { 1, 3, 6, 1, 2, 1, 37, 1, 1, 2 } ) */
void trafDescMapType( ATM_TRAFDESC_TABLE* tableRow, ATM_TRAFFIC_DESCR_PARM_ENTRY* driverTableRow )
{
   /* Set the default traffic descriptor */
   memcpy(  tableRow->atmTrafficDescrType, defAtmTrafficDescriptor, sizeof(defAtmTrafficDescriptor) );

   /* The ATM driver returns values from 1 to 15 for the traffic descriptor type.
   ** Modify only the last entry in the oid array. */
   tableRow->atmTrafficDescrType[ATM_TRAFDESC_OIDLEN-1] = driverTableRow->ulTrafficDescrType;
}

/* Copy the data from the driver table to the ATM traffic descriptor table */
void trafDescCopyDriverData( ATM_TRAFDESC_TABLE* tableRow, ATM_TRAFFIC_DESCR_PARM_ENTRY* driverTableRow )
{
   /* First, set the default values */
   trafDescSetDefValues( tableRow );

   /* Change only the values that can be obtained from the ATM driver.
   ** Don't change the other values (i.e. keep the defaults) */
   tableRow->atmTrafficDescrParamIndex = driverTableRow->ulTrafficDescrIndex;
   trafDescMapType( tableRow, driverTableRow );
   tableRow->atmTrafficDescrParam1   = driverTableRow->ulTrafficDescrParm1;
   tableRow->atmTrafficDescrParam2   = driverTableRow->ulTrafficDescrParm2;
   tableRow->atmTrafficDescrParam3   = driverTableRow->ulTrafficDescrParm3;
   tableRow->atmTrafficDescrParam4   = driverTableRow->ulTrafficDescrParm4;
   tableRow->atmTrafficDescrParam5   = driverTableRow->ulTrafficDescrParm5;
   tableRow->atmServiceCategory      = driverTableRow->ulServiceCategory;
   tableRow->atmTrafficFrameDiscard  = driverTableRow->ulTrafficFrameDiscard;
}

/* Set the default values for the ATM traffic descriptor table */
void trafDescSetDefValues( ATM_TRAFDESC_TABLE* tableRow )
{
   memcpy(  tableRow->atmTrafficDescrType, defAtmTrafficDescriptor, sizeof(defAtmTrafficDescriptor) );
   tableRow->atmTrafficDescrParam1   = ATM_TRAFDESC_PARAM;
   tableRow->atmTrafficDescrParam2   = ATM_TRAFDESC_PARAM;
   tableRow->atmTrafficDescrParam3   = ATM_TRAFDESC_PARAM;
   tableRow->atmTrafficDescrParam4   = ATM_TRAFDESC_PARAM;
   tableRow->atmTrafficDescrParam5   = ATM_TRAFDESC_PARAM;
   tableRow->atmTrafficQoSClass      = 0;
   tableRow->atmTrafficDescrRowStatus= ATM_TRAFDESC_ROWSTATUS_ACTIVE;
   tableRow->atmServiceCategory      = ATM_TRAFDESC_SERVICE_CAT_UBR;
   tableRow->atmTrafficFrameDiscard = 1;
}
#endif /* ATMTRAFFDESCTBL */

#ifdef AAL5VCCTBL
/* Copy the driver data to the user structure for the AAL5 VCC table */
void aal5vccCopyDriverData( AAL5_VCC_TABLE* tableRow, PATM_VCC_ADDR pAddrs, ATM_VCC_STATS* stats )
{
	/* First, set the default values */
	aal5vccSetDefValues( tableRow );

   /* Change only the values that can be obtained from the ATM driver.
   ** Keep the defaults for the other values */
   tableRow->aal5VccVpi                      = pAddrs->usVpi;
   tableRow->aal5VccVci                      = pAddrs->usVci;
   tableRow->aal5VccCrcErrors                = stats->u.AtmVccAal5Stats.ulAal5VccCrcErrors;
   tableRow->aal5VccOverSizedSDUs            = stats->u.AtmVccAal5Stats.ulAal5VccOverSizedSdus;
   tableRow->aal5VccSarTimeOuts              = stats->u.AtmVccAal5Stats.ulAal5VccSarTimeOuts;
}

/* Set the default values for the AAL5 VCC table */
void aal5vccSetDefValues( AAL5_VCC_TABLE* tableRow )
{
   tableRow->aal5VccCrcErrors       = 0;
   tableRow->aal5VccOverSizedSDUs   = 0;
   tableRow->aal5VccSarTimeOuts     = 0;
}

#endif /* AAL5VCCTBL */

#endif /* BUILD_SNMP_ATM_MIB */

#if defined(BUILD_SNMP_ATM_MIB) || defined(CPE_DSL_MIB)
int getVccAddrs( int numAddrs, PATM_VCC_ADDR pAddrs )
{
   int nRet = 0;
   UINT8 *pucPorts = NULL;
   UINT32 ulInterfaceId = 0;
   UINT32 ulAddrsReturned = 0;
   UINT32 i = 0;

   /* Get the addresses of all configured VCCs. */
   if( pAddrs )
   {
      UINT32 ulAddrsIndex = 0;

      pucPorts = (UINT8 *) (pAddrs + numAddrs);
      for( i = 0; i < PHY_NUM_PORTS; i++ )
      {
         ulInterfaceId = (UINT32) i;
         BcmAtm_GetInterfaceId( (UINT8) i, &ulInterfaceId );
         if( BcmAtm_GetVccAddrs(ulInterfaceId, pAddrs + ulAddrsIndex,
         numAddrs - ulAddrsIndex, &ulAddrsReturned) == STS_SUCCESS)
         {
            memset( pucPorts + ulAddrsIndex, i, ulAddrsReturned );
            ulAddrsIndex += ulAddrsReturned;
         }
      }
   }
   else
   {
      nRet = 1;
   }

   return( nRet );
}


#if defined(ATMVCLTBL) || defined(CPE_DSL_MIB)
/* Copy the driver data to the user structure for the ATM VCL table */
void vclCopyDriverData( ATM_VCL_TABLE* tableRow, PATM_VCC_ADDR pAddrs, ATM_VCC_CFG* vccCfg )
{
  /* First, set the default values */
  vclSetDefValues( tableRow );

  /* Change only the values that can be obtained from the ATM driver.
  ** Don't change the other values (i.e. keep the defaults) */
  tableRow->atmVclVpi = pAddrs->usVpi;
  tableRow->atmVclVci = pAddrs->usVci;
  tableRow->atmVclAdminStatus = vccCfg->ulAtmVclAdminStatus;
  tableRow->atmVclOperStatus  = vccCfg->ulAtmVclOperStatus;
  tableRow->atmVclLastChange  = vccCfg->ulAtmVclLastChange;
  tableRow->atmVccAalType = mapAalType( vccCfg->ulAalType );
  tableRow->atmVclTransmitTrafficDescrIndex = vccCfg->ulAtmVclTransmitTrafficDescrIndex;
  tableRow->atmVclReceiveTrafficDescrIndex  = tableRow->atmVclTransmitTrafficDescrIndex;
  tableRow->atmVccAal5EncapsType = vccCfg->u.Aal5Cfg.ulAtmVccEncapsType;
}

/* Set the default values for the ATM VCL table */
void vclSetDefValues( ATM_VCL_TABLE* tableRow )
{
   tableRow->atmVccAalType = ATM_VCL_AALTYPE_AAL5;
   tableRow->atmVccAal5CpcsTransmitSduSize = ATM_VCL_SDUSIZE;
   tableRow->atmVccAal5CpcsReceiveSduSize = ATM_VCL_SDUSIZE;
   tableRow->atmVccAalType = ATM_VCL_AALTYPE_AAL5;
   tableRow->atmVclCrossConnectIdentifier    = 0;
   tableRow->atmVclRowStatus = ATM_VCL_ROWSTATUS_ACTIVE;
   tableRow->atmVclCastType = ATM_VCL_CAST_TYPE_P2P;
   tableRow->atmVclConnKind = ATM_VCL_CONN_KIND_PVC;
}

/* Map the AAL type provided by the driver to a value understood by the MIB
** (used for the ATM VCL table) */
int mapAalType( UINT32 driverAalType )
{
  int aalType = ATM_VCL_AALTYPE_AAL5;

  switch ( driverAalType )
    {
    case AAL_2:
      aalType = ATM_VCL_AALTYPE_AAL2;
      break;
    case AAL_5:
      aalType = ATM_VCL_AALTYPE_AAL5;
      break;
    default:
      aalType = ATM_VCL_AALTYPE_UNKNOWN;
      break;
    }
  return( aalType );
}
#endif /* defined(ATMVCLTBL) || defined(CPE_DSL_MIB) */

#ifdef CPE_DSL_MIB
extern pCPE_PVC_ENTRY pPvcParam;
extern pWAN_INFO_LIST pWanList;
/* for all the following, return 0 if no error, 1 if entry not found, and
   -1 if error setting */
int vclSetAdminStatus(int port, int vpi, int vci, int value)
{
  UINT32 admin = 0;
  int error = 0;
  char *state[] = {"disable", "enable"};
  char cmd[SYS_CMD_LEN];
  FILE* fs = NULL;
  int vccId = -1;
  IFC_ATM_VCC_INFO vccInfo;

  if (value == ATM_ADMIN_UP)
    admin = 1;

  if ((vccId = BcmDb_getVccInfoByVpiVci(port, vpi,vci,&vccInfo)) != -1) {
    if (vccInfo.adminStatus == admin)
      return 0;
  }
  else
    return 1;

  sprintf(cmd, "atmctl vcc --state %d.%d.%d %s 2> /var/vclerr",
          port,vpi,vci,state[admin]);
  bcmSystem(cmd);
  // check to see if there's any error
  fs = fopen("/var/vclerr", "r");
  if (fs != NULL) {
    error = -1;
    bcmSystemMute("cat /var/vclerr");
    fclose(fs);
    bcmSystemMute("rm /var/vclerr");
  }
  if (error != -1) {
    /* save to PSI  */
    vccInfo.adminStatus = admin;
    BcmDb_setVccInfo(vccId,&vccInfo);
  }

  return error;
}

// delete upper protocol before deleting the VCC
int detachPvc(int vpi, int vci)
{
    pCPE_PVC_ENTRY pEntry;
    WAN_CON_ID wanId;
    char cmd[SYS_CMD_LEN];
    //char name[IFC_TINY_LEN];

    cpeScanWanInfo();
    pEntry = pWanList->head;
    while (pEntry) {
      if ((pEntry->vpi == vpi) && (pEntry->vci == vci)) 
        break;
      else
        pEntry = pEntry->next;
    }
    if (pEntry == NULL) {
      return 0;
    }
    wanId.vpi = pEntry->vpi;
    wanId.vci = pEntry->vci;
    wanId.conId = pEntry->conId;
    BcmWan_DeleteDataDirectory(&wanId);
    BcmDb_deleteWanInfo(&wanId,0);
    BcmDb_removeIpSrvInfo(&wanId);
    sprintf(cmd, "pvc2684ctl -d -v 0.%d.%d",wanId.vpi, wanId.vci);
    bcmSystemMute(cmd);
    return 0;
}

int vclDeleteEntry(int port, int vpi, int vci)
{
  char cmd[SYS_CMD_LEN];

  detachPvc(vpi,vci);
  BcmDb_removeVcc(port,vpi,vci);
  sprintf(cmd, "atmctl operate vcc --delete %lu.%d.%d",port,vpi,vci);
  bcmSystemMute(cmd);

  return 0;
}

int tdExist(int index)
{
  PATM_TRAFFIC_DESCR_PARM_ENTRY pTd = NULL, ptr;
  UINT32 tdTableSize = 0;
  int ret, i;

  /* get TD table from driver */
  BcmAtm_GetTrafficDescrTableSize(&tdTableSize);
  pTd = (PATM_TRAFFIC_DESCR_PARM_ENTRY)(malloc((tdTableSize) *
                                               sizeof(ATM_TRAFFIC_DESCR_PARM_ENTRY)));
  if (pTd == NULL)
    return 0;
  pTd->ulStructureId = ID_ATM_TRAFFIC_DESCR_PARM_ENTRY;
  ret = (int) BcmAtm_GetTrafficDescrTable(pTd,tdTableSize );
  if (ret == 0) {
    for (i = 0, ptr = pTd; i < tdTableSize; i++, ptr++) {
      if (pTd->ulTrafficDescrIndex == index)
        break;
    }
    if (i == tdTableSize)
      return 0;
  }
  return 1;
}


// create a bridge pvc whne a vcc is created
int createBridgePvc(int vpi, int vci)
{
    pCPE_PVC_ENTRY pEntry;

	if (pPvcParam == NULL) 
	  if ((pPvcParam = malloc(sizeof(CPE_PVC_ENTRY))) == NULL)
	    return -1;

    cpeScanWanInfo();
    pEntry = pWanList->head;
    while (pEntry) {
      if ((pEntry->vpi == vpi) && (pEntry->vci == vci)) 
        break;
      else
        pEntry = pEntry->next;
    }
    if (pEntry == NULL) {
      if (cpeAddWanInfo(vpi,vci) != 0)
        printf("cpeAddWanInfo failed.\n");

      /* scan again to see if an entry is created successfully */
      cpeScanWanInfo();
      pEntry = pWanList->head;
      while (pEntry) {
        if ((pEntry->vpi == vpi) && (pEntry->vci == vci)) 
          break;
        else
          pEntry = pEntry->next;
      }
      if (pEntry == NULL) {
	  if(pPvcParam){
        free(pPvcParam);
            pPvcParam=NULL;
	  }
	      return -1;   
      }
      memcpy(pPvcParam, pEntry, sizeof(CPE_PVC_ENTRY));
      cpeConfigWanInfo();
    }
#if 0
    memcpy(pPvcParam, pEntry, sizeof(CPE_PVC_ENTRY));

    pPvcParam->vpi = vpi;
    pPvcParam->vci = vci;
    pPvcParam->adminStatus = CPE_ADMIN_ENABLE;
    pPvcParam->natMode = CPE_ADMIN_DISABLE;
    pPvcParam->bridgeMode = CPE_ADMIN_ENABLE;
    pPvcParam->igmpMode = CPE_ADMIN_ENABLE;
    pPvcParam->dhcpcMode = CPE_ADMIN_DISABLE;
    cpeConfigWanInfo();
#endif
    return 0;
}

int vclAddEntry(ATM_VCL_TABLE* tableRow)
{
  int tdId;
  IFC_ATM_VCC_INFO vccInfo;
  /* first of all, check to see if the entry has already exists, do not
     add if it does */

  memset(&vccInfo, 0, sizeof(vccInfo));

  if (vclEntryExists(tableRow->atmVclVpi, tableRow->atmVclVci) == 1)
    return 1;
  // make sure the tdte index is available in the tdte table 
  tdId = tableRow->atmVclReceiveTrafficDescrIndex;

  /* check to see if there has already been such tdte index established */
  if (tdExist(tdId) == 0) {
    return -1;
  }

  /* do atmctl which also checks for vcc existence*/
  /* atmctl vcc -add port.vpi.vci <aal5|aal2|aal0pkt|aal0cell|aaltransparent> <tdte_index>
     <vcmux_routed|vcmux_bridged8023|llcencaps|other|unknown>] */
  switch (tableRow->atmVccAalType)
    {
    case ATM_VCL_AALTYPE_AAL5:
      vccInfo.aalType = AAL_5;
      break;
    case ATM_VCL_AALTYPE_AAL2:
      vccInfo.aalType = AAL_2;
      break;
    default:
      return -1;
    }
  switch (tableRow->atmVccAal5EncapsType)
    {
    case ATM_VCC_ENCAP_ROUTED:
      vccInfo.encapMode = ET_VC_MULTIPLEX_ROUTED_PROTOCOL;
      break;
    case ATM_VCC_ENCAP_BR_8023:
      vccInfo.encapMode = ET_VC_MULTIPLEX_BRG_PROTOCOL_8023;
      break;
    case ATM_VCC_ENCAP_LLC:
      vccInfo.encapMode = ET_LLC_ENCAPSULATION;
      break;
    case ATM_VCC_ENCAP_OTHER:
      vccInfo.encapMode = ET_OTHER;
      break;
    case ATM_VCC_ENCAP_UNKNOWN:
      vccInfo.encapMode = ET_UNKNOWN;
      break;
    default:
      return -1;
    }
  vccInfo.tdId = tdId;
  vccInfo.adminStatus = tableRow->atmVclAdminStatus;
  vccInfo.enblQos = 0;
  vccInfo.vccAddr.ulInterfaceId = ATM_LOGICAL_PORT_NUM;
  vccInfo.vccAddr.usVpi = tableRow->atmVclVpi;
  vccInfo.vccAddr.usVci = tableRow->atmVclVci;

  BcmDb_addVcc(&vccInfo);
  if (createBridgePvc(vccInfo.vccAddr.usVpi, vccInfo.vccAddr.usVci) != 0)
      printf("createBridgePvc failed\n");
  else
  {
      printf("createBridgePvc Ok\n");
      return 0;
  }

  return -1;
}

int vclSetTdIndex(int port, int vpi, int vci, int value)
{
  IFC_ATM_VCC_INFO vccInfo;
  int vccId = -1;
  ATM_VCC_ADDR addr;
  ATM_VCC_CFG atmcfg;
  int ret;

  if ((vccId = BcmDb_getVccInfoByVpiVci(port, vpi,vci,&vccInfo)) != -1) {
    if (value == vccInfo.tdId)
      return 0;
  }
  else
    return -1;

  /* check to see if there has already been such tdte index established */
  if (tdExist(value) == 0)
    return -1;

  /*  call driver to change the vcc's tdId */
  memset(&atmcfg, 0, sizeof(atmcfg));
  atmcfg.ulStructureId = ID_ATM_VCC_CFG;
  addr.ulInterfaceId = port;
  addr.usVpi = vpi;
  addr.usVci = vci;

  /* Read the current vcc configuration. */
  ret = (int) BcmAtm_GetVccCfg( &addr, &atmcfg );

  if(ret == 0) {
    atmcfg.ulAtmVclReceiveTrafficDescrIndex = value;
    atmcfg.ulAtmVclTransmitTrafficDescrIndex = value;
    ret = (int)BcmAtm_SetVccCfg(&addr,&atmcfg );
  }

  if (ret != 0) {
    /* save to PSI  */
    vccInfo.tdId = value;
    BcmDb_setVccInfo(vccId,&vccInfo);
  }
  return ret;
}

int vclSetAalType(int port, int vpi, int vci, int value)
{
  IFC_ATM_VCC_INFO vccInfo;
  int ret = 0;
  int vccId = -1;
  ATM_VCC_ADDR addr;
  ATM_VCC_CFG atmcfg;

  if ((vccId = BcmDb_getVccInfoByVpiVci(port, vpi,vci,&vccInfo)) != -1) {
    // for snmp aal5 defines  as 3, and for bcm, as 7.
    if (vccInfo.aalType == 7 && value == 3)
      return 0;
    if (value == vccInfo.aalType)
      return 0;
  }
  else
    return 1;

  /*  call driver to change the vcc's tdId */
  memset(&atmcfg, 0, sizeof(atmcfg));
  atmcfg.ulStructureId = ID_ATM_VCC_CFG;
  addr.ulInterfaceId = port;
  addr.usVpi = vpi;
  addr.usVci = vci;

  /* Read the current vcc configuration. */
  ret = (int) BcmAtm_GetVccCfg( &addr, &atmcfg );
  if(ret == 0) {
    atmcfg.ulAalType = value;

    ret = (int)BcmAtm_SetVccCfg(&addr,&atmcfg );
  }
  if (ret != 0) {
    /* save to PSI  */
    vccInfo.aalType = value;
    BcmDb_setVccInfo(vccId,&vccInfo);
  }
  return ret;
}

int vclSetAal5EncapType(int port, int vpi, int vci, int value)
{
  int ret = -1;
  IFC_ATM_VCC_INFO vccInfo;
  int vccId = -1;
  ATM_VCC_ADDR addr;
  ATM_VCC_CFG atmcfg;
  WAN_CON_ID wanId;
  WAN_CON_INFO wanInfo;  
  if ((vccId = BcmDb_getVccInfoByVpiVci(port, vpi,vci,&vccInfo)) != -1) 
  {
    if (value == vccInfo.encapMode)
	    {
    return 0;
  }
  }
  else
  {
  	printf("\r\n BcmDb_getVccInfoByVpiVci vccId=%d \r\n",vccId);
    return 1;
  }
  /*  call driver to change the vcc's tdId */
  memset(&atmcfg, 0, sizeof(atmcfg));
  atmcfg.ulStructureId = ID_ATM_VCC_CFG;
  addr.ulInterfaceId = port;
  addr.usVpi = vpi;
  addr.usVci = vci;


  /* Read the current vcc configuration. */
  ret = (int) BcmAtm_GetVccCfg( &addr, &atmcfg );
  if(ret == 0) {
    atmcfg.u.Aal5Cfg.ulAtmVccEncapsType = value;
    ret = (int)BcmAtm_SetVccCfg(&addr,&atmcfg );
  }

    if(0 == ret)
    {
    /* save to PSI  */
    vccInfo.encapMode = value;
        ret = BcmDb_setVccInfo(vccId,&vccInfo);

        wanId.vpi = vpi;
        wanId.vci = vci;
        wanId.conId = 1;
        BcmDb_getWanInfo(&wanId,&wanInfo);
        if(wanInfo.protocol== PROTO_PPPOA)
        {
           if(value == ET_LLC_ENCAPSULATION)
                wanInfo.encapMode  = 1;
           else if(value == ET_VC_MULTIPLEX_ROUTED_PROTOCOL)
                wanInfo.encapMode = 0;
	    else if(value == ET_VC_MULTIPLEX_BRG_PROTOCOL_8023)
		  wanInfo.encapMode = 0;
           else
                wanInfo.encapMode  = 0;
  }
        else
        {
            if(value == ET_LLC_ENCAPSULATION)
                wanInfo.encapMode  = 0;
            else if(value == ET_VC_MULTIPLEX_ROUTED_PROTOCOL)
                wanInfo.encapMode  = 1;
	     else if(value ==ET_VC_MULTIPLEX_BRG_PROTOCOL_8023)
		  wanInfo.encapMode = 1;
            else
                wanInfo.encapMode  = 1;
        }
        BcmDb_setWanInfo(&wanId,&wanInfo);
}

  return ret;
}
#endif /* CPE_DSL_MIB */

#endif /*  defined(BUILD_SNMP_ATM_MIB) || defined(CPE_DSL_MIB) */ 

int getAtmIntfOperStatus(void)
{
  ATM_INTERFACE_CFG Cfg;
  UINT32 ulInterfaceId;
  int port = 0;

  BcmAtm_GetInterfaceId( (UINT8) port, &ulInterfaceId );
  Cfg.ulStructureId = ID_ATM_INTERFACE_CFG;

  if (BcmAtm_GetInterfaceCfg( ulInterfaceId, &Cfg ) == STS_SUCCESS) 
    {
      if (Cfg.ulIfOperStatus == OPRSTS_UP)
	return IF_OPER_STATUS_UP;
      else 
	return IF_OPER_STATUS_DOWN;
    }
  return IF_OPER_STATUS_UNKNOWN;
}

int getAtmIntfAdminStatus(void)
{
  ATM_INTERFACE_CFG Cfg;
  UINT32 ulInterfaceId;
  int port = 0;

  BcmAtm_GetInterfaceId( (unsigned char) port, &ulInterfaceId );
  Cfg.ulStructureId = ID_ATM_INTERFACE_CFG;

  if (BcmAtm_GetInterfaceCfg( ulInterfaceId, &Cfg ) == STS_SUCCESS) 
    return (Cfg.ulIfAdminStatus);

  return IF_ADMIN_STATUS_DOWN;
}

int setAtmIntfAdminStatus(int status)
{
  ATM_INTERFACE_CFG Cfg;
  UINT32 ulInterfaceId;
  int port = 0;
  int ret = -1;

  Cfg.ulStructureId = ID_ATM_INTERFACE_CFG;
  ret = BcmAtm_GetInterfaceId( (UINT8) port, &ulInterfaceId );
  if (ret == 0) 
    {
      /* Change the state to the specified value. */
      Cfg.ulIfAdminStatus = status; 
      ret = (int)BcmAtm_SetInterfaceCfg(ulInterfaceId, &Cfg );
      if (ret != 0)
	ret = -1;
    }
  return ret;
}
