/***********************************************************************
 * <:copyright-BRCM:2007:DUAL/GPL:standard
 * 
 *    Copyright (c) 2007 Broadcom 
 *    All Rights Reserved
 * 
 * Unless you and Broadcom execute a separate written software license
 * agreement governing use of this software, this software is licensed
 * to you under the terms of the GNU General Public License version 2
 * (the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
 * with the following added to such license:
 * 
 *    As a special exception, the copyright holders of this software give
 *    you permission to link this software with independent modules, and
 *    to copy and distribute the resulting executable under terms of your
 *    choice, provided that you also meet, for each linked independent
 *    module, the terms and conditions of the license of that module.
 *    An independent module is a module which is not derived from this
 *    software.  The special exception does not apply to any modifications
 *    of the software.
 * 
 * Not withstanding the above, under no circumstances may you combine
 * this software in any way with any other Broadcom software provided
 * under a license other than the GPL, without Broadcom's express prior
 * written consent.
 * 
 * :>
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> // types
#include <string.h>
#include <net/if.h>

#include "bcm_flashutil.h"
#include "bcm_flashutil_nand.h"
#include "bcm_flashutil_emmc.h"

#include <sys/ioctl.h>
#include <unistd.h> // close

#include "board.h" /* in bcmdrivers/opensource/include/bcm963xx, for BCM_IMAGE_CFE */
#include "flash_api.h"

#include <fcntl.h> // for open
#include <linux/errno.h>
#include <linux/kernel.h>

#define IS_ERR_OR_NULL(x) ((x)==0)
#define IS_ERR(x) ((x)<0)
#define IS_NULL(x) ((x)==0)

#define ERROR -1
#define SUCCESS 0

