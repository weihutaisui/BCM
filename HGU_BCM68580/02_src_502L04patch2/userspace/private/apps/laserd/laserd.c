/***********************************************************************
 *
 *  Copyright (c) 2007  Broadcom Corporation
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/ethtool.h>
#include "ethswctl_api.h"
#include <signal.h>
#include <stdio.h>

#include <boardparms.h>

#include "cms.h"
#include "cms_log.h"
#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"
#include "prctl.h"
#include "laser.h"

/*
 * macros
 */
#define LASERD_VERSION                          "0.1"
#define LASERD                                  "\nlaserd"
#define LASERD_OPTICAL_PARAMS_SIZE              64
#define LASERD_OPTICS_TYPE_LEGACY               0
#define LASERD_OPTICS_TYPE_BOSA                 1
#define LASERD_OPTICAL_PARAMS_SIG1_VAL          0xA5
#define LASERD_OPTICAL_PARAMS_SIG2_VAL          0x5A
#define LASERD_OPTICAL_PARAMS_SIG1_IDX          62
#define LASERD_OPTICAL_PARAMS_SIG2_IDX          63
#define LASERD_OPTICAL_PARAMS_SOURCE_NVRAM      1
#define LASERD_OPTICAL_PARAMS_SOURCE_OPTICS     2



typedef enum
{
    LOAD=0,
    STORE=1
} LoadStore_t;

/*
 * global variables
 */
static char pgmName[80] = {0};

static const CmsEntityId myEid = EID_LASERD;

fd_set readFdsMaster, readFds;
SINT32 msgFd;
void *msgHandle = NULL;  
void *tmrHandle = NULL;  
  
CmsMsgHeader* pMsg = NULL;

static SINT32 shmId = 0;
unsigned char OpticalParams[LASERD_OPTICAL_PARAMS_SIZE] = {0};// * pOpticalParams = OpticalParams;

int LaserDevFd;
unsigned short GponOpticsType = 0;

extern int devCtl_getOpticalParams(unsigned char*);
extern int devCtl_getGponOpticsType(void);
extern int devCtl_getOpticalParams(unsigned char *);
extern int devCtl_setOpticalParams(unsigned char *);
extern int devCtl_getDefaultOpticalParams(unsigned char *);


static int Laserd_Init_Optics(void);
static int Laserd_Get_Gpon_Optics_Type(void);
static int Laserd_Read_Optical_Params_From_Nvram(unsigned char *);
static int Laserd_Read_Optical_Params_From_Optics(unsigned char *);
static int Laserd_Store_Optical_Params(UINT32, char *);
static int Laserd_Load_Optical_Params(UINT32, char *);
static int Laserd_Write_Optical_Params_To_Nvram(unsigned char *);
static int Laserd_Write_Optical_Params_To_Optics(unsigned char *);


/*
 * functions
 */


/*****************************************************************************
*
*  FUNCTION     :   Laserd_Init_Optics
*
*  DESCRIPTION  :   This function performs the basic initialization that is
*                   required to communicate with the laser drivers to support
*                   any required optics device init.
*
*  PARAMETERS   :   None
*
*  RETURNS      :   0 on success
*
*****************************************************************************/
static int Laserd_Init_Optics(void)
{

    int Ret=0;
    
    // Attempt to open pipe to laser_dev
    // Test for valid laser_dev pipe.
    if ((LaserDevFd = open(LASER_DEV, O_RDWR)) >= 0)
    {
        // determine if BOSA optics present and params need to be loaded
        if (LASERD_OPTICS_TYPE_BOSA==(GponOpticsType = Laserd_Get_Gpon_Optics_Type()))
        {                     
            Ret = Laserd_Load_Optical_Params(0, NULL);
        }
        else

        {
            // Report error.
            cmsLog_error(LASERD": Bosa Optics not present");
            close(LaserDevFd);
        }
    }
    else
    {
        // Report error.
        cmsLog_error(LASERD": Error, Laser_dev driver did not open\n");
        Ret = LaserDevFd;
    }
        
    return Ret;
}

