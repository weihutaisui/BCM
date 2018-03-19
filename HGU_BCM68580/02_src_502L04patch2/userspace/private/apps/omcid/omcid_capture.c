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

#include <errno.h>
#include <fcntl.h>

#include "me_handlers.h"
#include "cms_log.h"
#include "cms_utilc.h"

#include "omci_api.h"
#include "omcid_capture.h"

#ifdef CONFIG_BRCM_MINIGW
#define DEFAULT_LOG_FILE_ALLOC_SIZE     (100*1024)
#else
#define DEFAULT_LOG_FILE_ALLOC_SIZE     (4*1024*1024)
#endif

#define CAPTURE_TAG_LENGTH              35
// 131 bytes of tag + msg + new line char
#define LOG_FILE_RECORD_SIZE            132

#define OMCI_REQ_PACKET(p) \
  (!(OMCI_PACKET_AK(p) || (OMCI_MSG_TYPE_ALARM==OMCI_PACKET_MT(p)) || \
  (OMCI_MSG_TYPE_ATTRIBUTEVALUECHANGE==OMCI_PACKET_MT(p))))

OmciCaptureReplay_t OmciCapture = {STOP_ON_EOF,NULL,NULL,\
  "%010d.%010d:omci capture:","/var/","omci.msg","","","","",0,0,0,0,\
  {{0,0,0,0,0},{0,0,0,0,0}},{ 0,0,0,0}};

char blankMsg[131] = "0000000000000000000000000000000000000000000000000"
  "00000000000000000000000000000000000000000000000000000000000000000000"
  "00000000000000";


