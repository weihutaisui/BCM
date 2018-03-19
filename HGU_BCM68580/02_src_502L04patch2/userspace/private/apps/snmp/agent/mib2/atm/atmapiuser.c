/***************************************************************************
 * Broadcom Corp. Confidential
 * Copyright 2001 Broadcom Corp. All Rights Reserved.
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED 
 * SOFTWARE LICENSE AGREEMENT BETWEEN THE USER AND BROADCOM. 
 * YOU HAVE NO RIGHT TO USE OR EXPLOIT THIS MATERIAL EXCEPT 
 * SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************
 * File Name  : AtmApiUser.c
 *
 * Description: This file contains the implementation of the user mode
 *              ATM API library.
 *
 * Updates    : 08/27/2001  lat.  Created.
 ***************************************************************************/

/* Broadcom types. */
typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned long UINT32;


/* Includes. */
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>      /* open */ 
#include <unistd.h>     /* exit */
#include <sys/ioctl.h>  /* ioctl */
#include "atmapidrv.h"


/* Globals. */
int g_nAtmFd = -1;


/***************************************************************************
 * Function Name: OpenBcmAtm
 * Description  : Opens the bcmatm device.
 * Returns      : device handle if successsful or -1 if error
 ***************************************************************************/
static int OpenBcmAtm( void )
{
    int nFd = open( "/dev/bcmatm0", O_RDWR );

    if( nFd == -1 )
        printf( "BcmAtm_Initialize: open error %d\n", errno );

    return( nFd );
} /* OpenBcmAtm */

#if defined(BUILD_SNMP_ATM_MIB) || defined(CPE_DSL_MIB)
/***************************************************************************
 * Function Name: BcmAtm_Initialize
 * Description  : Initializes the object.
 * Returns      : STS_SUCCESS if successful or error status.
 ***************************************************************************/
BCMATM_STATUS BcmAtm_Initialize( PATM_INITIALIZATION_PARMS pInitParms )
{
    ATMDRV_INITIALIZE Arg = {pInitParms, STS_ERROR};

    if( g_nAtmFd == -1  )
        g_nAtmFd = OpenBcmAtm();

    if( g_nAtmFd != -1  )
        ioctl( g_nAtmFd, ATMIOCTL_INITIALIZE, &Arg );
    else
        Arg.baStatus = STS_STATE_ERROR;

    return( Arg.baStatus );
} // BcmAtm_Initialize


/***************************************************************************
 * Function Name: BcmAtm_Uninitialize
 * Description  : Clean up resources allocated during BcmAtm_Initialize.
 * Returns      : STS_SUCCESS if successful or error status.
 ***************************************************************************/
BCMATM_STATUS BcmAtm_Uninitialize( void )
{
    ATMDRV_STATUS_ONLY Arg = {STS_ERROR};

    if( g_nAtmFd == -1  )
        g_nAtmFd = OpenBcmAtm();

    if( g_nAtmFd != -1  )
    {
        ioctl( g_nAtmFd, ATMIOCTL_UNINITIALIZE, &Arg );
        close( g_nAtmFd );
        g_nAtmFd = -1;
    }
    else
        Arg.baStatus = STS_STATE_ERROR;

    return( Arg.baStatus );
} // BcmAtm_Uninitialize
#endif /* BUILD_SNMP_ATM_MIB || defined(CPE_DSL_MIB) */

/***************************************************************************
 * Function Name: BcmAtm_GetInterfaceId
 * Description  : Returns the interface id for the specified ATM port.
 * Returns      : STS_SUCCESS if successful or error status.
 ***************************************************************************/
BCMATM_STATUS BcmAtm_GetInterfaceId( UINT8 ucPhyPort,
    UINT32 *pulInterfaceId )
{
    ATMDRV_INTERFACE_ID Arg = {ucPhyPort,{0,0,0}, 0, STS_ERROR};

    if( g_nAtmFd == -1  )
        g_nAtmFd = OpenBcmAtm();

    if( g_nAtmFd != -1  )
    {
        Arg.ulInterfaceId = *pulInterfaceId;
        ioctl( g_nAtmFd, ATMIOCTL_GET_INTERFACE_ID, &Arg );
        *pulInterfaceId = Arg.ulInterfaceId;
    }
    else
        Arg.baStatus = STS_STATE_ERROR;

    return( Arg.baStatus );
} // BcmAtm_GetInterfaceId


/***************************************************************************
 * Function Name: BcmAtm_GetTrafficDescrTableSize
 * Description  : Returns the number of entries in the Traffic Descriptor
 *                Table.
 * Returns      : STS_SUCCESS if successful or error status.
 ***************************************************************************/
