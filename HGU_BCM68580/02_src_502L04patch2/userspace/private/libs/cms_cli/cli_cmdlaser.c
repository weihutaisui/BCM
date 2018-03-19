/***********************************************************************
<:copyright-BRCM:2012:proprietary:standard 

   Copyright (c) 2012 Broadcom 
   All Rights Reserved

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

/** command driven CLI code goes into this file */

#ifdef SUPPORT_CLI_CMD

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

/* shared/opensource/include/bcm963xx is not in the build path */
//#include <boardparms.h>
#define BP_OPTICAL_PARAMS_LEN 48

#include "cms.h"
#include "cms_log.h"
#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"
#include "cms_boardcmds.h"
#include "cli.h"
#include "laser.h"
#include "pmd.h"

#define base(x) ((x >= '0' && x <= '9') ? '0' : \
    (x >= 'a' && x <= 'f') ? 'a' - 10 : \
    (x >= 'A' && x <= 'F') ? 'A' - 10 : \
    '\255')

#define TOHEX(x) (x - base(x))


#define MAX_OPTS 8

/* Kernel Driver Interface Function Prototypes */
extern int devCtl_getGponOpticsType(void);
extern int devCtl_getDefaultOpticalParams(unsigned char *);
static int devLaser_Get_Default_Optical_Params(unsigned char * pOpticalParams);
static int devLaser_Read_Optical_Params_From_Optics(
    unsigned char *pOpticalParams, short len);
static int devLaser_Write_Optical_Params_To_Optics(
    unsigned char *pOpticalParams, short len);
static int devLaser_Read(unsigned long ioctl_cmd, unsigned long *value);
static CmsRet cmdLaserGeneralParamsSet(char *pszclient, char *pszoffset, char *pszlen, unsigned char *pszbuf);
static CmsRet cmdLaserGeneralParamsGet(char *pszclient, char *pszoffset, char *pszlen);
static CmsRet cmdLaserDumpData(void);
static CmsRet cmdLaserCalSet(char *pszparam, char *pszval, char *pszval1, char *pszval2, char *pszval3);
static CmsRet cmdLaserCalGet(char *pszparam, char *pszval);
static CmsRet cmdLaserMsgWrite(char *pszmsg_id,  char *pszlen, unsigned char *pszbuf);
static CmsRet cmdLaserMsgRead(char *pszmsg_id, char *pszlen);
static CmsRet cmdRssiCalSet(char *pszrssi,  char *pszop);
static CmsRet cmdLaserTempToApdSet(char *ApdList);


/*****************************************************************************
*
*  FUNCTION     :   cmdLaserHelp
*  DESCRIPTION  :   Prints help information about the Laser commands.
*  PARAMETERS   :   None
*  RETURNS      :   0 on success
*
*****************************************************************************/

static void cmdLaserHelp(char *help)
{
    static const char laserusage[] = "\nUsage:\n";
    static const char laserload[] =
        "        laser param --load [<filename>]\n";
    static const char laserdump[] =
        "        laser param --dump default | current\n";
    static const char laserpower[] =
        "        laser power\n";
    static const char laserpowerrxread[] =
        "        laser power --rxread\n";
    static const char laserpowertxread[] =
        "        laser power --txread\n";
    static const char lasertxbiasread[] =
        "        laser txbias      --read\n";
    static const char lasertempread[] =
        "        laser temperature --read\n";
    static const char laservoltageread[] =
        "        laser voltage     --read\n";
    static const char laserGeneralSet[] =
        "        laser general     --set   [client{0-REG,1-IRAM,2-DRAM} offset len{dec} data]\n";
    static const char laserGeneralGet[] =
        "        laser general     --get   [client{0-REG,1-IRAM,2-DRAM} offset len{dec}]\n";
    static const char laserCalSet[] =
        "        laser calibration --set   [parameter value]\n";
    static const char laserCalGet[] =
        "        laser calibration --get   [parameter]\n";

    static const char laserMsgSet[] =
        "        laser msg         --set   [msg_id len{number of dec bytes} data]\n";
    static const char laserMsgGet[] =
        "        laser msg         --get   [msg_id len {dec}]\n";
    static const char laserRssiCal[] =
        "        laser rssi cal set [rssi optic_power]\n";
    static const char laserTempToApdMap[] =
        "        laser temp2apd --set [parameters]\n";
    static const char laserDumpData[] =
            "        laser dumpdata --show\n";

   if(help == NULL || strcasecmp(help, "--help") == 0)
   {
       printf("%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s\n", laserusage, laserload, laserdump, laserpower, laserpowerrxread, laserpowertxread,
               lasertxbiasread, lasertempread, laservoltageread, laserGeneralSet, laserGeneralGet, laserCalSet, laserCalGet, laserMsgSet,
               laserMsgGet, laserRssiCal, laserTempToApdMap,laserDumpData);
   }
   else if(strcasecmp(help, "load") == 0)
   {
       printf("%s%s", laserusage, laserload);
   }
   else if(strcasecmp(help, "dump") == 0)
   {
       printf("%s%s", laserusage, laserdump);
   }
   else if(strcasecmp(help, "powerrxread") == 0)
   {
       printf("%s%s", laserusage, laserpowerrxread);
   }
   else if(strcasecmp(help, "powertxread") == 0)
   {
       printf("%s%s", laserusage, laserpowertxread);
   }
   else if(strcasecmp(help, "txbias") == 0)
   {
       printf("%s%s", laserusage, lasertxbiasread);
   }
   else if(strcasecmp(help, "temperature") == 0)
   {
       printf("%s%s", laserusage, lasertempread);
   }
   else if(strcasecmp(help, "voltage") == 0)
   {
       printf("%s%s", laserusage, laservoltageread);
   }
   else if(strcasecmp(help, "general") == 0)
   {
       printf("%s%s", laserusage, laserGeneralSet);
   }
   else if(strcasecmp(help, "msg") == 0)
   {
       printf("%s%s", laserusage, laserMsgSet);
   }
   else if(strcasecmp(help, "calibration") == 0)
   {
       printf("%s%s", laserusage, laserCalSet);
   }
   else if(strcasecmp(help, "rssi cal set") == 0)
   {
       printf("%s%s", laserusage, laserRssiCal);
   }
   else if(strcasecmp(help, "temp2apd") == 0)
   {
       printf("%s%s", laserusage, laserTempToApdMap);
   }
   else if(strcasecmp(help, "dumpdata") == 0)
   {
       printf("%s%s", laserusage, laserDumpData);
   }
}