/*****************************************************************************
*
*  FUNCTION     :   Laserd_Get_Gpon_Optics_Type
*
*  DESCRIPTION  :   This routine is used to retreive an indication of the type 
*                   of optics on the current board.
*
*  PARAMETERS   :   None
*
*  RETURNS      :   0 for legacy optics
*                   1 for BOSA optics
*
*****************************************************************************/
static int Laserd_Get_Gpon_Optics_Type(void)
{
    return (devCtl_getGponOpticsType());   
}
/*****************************************************************************
*
*  FUNCTION     :   Laserd_Read_Optical_Params_From_Nvram
*
*  DESCRIPTION  :   This routine is used to retreive a copy of the contents of 
*                   the optical calibration parameter DB in NVRAM.
*
*  PARAMETERS   :   OUT: pOpticalParams contains the address of char array of 
*                   params read from NVRAM.
*
*  RETURNS      :   0 on success
*
*****************************************************************************/
static int Laserd_Read_Optical_Params_From_Nvram(unsigned char * pOpticalParams)
{
    return (devCtl_getOpticalParams(pOpticalParams));   
}

/*****************************************************************************
*
*  FUNCTION     :   Laserd_Read_Optical_Params_From_BoardParams
*
*  DESCRIPTION  :   This routine is used to retreive a copy of the contents of 
*                   the optical calibration parameter DB in board params.
*
*  PARAMETERS   :   OUT: pOpticalParams contains the address of char array of 
*                   params read from board params.
*
*  RETURNS      :   0 on success
*
*****************************************************************************/
static int Laserd_Read_Optical_Params_From_BoardParams(unsigned char * pOpticalParams)
{
    return (devCtl_getDefaultOpticalParams(pOpticalParams));   
}

/*****************************************************************************
*
*  FUNCTION     :   Laserd_Read_Optical_Params_From_Optics
*
*  DESCRIPTION  :   This routine is used to retreive a copy of the optical  
*                   parameters from the registers of the BOSA optics device.
*
*  PARAMETERS   :   OUT: pOpticalParams contains the address of char array of 
*                   params read from the BOSA optics.
*
*  RETURNS      :   0 on success
*
*****************************************************************************/
static int Laserd_Read_Optical_Params_From_Optics(unsigned char * pOpticalParams)
{    
    return (ioctl(LaserDevFd, LASER_IOCTL_GET_OPTICAL_PARAMS, pOpticalParams));
}

/*****************************************************************************
*
*  FUNCTION     :   Laserd_Write_Optical_Params_To_Nvram
*
*  DESCRIPTION  :   This routine is used to write the contents of the optical 
*                   parameter DB in NVRAM
*
*  PARAMETERS   :   IN: pOpticalParams contains address of char array of BOSA 
*                   params to be written to NVRAM.
*
*  RETURNS      :   0 on success
*
*****************************************************************************/
static int Laserd_Write_Optical_Params_To_Nvram(unsigned char * pOpticalParams)
{
    return (devCtl_setOpticalParams(pOpticalParams));
}

/*****************************************************************************
*  FUNCTION     :   Laserd_Write_Optical_Params_To_Optics
*               :
*  DESCRIPTION  :   This routine is used to write the contents of the optical
*               :   parameter DB to registers in the optics device
*               :
*  PARAMETERS   :   IN: pOpticalParams contains address of char array of params
*               :   to be written to NVRAM.
*               :
*  RETURNS      :   0 on success
*****************************************************************************/
static int Laserd_Write_Optical_Params_To_Optics(unsigned char * pOpticalParams)
{
    return (ioctl(LaserDevFd, LASER_IOCTL_SET_OPTICAL_PARAMS, pOpticalParams));
}

static void Laserd_Tag_Optical_Param_Set(unsigned char * pOpticalParams)
{
    // tag the stored parameter set as valid
    pOpticalParams[LASERD_OPTICAL_PARAMS_SIG1_IDX] = LASERD_OPTICAL_PARAMS_SIG1_VAL;
    pOpticalParams[LASERD_OPTICAL_PARAMS_SIG2_IDX] = LASERD_OPTICAL_PARAMS_SIG2_VAL;
    
}