BCMATM_STATUS BcmAtm_GetTrafficDescrTableSize(
    UINT32 *pulTrafficDescrTableSize )
{
    ATMDRV_TRAFFIC_DESCR_TABLE_SIZE Arg = {0, STS_ERROR};

    if( g_nAtmFd == -1  )
        g_nAtmFd = OpenBcmAtm();

    if( g_nAtmFd != -1  )
    {
        ioctl( g_nAtmFd, ATMIOCTL_GET_TRAFFIC_DESCR_TABLE_SIZE, &Arg );
        *pulTrafficDescrTableSize = Arg.ulTrafficDescrTableSize;
    }
    else
        Arg.baStatus = STS_STATE_ERROR;

    return( Arg.baStatus );
} // BcmAtm_GetTrafficDescrTableSize


/***************************************************************************
 * Function Name: BcmAtm_GetTrafficDescrTable
 * Description  : Returns the Traffic Descriptor Table.
 * Returns      : STS_SUCCESS if successful or error status.
 ***************************************************************************/
BCMATM_STATUS BcmAtm_GetTrafficDescrTable(
    PATM_TRAFFIC_DESCR_PARM_ENTRY pTrafficDescrTable,
    UINT32 ulTrafficDescrTableSize )
{
    ATMDRV_TRAFFIC_DESCR_TABLE Arg = 
        {pTrafficDescrTable, ulTrafficDescrTableSize, STS_ERROR};

    if( g_nAtmFd == -1  )
        g_nAtmFd = OpenBcmAtm();

    if( g_nAtmFd != -1  )
        ioctl( g_nAtmFd, ATMIOCTL_GET_TRAFFIC_DESCR_TABLE, &Arg );
    else
        Arg.baStatus = STS_STATE_ERROR;

    return( Arg.baStatus );
} // BcmAtm_GetTrafficDescrTable

#ifdef BUILD_SNMP_ATM_MIB
/***************************************************************************
 * Function Name: BcmAtm_SetTrafficDescrTable
 * Description  : Saves the supplied Traffic Descriptor Table to a private
 *                data member.
 * Returns      : STS_SUCCESS if successful or error status.
 ***************************************************************************/
BCMATM_STATUS BcmAtm_SetTrafficDescrTable(
    PATM_TRAFFIC_DESCR_PARM_ENTRY pTrafficDescrTable,
    UINT32 ulTrafficDescrTableSize )
{
    ATMDRV_TRAFFIC_DESCR_TABLE Arg = 
        {pTrafficDescrTable, ulTrafficDescrTableSize, STS_ERROR};

    if( g_nAtmFd == -1  )
        g_nAtmFd = OpenBcmAtm();

    if( g_nAtmFd != -1  )
        ioctl( g_nAtmFd, ATMIOCTL_SET_TRAFFIC_DESCR_TABLE, &Arg );
    else
        Arg.baStatus = STS_STATE_ERROR;

    return( Arg.baStatus );
} // BcmAtm_SetTrafficDescrTable
#endif /* BUILD_SNMP_ATM_MIB */

/***************************************************************************
 * Function Name: BcmAtm_GetInterfaceCfg
 * Description  : Calls the interface object for the specified interface id
 *                to return the interface configuration record.
 * Returns      : STS_SUCCESS if successful or error status.
 ***************************************************************************/
BCMATM_STATUS BcmAtm_GetInterfaceCfg( UINT32 ulInterfaceId,
    PATM_INTERFACE_CFG pInterfaceCfg )
{
    ATMDRV_INTERFACE_CFG Arg = {ulInterfaceId, pInterfaceCfg, STS_ERROR};

    if( g_nAtmFd == -1  )
        g_nAtmFd = OpenBcmAtm();

    if( g_nAtmFd != -1  )
        ioctl( g_nAtmFd, ATMIOCTL_GET_INTERFACE_CFG, &Arg );
    else
        Arg.baStatus = STS_STATE_ERROR;

    return( Arg.baStatus );
} // BcmAtm_GetInterfaceCfg


/***************************************************************************
 * Function Name: BcmAtm_SetInterfaceCfg
 * Description  : Calls the interface object for the specified interface id
 *                to save a new interface configuration record.
 * Returns      : STS_SUCCESS if successful or error status.
 ***************************************************************************/