/*****************************************************************************
*
*  FUNCTION     :   cmdLaserLoad
*  DESCRIPTION  :   laser load [<filenamearg>]
*               :   Causes optical parameters to be loaded onto BOSA optics 
*               :   device, if present.  Default is to load params from NVRAM
*               :   source, but options can cause loading from external file.
*  PARAMETERS   :   
*  RETURNS      :   0 on success
*
*****************************************************************************/
static CmsRet cmdLaserLoad(char *pFileNameArg)
{
    CmsRet Ret = CMSRET_SUCCESS;
    unsigned char ucOpticalParams[BP_OPTICAL_PARAMS_LEN];

    if( pFileNameArg == NULL )
    {
        /* If a file name is not specified, get the default optical params. */
        printf("Loading default parameters into laser driver chip.\n");
        if(devLaser_Get_Default_Optical_Params(ucOpticalParams) != 0)
        {
            printf("Cannot read default optical parameters.\n");
            Ret = CMSRET_INVALID_ARGUMENTS;
        }
    }
    else
    {
        /* Read the default optical parameters from a file.  The file format is
         * <hex_offset_0x00_0x2f> <hex_value_0x00_0xff>
         */
        FILE *fp = fopen(pFileNameArg, "r");

        if( fp )
        {
            char buf[16];
            char *p;
            long idx;

            printf("Loading parameters from file %s into laser driver chip.\n",
                pFileNameArg);
            while( fgets(buf, sizeof(buf), fp) )
            {
                p = NULL;
                idx = strtol(buf, &p, 16);
                if( p && idx < sizeof(ucOpticalParams) )
                    ucOpticalParams[idx] = strtol(p, NULL, 16) & 0xff;
            }

            fclose( fp );
        }
        else
        {
            printf("Cannot open parameters file %s\n", pFileNameArg);
            Ret = CMSRET_INVALID_ARGUMENTS;
        }
    }

    if( Ret == CMSRET_SUCCESS )
    {
        if( devLaser_Write_Optical_Params_To_Optics(ucOpticalParams,
            sizeof(ucOpticalParams)) != 0 )
        {
            printf("Cannot write optical parameters to laser device.\n");
            Ret = CMSRET_INTERNAL_ERROR;
        }
    }

    return Ret;
}

/*****************************************************************************
*
*  FUNCTION     :   cmdLaserDump
*  DESCRIPTION  :   laser param --dump [r|n]
*               :   Causes optical parameters to be dumped to the console.
*  PARAMETERS   :   
*  RETURNS      :   0 on success
*
*****************************************************************************/
static CmsRet cmdLaserDump(char *pDumpArg)
{
    CmsRet Ret = CMSRET_SUCCESS;
    unsigned char ucOpticalParams[LASER_TOTAL_OPTICAL_PARAMS_LEN];
    int len;

    if( !strcmp(pDumpArg, "default") )
    {
        /* Get the default optical parameters. Only BP_OPTICAL_PARAMS_LEN
         * parameters are saved for progamming the laser driver chip.
         */
        len = BP_OPTICAL_PARAMS_LEN;
        if(devLaser_Get_Default_Optical_Params(ucOpticalParams) != 0)
            Ret = CMSRET_INVALID_ARGUMENTS;
    }
    else if( !strcmp(pDumpArg, "current") )
    {
        /* Read all (R/W and R only) optical parameters from the laser driver
         * chip.
         */
        len = LASER_TOTAL_OPTICAL_PARAMS_LEN;
        if(devLaser_Read_Optical_Params_From_Optics(ucOpticalParams,
            sizeof(ucOpticalParams)) != 0)
        {
            Ret = CMSRET_INVALID_ARGUMENTS;
        }
    }
    else
    {
        Ret = CMSRET_INVALID_ARGUMENTS;
        cmsLog_error("Invalid arguments\n");
        cmdLaserHelp("load");
    }

    if( Ret == CMSRET_SUCCESS )
    {
        int i;
        for( i = 0; i < len; i++ )
            printf("0x%2.2x 0x%2.2x\n", i, ucOpticalParams[i]);
    }

    return Ret;
}

/*****************************************************************************
*
*  FUNCTION     :   cmdLaserPowerRead
*  DESCRIPTION  :   Sends a command to the laser driver to read and calculate
*               :   the current Rx or Tx power in micro-watts.
*  PARAMETERS   :   
*  RETURNS      :   0 on success
*
*****************************************************************************/
static CmsRet cmdLaserPowerRead(int ioctl_cmd, unsigned long *pPower)
{
    CmsRet Ret = CMSRET_SUCCESS;

    if( devLaser_Read(ioctl_cmd, pPower) != 0 )
        Ret = CMSRET_INVALID_ARGUMENTS;

    if( Ret != CMSRET_SUCCESS )
    {
        // Invalid argument
        cmsLog_error("Power not read\n");
        cmdLaserHelp((ioctl_cmd == LASER_IOCTL_GET_RX_PWR)
            ? "powerrxread" : "powertxread" );
    }

    return( Ret );
}



/*****************************************************************************
*
*  FUNCTION     :   cmdLaserRead
*  DESCRIPTION  :   Sends a command to the laser driver to read
*               :   the current Tx Bias / Temperature / Voltage.
*  PARAMETERS   :   
*  RETURNS      :   0 on success
*
*****************************************************************************/
static CmsRet cmdLaserRead(int ioctl_cmd, unsigned long *pRead)
{
    CmsRet Ret = CMSRET_SUCCESS;

    if( devLaser_Read(ioctl_cmd, pRead) != 0 )
        Ret = CMSRET_INVALID_ARGUMENTS;

    if( Ret != CMSRET_SUCCESS )
    {
        cmsLog_error("Current status not read\n");
    }

    return( Ret );
}

/***************************************************************************
 * Function Name: processLaserCmd
 * Description  : Parses CMF commands
 ***************************************************************************/