/* #define FLASHUTIL_DEBUG 1 */
#if defined(FLASHUTIL_DEBUG)
#define flashutil_debug(fmt, arg...) \
  fprintf(stderr, "%s.%u: " fmt "\n", __FUNCTION__, __LINE__, ##arg)
#else
#define flashutil_debug(fmt, arg...)
#endif
#define flashutil_print(fmt, arg...) \
  fprintf(stderr, "%s.%u: " fmt "\n", __FUNCTION__, __LINE__, ##arg)
#define flashutil_error(fmt, arg...) \
  fprintf(stderr, "ERROR[%s.%u]: " fmt "\n", __FUNCTION__, __LINE__, ##arg)

#ifdef DESKTOP_LINUX
static int fake_board_ioctl(uint32_t command, BOARD_IOCTL_PARMS *ioctlParms)
{
   flashutil_print("command=%x, parms=%p", command, ioctlParms);
   return 0;
}
#endif /* DESKTOP_LINUX */

#ifndef DESKTOP_LINUX
/* flash info variable */
static unsigned int gFlashInfo = 0;
#define UPDATE_FLASH_INFO()    if(gFlashInfo == 0) {getFlashInfo(&gFlashInfo);}
#endif

// put this here so we do not rely on cms for ioctl
int bcm_boardIoctl(uint32_t boardIoctl,
                         BOARD_IOCTL_ACTION action,
                         char *string,
                         int32_t strLen,
                         int32_t offset,
                         void *data)
{
    BOARD_IOCTL_PARMS ioctlParms;
    int boardFd = 0;
    int rc;

    int ret = 0;

#ifdef DESKTOP_LINUX
    /* don't open anything, ioctl to this fd will be faked anyways */
    boardFd = 77777;
#else
    boardFd = open(BOARD_DEVICE_NAME, O_RDWR);
#endif

    if ( boardFd != -1 )
    {
        ioctlParms.string = string;
        ioctlParms.strLen = strLen;
        ioctlParms.offset = offset;
        ioctlParms.action = action;
        ioctlParms.buf    = data;
        ioctlParms.result = -1;

#ifdef DESKTOP_LINUX
        rc = fake_board_ioctl(boardIoctl, &ioctlParms);
#else
        rc = ioctl(boardFd, boardIoctl, &ioctlParms);
        close(boardFd);
#endif

        if (rc < 0)
        {
           fprintf(stderr, "ERROR!!! ioctl boardIoctl=0x%x action=%d rc=%d", boardIoctl, action, rc);
           ret = ERROR;
        }

        /* ioctl specific return data */
        if (!IS_ERR(ret))
        {
           if ((boardIoctl == BOARD_IOCTL_GET_PSI_SIZE) ||
               (boardIoctl == BOARD_IOCTL_GET_BACKUP_PSI_SIZE) ||
               (boardIoctl == BOARD_IOCTL_GET_SYSLOG_SIZE) ||
               (boardIoctl == BOARD_IOCTL_GET_CHIP_ID) ||
               (boardIoctl == BOARD_IOCTL_GET_CHIP_REV) ||
               (boardIoctl == BOARD_IOCTL_GET_NUM_ENET_MACS) ||
               (boardIoctl == BOARD_IOCTL_GET_NUM_FE_PORTS) ||
               (boardIoctl == BOARD_IOCTL_GET_NUM_GE_PORTS) ||
               (boardIoctl == BOARD_IOCTL_GET_PORT_MAC_TYPE) ||
               (boardIoctl == BOARD_IOCTL_GET_NUM_VOIP_PORTS) ||
               (boardIoctl == BOARD_IOCTL_GET_SWITCH_PORT_MAP) ||
               (boardIoctl == BOARD_IOCTL_GET_NUM_ENET_PORTS) ||
               (boardIoctl == BOARD_IOCTL_GET_SDRAM_SIZE) ||
               (boardIoctl == BOARD_IOCTL_GET_BTRM_BOOT) ||
               (boardIoctl == BOARD_IOCTL_GET_BOOT_SECURE) ||
               (boardIoctl == BOARD_IOCTL_FLASH_READ && action == FLASH_SIZE) ||
               (boardIoctl == BOARD_IOCTL_GET_FLASH_TYPE) ||
               (boardIoctl == BOARD_IOCTL_GETSET_BOOT_INACTIVE_IMAGE))
           {
              if (data != NULL)
              {
                 *((uint32_t *)data) = (uint32_t) ioctlParms.result;
              }
           }
        }
    }
    else
    {
       fprintf(stderr, "ERROR!!! Unable to open device %s", BOARD_DEVICE_NAME);
       ret = ERROR;
    }

    return ret;
}


/** read the nvramData struct from the in-memory copy of nvram.
 * The caller is not required to have flashImageMutex when calling this
 * function.  However, if the caller is doing a read-modify-write of
 * the nvram data, then the caller must hold flashImageMutex.  This function
 * does not know what the caller is going to do with this data, so it
 * cannot assert flashImageMutex held or not when this function is called.
 *
 * @return pointer to NVRAM_DATA buffer which the caller must free
 *         or NULL if there was an error
 */
int readNvramData(void *nvramData)
{
#ifndef DESKTOP_LINUX
    int ret = 0;
    UPDATE_FLASH_INFO();
    switch( gFlashInfo )
    {
        case FLASH_INFO_FLAG_NAND:
            ret = nandReadNvramData(nvramData);
        break;

        case FLASH_INFO_FLAG_EMMC:
            ret = emmcReadNvramData(nvramData);
        break;

        default:
        break;
    }
    return ret;
#else
    flashutil_print("readNvramData(%p)", nvramData);
    return 1;
#endif
}


int getFlashInfo(unsigned int *flags)
{
    if (flags == NULL)
    {
       fprintf(stderr, "flags is NULL!");
       return ERROR;
    }
    else
    {
       *flags = 0;
    }

#ifndef DESKTOP_LINUX
    {
        /* 1 - Look for NAND */
        if( nandIsBootDevice() )
            *flags = FLASH_INFO_FLAG_NAND;

        /* 2 - Look for NAND */
        if( emmcIsBootDevice() )
            *flags = FLASH_INFO_FLAG_EMMC;

        /* 3 - default to NOR */
        if( !(*flags) )
            *flags = FLASH_INFO_FLAG_NOR;
    }
#else
    *flags = FLASH_INFO_FLAG_NAND;
#endif

    return SUCCESS;
}

unsigned int get_flash_type(void)
{
    unsigned int value = 0;

    bcm_boardIoctl(BOARD_IOCTL_GET_FLASH_TYPE, 0, NULL, 0, 0, (void *)&value);

    return(value);
}

unsigned int otp_is_btrm_boot(void)
{
    unsigned int value = 0;
#ifndef DESKTOP_LINUX
    bcm_boardIoctl(BOARD_IOCTL_GET_BTRM_BOOT, 0, NULL, 0, 0, (void *)&value);
#endif
    return(value);
}

unsigned int otp_is_boot_secure(void)
{
    unsigned int value = 0;
#ifndef DESKTOP_LINUX
    bcm_boardIoctl(BOARD_IOCTL_GET_BOOT_SECURE, 0, NULL, 0, 0, (void *)&value);
#endif
    return(value);
}

/* Only gen3 bootroms and later support a manufacturing secure stage (ie 4908, 6858, etc) */
unsigned int otp_is_boot_mfg_secure(void)
{
    unsigned int value = 0;
#ifndef DESKTOP_LINUX
    bcm_boardIoctl(BOARD_IOCTL_GET_BOOT_MFG_SECURE, 0, NULL, 0, 0, (void *)&value);
#endif
    return(value);
}


unsigned int get_chip_id(void)
{
    unsigned int chip_id = 0;
#ifndef DESKTOP_LINUX
    bcm_boardIoctl(BOARD_IOCTL_GET_CHIP_ID, 0, NULL, 0, 0, (void *)&chip_id);
#endif
    return(chip_id);
}


#ifndef DESKTOP_LINUX
unsigned int getset_boot_inactive_image(int flag)
{
    unsigned int value = 0;
    bcm_boardIoctl(BOARD_IOCTL_GETSET_BOOT_INACTIVE_IMAGE, 0, NULL, 0, flag, (void *)&value);
    return(value);
}
#else
unsigned int getset_boot_inactive_image(int flag __attribute__((unused)))
{
    return(0);
}
#endif

int getSequenceNumber(int imageNumber)
{ /* NAND Flash */
#ifndef DESKTOP_LINUX
    int seqNumber = -1;
    UPDATE_FLASH_INFO();
    switch( gFlashInfo )
    {
        case FLASH_INFO_FLAG_NAND:
            seqNumber = nandGetSequenceNumber(imageNumber);
        break;

        case FLASH_INFO_FLAG_EMMC:
            seqNumber = emmcGetSequenceNumber(imageNumber);
        break;

        default:
        break;
    }
#else
    int seqNumber = imageNumber;
#endif /* DESKTOP_LINUX */

    return(seqNumber);
}

int getNextSequenceNumber( int seqNumImg1, int seqNumImg2 )
{
    int seq_num = -1;

    if( (seqNumImg1 == -1) && (seqNumImg2 == -1) )
        seq_num = 999;
    else
        seq_num = seqNumImg1 > seqNumImg2 ? seqNumImg1+1:seqNumImg2+1;

    /* Handle wrap-around case */
    if( seq_num > 999 )
        seq_num = 0;

    /* Handle zero case */
    if( (seq_num == seqNumImg1) || (seq_num == seqNumImg2) )
        seq_num++;

    return seq_num;
}

#ifdef DESKTOP_LINUX
int getImageVersion(uint8_t *imagePtr __attribute__((unused)), int imageSize __attribute__((unused)), char *image_name, int image_name_len)
{
    strncpy(image_name, "DESKTOPLINUX", image_name_len);
    return 0;
}

int setBootImageState(int newState __attribute__((unused)))
{
    return 0;
}

int getBootImageState(void)
{
    return 0;
}

int getBootedValue(void)
{
    return 0;
}

#else
int getImageVersion(uint8_t *imagePtr, int imageSize, char *image_name, int image_name_len)
{
    int ret = -1;
    UPDATE_FLASH_INFO();
    switch( gFlashInfo )
    {
        case FLASH_INFO_FLAG_NAND:
            ret = nandGetImageVersion(imagePtr, imageSize, image_name, image_name_len);
        break;

        case FLASH_INFO_FLAG_EMMC:
            ret = emmcGetImageVersion(imagePtr, imageSize, image_name, image_name_len);
        break;

        default:
        break;
    }
    return ret;
}


/***********************************************************************
 * Function Name: getBootPartition
 * Description  : Returns the booted partition.
 * Returns      : boot partition or -1 for failure
 ***********************************************************************/
int getBootPartition( void )
{
    int ret = -1;
    UPDATE_FLASH_INFO();
    switch( gFlashInfo )
    {
        case FLASH_INFO_FLAG_NAND:
            ret = nandGetBootPartition();
        break;

        case FLASH_INFO_FLAG_EMMC:
            ret = emmcGetBootPartition();
        break;

        default:
        break;
    }
    return ret;
}


/***********************************************************************
 * Function Name: commit
 * Description  : Gets/sets the commit flag for an image.
 * Returns      : 0 for success or -1 for failure
 ***********************************************************************/
int commit( int partition, char *string )
{
    int ret = -1;
    UPDATE_FLASH_INFO();
    switch( gFlashInfo )
    {
        case FLASH_INFO_FLAG_NAND:
            ret = nandCommit(partition, string);
        break;

        case FLASH_INFO_FLAG_EMMC:
            ret = emmcCommit(partition, string);
        break;

        default:
        break;
    }
    return ret;
}


/***********************************************************************
 * Function Name: setBootImageState (kerSysSetBootImageState)
 * Description  : Persistently sets the state of an image update.
 * Returns      : 0 - success, -1 - failure
 ***********************************************************************/
int setBootImageState( int newState )
{
    int ret = -1;
    int currState = getBootImageState();
    int seq1 = getSequenceNumber(1);
    int seq2 = getSequenceNumber(2);
    char string1 = 0;
    char string2 = 0;
    int pureubi1 = commit(1, &string1);
    int pureubi2 = commit(2, &string2);

    if ((seq1 == 0) && (seq2 == 999))
        seq1 = 1000;
    if ((seq2 == 0) && (seq1 == 999))
        seq2 = 1000;

    switch(newState)
    { // convert state to OMCI states
        case BOOT_SET_NEW_IMAGE:
            if( seq1 > seq2 )
                newState = BOOT_SET_PART1_IMAGE;
            else
                newState = BOOT_SET_PART2_IMAGE;
            break;

        case BOOT_SET_OLD_IMAGE:
            if( seq2 > seq1 )
                newState = BOOT_SET_PART1_IMAGE;
            else
                newState = BOOT_SET_PART2_IMAGE;
            break;

        case BOOT_SET_NEW_IMAGE_ONCE:
            if( seq1 > seq2 )
                newState = BOOT_SET_PART1_IMAGE_ONCE;
            else
                newState = BOOT_SET_PART2_IMAGE_ONCE;
            break;

        case BOOT_SET_OLD_IMAGE_ONCE:
            if( seq2 > seq1 )
                newState = BOOT_SET_PART1_IMAGE_ONCE;
            else
                newState = BOOT_SET_PART2_IMAGE_ONCE;
            break;

        default:
            break;
    }

    if( currState == newState )
        return(0);

    if (!pureubi1 && !pureubi2)
    { // two pureUBI images
        switch(newState)
        {
            case BOOT_SET_PART1_IMAGE:
            case BOOT_SET_PART2_IMAGE_ONCE:
                commit(1, "1");
                if (seq2 > seq1)
                    commit(2, "0");
                break;

            case BOOT_SET_PART2_IMAGE:
            case BOOT_SET_PART1_IMAGE_ONCE:
                commit(2, "1");
                if (seq1 > seq2)
                    commit(1, "0");
                break;

            default:
                break;
        }

        if ( (newState == BOOT_SET_PART1_IMAGE_ONCE) || (newState == BOOT_SET_PART2_IMAGE_ONCE) )
        { // set bit to boot the inactive image
                getset_boot_inactive_image(1);
        }
    }

    { /* NAND flash, old method of boot_state */
        FILE *fp;
        char state_name[] = "/data/" NAND_BOOT_STATE_FILE_NAME;

        /* Update the image state persistently using "new image" and "old image"
         * states.  Convert "partition" states to "new image" state for
         * compatibility with the non-OMCI image update.
         */
        switch(newState)
        { // convert OMCI state to internal state
            case BOOT_SET_PART1_IMAGE:
                if( seq1 > seq2 )
                    newState = BOOT_SET_NEW_IMAGE;
                else
                    newState = BOOT_SET_OLD_IMAGE;
                break;

            case BOOT_SET_PART2_IMAGE:
                if( seq2 > seq1 )
                    newState = BOOT_SET_NEW_IMAGE;
                else
                    newState = BOOT_SET_OLD_IMAGE;
                break;

            case BOOT_SET_PART1_IMAGE_ONCE:
                if( seq1 > seq2 )
                    newState = BOOT_SET_NEW_IMAGE_ONCE;
                else
                    newState = BOOT_SET_OLD_IMAGE_ONCE;
                break;

            case BOOT_SET_PART2_IMAGE_ONCE:
                if( seq2 > seq1 )
                    newState = BOOT_SET_NEW_IMAGE_ONCE;
                else
                    newState = BOOT_SET_OLD_IMAGE_ONCE;
                break;

            default:
                break;
        }

        /* Remove old file:
         * This must happen before a new file is created so that the new file-
         * name will have a higher version in the FS, this is also the reason
         * why renaming might not work well (higher version might exist as
         * deleted in FS) However this discrimination should be fixed now in CFE */
        {
            int rc;
            char command[64];

            state_name[strlen(state_name) - 1] = '*';
            snprintf(command, sizeof command, "rm %s >/dev/null 2>&1", state_name); // suppress command output in case no files were found to delete
            rc = system(command);
            if (rc < 0)
            {
               fprintf(stderr, "rm command failed.\n");
            }
        }

        /* Create new state file name. */
        state_name[strlen(state_name) - 1] = newState;

        fp = fopen(state_name,"w");

        if (!IS_NULL(fp))
        {
            fwrite(state_name, strlen(state_name), 1, fp);

            fclose(fp);
        }
        else
            printf("Unable to open '%s'.\n", state_name);

        ret = 0;
    }

    return( ret );
}


/***********************************************************************
 * Function Name: getBootImageState (kerSysGetBootImageState)
 * Description  : Gets the state of an image update from flash.
 * Returns      : state constant or -1 for failure
 ***********************************************************************/
int getBootImageState(void)
{
    int seq1 = getSequenceNumber(1);
    int seq2 = getSequenceNumber(2);
    char commitflag1 = 0;
    char commitflag2 = 0;
    int ret = -1;
    int pureubi1 = commit(1, &commitflag1);
    int pureubi2 = commit(2, &commitflag2);

    if ((seq1 == 0) && (seq2 == 999))
        seq1 = 1000;
    if ((seq2 == 0) && (seq1 == 999))
        seq2 = 1000;

    if (!pureubi1 && !pureubi2)
    { // two pureUBI images, boot state undefined if neither flag is 1
        if (seq1 > seq2)
        {
            if (commitflag1 == '1')
                ret = BOOT_SET_PART1_IMAGE;
            else if (commitflag2 == '1')
                ret = BOOT_SET_PART2_IMAGE;
        }
        else
        {
            if (commitflag2 == '1')
                ret = BOOT_SET_PART2_IMAGE;
            else if (commitflag1 == '1')
                ret = BOOT_SET_PART1_IMAGE;
        }

        if (getset_boot_inactive_image(-1) == 1)
        {
            if (ret == BOOT_SET_PART1_IMAGE)
                ret = BOOT_SET_PART2_IMAGE_ONCE;
            if (ret == BOOT_SET_PART2_IMAGE)
                ret = BOOT_SET_PART1_IMAGE_ONCE;
        }
    }
    else if ((seq1 != -1) && (seq2 != -1))
    { // two images and at least one JFFS2 image, boot state is in data partition
        /* NAND flash */
        char states[] = {BOOT_SET_NEW_IMAGE, BOOT_SET_OLD_IMAGE,
            BOOT_SET_NEW_IMAGE_ONCE, BOOT_SET_OLD_IMAGE_ONCE};
        char boot_state_name[] = "/data/" NAND_BOOT_STATE_FILE_NAME;
        int i;

        /* The boot state is stored as the last character of a file name on
         * the data partition, /data/boot_state_X, where X is
         * BOOT_SET_NEW_IMAGE, BOOT_SET_OLD_IMAGE, BOOT_SET_NEW_IMAGE_ONCE.
         */
        for( i = 0; i < (int)sizeof(states); i++ )
        {
            FILE *fp;

            boot_state_name[strlen(boot_state_name) - 1] = states[i];

            fp = fopen(boot_state_name,"r");

            if (fp != NULL)
            {
                fclose(fp);

                ret = (int) states[i];
                break;
            }
        }

        switch(ret)
        { // convert state to OMCI states
            case BOOT_SET_NEW_IMAGE:
                if( seq1 > seq2 )
                    ret = BOOT_SET_PART1_IMAGE;
                else
                    ret = BOOT_SET_PART2_IMAGE;
                break;

            case BOOT_SET_OLD_IMAGE:
                if( seq2 > seq1 )
                    ret = BOOT_SET_PART1_IMAGE;
                else
                    ret = BOOT_SET_PART2_IMAGE;
                break;

            case BOOT_SET_NEW_IMAGE_ONCE:
                if( seq1 > seq2 )
                    ret = BOOT_SET_PART1_IMAGE_ONCE;
                else
                    ret = BOOT_SET_PART2_IMAGE_ONCE;
                break;

            case BOOT_SET_OLD_IMAGE_ONCE:
                if( seq2 > seq1 )
                    ret = BOOT_SET_PART1_IMAGE_ONCE;
                else
                    ret = BOOT_SET_PART2_IMAGE_ONCE;
                break;

            default:
                break;
        }
    }

    return( ret );
}


/***********************************************************************
 * Function Name: getBootedValue (devCtl_getBootedImagePartition)
 * Description  : Gets the state of an image update from flash.
 * Returns      : state constant or -1 for failure
 ***********************************************************************/
int getBootedValue(void)
{
    int ret = -1;
    UPDATE_FLASH_INFO();
    switch( gFlashInfo )
    {
        case FLASH_INFO_FLAG_NAND:
            ret = nandGetBootedValue();
        break;

        case FLASH_INFO_FLAG_EMMC:
            ret = emmcGetBootedValue();
        break;

        default:
        break;
    }
    return ret;
}
#endif /* DESKTOP_LINUX */