BCMATM_STATUS BcmAtm_SetInterfaceCfg( UINT32 ulInterfaceId,
    PATM_INTERFACE_CFG pInterfaceCfg )
{
    ATMDRV_INTERFACE_CFG Arg = {ulInterfaceId, pInterfaceCfg, STS_ERROR};

    if( g_nAtmFd == -1  )
        g_nAtmFd = OpenBcmAtm();

    if( g_nAtmFd != -1  )
        ioctl( g_nAtmFd, ATMIOCTL_SET_INTERFACE_CFG, &Arg );
    else
        Arg.baStatus = STS_STATE_ERROR;

    return( Arg.baStatus );
} // BcmAtm_SetInterfaceCfg

#if defined(BUILD_SNMP_ATM_MIB) || defined(CPE_DSL_MIB)
/***************************************************************************
 * Function Name: BcmAtm_GetVccCfg
 * Description  : Returns the VCC configuration record for the specified VCC
 *                address.
 * Returns      : STS_SUCCESS if successful or error status.
 ***************************************************************************/
BCMATM_STATUS BcmAtm_GetVccCfg( PATM_VCC_ADDR pVccAddr,
    PATM_VCC_CFG pVccCfg )
{
    ATMDRV_VCC_CFG Arg = {{pVccAddr->ulInterfaceId, pVccAddr->usVpi,
        pVccAddr->usVci}, pVccCfg, STS_ERROR};

    if( g_nAtmFd == -1  )
        g_nAtmFd = OpenBcmAtm();

    if( g_nAtmFd != -1  )
        ioctl( g_nAtmFd, ATMIOCTL_GET_VCC_CFG, &Arg );
    else
        Arg.baStatus = STS_STATE_ERROR;

    return( Arg.baStatus );
} // BcmAtm_GetVccCfg


/***************************************************************************
 * Function Name: BcmAtm_SetVccCfg
 * Description  : Saves the VCC configuration record for the specified VCC
 *                address.
 * Returns      : STS_SUCCESS if successful or error status.
 ***************************************************************************/
BCMATM_STATUS BcmAtm_SetVccCfg( PATM_VCC_ADDR pVccAddr,
    PATM_VCC_CFG pVccCfg )
{
    ATMDRV_VCC_CFG Arg = {{pVccAddr->ulInterfaceId, pVccAddr->usVpi,
        pVccAddr->usVci}, pVccCfg, STS_ERROR};

    if( g_nAtmFd == -1  )
        g_nAtmFd = OpenBcmAtm();

    if( g_nAtmFd != -1  )
        ioctl( g_nAtmFd, ATMIOCTL_SET_VCC_CFG, &Arg );
    else
        Arg.baStatus = STS_STATE_ERROR;

    return( Arg.baStatus );
} // BcmAtm_SetVccCfg


/***************************************************************************
 * Function Name: BcmAtm_GetVccAddrs
 * Description  : Returns the configured VCC addresses for an interface. 
 * Returns      : STS_SUCCESS if successful or error status.
 ***************************************************************************/
BCMATM_STATUS BcmAtm_GetVccAddrs( UINT32 ulInterfaceId, PATM_VCC_ADDR pVccAddrs,
    UINT32 ulNumVccs, UINT32 *pulNumReturned )
{
    ATMDRV_VCC_ADDRS Arg = {ulInterfaceId, pVccAddrs, ulNumVccs, 0, STS_ERROR};

    if( g_nAtmFd == -1  )
        g_nAtmFd = OpenBcmAtm();

    if( g_nAtmFd != -1  )
    {
        ioctl( g_nAtmFd, ATMIOCTL_GET_VCC_ADDRS, &Arg );
        *pulNumReturned = Arg.ulNumReturned;
    }
    else
        Arg.baStatus = STS_STATE_ERROR;

    return( Arg.baStatus );
} // BcmAtm_GetVccAddrs


/***************************************************************************
 * Function Name: BcmAtm_GetInterfaceStatistics
 * Description  : Returns the statistics record for an interface.
 * Returns      : STS_SUCCESS if successful or error status.
 ***************************************************************************/
BCMATM_STATUS BcmAtm_GetInterfaceStatistics( UINT32 ulInterfaceId,
    PATM_INTERFACE_STATS pStatistics, UINT32 ulReset )
{
    ATMDRV_INTERFACE_STATISTICS Arg = {ulInterfaceId, pStatistics, ulReset,
        STS_ERROR};

    if( g_nAtmFd == -1  )
        g_nAtmFd = OpenBcmAtm();

    if( g_nAtmFd != -1  )
        ioctl( g_nAtmFd, ATMIOCTL_GET_INTERFACE_STATISTICS, &Arg );
    else
        Arg.baStatus = STS_STATE_ERROR;

    return( Arg.baStatus );
} // BcmAtm_GetInterfaceStatistics