void processLaserCmd(char *cmdLine)
{
    SINT32 argc = 0;
    char *argv[MAX_OPTS]={NULL};
    char *last = NULL;
    CmsRet ret = CMSRET_SUCCESS;

    /* parse the command line and build the argument vector */
    argv[0] = strtok_r(cmdLine, " ", &last);

    if(argv[0] != NULL)
    {
        for(argc=1; argc<MAX_OPTS; ++argc)
        {
            argv[argc] = strtok_r(NULL, " ", &last);

            if (argv[argc] == NULL)
            {
                break;
            }

            cmsLog_debug("arg[%d]=%s", argc, argv[argc]);
        }
    }

    if (argv[0] == NULL)
    {
        cmdLaserHelp(NULL);
    }
    else if(strcasecmp(argv[0], "--help") == 0)
    {
        cmdLaserHelp(argv[0]);
    }
    else if(strcasecmp(argv[0], "param") == 0)
    {
        if (NULL != argv[1])
        {
            if(strcasecmp(argv[1], "--load") == 0)
            {
                ret = cmdLaserLoad(argv[2]);
            }
            else if(strcasecmp(argv[1], "--dump") == 0)
            {
                ret = cmdLaserDump(argv[2]);
            }
            else
            {
                cmsLog_error("Invalid Laser Command\n");
                cmdLaserHelp(NULL);
            } 
            if (ret != CMSRET_SUCCESS)
            {
                cmsLog_error("Unknown Error\n");
            }
        }
        else
        {
            cmsLog_error("Invalid Laser Command\n");
            cmdLaserHelp(NULL);
        }
    }
    else if(strcasecmp(argv[0], "power") == 0)
    {
        unsigned long power;

        printf("\n");
        if(argv[1] == NULL)
        {
            ret = cmdLaserPowerRead(LASER_IOCTL_GET_RX_PWR, &power);
            if( ret == CMSRET_SUCCESS )
                printf("Rx Laser Power           = %lu uW\n", power);

            ret = cmdLaserPowerRead(LASER_IOCTL_GET_TX_PWR, &power);
            if( ret == CMSRET_SUCCESS )
                printf("Tx Laser Power           = %lu uW\n", power);
        }
        else if(strcasecmp(argv[1], "--rxread") == 0)
        {
            ret = cmdLaserPowerRead(LASER_IOCTL_GET_RX_PWR, &power);
            if( ret == CMSRET_SUCCESS )
            {
                printf("Rx Laser Power           = %f uW\n", (float)(((float) power)/10));
                printf("                         = %f dBm\n", (float)
                    (10.0 * (log10(((float) power) / 10000.0))));
            }
        }
        else if(strcasecmp(argv[1], "--txread") == 0)
        {
            ret = cmdLaserPowerRead(LASER_IOCTL_GET_TX_PWR, &power);
            if( ret == CMSRET_SUCCESS )
                printf("Tx Laser Power           = %f uW\n", (float)(((float) power)/10));
                printf("                         = %f dBm\n", (float)
                    (10.0 * (log10(((float) power) / 10000.0))));
        }
        else
        {
            cmsLog_error("Invalid Laser Command\n");
            cmdLaserHelp(NULL);
        }
        printf("\n");
    }
    else if(strcasecmp(argv[0], "txbias") == 0)
    {
        unsigned long bias;

        if (NULL != argv[1])
        {
            if(strcasecmp(argv[1], "--read") == 0)
            {
                ret = cmdLaserRead(LASER_IOCTL_GET_BIAS_CURRENT, &bias);
                if( ret == CMSRET_SUCCESS )
                    printf("Tx Bias Current          = %ld uA\n", bias*2);
            }
            else
            {
                cmsLog_error("Invalid Laser Command\n");
                cmdLaserHelp(NULL);
            }           
        }
        else
        {
            cmsLog_error("Invalid Laser Command\n");
            cmdLaserHelp(NULL);
        }
    }
    else if(strcasecmp(argv[0], "voltage") == 0)
    {
        unsigned long voltage;

        if (NULL != argv[1])
        {
            if(strcasecmp(argv[1], "--read") == 0)
            {
                ret = cmdLaserRead(LASER_IOCTL_GET_VOTAGE, &voltage);
                if( ret == CMSRET_SUCCESS )
                    printf("Voltage                  = %ld mV\n", voltage/10);
            }
            else
            {
                cmsLog_error("Invalid Laser Command\n");
                cmdLaserHelp(NULL);
            }           
        }
        else
        {
            cmsLog_error("Invalid Laser Command\n");
            cmdLaserHelp(NULL);
        }
    }
    else if(strcasecmp(argv[0], "temperature") == 0)
    {
        unsigned long temp;

        if (NULL != argv[1])
        {
            if(strcasecmp(argv[1], "--read") == 0)
            {
                ret = cmdLaserRead(LASER_IOCTL_GET_TEMPTURE, &temp);
                if( ret == CMSRET_SUCCESS )
                    printf("Temperature              = %ld c\n", (temp >> 8) & 0xff);
            }
            else
            {
                cmsLog_error("Invalid Laser Command\n");
                cmdLaserHelp(NULL);
            }           
        }
        else
        {
            cmsLog_error("Invalid Laser Command\n");
            cmdLaserHelp(NULL);
        }
    }
    else if(strcasecmp(argv[0], "general") == 0)
    {
        if (NULL != argv[1])
        {
            if(strcasecmp(argv[1], "--set") == 0)
            {
                ret = cmdLaserGeneralParamsSet(argv[2], argv[3], argv[4], (unsigned char *)argv[5]);

                if( ret == CMSRET_SUCCESS )
                {
                    printf("-Set general parameters done \n");
                }
            }
            else if(strcasecmp(argv[1], "--get") == 0)
            {
                ret = cmdLaserGeneralParamsGet(argv[2], argv[3], argv[4]);
            }
            else
            {
                cmsLog_error("Invalid Laser Command\n");
                cmdLaserHelp(NULL);
            }
            if (ret != CMSRET_SUCCESS)
            {
                cmsLog_error("Unknown Error\n");
            }
        }
        else
        {
            cmsLog_error("Invalid Laser Command\n");
            cmdLaserHelp(NULL);
        }
    }
    else if(strcasecmp(argv[0], "calibration") == 0)
    {
        if (NULL != argv[1])
        {       
            if(strcasecmp(argv[1], "--set") == 0)
            {
                ret = cmdLaserCalSet(argv[2], argv[3], argv[4], argv[5], argv[6]);
                if( ret == CMSRET_SUCCESS )
                {
                    printf("-file write done \n");
                }
            }
	    else if(strcasecmp(argv[1], "--get") == 0)
            {
                ret = cmdLaserCalGet(argv[2], argv[3]);

                if( ret == CMSRET_SUCCESS )
                {
                    printf("-file read done \n");
                }
            }
			else if(strcasecmp(argv[1], "?") == 0)
			{
			    printf("0:watermark 1:version 2:level_0 3:level_1 4:bias 5:mod 6:apd {type voltage} "
			            "7:mpd config{dir tia_gain vga_gain} 8:mpd gains {bias mod} 9:apdoi_ctrl\n");
			    printf("10:rssi_a_factor 11:rssi_b_factor 12:rssi_c_factor 13: temp_0 14: temp coff high "
			            "15: temp coff low 16:esc thr 17:rogue thr 18:avg_level_0 19:avg_level_1 \n");
			    printf("20:dacrange 21: los thr {assert desassert} 22: sat pos {high low} 23: sat neg {high low} 24: edge {rate dload} 25: preemphasis weight\n");
			    printf("26:preemphasis_delay 27:duty_cycle 28: mpd_calibctrl\n");
			    printf("29:tx_power 30: bias0 31:temp_offset \n");
			}
			else
			{
				cmsLog_error("Invalid Laser Command\n");
				cmdLaserHelp(NULL);
			}
            if(ret != CMSRET_SUCCESS)
            {
                cmsLog_error("Unknown Error\n");
            }
        }
        else
        {
            cmsLog_error("Invalid Laser Command\n");
            cmdLaserHelp(NULL);
        }
    }
    else if(strcasecmp(argv[0], "msg") == 0)
    {
        if (NULL != argv[1])
        {
            if(strcasecmp(argv[1], "--get") == 0)
            {
                ret = cmdLaserMsgRead(argv[2], argv[3]);

                if( ret == CMSRET_SUCCESS )
                {
                    printf("-msg read done \n");
                }
            }
            else if(strcasecmp(argv[1], "--set") == 0)
            {
                ret = cmdLaserMsgWrite(argv[2], argv[3], (unsigned char *)argv[4]);
                if( ret == CMSRET_SUCCESS )
                {
                    printf("-msg write done \n");
                }
            }
            else
            {
                cmsLog_error("Invalid Laser Command\n");
                cmdLaserHelp(NULL);
            }
            if (ret != CMSRET_SUCCESS)
            {
                cmsLog_error("Unknown Error\n");
            }
        }
        else
        {
            cmsLog_error("Invalid Laser Command\n");
            cmdLaserHelp(NULL);
        }
    }
    else if( strcasecmp(argv[0], "rssi") == 0 && strcasecmp(argv[1], "cal") == 0 && strcasecmp(argv[2], "set") == 0 )
    {
        ret = cmdRssiCalSet(argv[3], argv[4]);
        if( ret == CMSRET_SUCCESS )
        {
            printf("-RSSI Calibration write done \n");
        }
    }
    else if(strcasecmp(argv[0], "temp2apd") == 0)
    {
        if (NULL != argv[1])
        {
            if(strcasecmp(argv[1], "--get") == 0)
            {
                /*ret = cmdLaserMsgRead(argv[2], argv[3]);

                if( ret == CMSRET_SUCCESS )
                {
                    printf("-msg read done \n");
                }*/
            }
            else if(strcasecmp(argv[1], "--set") == 0)
            {
                ret = cmdLaserTempToApdSet(argv[2]);
                if( ret == CMSRET_SUCCESS )
                {
                    printf("temp2apd set done \n");
                }
            }
            else
            {
                cmsLog_error("Invalid Laser Command\n");
                cmdLaserHelp(NULL);
            }

            if (ret != CMSRET_SUCCESS)
            {
                cmsLog_error("Unknown Error\n");
            }
        }
        else
        {
            cmsLog_error("Invalid Laser Command\n");
            cmdLaserHelp(NULL);
        }
    }
    else if(strcasecmp(argv[0], "dumpdata") == 0)
    {
    	ret = cmdLaserDumpData();
    	if (ret == CMSRET_SUCCESS)
    	{
    	    if(argv[1] != NULL)
    	    {
                if(strcasecmp(argv[1], "--show") == 0)
                    system("cat data/pmd_dump_data");
    	    }
        }
        else
            cmsLog_error("Unknown Error\n"); 
    }
    else
    {
        cmsLog_error("Invalid Laser Command\n");
        cmdLaserHelp(NULL);
    }
}


