/*
* <:copyright-BRCM:2007:proprietary:gpon
*
*    Copyright (c) 2007 Broadcom
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


#include "bcm_ploam.h"
#include "bcm_ploamCounterMon.h"
#include "bcm_ploamAlarm.h"
#include <rdpa_ag_gpon.h>
#include <rdpa_ag_port.h>
#include "board.h"

#define PLOAM_DRIVER_VERSION     "0.8"
#define PLOAM_DRIVER_VER_STR     "v" PLOAM_DRIVER_VERSION

/**
 * Module Parameters
 **/

int KEEP_CONFIG = 1;

#define GPON_IFNAME "gpon0"

#define SN_LEN 13
#define DEFAULT_SN "BRCM12345678"
static char *SN = DEFAULT_SN;
module_param(SN, charp, S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(SN, "ONU Serial Number");

#define PW_INITIALIZER { [ 0 ... BCM_PLOAM_PASSWORD_SIZE_BYTES-1 ] = '_' }         // should never be returned
static char PW[BCM_PLOAM_PASSWORD_SIZE_BYTES] = PW_INITIALIZER ;
module_param_string(PW, PW, BCM_PLOAM_PASSWORD_SIZE_BYTES, S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(PW, "ONU Password");

/**
 * Local variables
 **/
static PloamState ploamState;

static PloamState* statep = &ploamState; /*internal shortcut*/

bdmf_object_handle gponObj = NULL;
static bdmf_object_handle wanPort = NULL;
static rdpa_wan_type wanType = rdpa_wan_none;


static int parseSNPW(OUT BCM_Ploam_SerialPasswdInfo* serialPasswd)
{
    const char UNINIT_PW[BCM_PLOAM_PASSWORD_SIZE_BYTES] = PW_INITIALIZER ;
    int i;
    char *snPtr=SN;

    BCM_LOG_DEBUG(BCM_LOG_ID_PLOAM_FSM,"");
    BCM_ASSERT(serialPasswd);

#if defined(BCM_GPON_USE_NVRAM)
    /* If not supplied as module parameter, attempt to get serial number from NVRAM */
    if (strncmp(SN, DEFAULT_SN, SN_LEN)==0) {
        BCM_ASSERT(NVRAM_GPON_SERIAL_NUMBER_LEN == SN_LEN);
        kerSysGetGponSerialNumber(SN);
    }

    /* If supplied as module parameter use it.  else if found in /data use it.  else get it from NVRAM (or default) */
    if ( strncmp(PW, UNINIT_PW, BCM_PLOAM_PASSWORD_SIZE_BYTES) == 0 ) {
        int ret = kerSysFsFileGet(RDPA_WAN_REG_ID, PW, BCM_PLOAM_PASSWORD_SIZE_BYTES);

        if ( ret ) {
            memcpy( PW, UNINIT_PW, sizeof(PW) );
            // BCM_ASSERT(NVRAM_GPON_PASSWORD_LEN == BCM_PLOAM_PASSWORD_SIZE_BYTES);  // 11 != 36
            kerSysGetGponPassword(PW);
        }
    }
#endif

    if (strnlen(snPtr, SN_LEN) != SN_LEN-1) {
        BCM_LOG_ERROR(BCM_LOG_ID_PLOAM_FSM,"Invalid SN Number (1) ([A-Z]4[0-9a-fA-F]8). Given: %s", SN);
        return -EINVAL;
    }

    /*copy the 1st four characters*/
    memcpy(serialPasswd->serialNumber, snPtr, 4);
    snPtr +=4;

    /*Convert the rest, matching each pair of digits as a hex byte*/
    for (i=4; i<BCM_PLOAM_SERIAL_NUMBER_SIZE_BYTES; i++) {
        uint32_t val;
        char tmpStr[3]="  ";
        memcpy(tmpStr, snPtr, 2);
        snPtr +=2;
        if (sscanf(tmpStr, "%x", &val) !=1 ) {
            BCM_LOG_ERROR(BCM_LOG_ID_PLOAM_FSM, "Invalid SN Number (2) ([A-Z]4[0-9a-fA-F]8). Given:%s", SN);
            return -EINVAL ;
        }
        serialPasswd->serialNumber[i] = (uint8_t)val;
    }

    memcpy(serialPasswd->password, PW, BCM_PLOAM_PASSWORD_SIZE_BYTES);
    switch (wanType) {
        case rdpa_wan_gpon:
            memset(serialPasswd->password + (NVRAM_GPON_PASSWORD_LEN - 1), 0, BCM_PLOAM_PASSWORD_SIZE_BYTES - (NVRAM_GPON_PASSWORD_LEN - 1));
        case rdpa_wan_xgpon:
            break;
        default:
            BCM_LOG_ERROR(BCM_LOG_ID_PLOAM_FSM, "Unexpected wanType %d", wanType);
            break;
    }

    return 0;
}


static void printSerialPasswd(IN BCM_Ploam_SerialPasswdInfo* serialPasswd)
{
    uint8_t* sn;
    uint8_t* pw;

    BCM_ASSERT(serialPasswd);

    sn = serialPasswd->serialNumber;
    pw = serialPasswd->password;

    BCM_LOG_NOTICE(BCM_LOG_ID_PLOAM_FSM, "SN=%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",
                 sn[0], sn[1], sn[2], sn[3], sn[4], sn[5], sn[6], sn[7]);
    if (wanType == rdpa_wan_gpon) {
        BCM_LOG_NOTICE(BCM_LOG_ID_PLOAM_FSM, "PW=%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",
                 pw[0], pw[1], pw[2], pw[3], pw[4], pw[5], pw[6], pw[7], pw[8], pw[9]);
    } else {
        BCM_LOG_NOTICE(BCM_LOG_ID_PLOAM_FSM, "PW=%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",
                 pw[ 0], pw[ 1], pw[ 2], pw[ 3], pw[ 4], pw[ 5], pw[ 6], pw[ 7], pw[ 8], pw[ 9],
                 pw[10], pw[11], pw[12], pw[13], pw[14], pw[15], pw[16], pw[17], pw[18], pw[19],
                 pw[20], pw[21], pw[22], pw[23], pw[24], pw[25], pw[26], pw[27], pw[28], pw[29],
                 pw[30], pw[31], pw[32], pw[33], pw[34], pw[35]);
    }
}


/**
 * Public Functions:
 **/

void bcm_ploamSetSerialPasswd(IN BCM_Ploam_SerialPasswdInfo *serialPasswdInfo) {

    rdpa_onu_password_t password ;
    rdpa_onu_sn_t sn;
    bdmf_error_t rc;
    int i;

    BCM_LOG_DEBUG(BCM_LOG_ID_PLOAM,"");

    /* Serial number */
    for ( i = 0 ; i < sizeof(sn.vendor_id); i ++ )
    {
        sn.vendor_id [i] = serialPasswdInfo->serialNumber [i] ;
    }

    for ( i = 0 ; i <  sizeof(sn.vendor_specific); i ++ )
    {
        sn.vendor_specific [i] = serialPasswdInfo->serialNumber [ i + sizeof(sn.vendor_id) ] ;
    }

    bdmf_lock();
    rc = rdpa_gpon_onu_sn_set(gponObj, &sn);

    /* Password */
    for ( i = 0 ; i < BCM_PLOAM_PASSWORD_SIZE_BYTES ; i ++ )
    {
        password.password [i] = serialPasswdInfo->password [i] ;
    }

    rc = rc ? rc : rdpa_gpon_password_set(gponObj, &password, sizeof(rdpa_onu_password_t));
    bdmf_unlock();

    if (rc)
        BCM_LOG_ERROR( BCM_LOG_ID_PLOAM,"bcm_ploamSetSerialPasswd: error = %d", rc);

    printSerialPasswd(serialPasswdInfo);
}




void bcm_ploamGetSerialPasswd(OUT BCM_Ploam_SerialPasswdInfo *serialPasswdInfo)
{
    int i;
    rdpa_onu_sn_t sn;
    rdpa_onu_password_t password;
    bdmf_error_t rc;

    memset ( serialPasswdInfo, 0, sizeof ( BCM_Ploam_SerialPasswdInfo ) ) ;

    /* Serial Number */
    rc = rdpa_gpon_onu_sn_get(gponObj, &sn);
    for ( i = 0 ; i < sizeof(sn.vendor_id) ; i ++ )
       serialPasswdInfo->serialNumber [ i ] = sn.vendor_id [ i ] ;
    for ( i = 0 ; i < sizeof(sn.vendor_specific) ; i ++ )
       serialPasswdInfo->serialNumber [ i + sizeof(sn.vendor_id) ] =  sn.vendor_specific [ i ] ;

    /* Password */
    rc = rc ? rc : rdpa_gpon_password_get (gponObj, &password, sizeof(password));
    for ( i = 0 ; i < BCM_PLOAM_PASSWORD_SIZE_BYTES ; i ++ )
        serialPasswdInfo->password [ i ]  = password.password [ i ] ;

    if (rc < 0)
        BCM_LOG_ERROR( BCM_LOG_ID_PLOAM,"bcm_ploamGetSerialPasswd: error = %d", rc);
}

void bcm_ploamLoadNewSerialPasswd(void) {

     bcm_ploamSetSerialPasswd( &statep->newSerialPasswd );
}

void bcm_ploamSetNewSerialPasswd(BCM_Ploam_SerialPasswdInfo* serialPasswdInfop) {

     memcpy( &statep->newSerialPasswd.serialNumber,
             serialPasswdInfop->serialNumber,
             BCM_PLOAM_SERIAL_NUMBER_SIZE_BYTES );
     memcpy( &statep->newSerialPasswd.password,
             serialPasswdInfop->password,
             BCM_PLOAM_PASSWORD_SIZE_BYTES );
}


void bcm_ploamGetCounters(OUT BCM_PloamSwCounters* ploamCounters, int reset)
{
    bdmf_error_t rc;
    rdpa_gpon_stat_t link_stat;

    BCM_LOG_DEBUG(BCM_LOG_ID_PLOAM_FSM,"");
    BCM_ASSERT(statep->initialized);
    BCM_ASSERT(ploamCounters);

    rc = rdpa_gpon_link_stat_get(gponObj, &link_stat);

    ploamCounters->rxPloamsUcast = link_stat.rx_onu_id;
    ploamCounters->rxPloamsBcast = link_stat.rx_broadcast;
    ploamCounters->rxPloamsNonStd = link_stat.rx_unknown;
    ploamCounters->rxPloamsTotal = ploamCounters->rxPloamsUcast + ploamCounters->rxPloamsBcast;
    ploamCounters->txPloams = link_stat.tx_idle + link_stat.tx_ploam;

    /* counters we don't support */
    ploamCounters->txPloamsNonStd = 0;
    ploamCounters->rxPloamsDiscarded = 0;

    statep->swCounters.rxPloamsUcast    += ploamCounters->rxPloamsUcast;
    statep->swCounters.rxPloamsBcast    += ploamCounters->rxPloamsBcast;
    statep->swCounters.rxPloamsNonStd   += ploamCounters->rxPloamsNonStd;
    statep->swCounters.rxPloamsTotal    += ploamCounters->rxPloamsTotal;
    statep->swCounters.rxPloamsDiscarded+= ploamCounters->rxPloamsDiscarded;
    statep->swCounters.txPloams         += ploamCounters->txPloams;
    statep->swCounters.txPloamsNonStd   += ploamCounters->txPloamsNonStd;

    ploamCounters->rxPloamsUcast     = statep->swCounters.rxPloamsUcast;
    ploamCounters->rxPloamsBcast     = statep->swCounters.rxPloamsBcast;
    ploamCounters->rxPloamsNonStd    = statep->swCounters.rxPloamsNonStd ;
    ploamCounters->rxPloamsTotal     = statep->swCounters.rxPloamsTotal;
    ploamCounters->rxPloamsDiscarded = statep->swCounters.rxPloamsDiscarded;
    ploamCounters->txPloams          = statep->swCounters.txPloams;
    ploamCounters->txPloamsNonStd    = statep->swCounters.txPloamsNonStd;

    if (reset)
        memset(&statep->swCounters, 0, sizeof(BCM_PloamSwCounters));

    if (rc)
        BCM_LOG_ERROR( BCM_LOG_ID_PLOAM,"bcm_ploamGetCounters: error = %d", rc);
}


void bcm_ploamDyingGaspHandler(void *ctxt)
{
    bcm_ploamSendDyingGasp();
}


static rdpa_wan_type getRdpaWanType(void)
{
    int count= 0;
    rdpa_wan_type wanType = rdpa_wan_none;
    char wan_type_buffer[PSP_BUFLEN_16] = {};

    count = kerSysScratchPadGet(RDPA_WAN_TYPE_PSP_KEY, wan_type_buffer, PSP_BUFLEN_16);
    if (count > 0)
    {
        if ((!strcasecmp(wan_type_buffer, RDPA_WAN_TYPE_VALUE_XGPON1)) ||
            (!strcasecmp(wan_type_buffer, RDPA_WAN_TYPE_VALUE_NGPON2)) ||
            (!strcasecmp(wan_type_buffer, RDPA_WAN_TYPE_VALUE_XGS)))
        {
            wanType = rdpa_wan_xgpon;
        }
        else if (!strcasecmp(wan_type_buffer, RDPA_WAN_TYPE_VALUE_GPON))
        {
            wanType = rdpa_wan_gpon;
        }
    }
    return wanType;
}


static int bcm_createGponObj(void)
{
    BDMF_MATTR(gpon_attrs, rdpa_gpon_drv());
    BDMF_MATTR(port_attrs, rdpa_port_drv());
    rdpa_if wan_if = rdpa_if_none;
    bdmf_object_handle cpu_obj = NULL;
    int rc;

    /* Indication for OMCI */
    wanType = getRdpaWanType();
    if (wanType == rdpa_wan_none)
    {
        BCM_LOG_ERROR(BCM_LOG_ID_GPON, "Failed to get correct gpon wan type");
        return -1;
    }
    wan_if = rdpa_wan_type_to_if(wanType);
    rdpa_gpon_user_ic_set(gpon_attrs, GponDrvIndicationCallback);

    rdpa_port_index_set(port_attrs, wan_if);
    /* wan_type MUST come after index */
    rdpa_port_wan_type_set(port_attrs, wanType);
    rc = bdmf_new_and_set(rdpa_port_drv(), NULL, port_attrs, &wanPort);
    if (rc < 0)
    {
        BCM_LOG_ERROR(BCM_LOG_ID_GPON,"Problem creating gpon wan port object\n");
        return rc;
    }

    rc = bdmf_new_and_set(rdpa_gpon_drv(), wanPort, gpon_attrs, &gponObj);
    if (rc < 0)
    {
        BCM_LOG_ERROR(BCM_LOG_ID_GPON, "Failed to create GPON object, rc %d\n", rc);
        bdmf_destroy(wanPort);
        return rc;
    }

    rc = rdpa_cpu_get(rdpa_cpu_host, &cpu_obj);
#ifdef XRDP
    rc = rc ? rc : rdpa_port_cpu_obj_set(wanPort, cpu_obj);
    if (rc)
    {
        BCM_LOG_ERROR(BCM_LOG_ID_GPON, "Failed to set CPU object for WAN port, error %d\n", rc);
        bdmf_destroy(wanPort);
    }
#else
    rc = 0;
#endif

    if (cpu_obj)
        bdmf_put(cpu_obj);

    return rc;
}


void bcm_destroyGponObj(void)
{
    bdmf_destroy(gponObj);
    bdmf_destroy(wanPort);
}


int __devinit bcm_ploamCreate()
{
    int ret;

    ret = bcm_createGponObj();
    if (ret < 0)
    {
        BCM_LOG_ERROR(BCM_LOG_ID_PLOAM_FSM, "Failed to create GPON object.");
        return ret;
    }

    /* Retrieve serial and password from module params or NVRAM */
    ret = parseSNPW(&statep->newSerialPasswd);
    if (ret)
    {
        BCM_LOG_ERROR(BCM_LOG_ID_PLOAM_FSM, "parseSNPW failed.");
        return ret;
    }

    printSerialPasswd (&statep->newSerialPasswd);

    ret = bcm_ploamUsrCreate();
    if (ret)
    {
        BCM_LOG_ERROR(BCM_LOG_ID_PLOAM_FSM, "ploamUsrCreate failed.");
        return ret;
    }

    kerSysRegisterDyingGaspHandler(GPON_IFNAME, bcm_ploamDyingGaspHandler, 0);

    statep->created = 1;
    BCM_LOG_NOTICE(BCM_LOG_ID_PLOAM_FSM, "ploam driver created.");

    return ret;
}


static void ploamInitCfg(bool keepConfig){

    bcm_ploamPortCtrlInit(1, &statep->usrEventObj);

}


void bcm_ploamInit(bool keepConfig)
{

    BCM_LOG_INFO(BCM_LOG_ID_PLOAM_FSM,"");

    bcm_ploamEventInit(&statep->usrEventObj);
    bcm_ploamAlarmInit(&statep->usrEventObj);
    bcm_ploamUsrInit(&statep->usrEventObj);


    ploamInitCfg(keepConfig);

    bcm_omciInit();

    statep->initialized = 1;

    bcm_ploamCounterMonInit();

    BCM_LOG_INFO(BCM_LOG_ID_PLOAM_FSM, "ploam driver initialized.");

}


int bcm_ploamSendDyingGasp(void)
{
    rdpa_send_ploam_params_t send_ploam = {};

    send_ploam.ploam_type = gpon_ploam_type_dgasp;
    return (rdpa_gpon_send_ploam_set(gponObj, &send_ploam));
}


void bcm_ploamDelete(void)
{
    kerSysDeregisterDyingGaspHandler(GPON_IFNAME);
    bcm_ploamUsrDelete();

    bcm_destroyGponObj();

    statep->created = 0;
    BCM_LOG_NOTICE(BCM_LOG_ID_PLOAM_FSM, "ploam driver deleted.");
}


int bcm_ploamStart(BCM_Ploam_OperState initOperState)
{
    bdmf_error_t rc = BDMF_ERR_OK;
    rdpa_link_activate_t initial_state = rdpa_link_activate_O1;

    BCM_LOG_DEBUG(BCM_LOG_ID_PLOAM,"");
    BCM_LOG_INFO(BCM_LOG_ID_PLOAM_FSM, "initOperState = %d\n", initOperState );

    if(initOperState != BCM_PLOAM_OSTATE_INITIAL_O1 &&
        initOperState != BCM_PLOAM_OSTATE_EMERGENCY_STOP_O7) {
        return -EINVAL_PLOAM_INIT_OPER_STATE;
    }

    /* Support starting from "Emergency Stop" state */
    if ( initOperState == BCM_PLOAM_OSTATE_EMERGENCY_STOP_O7 ) {
        initial_state = rdpa_link_activate_O7;
    }

    rc = rdpa_gpon_onu_sn_set(gponObj, (rdpa_onu_sn_t *)&statep->newSerialPasswd.serialNumber);
    rc = rc ? rc : rdpa_gpon_password_set(gponObj,  (rdpa_onu_password_t *)&statep->newSerialPasswd.password, sizeof(statep->newSerialPasswd.password));
    if (rc < 0)
    {
        BCM_LOG_NOTICE(BCM_LOG_ID_GPON, "Failed to configure serial number and/or password, rc %d\n", rc);
        return rc;
    }

    rc = rdpa_gpon_link_activate_set(gponObj, initial_state);
    if (rc < 0)
    {
        BCM_LOG_NOTICE(BCM_LOG_ID_GPON, "Failed to activate GPON link, rc %d\n", rc);
        return rc;
    }

    statep->adminState = BCM_PLOAM_ASTATE_ON;

    return rc;
}


int get_gem_flow_id_by_gem_port(bdmf_number gemPortID, UINT16 *gemPortIdx)
{
    bdmf_object_handle gem = NULL;
    bdmf_number foundGemPortID = BCM_PLOAM_GEM_PORT_ID_UNASSIGNED;
    bdmf_number gemPortIndex;

    while((gem = bdmf_get_next(rdpa_gem_drv(), gem, NULL)))
    {
        rdpa_gem_gem_port_get(gem, &foundGemPortID);
        if (gemPortID == foundGemPortID)
            break;
    }
    if (!gem)
        return -ENOENT;

    rdpa_gem_index_get(gem, &gemPortIndex);
    *gemPortIdx = (UINT16)gemPortIndex;

    bdmf_put(gem);

    return 0;
}