/*****************************************************************************
*  FUNCTION     :   Laserd_Get_Optical_Params
*               :
*  DESCRIPTION  :   This routine is used to get and return the set of optical 
*               :   params for use in a load or store operation.  These may  
*               :   come from NVRAM, board params or an external file for a  
*               :   LOAD, and optics, or an external file for a STORE.
*               :
*  PARAMETERS   :   IN: pOpticalParams contains address of char array of params
*               :   to be written to NVRAM.
*               :
*  RETURNS      :   0 on success
*****************************************************************************/
static int Laserd_Get_Optical_Params (UINT32 ExtFileOption, char * pFileName, 
                                      LoadStore_t LoadStoreOp, unsigned char * pOpticalParams)
{

    int Ret = 0;


    if (FALSE != ExtFileOption)
    {    
        FILE * pFileHandle = fopen(pFileName, "r");

        // file is valid if we opened it so go ahead and process it
        if (NULL != pFileHandle)
        {
            UINT32 Lcv;
            char Buf[8];
            
            for (Lcv=0; Lcv < LASERD_OPTICAL_PARAMS_SIZE; Lcv++)
            {
                if (NULL != fgets(Buf, 8, pFileHandle))
                {
                    pOpticalParams[Lcv] = (unsigned char) strtol(Buf, NULL,16); 
                }
                else
                {
                    if (Lcv<LASERD_OPTICAL_PARAMS_SIZE)
                    {
                        Ret = CMSRET_INVALID_ARGUMENTS;
                        cmsLog_error(LASERD": end of file encountered before all optical parameters read\n");
                    }
                    break;
                }
            }
            fclose (pFileHandle);
            
            Laserd_Tag_Optical_Param_Set(pOpticalParams);
                        
        }
        else
        {
            cmsLog_error(LASERD": can't open file %s\n", pFileName);
            Ret = CMSRET_INVALID_ARGUMENTS;
        }
        
    }
    else
    {    
        switch (LoadStoreOp)
        {
            case LOAD:
            {
                // attempt to load and validate params from NVRAM  first
                printf(LASERD": Loading calibrated optical parameters from NVRAM.\n");

                if (0 == ( Ret = Laserd_Read_Optical_Params_From_Nvram(pOpticalParams)))
                {
                    if ((LASERD_OPTICAL_PARAMS_SIG1_VAL != (pOpticalParams[LASERD_OPTICAL_PARAMS_SIG1_IDX] & 0xFF)) ||
                        (LASERD_OPTICAL_PARAMS_SIG2_VAL != (pOpticalParams[LASERD_OPTICAL_PARAMS_SIG2_IDX] & 0xFF)))
                    {                        
                        printf(LASERD": calibrated optical parameters not found in NVRAM.\n");
                        Ret = 1;
                    }
                    else
                    {
                        printf(LASERD": calibrated optical parameters found in NVRAM.\n");
                    }
                }
                else
                {
                    cmsLog_error(LASERD": I/O error occurred trying to read optical params from NVRAM\n");
                }
                if (0 != Ret)
                {    
                    printf(LASERD": Loading partially calibrated optical parameters from board params.\n");
                                
                    if (0 == ( Ret = Laserd_Read_Optical_Params_From_BoardParams(pOpticalParams)))
                    {
                        if ((LASERD_OPTICAL_PARAMS_SIG1_VAL != (pOpticalParams[LASERD_OPTICAL_PARAMS_SIG1_IDX] & 0xFF)) ||
                            (LASERD_OPTICAL_PARAMS_SIG2_VAL != (pOpticalParams[LASERD_OPTICAL_PARAMS_SIG2_IDX] & 0xFF)))
                        {
                            printf(LASERD": Error, partially calibrated optical parameters not found in board params.\n");
                            Ret = 1;
                        }
                    }
                    else
                    {
                        cmsLog_error(LASERD": I/O error occurred trying to read optical parameters from board params.\n");
                    }
                }
                    
                break;
            }
            case STORE:
            {
                if (0 < (Ret = Laserd_Read_Optical_Params_From_Optics(pOpticalParams)))
                {
                    cmsLog_error(LASERD": Error, failed to read optical params from optical device\n");
                }
                break;
            }
            default:
            {
                cmsLog_error(LASERD": Invalid load/store operation %d\n", LoadStoreOp);
            }
        }
    }

    return Ret;
}