/*****************************************************************************
 * Kernel Driver Interface Functions
 *****************************************************************************/

/*****************************************************************************
*
*  FUNCTION     :   devLaser_Get_Default_Optical_Params
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
static int devLaser_Get_Default_Optical_Params(unsigned char * pOpticalParams)
{
    return (devCtl_getDefaultOpticalParams(pOpticalParams));   
}

/*****************************************************************************
*
*  FUNCTION     :   devLaser_Read_Optical_Params_From_Optics
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
static int devLaser_Read_Optical_Params_From_Optics(
    unsigned char *pOpticalParams, short len)
{
    int ret = -1;
    int LaserDevFd = open(LASER_DEV, O_RDWR);

    if( LaserDevFd >= 0)
    {
        LASER_OPTICAL_PARAMS lop = {len, pOpticalParams};
        ret = ioctl(LaserDevFd, LASER_IOCTL_GET_OPTICAL_PARAMS, &lop);
        close(LaserDevFd);
    }

    return( ret );
}

/*****************************************************************************
*  FUNCTION     :   devLaser_Write_Optical_Params_To_Optics
*               :
*  DESCRIPTION  :   This routine is used to write the contents of the optical
*               :   parameter DB to registers in the optics device
*               :
*  PARAMETERS   :   IN: pOpticalParams contains address of char array of params
*               :   to be written to NVRAM.
*               :
*  RETURNS      :   0 on success
*****************************************************************************/
static int devLaser_Write_Optical_Params_To_Optics(
    unsigned char *pOpticalParams, short len)
{
    int ret = -1;
    int LaserDevFd = open(LASER_DEV, O_RDWR);

    if( LaserDevFd >= 0)
    {
        LASER_OPTICAL_PARAMS lop = {len, pOpticalParams};
        ret = ioctl(LaserDevFd, LASER_IOCTL_SET_OPTICAL_PARAMS, &lop);
        close(LaserDevFd);
    }

    return( ret );
}

/*****************************************************************************
*  FUNCTION     :   devLaser_Read
*               :
*  DESCRIPTION  :   This routine is used to read the current Rx or Tx power
*               :   value.
*               :
*  PARAMETERS   :   IN: IOCTL command (Rx or Tx) and pointer to a word to
*               :   hold the return value
*               :
*  RETURNS      :   0 on success
*****************************************************************************/
static int devLaser_Read(unsigned long ioctl_cmd, unsigned long *value)
{
    int ret = -1;
    int LaserDevFd = open(LASER_DEV, O_RDWR);

    if( LaserDevFd >= 0)
    {
        ret = ioctl(LaserDevFd, ioctl_cmd, value);
        close(LaserDevFd);
    }

    return( ret );
}