/***************************************************************************
 * Function Name: BcmAtm_GetVccStatistics
 * Description  : Returns the VCC statistics record for the specified VCC
 *                address.
 * Returns      : STS_SUCCESS if successful or error status.
 ***************************************************************************/
BCMATM_STATUS BcmAtm_GetVccStatistics( PATM_VCC_ADDR pVccAddr,
    PATM_VCC_STATS pVccStatistics, UINT32 ulReset )
{
    ATMDRV_VCC_STATISTICS Arg = {{pVccAddr->ulInterfaceId, pVccAddr->usVpi,
        pVccAddr->usVci}, pVccStatistics, ulReset, STS_ERROR};

    if( g_nAtmFd == -1  )
        g_nAtmFd = OpenBcmAtm();

    if( g_nAtmFd != -1  )
        ioctl( g_nAtmFd, ATMIOCTL_GET_VCC_STATISTICS, &Arg );
    else
        Arg.baStatus = STS_STATE_ERROR;

    return( Arg.baStatus );
} // BcmAtm_GetVccStatistics


/***************************************************************************
 * Function Name: BcmAtm_SetInterfaceLinkInfo
 * Description  : Calls the interface object for the specified interface id
 *                to set physical link information.
 * Returns      : STS_SUCCESS if successful or error status.
 ***************************************************************************/
BCMATM_STATUS BcmAtm_SetInterfaceLinkInfo( UINT32 ulInterfaceId,
    PATM_INTERFACE_LINK_INFO pLi )
{
    ATMDRV_INTERFACE_LINK_INFO Arg = {ulInterfaceId, {pLi->ulStructureId,
        pLi->ulLinkState, pLi->ulLineRate}, STS_ERROR};

    if( g_nAtmFd == -1  )
        g_nAtmFd = OpenBcmAtm();

    if( g_nAtmFd != -1  )
        ioctl( g_nAtmFd, ATMIOCTL_SET_INTERFACE_LINK_INFO, &Arg );
    else
        Arg.baStatus = STS_STATE_ERROR;

    return( Arg.baStatus );
} // BcmAtm_SetInterfaceLinkInfo


/***************************************************************************
 * Function Name: BcmAtm_SetNotifyCallback
 * Description  : Adds the specified callback function to the list of
 *                functions that are called when an ATM notification
 *                event occurs.
 * Returns      : STS_SUCCESS if successful or error status.
 ***************************************************************************/
BCMATM_STATUS BcmAtm_SetNotifyCallback( FN_NOTIFY_CB pFnNotifyCb )
{
    return( STS_NOT_SUPPORTED );
} // BcmAtm_SetNotifyCallback


/***************************************************************************
 * Function Name: BcmAtm_ResetNotifyCallback
 * Description  : Removes the specified callback function from the list of
 *                functions that are called when an ATM notification
 *                event occurs.
 * Returns      : STS_SUCCESS if successful or error status.
 ***************************************************************************/
BCMATM_STATUS BcmAtm_ResetNotifyCallback( FN_NOTIFY_CB pFnNotifyCb )
{
    return( STS_NOT_SUPPORTED );
} // BcmAtm_ResetNotifyCallback


/***************************************************************************
 * Function Name: BcmAtm_AttachVcc
 * Description  : Attaches an application to a VCC.
 * Returns      : STS_SUCCESS if successful or error status.
 ***************************************************************************/
BCMATM_STATUS BcmAtm_AttachVcc( PATM_VCC_ADDR pVccAddr,
    PATM_VCC_ATTACH_PARMS pAttachParms )
{
    return( STS_NOT_SUPPORTED );
} // BcmAtm_AttachVcc


/***************************************************************************
 * Function Name: BcmAtm_AttachMgmtCells
 * Description  : Attaches an application to send and receive ATM managment
 *                cells on any VCC.
 * Returns      : STS_SUCCESS if successful or error status.
 ***************************************************************************/
BCMATM_STATUS BcmAtm_AttachMgmtCells( UINT32 ulInterfaceId,
    PATM_VCC_ATTACH_PARMS pAttachParms )
{
    return( STS_NOT_SUPPORTED );
} // BcmAtm_AttachMgmtCells


/***************************************************************************
 * Function Name: BcmAtm_AttachTransparent
 * Description  : Attaches an application to send and receive transparent
 *                ATM cells.
 * Returns      : STS_SUCCESS if successful or error status.
 ***************************************************************************/