/*****************************************************************************
*  FUNCTION     :   Laserd_Load_Optical_Params
*               :
*  DESCRIPTION  :   This routine is used to retrieve the BOSA optical 
*               :   parameters and to write them to the BOSA optics device.  
*               :   
*  PARAMETERS   :   IN
*               :      UINT32 FileNameOption -
*               :               Flag indicating if param source is to be an  
*               :               external file or NVRAM.  
*               :      char * pFileName -   
*               :               pointer to filename if FileNameOption is TRUE.
*               :   
*  RETURNS      :   0 on success
*****************************************************************************/
static int Laserd_Load_Optical_Params(UINT32 FileNameOption, char * pFileName)
{

    int Ret;

    if (0==(Ret = Laserd_Get_Optical_Params(FileNameOption, pFileName, LOAD, OpticalParams)))
    {                   
        if (0 < (Ret = Laserd_Write_Optical_Params_To_Optics(OpticalParams)))
        {
            cmsLog_error(LASERD": Error, failed to write optical params to optical device\n");
        }
    }
    else
    {
        cmsLog_error(LASERD": Error, failed to get optical params\n");
    }

    return Ret;
    
}

/*****************************************************************************
*  FUNCTION     :   Laserd_Store_Optical_Params
*               :
*  DESCRIPTION  :   This routine is used to retrieve the current BOSA optical 
*               :   parameters from the registers of the optical device, and 
*               :   to write them to NVRAM.
*               :
*  PARAMETERS   :   IN
*               :      UINT32 FileNameOption -
*               :               Flag indicating if param source is to be an  
*               :               external file or NVRAM.  
*               :      char * pFileName -   
*               :               pointer to filename if FileNameOption is TRUE.
*               :   
*  RETURNS      :   0 on success
*****************************************************************************/
static int Laserd_Store_Optical_Params(UINT32 FileNameOption, char * pFileName)
{
    int Ret;
    unsigned char LocalOpticalParams[LASERD_OPTICAL_PARAMS_SIZE]= {0};
 
    
    if (0==(Ret = Laserd_Get_Optical_Params(FileNameOption, pFileName, STORE, LocalOpticalParams)))
    {
        Laserd_Tag_Optical_Param_Set(LocalOpticalParams);
        
        if (0==(Ret = Laserd_Write_Optical_Params_To_Nvram(LocalOpticalParams)))
        {
            // commit to memory based storage
            memcpy(OpticalParams, LocalOpticalParams, LASERD_OPTICAL_PARAMS_SIZE);
        }
        else
        {
            cmsLog_error(LASERD": Error, failed to write optical params to optical device\n");
        }
    }
    else
    {
        cmsLog_error(LASERD": Error, failed to get optical params to be stored\n");
    }

    return Ret;
    
}

/*****************************************************************************
*  FUNCTION     :   Laserd_Clear_Optical_Params
*               :
*  DESCRIPTION  :   This routine is used to clear the BOSA optical params in
*               :   either NVRAM or the BOSA optics device.  
*               :   
*  PARAMETERS   :   IN
*               :      UINT32 ParamSource -
*               :               Indication of param source to be cleared.
*               :   
*  RETURNS      :   0 on success
*****************************************************************************/
static int Laserd_Clear_Optical_Params(UINT32 ParamSource)
{

    int Ret;
    unsigned char LocalOpticalParams[LASERD_OPTICAL_PARAMS_SIZE]={0};

    switch (ParamSource)
    {
        case LASERD_OPTICAL_PARAMS_SOURCE_NVRAM:
        {
            Ret = Laserd_Write_Optical_Params_To_Nvram(LocalOpticalParams);
            
            if (0 != Ret)
            {
                cmsLog_error(LASERD": Error, failed to clear optical params in NVRAM\n");
            }
            break;
        }
        case LASERD_OPTICAL_PARAMS_SOURCE_OPTICS:
        {
            Ret = Laserd_Write_Optical_Params_To_Optics(LocalOpticalParams);
            
            if (0 != Ret)
            {
                cmsLog_error(LASERD": Error, failed to clear optical params in Optics\n");
            }
            break;
        }
    }   

    return Ret;

}