/*****************************************************************************
*  FUNCTION     :   devLaser_SetGeneralParams
*               :
*  DESCRIPTION  :
*               :
*               :
*  PARAMETERS   :
*               :
*  RETURNS      :   0 on success
*****************************************************************************/
static int devLaser_SetGeneralParams(uint8_t client, uint16_t offset, unsigned char *buf, uint16_t len)
{
	pmd_params param = {.client = client, .offset = offset, .len = len, .buf = buf};

	int ret = -1;

	int LaserDevFd = open(LASER_DEV, O_RDWR);

    if( LaserDevFd >= 0)
    {
        ret = ioctl(LaserDevFd, PMD_IOCTL_SET_PARAMS, &param);
        close(LaserDevFd);
    }

    return( ret );
}

/*****************************************************************************
*  FUNCTION     :   devLaser_GetGeneralParams
*               :
*  DESCRIPTION  :
*               :
*               :
*  PARAMETERS   :
*               :
*  RETURNS      :   0 on success
*****************************************************************************/
static int devLaser_GetGeneralParams(uint8_t client, uint16_t offset, uint16_t len)
{
	unsigned char buf[PMD_BUF_MAX_SIZE];
    pmd_params param = {.client = client, .offset = offset, .len = len, .buf = buf};
	int ret = -1;
	uint16_t count = 0;

	int LaserDevFd = open(LASER_DEV, O_RDWR);

    if( LaserDevFd >= 0)
    {
        ret = ioctl(LaserDevFd, PMD_IOCTL_GET_PARAMS, &param);
        close(LaserDevFd);
    }

    if ( !ret)
    {
        while( count < len)
        {
            if (count && count % 4 == 0)
                printf(" ");

            printf("%02x", param.buf[count]);
            count++;
        }
        printf("\n");
    }

    return( ret );
}

/*****************************************************************************
*  FUNCTION     :   devLaser_SetCalParams
*               :
*  DESCRIPTION  :
*               :
*               :
*  PARAMETERS   :
*               :
*  RETURNS      :   0 on success
*****************************************************************************/
static int devLaser_SetCalParams(uint16_t offset, int32_t val, uint16_t cal_index)
{
    pmd_params param = {.offset = offset, .len = cal_index, .buf = (unsigned char *)&val};

    int ret = -1;

    int LaserDevFd = open(LASER_DEV, O_RDWR);

    if( LaserDevFd >= 0)
    {
        ret = ioctl(LaserDevFd, PMD_IOCTL_CAL_FILE_WRITE, &param);
        close(LaserDevFd);
    }

    return( ret );
}


/*****************************************************************************
*  FUNCTION     :   devLaser_GetCalParams
*               :
*  DESCRIPTION  :
*               :
*               :
*  PARAMETERS   :
*               :
*  RETURNS      :   0 on success
*****************************************************************************/
static int devLaser_GetCalParams(uint16_t offset, int32_t cal_index)
{
    int32_t val = 0;
    pmd_params param = {.offset = offset , .len = cal_index, .buf = (unsigned char *)&val};
    int ret = -1;

    int LaserDevFd = open(LASER_DEV, O_RDWR);

    if( LaserDevFd >= 0)
    {
        ret = ioctl(LaserDevFd, PMD_IOCTL_CAL_FILE_READ, &param);
        close(LaserDevFd);
    }

    if (!ret)
    {
        if (val == CAL_FILE_INVALID_ENTRANCE)
        {
            printf("Parameter is invalid \n");
            return( ret );
        }

        if( offset == pmd_mpd_gains )
        {
            float bias_gain, mod_gain;

            bias_gain = (float)(val & 0xff)/ 256;
            mod_gain  = (float)(val >> 8 )/ 256;

            printf("bias gain %f mod gain %f\n",bias_gain, mod_gain);
        }
        else if (offset == pmd_apd )
        {
            uint16_t type, voltage;
            type = (val & 400) >> 10;
            voltage = val & 0x3ff;

            printf("apd type %d apd voltage %02x\n",type, voltage);
        }
        else if( offset == pmd_mpd_config )
        {
            uint16_t dir, tia, vga;
            dir = (val & 0x200) >> 9;
            vga = (val & 0xc00) >> 10;
            tia = (val & 0xF000) >> 12;

            printf("dir %d tia %d vga %d \n",dir, tia, vga);

        }
        else if (offset == pmd_rssi_a || offset == pmd_rssi_b || offset == pmd_rssi_c)
        {
            float rssi = (float)val / 256;
            printf("%f\n", rssi);
            return ret;
        }
        else if( offset == pmd_temp_coff_h )
        {

            float alph_h = (float)val / 4096;
            printf("%f \n", alph_h);
            return ret;
        }
        else if( offset == pmd_temp_coff_l )
        {
            float alph_l = (float)val / 4096;
            printf("%f \n", alph_l);
            return ret;
        }
        else if( offset == pmd_dacrange )
        {
            printf("dacrange %x\n",val & 0x3);
        }
        else if( offset == pmd_los_thr )
        {
            uint16_t assert, desassert;
            assert = val & 0xff;
            desassert = (val & 0xff00) >> 8;

            printf("assert %x desassert %x \n",assert, desassert);

        }
        else if( offset == pmd_sat_pos )
        {
            uint16_t high, low;
            high = val & 0xff;
            low = (val & 0xff00) >> 8;

            printf("high %x low %x \n",high, low);

        }
        else if( offset == pmd_sat_neg )
        {
            uint16_t high, low;
            high = val & 0xff;
            low = (val & 0xff00) >> 8;

            printf("high %x low %x \n",high, low);
        }
        else if(offset == pmd_edge_rate)
        {
            uint16_t rate, dload;
            rate = val & 0xff;
            dload = (val & 0xff00) >> 8;

            printf("edge rate %x dload rate %x \n",rate, dload);
        }
        else
            printf("%02x\n", val);

    }

    return( ret );
}