/*****************************************************************************
*  FUNCTION:  omci_msg_capture
*  PURPOSE:   Perform the capture of each msg provided as input to the log
*             file, console, or both.
*  PARAMETERS:
*      RxBuffer - pointer to the receive buffer holding the msg to be captured.
*      Len - actual size of the received msg.
*  RETURN:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void omci_msg_capture(UINT8 *RxBuffer, int Len)
{
    CmsTimestamp tms;
    char *hexString = NULL;

    // Only log a maximum length of OMCI_PACKET_A_SIZE, be careful,
    // packet might be invalid, so can't test for type
    Len = Len > OMCI_PACKET_A_SIZE ? OMCI_PACKET_A_SIZE : Len;

    // Capture OMCI msgs from OLT
    if ((TRUE == OmciCapture.Flags.Capturing) &&
      (FALSE == OmciCapture.Flags.Replaying))
    {
        memset(OmciCapture.TextBuffer, 0, LOG_FILE_RECORD_SIZE);
        cmsTms_get(&tms);
        sprintf(OmciCapture.TextBuffer, OmciCapture.TagFormatStr,
          tms.sec, tms.nsec);

        cmsUtl_binaryBufToHexString(RxBuffer, Len, &hexString);
        memcpy(&OmciCapture.TextBuffer[CAPTURE_TAG_LENGTH], hexString, Len*2);
        cmsMem_free(hexString);

        fprintf(OmciCapture.pDstLogFile, "%s\n", OmciCapture.TextBuffer);
        OmciCapture.ActualMsgCnt++;
        OmciCapture.Section[0].ActualMsgCnt++;
        OmciCapture.Section[0].NextMsg =
          OmciCapture.Section[0].ActualMsgCnt + 1;
        if (OmciCapture.ActualMsgCnt >= OmciCapture.MaxMsgCount)
        {
            OmciCapture.Flags.Capturing = 0;
            printf("Omci capture: End of file reached, capture terminated\n");
        }
    }
}

/*****************************************************************************
*  FUNCTION:  omci_capture_view
*  PURPOSE:   View a set of captured OMCI messages by printing each one in
*             the file.
*  PARAMETERS:
*      UseAltFilenameFlag - indication of using the pAltFilename.
*      pAltFilename - location of the saved captured file.
*  RETURN:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void omci_capture_view(UINT32 UseAltFilenameFlag, char* pAltCaptureFilename)
{
    char* pFilename = OmciCapture.FullFileName;

    if (FALSE == (OmciCapture.Flags.Capturing || OmciCapture.Flags.Replaying))
    {
        if (NULL != OmciCapture.pDstLogFile)
        {
            omci_capture_control(0,STOP);
        }

        // check for handle to user provided filename
        if (TRUE == UseAltFilenameFlag)
        {
            pFilename = pAltCaptureFilename;
        }

        printf ("OMCI Capture view: Opening file %s ...\n", pFilename);

        OmciCapture.pSrcLogFile = fopen(pFilename, "r");

        if (NULL != OmciCapture.pSrcLogFile)
        {
            UINT32 msgCounter = 0;

            omci_capture_get_log_file_header();

            printf("\nOMCI capture view:  message viewing is enabled.\n\n");

            sprintf(OmciCapture.TextBuffer,
              "%s %s %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
              OmciCapture.StartTimeStamp, OmciCapture.StopTimeStamp,
              OmciCapture.LogFileAllocSize, OmciCapture.MaxMsgCount,
              OmciCapture.ActualMsgCnt, OmciCapture.EofOption,
              OmciCapture.Section[0].MaxMsgCnt,
              OmciCapture.Section[0].ActualMsgCnt,
              OmciCapture.Section[0].LostMsgCnt,
              OmciCapture.Section[0].NextMsg,
              OmciCapture.Section[0].OldestMsg,
              OmciCapture.Section[1].MaxMsgCnt,
              OmciCapture.Section[1].ActualMsgCnt,
              OmciCapture.Section[1].LostMsgCnt,
              OmciCapture.Section[1].NextMsg,
              OmciCapture.Section[1].OldestMsg
            );
            printf("%-256.256s\n", OmciCapture.TextBuffer);

            // we have both files open so copy src to dst
            while ((msgCounter < OmciCapture.ActualMsgCnt) &&
              (NULL != fgets(OmciCapture.TextBuffer, LOG_FILE_RECORD_SIZE + 2,
              OmciCapture.pSrcLogFile)))
            {
                printf ("%s",OmciCapture.TextBuffer);
                msgCounter++;
            }
            printf("\nOMCI capture view: %d messages displayed from %s. "
              "Msg file viewing complete.\n", msgCounter, pFilename);

            fclose(OmciCapture.pSrcLogFile);
            OmciCapture.pSrcLogFile = NULL;
        }
        else
        {
            printf("\nFailed to open internal omci message capture file "
              "/var/omci.msg for viewing \n");
        }
    }
    else
    {
        printf("\nCan't start OMCI captured message viewing while capture or "
          "replay are active\n");
    }
}

/*****************************************************************************
*  FUNCTION:  omci_capture_replay_control
*  PURPOSE:   OMCI replay control.
*  PARAMETERS:
*      UseAltFilenameFlag - indication of using the pAltCaptureFilename.
*      pAltFilename - file name.
*      Option - start, stop or restart.
*  RETURN:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void omci_capture_replay_control(UINT32 UseAltFilenameFlag,
  char *pAltCaptureFilename, OmciCaptureState_t Option)
{
    char* pFilename = OmciCapture.FullFileName;

    switch (Option)
    {
        case STOP: // 0 - turn it off
        {
            if (TRUE == OmciCapture.Flags.Replaying)
            {
                if (NULL != OmciCapture.pSrcLogFile)
                {
                    fclose(OmciCapture.pSrcLogFile);
                }
                OmciCapture.pSrcLogFile     = NULL;
                OmciCapture.Flags.Replaying = FALSE;
            }

            printf("\nOMCI message replay is disabled.\n");
            break;
        }
        case RESTART: // 1 - turn it on
        case START: // 1 - turn it on
        {
            if (FALSE == (OmciCapture.Flags.Capturing))
            {
                if (NULL != OmciCapture.pDstLogFile)
                {
                    omci_capture_control(0, STOP);
                }
                // start by checking for handle to user provided filename
                if (TRUE == UseAltFilenameFlag)
                {
                    pFilename = pAltCaptureFilename;
                }

                printf("OMCI Capture Replay: Opening file %s ...\n", pFilename);
                OmciCapture.pSrcLogFile = fopen(pFilename, "r");

                if (NULL == OmciCapture.pSrcLogFile)
                {
                    printf("\nFailed to open omci message file %s for replay\n",
                      pFilename);
                    break;
                }

                // read in file header and position read pointer at first msg
                omci_capture_get_log_file_header();

                printf("\nOMCI Capture Replay: "
                  "Will replay %d messages from %s\n",
                  OmciCapture.ActualMsgCnt, pFilename);

                OmciCapture.Flags.Replaying = TRUE;

                OmciCapture.ReplayedMsgCount = 0;
            }
            else
            {
                printf("\nCan't start OMCI message replay while "
                  "capture or viewing are active\n");
            }

            break;
        }
    }
}

/*****************************************************************************
*  FUNCTION:  omci_capture_put_log_file_header
*  PURPOSE:   Performs a write to the beginning of the log file currently
*             as the dst log file and outputs the current values of the log
*             file header. This operation is performed at file creation
*             to reserve space for the final values written when the file is
*             closed when capture is terminated.
*  PARAMETERS:
*      tempStr - pointer to a string representing the current value of the
*        stop time stamp, which may just be a dummy value.
*  RETURN:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void omci_capture_put_log_file_header(char *tempStr)
{
    fseek(OmciCapture.pDstLogFile, 0, 0);

    sprintf(OmciCapture.TextBuffer,
      "%s %s %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
      OmciCapture.StartTimeStamp, tempStr, OmciCapture.LogFileAllocSize,
      OmciCapture.MaxMsgCount, OmciCapture.ActualMsgCnt, OmciCapture.EofOption,
      OmciCapture.Section[0].MaxMsgCnt, OmciCapture.Section[0].ActualMsgCnt,
      OmciCapture.Section[0].LostMsgCnt, OmciCapture.Section[0].NextMsg,
      OmciCapture.Section[0].OldestMsg,
      OmciCapture.Section[1].MaxMsgCnt, OmciCapture.Section[1].ActualMsgCnt,
      OmciCapture.Section[1].LostMsgCnt, OmciCapture.Section[1].NextMsg,
      OmciCapture.Section[1].OldestMsg);
    fprintf(OmciCapture.pDstLogFile, "%-256.256s\n", OmciCapture.TextBuffer);
}

/*****************************************************************************
*  FUNCTION:  omci_capture_get_log_file_header
*  PURPOSE:   Perform a read from the beginning of the file currently opened
*             as the src log file and input the previously written values
*             of the log file header. This operation is put into a function
*             because it needs to be performed at multiple locations in the
*             code.
*  PARAMETERS:
*      None.
*  RETURN:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void omci_capture_get_log_file_header(void)
{
    int rc;

    fseek(OmciCapture.pSrcLogFile, 0, 0);

    rc = fscanf(OmciCapture.pSrcLogFile,
      "%s %s %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
      OmciCapture.StartTimeStamp, OmciCapture.StopTimeStamp,
      &OmciCapture.LogFileAllocSize, &OmciCapture.MaxMsgCount,
      &OmciCapture.ActualMsgCnt, (UINT32*)&OmciCapture.EofOption,
      &OmciCapture.Section[0].MaxMsgCnt, &OmciCapture.Section[0].ActualMsgCnt,
      &OmciCapture.Section[0].LostMsgCnt, &OmciCapture.Section[0].NextMsg,
      &OmciCapture.Section[0].OldestMsg, &OmciCapture.Section[1].MaxMsgCnt,
      &OmciCapture.Section[1].ActualMsgCnt, &OmciCapture.Section[1].LostMsgCnt,
      &OmciCapture.Section[1].NextMsg, &OmciCapture.Section[1].OldestMsg);
    if (rc < 0)
    {
        cmsLog_error("fscanf() failed");
    }
}

/*****************************************************************************
*  FUNCTION:  omci_capture_control
*  PURPOSE:   Enable/disable capture control.
*  PARAMETERS:
*      OverwriteFlag - yes: overwrite the existing file; no: append.
*      Option - start, stop or restart.
*  RETURN:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void omci_capture_control(UINT32 OverwriteFlag, OmciCaptureState_t Option)
{
    char tempStr[32] = "";
    CmsTimestamp tms;

    cmsTms_get(&tms);

    sprintf(tempStr, "%010d.%010d", tms.sec, tms.nsec);

    switch (Option)
    {
        case STOP: // 0 - turn it off
        {
            cmsUtl_strncpy(OmciCapture.StopTimeStamp, tempStr, 32);

            // silly as it might sound this msg could come while capture
            // is not active so qualify actions
            if (TRUE == OmciCapture.Flags.Capturing ||
              NULL != OmciCapture.pDstLogFile)
            {
                // stopping omci capture so write final record at the top
                // of the file
                omci_capture_put_log_file_header(tempStr);
                fclose(OmciCapture.pDstLogFile);
            }

            OmciCapture.pDstLogFile        = NULL;
            OmciCapture.Flags.Capturing = FALSE;

            printf("\nOMCI message capture has been disabled.\n");

            break;
        }
        case START: // 1 - turn it on
        {
            cmsUtl_strncpy(OmciCapture.StartTimeStamp, tempStr, 32);

            // open file for first time this session
            if (NULL != (OmciCapture.pDstLogFile =
              fopen(OmciCapture.FullFileName, "wb+")))
            {
                printf("\nOMCI message capture enabled.\n");
            }
            else
            {
                printf("\nOMCI message capture file %s could failed to open.\n",
                  OmciCapture.FullFileName);
                break;
            }

            OmciCapture.Flags.Capturing         = TRUE;
            OmciCapture.LogFileAllocSize        = DEFAULT_LOG_FILE_ALLOC_SIZE;
//            OmciCapture.MaxMsgCount         = DEFAULT_LOG_FILE_ALLOC_SIZE / LOG_FILE_RECORD_SIZE;
            OmciCapture.ActualMsgCnt            = 0;
            OmciCapture.EofOption               = STOP_ON_EOF;
            OmciCapture.Section[0].ActualMsgCnt = 0;
            OmciCapture.Section[0].NextMsg      = 1;
            OmciCapture.Section[0].OldestMsg    = 1;

            //write out the file header defaults
            omci_capture_put_log_file_header("0000000000.0000000000");

            //preallocate file by writing out block msg blocks to
            for (OmciCapture.MaxMsgCount = 0;
              OmciCapture.MaxMsgCount <
              DEFAULT_LOG_FILE_ALLOC_SIZE / LOG_FILE_RECORD_SIZE;
              OmciCapture.MaxMsgCount++)
            {
                fprintf (OmciCapture.pDstLogFile, "%s\n", blankMsg);
            }

            OmciCapture.Section[0].MaxMsgCnt = OmciCapture.MaxMsgCount;

            fflush(OmciCapture.pDstLogFile);

            // move back to top of file and reposition pointers
            fseek(OmciCapture.pDstLogFile, 256, 0);

            omci_capture_put_log_file_header("0000000000.0000000000");

            break;
        }
        case RESTART:
        // 2 - restart capture either while active or having been
        // previously started this session
        {
            //save value of overwrite flag
            OmciCapture.Flags.OverwriteFile = OverwriteFlag;

            if (TRUE == (OmciCapture.Flags.Replaying ||
              OmciCapture.Flags.Viewing))
            {
                printf("\nCan't start OMCI message capture while playback "
                  "or viewing are active\n");
            }
            else
            {
                if (TRUE == OmciCapture.Flags.Capturing)
                {
                    printf("\nOMCI message capture already active\n");
                }
                else
                {
                    if (NULL != OmciCapture.pDstLogFile)
                    {
                        omci_capture_control(0, STOP);
                    }

                    // open file and start capture based on whether
                    // overwrite is true
                    if (TRUE == OmciCapture.Flags.OverwriteFile)
                    {
                        // this condition degrades to a start operation
                        omci_capture_control(0, START);
                        break;
                    }

                    // in this case we need to open in append mode to avoid
                    // deleting the contents of the file
                    OmciCapture.pDstLogFile = fopen(OmciCapture.FullFileName,
                      "rb+");
                    if (NULL == OmciCapture.pDstLogFile)
                    {
                        //didn't open for some reason
                        printf("\nOMCI message capture file %s could not "
                          "be opened in append mode.\n",
                          OmciCapture.FullFileName);
                        break;
                    }

                    // now read in the file header and reposition the
                    // write pointer
                    OmciCapture.pSrcLogFile = OmciCapture.pDstLogFile;
                    omci_capture_get_log_file_header();
                    OmciCapture.pDstLogFile = OmciCapture.pSrcLogFile;
                    OmciCapture.pSrcLogFile=NULL;
                    fseek(OmciCapture.pDstLogFile,
                      OmciCapture.ActualMsgCnt*LOG_FILE_RECORD_SIZE, SEEK_CUR);
                    strcpy(OmciCapture.StopTimeStamp, "0000000000.0000000000");
                    OmciCapture.Flags.Capturing = TRUE;
                    printf("\nOMCI message capture enabled.\n");
                }
            }

            break;
        }
    }
}

/*****************************************************************************
*  FUNCTION:  omci_capture_save
*  PURPOSE:   Save the captured OMCI message file omci.msg to an external
*             device.
*  PARAMETERS:
*      UseAltFilenameFlag - indication of using the pAltFilename.
*      pAltFilename -- file name.
*  RETURN:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void omci_capture_save(UINT32 UseAltFilenameFlag, char* pAltFilename)
{
    char SaveFilename[256] = "";
    char UsbDir1[] = "/mnt/usb1_1/";
    char UsbDir2[] = "/mnt/usb2_2/";
    char *pFilename = OmciCapture.BaseFileName;

    // note that we don't include viewing because the way the code
    // operates we can't be viewing if we are handling this CLI...
    if (FALSE == (OmciCapture.Flags.Capturing || OmciCapture.Flags.Replaying))
    {
        if (NULL != OmciCapture.pDstLogFile)
        {
            omci_capture_control(0, STOP);
        }

        OmciCapture.pSrcLogFile = fopen(OmciCapture.FullFileName, "r");

        if (NULL != OmciCapture.pSrcLogFile)
        {
            // start by checking for handle to user provided filename
            if (TRUE == UseAltFilenameFlag)
            {
                pFilename = pAltFilename;
            }

            // check if usb directory exists for download
            if (access(UsbDir1, F_OK) == 0)
            {
                strcat(SaveFilename, UsbDir1);
            }
            else if (access(UsbDir2, F_OK) == 0)
            {
                strcat(SaveFilename, UsbDir2);
            }

            if (strlen(SaveFilename) > 0)
            {
                strcat(SaveFilename, pFilename);
                printf ("OMCI Capture Save: Opening file %s ...\n",
                  SaveFilename);
                OmciCapture.pDstLogFile = fopen(SaveFilename, "w");
            }
            else
            {
                printf("OMCI Capture Save: Failed to open external omci "
                  "message file %s on USB devices %s and %s\n",
                  pFilename, UsbDir1, UsbDir2);
            }

            if (NULL != OmciCapture.pDstLogFile)
            {
                UINT32 msgCounter = 0;

                // read in capture file header and position read pointer
                // at first msg beyond header
                omci_capture_get_log_file_header();

                // write header to dst file and position write pointer
                // at first msg beyond header
                omci_capture_put_log_file_header(OmciCapture.StopTimeStamp);

                printf("\nOMCI Capture Save:  save is enabled.\n");

                // we have both files open so copy src to dst
                while ((msgCounter < OmciCapture.ActualMsgCnt) &&
                  (NULL != fgets(OmciCapture.TextBuffer,
                  LOG_FILE_RECORD_SIZE+2, OmciCapture.pSrcLogFile)))
                {
                    fputs(OmciCapture.TextBuffer , OmciCapture.pDstLogFile);
                    msgCounter++ ;
                }

                printf("\nOMCI Capture Save:  %d msgs written to %s. "
                  "Save operation is complete.\n", msgCounter, SaveFilename);

                fclose(OmciCapture.pDstLogFile);
                OmciCapture.pDstLogFile = NULL;
            }
            else
            {
                printf("\nOMCI Capture Save: Failed to open external "
                  "omci message file %s\n", SaveFilename);
            }

            fclose(OmciCapture.pSrcLogFile);
            OmciCapture.pSrcLogFile = NULL;

        }
        else
        {
            printf("\nOMCI Capture Save: Failed to open internal omci "
              "message capture file %s\n", OmciCapture.FullFileName);
        }
    }
    else
    {
        printf("\nCan't download OMCI message capture file while viewing, "
          "capture or replay are active\n");
    }
}

/*****************************************************************************
*  FUNCTION:  omci_capture_restore
*  PURPOSE:   Restore saved OMCI capture file to the internal capture file
*             location, so that it can be replayed.
*  PARAMETERS:
*      UseAltFilenameFlag - indication of using the pAltFilename.
*      pAltFilename - file name.
*  RETURN:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void omci_capture_restore(UINT32 UseAltFilenameFlag, char* pAltFilename)
{
    char FullFilename[256] = "";
    char UsbDir1[] = "/mnt/usb1_1/";
    char UsbDir2[] = "/mnt/usb2_2/";
    char* pFilename = OmciCapture.BaseFileName;

    // note that we don't include viewing because the way the code
    // operates we can't be viewing if we are handling this CLI...
    if (FALSE == (OmciCapture.Flags.Capturing || OmciCapture.Flags.Replaying))
    {
        // make sure Dst file handle is NULL as second test capture not active
        if (NULL != OmciCapture.pDstLogFile)
        {
            fclose(OmciCapture.pDstLogFile);
        }

        OmciCapture.pDstLogFile = fopen(OmciCapture.FullFileName, "w");

        if (NULL != OmciCapture.pDstLogFile)
        {
            // got the internal file open now look for file to upload
            // on USB ports

            // start by checking for handle to user provided filename
            if (TRUE == UseAltFilenameFlag)
            {
                pFilename = pAltFilename;
            }

            FullFilename[0] = '\0';

            // check if usb directory exists for download
            if (access(UsbDir1, F_OK) ==  0)
            {
                strcat(FullFilename, UsbDir1);
            }
            else if (access(UsbDir2, F_OK) ==  0)
            {
                strcat(FullFilename, UsbDir2);
            }

            if ('\0' != FullFilename[0])
            {
                strcat(FullFilename, pFilename);
                printf ("OMCI Capture Restore: Opening file %s ...\n",
                  FullFilename);
                OmciCapture.pSrcLogFile = fopen(FullFilename, "r");
            }
            else
            {
                printf("\nOMCI Capture Restore: "
                  "Couldn't find USB devices at %s or %s\n",
                   UsbDir1, UsbDir2);
            }

            if (NULL != OmciCapture.pSrcLogFile)
            {
                UINT32 msgCounter = 0;

                printf("\nOMCI Capture Restore:  "
                  "restoring file %s from file %s\n",
                  OmciCapture.FullFileName, FullFilename);

                // pull in the header from the src file being restored
                omci_capture_get_log_file_header();

                // write the header to the dst file
                omci_capture_put_log_file_header(OmciCapture.StopTimeStamp);

                // got both the src and dst files open so copy...
                while ((msgCounter < OmciCapture.ActualMsgCnt) &&
                  (NULL != fgets(OmciCapture.TextBuffer,
                  LOG_FILE_RECORD_SIZE+2 , OmciCapture.pSrcLogFile)))
                {
                    fputs(OmciCapture.TextBuffer , OmciCapture.pDstLogFile);
                    msgCounter++;
                }
                printf("\nOMCI Capture Restore:  "
                  "%d messages copied to %s, file restore is complete.\n",
                  msgCounter, OmciCapture.FullFileName);

                fclose(OmciCapture.pSrcLogFile);
                OmciCapture.pSrcLogFile = NULL;

            }
            else
            {
                printf("\nOMCI Capture Restore: "
                  "Couldn't open OMCI message file to be restored %s\n",
                  FullFilename);
            }
            fclose(OmciCapture.pDstLogFile);
            OmciCapture.pDstLogFile = NULL;
        }
        else
        {
            printf("\nOMCI Capture Restore: "
              "Failed to open internal OMCI capture file %s\n",
              OmciCapture.FullFileName);
        }
    }
    else
    {
        printf("\nCan't restore OMCI message capture file while viewing, "
          "capture or replay are active\n");
    }
}

/*****************************************************************************
*  FUNCTION:  omci_capture_get_msg_to_replay
*  PURPOSE:   Convert OMCI message from text to omciPacket format.
*  PARAMETERS:
*      pPacket - pointer to OMCI packet.
*  RETURN:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void omci_capture_get_msg_to_replay(omciPacket *pPacket)
{
    SINT32 CharPosition = 0;
    UINT8 *buf = NULL;
    UINT32 bufSize = 0;

    // LOG_FILE_RECORD_SIZE + 2 = 134
    // 35 bytes of tag + 96 bytes of msg + 1 new line char + 1 null char
    if (((OmciCapture.ReplayedMsgCount < OmciCapture.ActualMsgCnt)) &&
      (NULL != fgets(OmciCapture.TextBuffer, LOG_FILE_RECORD_SIZE + 2,
      OmciCapture.pSrcLogFile)))
    {
        // LOG_FILE_RECORD_SIZE = 132
        // 35 bytes of tag + 96 bytes of msg + 1 new line char
        // make sure string buffer is terminated without the last new line
        // character...
        OmciCapture.TextBuffer[LOG_FILE_RECORD_SIZE-1] = '\0';

        CharPosition = ((UINT32)strstr(OmciCapture.TextBuffer, "capture:")) -
          ((UINT32) OmciCapture.TextBuffer);

        if (CharPosition > 0)
        {
            // got a valid msg so advance char position index
            CharPosition += 8;
        }
        else
        {
            //still support alt capture string

            CharPosition = ((UINT32)strstr(OmciCapture.TextBuffer,
              "req pkt:")) - ((UINT32) OmciCapture.TextBuffer);
            if (CharPosition > 0)
            {
                CharPosition += 8;
            }
            else
            {
                errno = EBADMSG;
                cmsLog_error("Bad OMCI message cannot be replayed: '%s'",
                  OmciCapture.TextBuffer);
                return;
            }
        }

        OmciCapture.ReplayedMsgCount++;

        // if we get here whe have some sort of captured packet to work with,
        // but it is in text format
        cmsUtl_hexStringToBinaryBuf(&OmciCapture.TextBuffer[CharPosition],
          &buf, &bufSize);

        if (bufSize == OMCI_PACKET_A_SIZE)
        {
            memcpy(pPacket, buf, bufSize);
            // at this point the pkt has been converted from string to binary
            // we need to see if we replay it or not by checking for AK,
            // alarms and AVCs
            if (OMCI_REQ_PACKET(pPacket))
            {
                errno = 0;
                cmsLog_notice("Replaying OMCI message : %s",
                  OmciCapture.TextBuffer);
            }
            else
            {
                // this wasn't a req pkt so loop around and get next line of
                // text from file....
                errno = EBADMSG;
            }
        }

        cmsMem_free(buf);
    }
    else
    {
        // we hit the end of file so shut down replay completely
        omci_capture_replay_control(0, "", STOP);
        errno = EBADMSG;
        cmsLog_notice("OMCI capture replay has completed\n");
    }
}

/*****************************************************************************
*  FUNCTION:  omci_capture_init
*  PURPOSE:   Initialize capturing data structures.
*  PARAMETERS:
*      None.
*  RETURN:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void omci_capture_init(void)
{
    strcat(OmciCapture.FullFileName, OmciCapture.Directory);
    strcat(OmciCapture.FullFileName, OmciCapture.BaseFileName);
    omci_capture_control(0, START);
}