/*****************************************************************************
*  FUNCTION     :   Laserd_Dump_Optical_Params
*               :
*  DESCRIPTION  :   This routine is used to dump to the console the BOSA optical 
*               :   currently params in either NVRAM or the BOSA optics device.  
*               :   
*  PARAMETERS   :   IN
*               :      UINT32 ParamSource -
*               :               Indication of param source to be cleared.
*               :   
*  RETURNS      :   0 on success
*****************************************************************************/
static int Laserd_Dump_Optical_Params(UINT32 ParamSource)
{

    int Ret;
    unsigned char LocalOpticalParams[LASERD_OPTICAL_PARAMS_SIZE]={0};

    switch (ParamSource)
    {
        case LASERD_OPTICAL_PARAMS_SOURCE_NVRAM:
        {
            Ret = Laserd_Read_Optical_Params_From_Nvram(LocalOpticalParams);
            
            if (0 != Ret)
            {
                cmsLog_error(LASERD": Error, failed to get optical params from NVRAM\n");
            }            
            break;
        }
        case LASERD_OPTICAL_PARAMS_SOURCE_OPTICS:
        {
            Ret = Laserd_Read_Optical_Params_From_Optics(LocalOpticalParams);
            
            if (0 != Ret)
            {
                cmsLog_error(LASERD": Error, failed to get optical params from Optics\n");
            }
            break;
        }
        default:
        {
            cmsLog_error(LASERD": Error, invalid optical params source arg\n");
            Ret = CMSRET_INVALID_ARGUMENTS;
        }
    }   

    // dump the results to console if RV ok.
    if (Ret == 0)
    {
        int Lcv;
        
        for (Lcv = 0; Lcv < 48; Lcv++)
        {
            printf("0x%02X 0x%02X\n", Lcv, (LocalOpticalParams[Lcv] & 0xff));
        }
    }

    return Ret;

}

/*****************************************************************************
*  FUNCTION     :   Laserd_Cms_Init
*               :
*  DESCRIPTION  :   Initializes all CMS resources
*               :
*  PARAMETERS   :   None
*               :
*  RETURNS      :   CMS Return
*****************************************************************************/

static CmsRet Laserd_Cms_Init(void)
{
    SINT32 sessionPid;
    CmsRet Ret;
    CmsMsgHeader *buf;

    /* initialize CMS logging */
    cmsLog_init(myEid);

    /*
    * Detach myself from the terminal so I don't get any control-c/sigint.
    * On the desktop, it is smd's job to catch control-c and exit.
    * When omcid detects that smd has exited, omcid will also exit.
    */
    if ((sessionPid = setsid()) == -1)
    {
        cmsLog_error(LASERD": Could not detach from terminal");
    }
    else
    {
        cmsLog_debug("Detached from terminal");
    }

    if ((Ret = cmsMsg_init(myEid, &msgHandle)) != CMSRET_SUCCESS)
    {
        cmsLog_error(LASERD": msg initialization failed, ret=%d", Ret);
        return Ret;
    }

    ;
    if(CMSRET_SUCCESS != (Ret = cmsMsg_receiveWithTimeout(msgHandle, &buf, 100)))
    {        
        cmsLog_error(LASERD": cmsMsg_receiveWithTimeout failed, ret=%d", Ret);
        return Ret;
    }
    else
    {
        cmsMem_free(buf);
    }

    if (CMSRET_SUCCESS != (Ret = cmsMdm_init(myEid, msgHandle, &shmId)))
    {
        cmsLog_error(LASERD": cmsMdm_init failed, ret=%d", Ret);
        return Ret;
    }

    cmsLog_notice(LASERD": cmsMdm_init successful, shmId=%d", shmId);

    // Get the CMS messaging FD.
    cmsMsg_getEventHandle(msgHandle, &msgFd);

    // Attempt to read any previous message.
    Ret = cmsMsg_receiveWithTimeout(msgHandle, &pMsg, 100);
    if (Ret == CMSRET_SUCCESS)
    {
        // Release any received message.
        cmsMem_free(pMsg);
    }

    return CMSRET_SUCCESS;
}
/*****************************************************************************
*  FUNCTION     :   Laserd_Cms_Cleanup
*               :
*  DESCRIPTION  :   Frees and cleans up all CMS resources in use.
*               :
*  PARAMETERS   :   None
*               :
*  RETURNS      :   None
*****************************************************************************/

static void Laserd_Cms_Cleanup(void)
{
    cmsMdm_cleanup();
    cmsMsg_cleanup(&msgHandle);
    cmsLog_cleanup();
}