/*****************************************************************************
*  FUNCTION     :   devLaser_MsgWrite
*               :
*  DESCRIPTION  :
*               :
*               :
*  PARAMETERS   :
*               :
*  RETURNS      :   0 on success
*****************************************************************************/
static int devLaser_MsgWrite(uint16_t msg_id, uint16_t len, unsigned char *buf)
{
    pmd_params param = { .offset = msg_id, .len = len, .buf = buf};

    int ret = -1;

    int LaserDevFd = open(LASER_DEV, O_RDWR);

    if( LaserDevFd >= 0)
    {
        ret = ioctl(LaserDevFd, PMD_IOCTL_MSG_WRITE, &param);
        close(LaserDevFd);
    }

    return( ret );
}

/*****************************************************************************
*  FUNCTION     :   devLaser_MsgRead
*               :
*  DESCRIPTION  :
*               :
*               :
*  PARAMETERS   :
*               :
*  RETURNS      :   0 on success
*****************************************************************************/
static int devLaser_MsgRead(uint16_t msg_id, uint16_t len)
{
    unsigned char buf[PMD_BUF_MAX_SIZE];
    pmd_params param = { .offset = msg_id, .len = len, .buf = buf};
    int ret = -1;
    uint16_t count = 0;
    int16_t tmp_16;
    int32_t tmp_32;
    float output;

    int LaserDevFd = open(LASER_DEV, O_RDWR);

    if( LaserDevFd >= 0)
    {
        ret = ioctl(LaserDevFd, PMD_IOCTL_MSG_READ, &param);
        close(LaserDevFd);
    }

    if(!ret)
    {
        if(msg_id  == PMD_RSSI_GET_MSG)
        {
            tmp_32 = *(int32_t *)(param.buf);
            tmp_32 = ((tmp_32 & (int32_t)0x000000ffUL) << 24) | ((tmp_32 & (int32_t)0x0000ff00UL) <<  8) |
            		 ((tmp_32 & (int32_t)0x00ff0000UL) >>  8) | (((uint32_t)(tmp_32 & (int32_t)0xff000000UL)) >> 24);
            output = (float)tmp_32 / 65536;
            printf("%f\n", output);
            return ret;
        }
        if(msg_id  == PMD_ESTIMATED_OP_GET_MSG)
        {
            tmp_16 = *(int16_t *)(param.buf);
            tmp_16 = ((tmp_16 & (int16_t)0x00ffU) << 8) | (((uint16_t)(tmp_16 & (int16_t)0xff00U)) >> 8);
            output = (float)tmp_16 / 16;
            printf("%f uW\n", output);
            return ret;
        }
        while( count < len)
        {
            if (count && count % 4 == 0)
                printf(" ");

            printf("%02x", param.buf[count]);
            count++;
        }
        printf("\n");
    }

    return( ret );
}

/*****************************************************************************
*
*  FUNCTION     :   cmdLaserGeneralParamsSet
*  DESCRIPTION  :
*               :
*               :
*  PARAMETERS   :
*  RETURNS      :   0 on success
*
*****************************************************************************/
static CmsRet cmdLaserGeneralParamsSet(char *pszclient, char *pszoffset, char *pszlen, unsigned char *pszbuf)
{
    CmsRet Ret = CMSRET_SUCCESS;
    int i;

    if( pszclient && pszoffset && pszlen && pszbuf)
    {
    	uint8_t client        = (uint8_t) strtol( pszclient, NULL, 16 );
        unsigned short offset = (unsigned short) strtol( pszoffset, NULL, 16 );
        unsigned short len    = (unsigned short) strtol( pszlen, NULL, 10 );

        len = len < PMD_BUF_MAX_SIZE ? len : PMD_BUF_MAX_SIZE;

        for( i=0; i<len; i++)
        {
        	pszbuf[i*2]=TOHEX(pszbuf[i*2]);
        	pszbuf[i*2+1]=TOHEX(pszbuf[i*2+1]);
        	pszbuf[i]=((pszbuf[i*2] <<4)+pszbuf[i*2+1]);
        }

        if( devLaser_SetGeneralParams(client, offset, pszbuf, len) != 0 )
            Ret = CMSRET_INVALID_ARGUMENTS;
    }
    else
        Ret = CMSRET_INVALID_ARGUMENTS;

    if( Ret != CMSRET_SUCCESS )
    {
        // Invalid argument
        cmsLog_error("General parameters set error\n");
        cmdLaserHelp("LaserGeneralParamsSet");
    }

    return( Ret );
}


/*****************************************************************************
*
*  FUNCTION     :   cmdLaserGeneralParamsGet
*  DESCRIPTION  :
*               :
*               :
*  PARAMETERS   :
*  RETURNS      :   0 on success
*
*****************************************************************************/
static CmsRet cmdLaserGeneralParamsGet(char *pszclient, char *pszoffset, char *pszlen)
{
    CmsRet Ret = CMSRET_SUCCESS;

    if( pszclient && pszoffset && pszlen)
    {
    	uint8_t client        = (uint8_t) strtol( pszclient, NULL, 16 );
        unsigned short offset = (unsigned short) strtol( pszoffset, NULL, 16 );
        unsigned short len    = (unsigned short) strtol( pszlen, NULL, 10 );

        len = len < PMD_BUF_MAX_SIZE ? len : PMD_BUF_MAX_SIZE;

        if( devLaser_GetGeneralParams(client, offset, len) != 0 )
            Ret = CMSRET_INVALID_ARGUMENTS;
    }
    else
        Ret = CMSRET_INVALID_ARGUMENTS;

    if( Ret != CMSRET_SUCCESS )
    {
        // Invalid argument
        cmsLog_error("General parameters get error\n");
        cmdLaserHelp("LaserGeneralParamsGet");
    }

    return( Ret );
}


/*****************************************************************************
*
*  FUNCTION     :   cmdLaserDumpData
*  DESCRIPTION  :
*               :
*               :
*  PARAMETERS   :
*  RETURNS      :   0 on success
*
*****************************************************************************/
static CmsRet cmdLaserDumpData()
{
    int ret = -1;

	int LaserDevFd = open(LASER_DEV, O_RDWR);

	if( LaserDevFd >= 0)
	{
	    ret = ioctl(LaserDevFd, PMD_IOCTL_DUMP_DATA);
	    close(LaserDevFd);
	}

	return( ret );
}