BCMATM_STATUS BcmAtm_AttachTransparent( UINT32 ulInterfaceId,
    PATM_VCC_ATTACH_PARMS pAttachParms )
{
    return( STS_NOT_SUPPORTED );
} // BcmAtm_AttachTransparent


/***************************************************************************
 * Function Name: BcmAtm_Detach
 * Description  : Ends an application attachment to a VCC, management cells
 *                or transparent cells.
 * Returns      : STS_SUCCESS if successful or error status.
 ***************************************************************************/
BCMATM_STATUS BcmAtm_Detach( UINT32 ulHandle )
{
    return( STS_NOT_SUPPORTED );
} // BcmAtm_Detach


/***************************************************************************
 * Function Name: BcmAtm_SetAal2ChannelIds
 * Description  : Specifies a list of AAL2 channel ids that an application
 *                wants to send and receive data for an AAL2 VCC.
 * Returns      : STS_SUCCESS if successful or error status.
 ***************************************************************************/
BCMATM_STATUS BcmAtm_SetAal2ChannelIds( UINT32 ulHandle,
    PATM_VCC_AAL2_CHANNEL_ID_PARMS pChannelIdParms, UINT32 ulNumChannelIdParms )
{
    return( STS_NOT_SUPPORTED );
} // BcmAtm_SetAal2ChannelIds


/***************************************************************************
 * Function Name: BcmAtm_SendVccData
 * Description  : Sends data on a VCC.
 * Returns      : STS_SUCCESS if successful or error status.
 ***************************************************************************/
BCMATM_STATUS BcmAtm_SendVccData( UINT32 ulHandle,
    PATM_VCC_DATA_PARMS pDataParms )
{
    return( STS_NOT_SUPPORTED );
} // BcmAtm_SendVccData


/***************************************************************************
 * Function Name: BcmAtm_SendMgmtData
 * Description  : Sends a managment cell.
 * Returns      : STS_SUCCESS if successful or error status.
 ***************************************************************************/
BCMATM_STATUS BcmAtm_SendMgmtData( UINT32 ulHandle,
    PATM_VCC_ADDR pVccAddr, PATM_VCC_DATA_PARMS pDataParms )
{
    return( STS_NOT_SUPPORTED );
} // BcmAtm_SendMgmtData


/***************************************************************************
 * Function Name: BcmAtm_SendTransparentData
 * Description  : Sends a transparent ATM cell.
 * Returns      : STS_SUCCESS if successful or error status.
 ***************************************************************************/
BCMATM_STATUS BcmAtm_SendTransparentData( UINT32 ulHandle,
    UINT32 ulInterfaceId, PATM_VCC_DATA_PARMS pDataParms )
{
    return( STS_NOT_SUPPORTED );
} // BcmAtm_SendTransparentData


/***************************************************************************
 * Function Name: AtmDrv_DoTest
 * Description  : Undocumented entry point that executes a loopback test.
 * Returns      : STS_SUCCESS if successful or error status.
 ***************************************************************************/
BCMATM_STATUS AtmDrv_DoTest( PATM_VCC_ADDR pVccAddr, UINT32 ulNumToSend )
{
    ATMDRV_TEST Arg = {{pVccAddr->ulInterfaceId, pVccAddr->usVpi,
        pVccAddr->usVci}, ulNumToSend, STS_ERROR};

    if( g_nAtmFd == -1  )
        g_nAtmFd = OpenBcmAtm();

    if( g_nAtmFd != -1  )
        ioctl( g_nAtmFd, ATMIOCTL_TEST, &Arg );
    else
        Arg.baStatus = STS_STATE_ERROR;

    return( Arg.baStatus );
} // AtmDrv_DoTest


BCMATM_STATUS BcmAtm_SendOamLoopbackTest(UINT32 type, PATM_VCC_ADDR pVccAddr)
{
    ATMDRV_OAM_LOOPBACK Arg = {{pVccAddr->ulInterfaceId, pVccAddr->usVpi,
        pVccAddr->usVci}, type, STS_ERROR};
    if( g_nAtmFd == -1  )
        g_nAtmFd = OpenBcmAtm();

    if( g_nAtmFd != -1  )
        ioctl( g_nAtmFd, ATMIOCTL_OAM_LOOPBACK_TEST, &Arg );
    else
        Arg.baStatus = STS_STATE_ERROR;

    return( Arg.baStatus );
} //BcmAtm_SendOamLoopbackTest
#endif /* BUILD_SNMP_ATM_MIB */