/*****************************************************************************
*  FUNCTION     :   
*               :
*  DESCRIPTION  :
*               :
*  PARAMETERS   :
*               :
*  RETURNS      :
*****************************************************************************/
int main(int argc, char **argv)
{
    CmsRet CmsRetVal;
    int RetVal = 0;
    struct timeval tm;
    UINT8 stopFlag = FALSE;

    CmsRetVal = Laserd_Cms_Init();

    // Setup select timeout rec.
    tm.tv_sec = 10*MSECS_IN_SEC;
    tm.tv_usec = 0;


    if (CmsRetVal == CMSRET_SUCCESS)
    {    
        RetVal = Laserd_Init_Optics();
        
        if (0 == RetVal)
        {
            // Set up FD for select.
            FD_ZERO(&readFdsMaster);
            FD_SET(msgFd, &readFdsMaster);

            // Test for success.
            if (CmsRetVal == CMSRET_SUCCESS)
            {
                // Loop until system reset or power-down (or until error).
                while (stopFlag == FALSE)
                {
                    // Setup Fds copy.
                    readFds = readFdsMaster;

                    // Wait for system timeout or error.
                    RetVal = select(msgFd + 1, &readFds, NULL, NULL, &tm);

                    // Test for select() error.
                    if (RetVal < 0)
                    {
                        // Continue with loop on select() error.
                        continue;
                    }

                    // Get global current log level.
    //                crntLogLevel = cmsLog_getLevel();

                    // Test for incoming message.
                    if (FD_ISSET(msgFd, &readFds))
                    {
                        // Attempt to receive message (with timeout).
                        if ((CmsRetVal = cmsMsg_receiveWithTimeout(msgHandle, &pMsg, 500)) != CMSRET_SUCCESS)
                        {
                            // Signal error.
                            cmsLog_error(LASERD": Failed to receive message, CmsRetVal: %d", CmsRetVal);
                        }
                        else
                        {
                            // Switch on message type.
                            switch (pMsg->type)
                            {
                                case CMS_MSG_LASERD_LOAD_PARAMS:
                                {
                                    if (LASERD_OPTICS_TYPE_BOSA==GponOpticsType)
                                    {
                                        Laserd_Load_Optical_Params(pMsg->wordData, ((char *)pMsg)+sizeof(CmsMsgHeader));
                                    }
                                    else
                                    {
                                        cmsLog_error(LASERD": command not supported for non BOSA GPON optics\n");
                                    }
                                    break;
                                }
                                case CMS_MSG_LASERD_STORE_PARAMS:
                                {
                                    if (LASERD_OPTICS_TYPE_BOSA==GponOpticsType)
                                    {
                                        Laserd_Store_Optical_Params(pMsg->wordData, ((char *)pMsg)+sizeof(CmsMsgHeader));
                                    }
                                    else
                                    {
                                        cmsLog_error(LASERD": command not supported for non BOSA GPON optics\n");
                                    }
                                    break;
                                }
                                case CMS_MSG_LASERD_CLEAR_PARAMS:
                                {
                                    if (LASERD_OPTICS_TYPE_BOSA==GponOpticsType)
                                    {
                                        Laserd_Clear_Optical_Params(pMsg->wordData);
                                    }
                                    else
                                    {
                                        cmsLog_error(LASERD": command not supported for non BOSA GPON optics\n");
                                    }
                                    break;
                                }
                                case CMS_MSG_LASERD_DUMP_PARAMS:
                                {
                                    if (LASERD_OPTICS_TYPE_BOSA==GponOpticsType)
                                    {
                                        Laserd_Dump_Optical_Params(pMsg->wordData);
                                    }
                                    else
                                    {
                                        cmsLog_error(LASERD": command not supported for non BOSA GPON optics\n");
                                    }
                                    break;
                                }
                                default:
                                    cmsLog_error(LASERD": Invalid message type: 0x%08X", (unsigned int)pMsg->type);
                                    break;
                            }

                            // Release message buffer.
                            CMSMEM_FREE_BUF_AND_NULL_PTR(pMsg);
                        }
                    }
                }
            }
            else
            {
                // Signal CMS timer error.
                cmsLog_error(LASERD": cmsTmr_init failed, CmsRetVal: %d", CmsRetVal);
            }            
        }// end laser optics init OK
    }// end CMS init OK
    else
    {
        // CMS init failed.
        cmsLog_error(LASERD": CMS resource init failed, CmsRetVal: %d", CmsRetVal);
    }

    Laserd_Cms_Cleanup();

    if (LaserDevFd >=0)
    {
        close(LaserDevFd);
    }

    return 0;
}