/*****************************************************************************
*
*  FUNCTION     :   cmdLaserCalSet
*  DESCRIPTION  :
*               :
*               :
*  PARAMETERS   :
*  RETURNS      :   0 on success
*
*****************************************************************************/
static CmsRet cmdLaserCalSet(char *pszparam, char *pszval, char *pszval1, char *pszval2, char *pszval3)
{
    CmsRet Ret = CMSRET_SUCCESS;
    int32_t val = 0;
    uint16_t cal_index = 0;

    if( pszparam && pszval)
    {
        /* parse the command line and build the argument vector */
        uint16_t param  = (uint16_t) strtol( pszparam, NULL, 10 );

        val = (int32_t) strtol( pszval, NULL, 16 );

        if (val != CAL_FILE_INVALID_ENTRANCE)
        {
            switch (param)
            {
                case pmd_mpd_gains:
                {
                    float bias_gain, mod_gain;
                    int16_t tmp_bias, tmp_mod;

                    if( pszval1 == NULL)
                        return CMSRET_INVALID_ARGUMENTS;

                    bias_gain  = (float) atof( pszval );
                    mod_gain  = (float) atof( pszval1 );
                    tmp_bias = (int16_t)(bias_gain * 256);
                    tmp_mod = (int16_t)(mod_gain * 256);

                    val = tmp_mod << 8 | tmp_bias;
                    break;
                }

                case pmd_apd:
                {
                    uint16_t type = (uint16_t) val;
                    uint16_t en = 0x800;
                    uint16_t voltage = (uint16_t) strtol( pszval1, NULL, 16 );

                    val = type << 10 | voltage | en;
                    break;
                }

                case pmd_mpd_config:
                {
                    uint16_t dir = (uint16_t) strtol( pszval, NULL, 16 );
                    uint16_t tia = (uint16_t) strtol( pszval1, NULL, 16 );
                    uint16_t vga = (uint16_t) strtol( pszval2, NULL, 16 );

                    val = ((tia & 0xf) << 12) | ((vga & 0x3) << 10) |((dir & 0x1) << 9);
                    break;
                }

                case pmd_rssi_a:
                case pmd_rssi_b:
                case pmd_rssi_c:
                {
                    float rssi = (float) atof( pszval );
                    val = rssi * 256;
                    break;
                }

                case pmd_temp_coff_h:
                {
                    float alph_h = (float) atof( pszval );
                    if (alph_h >= 8)
                    {
                        printf("wrong parameter value \n");
                        Ret = CMSRET_INVALID_ARGUMENTS;
                    }
                    else
                        val = alph_h * 4096;

                    break;
                }

                case pmd_temp_coff_l:
                {
                    float alph_l  = (float) atof( pszval );
                    if (alph_l >= 8)
                    {
                        printf("wrong parameter value \n");
                        Ret = CMSRET_INVALID_ARGUMENTS;
                    }
                    else
                        val = alph_l * 4096;

                    break;
                }

                case pmd_dacrange:
                {
                    val = (0x10 << 7) | (0x10 << 2) | (val & 0x3);
                    break;
                }

                case pmd_los_thr:
                {
                    uint16_t assert = (uint16_t) strtol( pszval, NULL, 16 );
                    uint16_t desassert = (uint16_t) strtol( pszval1, NULL, 16 );

                    val = (desassert & 0xff) << 8 | (assert & 0xff) ;

                    break;
                }

                case pmd_sat_pos:
                {
                    uint16_t high = (uint16_t) strtol( pszval, NULL, 16 );
                    uint16_t low = (uint16_t) strtol( pszval1, NULL, 16 );

                    val = (low & 0xff) << 8 | (high & 0xff) ;

                    break;
                }

                case pmd_sat_neg:
                {
                    uint16_t high = (uint16_t) strtol( pszval, NULL, 16 );
                    uint16_t low = (uint16_t) strtol( pszval1, NULL, 16 );

                    val = (low & 0xff) << 8 | (high & 0xff) ;

                    break;
                }

                case pmd_edge_rate:
                {
                    cal_index = (uint16_t) strtol( pszval, NULL, 16 );
                    uint16_t rate = (uint16_t) strtol( pszval1, NULL, 16 );
                    uint16_t dload = (uint16_t) strtol( pszval2, NULL, 16 );

                    val = (dload & 0xff) << 8 | (rate & 0xff) ;

                    break;
                }

                case pmd_preemphasis_weight:
                case pmd_preemphasis_delay:
                {
                    cal_index = (uint16_t) strtol( pszval, NULL, 16 );
                    val = (int32_t) strtol( pszval1, NULL, 16 );

                    break;
                }

                default:
                    break;

            }
        }

        if( devLaser_SetCalParams(param, val, cal_index) != 0 )
            Ret = CMSRET_INVALID_ARGUMENTS;
    }
    else
        Ret = CMSRET_INVALID_ARGUMENTS;

    if( Ret != CMSRET_SUCCESS )
    {
        // Invalid argument
        cmsLog_error("Cal write error\n");
        cmdLaserHelp("LaserCalSet");
    }

    return( Ret );
}

/*****************************************************************************
*
*  FUNCTION     :   cmdLaserCalGet
*  DESCRIPTION  :
*               :
*               :
*  PARAMETERS   :
*  RETURNS      :   0 on success
*
*****************************************************************************/
static CmsRet cmdLaserCalGet(char *pszparam, char *pszval)
{
    CmsRet Ret = CMSRET_SUCCESS;
    uint16_t cal_index = 0;

    if( pszparam )
    {
        uint16_t param = (uint16_t) strtol( pszparam, NULL, 10 );

        if (pszval)
            cal_index = (uint16_t) strtol( pszval, NULL, 16 );

        if( devLaser_GetCalParams(param, cal_index) != 0 )
            Ret = CMSRET_INVALID_ARGUMENTS;
    }
    else
        Ret = CMSRET_INVALID_ARGUMENTS;

    if( Ret != CMSRET_SUCCESS )
    {
        // Invalid argument
        cmsLog_error("Cal read error\n");
        cmdLaserHelp("LaserFileGet");
    }

    return( Ret );
}

/*****************************************************************************
*
*  FUNCTION     :   cmdLaserMsgWrite
*  DESCRIPTION  :
*               :
*               :
*  PARAMETERS   :
*  RETURNS      :   0 on success
*
*****************************************************************************/
static CmsRet cmdLaserMsgWrite(char *pszmsg_id,  char *pszlen, unsigned char *pszbuf)
{
    CmsRet Ret = CMSRET_SUCCESS;
    int i;

    if( pszmsg_id && pszlen && pszbuf)
    {
        unsigned short msg_id  = (unsigned short) strtol( pszmsg_id, NULL, 16 );
        unsigned short len     = (unsigned short) strtol( pszlen, NULL, 10 );

        len = len < PMD_BUF_MAX_SIZE ? len : PMD_BUF_MAX_SIZE;

        for( i=0; i<len; i++)
        {
            pszbuf[i*2]=TOHEX(pszbuf[i*2]);
            pszbuf[i*2+1]=TOHEX(pszbuf[i*2+1]);
            pszbuf[i]=((pszbuf[i*2] <<4)+pszbuf[i*2+1]);
        }


        if( devLaser_MsgWrite(msg_id, len, pszbuf) != 0 )
            Ret = CMSRET_INVALID_ARGUMENTS;
    }
    else
        Ret = CMSRET_INVALID_ARGUMENTS;

    if( Ret != CMSRET_SUCCESS )
    {
        // Invalid argument
        cmsLog_error("Msg write error\n");
        cmdLaserHelp("laserGeneralGet");
    }

    return( Ret );
}

/*****************************************************************************
*
*  FUNCTION     :   cmdLaserMsgRead
*  DESCRIPTION  :
*               :
*               :
*  PARAMETERS   :
*  RETURNS      :   0 on success
*
*****************************************************************************/
static CmsRet cmdLaserMsgRead(char *pszmsg_id, char *pszlen)
{
    CmsRet Ret = CMSRET_SUCCESS;

    if( pszmsg_id && pszlen)
    {
        unsigned short msg_id = (unsigned short) strtol( pszmsg_id, NULL, 16 );
        unsigned short len    = (unsigned short) strtol( pszlen, NULL, 10 );

        len = len < PMD_BUF_MAX_SIZE ? len : PMD_BUF_MAX_SIZE;

        if( devLaser_MsgRead(msg_id, len) != 0 )
            Ret = CMSRET_INVALID_ARGUMENTS;
    }
    else
        Ret = CMSRET_INVALID_ARGUMENTS;

    if( Ret != CMSRET_SUCCESS )
    {
        // Invalid argument
        cmsLog_error("Msg read error\n");
        cmdLaserHelp("LaserRegFileRead");
    }

    return( Ret );
}

int cli_pmd_save_rssi_op_couples(float rssi, float op)
{
    static float rssi_op_couple[2][2] = {};
    static uint16_t index = 0;
    float a,b;
    int32_t i, tmp_a, tmp_b;
    CmsRet Ret = CMSRET_SUCCESS;

    i = index % 2;

    rssi_op_couple[i][0] = rssi;
    rssi_op_couple[i][1] = op;

    printf (" rssi %f  op %f \n", rssi, op);

    if (index >= 1)
    {
        a = (rssi_op_couple[1][1] - rssi_op_couple[0][1]) / (rssi_op_couple[1][0] - rssi_op_couple[0][0]);
        b = rssi_op_couple[0][1] - a * rssi_op_couple[0][0];

        if (a > 1 || b > 1)
        {
            printf("rssi op couple are unreasonable\n");
            return CMSRET_INVALID_ARGUMENTS;
        }

        tmp_a = (int32_t)(a * 256);
        tmp_b = (int32_t)(b * 256);

        printf("\n a %f b %f \n",a ,b);

        if( devLaser_MsgWrite(PMD_RSSI_A_FACTOR_CAL_SET_MSG, 4, (unsigned char *)&tmp_a) != 0 )
            Ret = CMSRET_INVALID_ARGUMENTS;
        else if( devLaser_MsgWrite(PMD_RSSI_B_FACTOR_CAL_SET_MSG, 4, (unsigned char *)&tmp_b) != 0 )
            Ret = CMSRET_INVALID_ARGUMENTS;

        if( devLaser_SetCalParams(pmd_rssi_a, tmp_a, 0) != 0 )
            Ret = CMSRET_INVALID_ARGUMENTS;
        else if( devLaser_SetCalParams(pmd_rssi_b, tmp_b, 0) != 0 )
            Ret = CMSRET_INVALID_ARGUMENTS;
    }
    index++;

    return (Ret);
}

/*****************************************************************************
*
*  FUNCTION     :   cmdRssiCalSet
*  DESCRIPTION  :
*               :
*               :
*  PARAMETERS   :
*  RETURNS      :   0 on success
*
*****************************************************************************/
static CmsRet cmdRssiCalSet(char *pszrssi,  char *pszop)
{
    CmsRet Ret = CMSRET_SUCCESS;

    if( pszrssi && pszop)
    {
        float rssi  = (float) atof( pszrssi );
        float op    = (float) atof( pszop );

        Ret = cli_pmd_save_rssi_op_couples(rssi, op);
    }
    else
        Ret = CMSRET_INVALID_ARGUMENTS;

    if( Ret != CMSRET_SUCCESS )
    {
        // Invalid argument
        cmsLog_error("RSSI calibration error\n");
        cmdLaserHelp("laserRssiCal");
    }

    return( Ret );
}

/*****************************************************************************
*  FUNCTION     :   devLaser_MsgWrite
*               :
*  DESCRIPTION  :
*               :
*               :
*  PARAMETERS   :
*               :
*  RETURNS      :   0 on success
*****************************************************************************/
static int devLaser_TempToApdWrite(uint16_t len, unsigned char *buf)
{
    int ret = -1;
    pmd_params param = { .len = len, .buf = buf};
    int LaserDevFd = open(LASER_DEV, O_RDWR);

    if( LaserDevFd >= 0)
    {
        ret = ioctl(LaserDevFd, PMD_IOCTL_TEMP2APD_WRITE, &param);
        close(LaserDevFd);
    }

    return( ret );
}

static CmsRet cmdLaserTempToApdSet(char *ApdList)
{
    CmsRet Ret = CMSRET_SUCCESS;
    uint16_t pmdTempToApd[160] = {0};
    int index = 0;
    char * apd;

    apd = strtok (ApdList," ,;");
    while ((apd != NULL) && (index < 160))
    {
        pmdTempToApd[index] = (uint16_t)atoi(apd);
        apd = strtok(NULL, " ,;");
        index++;
    }

    if (index < 160)
    {
        printf("missing %d entries \n", 160-index);
        Ret = CMSRET_INVALID_ARGUMENTS;
    }
    else
    {
        devLaser_TempToApdWrite(sizeof(pmdTempToApd), (unsigned char*)pmdTempToApd);
    }

    return Ret;
}



#endif /* SUPPORT_CLI_CMD */
