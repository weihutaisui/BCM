/***********************************************************************
 *
 *  Copyright (c) 2007  Broadcom Corporation
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

/** This file contains httpd specific code for upload.
 *  This file may not actually contain any cgi code, but the cgi_
 *  prefix is used to distinguish this file from other files named upload
 *  in CMS.
 */


#include <errno.h>
#include <string.h>
#include "cms.h"
#include "cms_util.h"
#include "cms_boardcmds.h"
#include "../../../../shared/opensource/include/bcm963xx/bcm_hwdefs.h"
#include "httpd.h"
#include "cms_image.h"
#include "bcm_flashutil.h"
#include "bcm_imgutil_api.h"
#include "pmd.h"


#define  FN_TOKEN          "filename="
#define  FN_TOKEN_LEN      strlen(FN_TOKEN)

#define  LINE_BUF_SIZE          1024

extern char connIfName[CMS_IFNAME_LENGTH];


#define DEFAULT_UPLOAD_BUF_SIZE 0x20000

static IMGUTIL_HANDLE imgutilHandle = IMGUTIL_HANDLE_INVALID;

static UINT32 calcUploadBufAllocSize(UINT32 remainImgLen, UINT32 boundaryLen);
static UINT32 adjustUploadBufSize(UINT32 uploadBufLenOld, UINT32 remainImgLen,
  UINT32 boundaryLen);
static CmsRet imgIncrementalWriteProc(char **uploadBufPP, UINT32 imgBlockSize,
  UINT32 uploadBufLenNew);


#ifdef SUPPORT_DEBUG_TOOLS
struct UploadDiag
{
   char startBoundary[POST_BOUNDARY_LENGTH];
   char line1[512];
   char line2[512];
   UINT32 uploadBufLen;
   UINT32 uploadSize;
   UINT32 sleepCountH;
   UINT32 sleepCountM;
   CmsTimestamp startTms;
};

static struct UploadDiag uploadDiag;
#ifdef SUPPORT_TR69C_AUTONOMOUS_TRANSFER_COMPLETE
CmsImageTransferStats imageTransferStats;
#endif


void dumpHex(const unsigned char *buf, UINT32 len)
{
   int j;
   UINT32 t=0;

   while (t < len)
   {
      for (j=0; j<16 && t<len; j++, t++)
         printf("%02x ", buf[t]);
      printf("\n");
   }
   printf("\n");

}
#endif  /* SUPPORT_DEBUG_TOOLS */


/*
 * Generic function to make sure we get at least some data in a single
 * fgets read.  Wait for at most 5 seconds.
 * This function is used exclusively by uploadImage.
 */
static void getLineFromStream(FILE *stream, char *buf, UINT32 bufSize)
{
   char *frv=NULL;
   UINT32 sleepCount=0;

   while ((NULL == frv) && (sleepCount < 10*5))
   {
      if (httpd_gets(buf, bufSize) == FALSE)
      {
         usleep(100 * USECS_IN_MSEC);
         sleepCount++;
          /* this clears the EOF indication so we can keep reading if necessary. */
          clearerr(stream);
      }
      else
         frv = buf;
   }

   if (NULL == frv)
   {
      cmsLog_error("could not get a line, sleepCount=%d", sleepCount);
   }

#ifdef SUPPORT_DEBUG_TOOLS
   uploadDiag.sleepCountH += sleepCount;
#endif

}

#define READ_BUF_LEN 8192
unsigned char read_buf[READ_BUF_LEN];
static UINT32 read_buf_end = 0;
static UINT32 read_buf_cur = 0;

static int read_file(FILE *stream)
{
    return fread(read_buf, 1, sizeof(read_buf), stream);
}

static int get_single_char_from(FILE *stream, int (*buf_read)(FILE *))
{
    int rc;
    static int total_read = 0;

    if (read_buf_cur < read_buf_end)
    {
        /*
         * There is still data left over from our previous read.  Just return that
         * and advance our curr pointer.
         */
        return (int)read_buf[read_buf_cur++];
    }

    /* no data in our buffer, clear state, and read in more */
    memset(read_buf, 0, sizeof(read_buf));
    read_buf_cur = read_buf_end = 0;

    rc = buf_read(stream);
    if (rc > 0)
    {
        total_read += rc;
        read_buf_end = rc;
        return (int)read_buf[read_buf_cur++];
    }
    else if (rc == 0)
    {
        /*
         * no data in buffer, either entire image is now transfered or
         * the network is stalled.  Hopefully, we don't end up blocking indefinately here.
         */
        cmsLog_debug("got rc=0 after %d bytes", total_read);
    }
    else
    {
        /*
         * In testing, I see rc=-1 when we finish reading the entire image.
         * Just return EOF and let uploadImage look for the end boundary marker
         * in the buffer.
         */
        cmsLog_debug("got error from read after %d bytes", total_read);
    }

    return EOF;
}

#if defined(SUPPORT_HTTPS)
static int read_ssl(FILE *stream)
{
    return BIO_read(io, (char *)read_buf, sizeof(read_buf));
}
#endif   /* SUPPORT_HTTPS */


static int get_single_char(FILE *stream)
{
#if defined(SUPPORT_HTTPS)
    if (serverPort == HTTPD_PORT_SSL)
        return get_single_char_from(stream, read_ssl);
#endif   /* SUPPORT_HTTPS */

    return get_single_char_from(stream, read_file);
}

/** Upload the image from the stream.
 *
 * @return CmsRet enum from among the following: CMSRET_SUCCESS,
 *         CMSRET_RESOURCE_EXCEEDED (out of memory),
 *         CMSRET_INVALID_FILENAME
 *         CMSRET_INVALID_IMAGE
 *         CMSRET_DOWNLOAD_FAILURE
 */
CmsRet upLoadImage(FILE *stream, UINT32 content_len, int upload_type,
                   char **imageBufPtrPtr, UINT32 *imageLen, UBOOL8 flashB)
{
    char lineBuf[LINE_BUF_SIZE]={0};
    char boundary[POST_BOUNDARY_LENGTH]={0};
    int boundaryLen = 0;
    UINT32 uploadBufLen;
    UINT32 imageSizeEst;
    UINT32 imageSizeAcc;
    UINT32 validImageBlockSize = 0;
    UINT32 sleepCount = 0;
    unsigned int uploadSize = 0;
    char *pdest = NULL;
    CmsRet result = CMSRET_SUCCESS;
    char *curPtr = NULL;
    char *uploadBufPtr = NULL;
    UBOOL8 isTerminated=FALSE;
    UBOOL8 endBoundaryFound=FALSE;
    UBOOL8 keepReading=TRUE;
    UBOOL8 incWriteCheck = FALSE;
    imgutil_open_parms_t openParams;
    int intc;

    *imageBufPtrPtr = NULL;
    *imageLen = 0;

    cmsLog_debug("content_len=%d upload_type=%d", content_len, upload_type);

    /*
     * Read the first line, it should be the boundary marker.
     */
    getLineFromStream(stream, boundary, sizeof(boundary));

    boundaryLen = cmsMem_strnlen(boundary, sizeof(boundary), &isTerminated) - 2;
    if (!isTerminated || boundaryLen <= 0)
    {
       cmsLog_error("Failed to find boundary marker within the first %d bytes", sizeof(boundary));
       cmsLog_error("boundary=>%s<=", boundary);
       return CMSRET_DOWNLOAD_FAILURE;
    }

    cmsLog_debug("start boundary=>%s<=", boundary);

#ifdef SUPPORT_DEBUG_TOOLS
    strncpy(uploadDiag.startBoundary, boundary, sizeof(uploadDiag.startBoundary)-1);
#endif

    /*
     * The start boundary contains two more '-' than the boundary
     * in the header.
     */
    if (!strstr(boundary, glbBoundary))
    {
       cmsLog_error("mismatch between header boundary and start boundary");
       cmsLog_error("header boundary=>%s<=", glbBoundary);
       cmsLog_error("start  boundary=>%s<=", boundary);
       // just note the mismatch, but continue
    }

    // get Content-Disposition: form-data; name="filename"; filename="test"
    // check filename, if "", user click no filename and report
    getLineFromStream(stream, lineBuf, sizeof(lineBuf));

#ifdef SUPPORT_DEBUG_TOOLS
    strncpy(uploadDiag.line1, lineBuf, sizeof(uploadDiag.line1)-1);
#endif

    if ((pdest = strstr(lineBuf, FN_TOKEN)) == NULL)
    {
        cmsLog_error("could not find filename within the first %d bytes", sizeof(lineBuf));
        return CMSRET_DOWNLOAD_FAILURE;
    }

    pdest += FN_TOKEN_LEN;
    if (*pdest == '"' && *(pdest + 1) == '"')
    {
        cmsLog_error("No filename selected");
        result = CMSRET_INVALID_FILENAME;
        /* even though there is already an error, keep processing the post message */
    }
    else
    {
        cmsLog_debug("filename %s", pdest);
    }

    /*
     * get [Content-Type: application/octet-stream] and NL (cr/lf) and discard them
     * On Chrome, if the filename has a .xxx suffix, then there is no Content-Type line,
     * just the cr/lf.  So check for that on the first line.
     */
    memset(lineBuf, 0, sizeof(lineBuf));
    getLineFromStream(stream, lineBuf, sizeof(lineBuf));
    if (lineBuf[0] != 0xd && lineBuf[1] != 0xa) {
       memset(lineBuf, 0, sizeof(lineBuf));
       getLineFromStream(stream, lineBuf, sizeof(lineBuf));
    }

#ifdef SUPPORT_DEBUG_TOOLS
    strncpy(uploadDiag.line2, lineBuf, sizeof(uploadDiag.line2)-1);
#endif

    /*
     * Image buffer does not hold the start boundary marker, so we can subtract its length
     * Using uploadBufLen to check willFitInFlash is not exactly right, since the uploadBufLen
     * is still slightly bigger than the actual image.
     */
    imageSizeEst = content_len - boundaryLen;

    if (result == CMSRET_SUCCESS && upload_type != WEB_PMD_UPLOAD_CALIBRATION)
    {
        if (!(cmsImg_willFitInFlash(imageSizeEst)))
        {
            cmsLog_error("Image of %d bytes will not fit in flash", imageSizeEst);
            result = CMSRET_INVALID_IMAGE;
        }
        else if (imageSizeEst < CMS_IMAGE_MIN_LEN &&
                 upload_type == WEB_UPLOAD_IMAGE)  // for fake file or just the path without file name.
        {
            cmsLog_error("Wrong file or illegal size, imageSizeEst=%d type=%d", imageSizeEst, upload_type);
            result = CMSRET_INVALID_IMAGE;
        }
    }

    uploadBufLen = imageSizeEst;

    if (flashB != 0 && upload_type != WEB_PMD_UPLOAD_CALIBRATION)
    {
        memset(&openParams, 0x0, sizeof(imgutil_open_parms_t));
        openParams.maxBufSize = imageSizeEst;
        if (upload_type != WEB_UPLOAD_SETTINGS) 
        {
            openParams.forceWholeFlashB = 0;
            openParams.clientCtxP = msgHandle;
            openParams.options = CMS_IMAGE_WR_OPT_NO_REBOOT;
            imgutilHandle = img_util_open(&openParams);
        }
        else
        {
            openParams.forceWholeFlashB = 1;
            openParams.clientValidateFuncP = cmsImg_ConfigFileValidate;
            openParams.clientFlashFuncP = cmsImg_ConfigFileWrite;
            openParams.clientCtxP = msgHandle;
            openParams.options = CMS_IMAGE_WR_OPT_NO_REBOOT;
            imgutilHandle = img_util_open(&openParams);
        }

        if (imgutilHandle != IMGUTIL_HANDLE_INVALID)
        {
            uploadBufLen = calcUploadBufAllocSize(imageSizeEst, boundaryLen);
            printf("httpd: incremental image writing.\n");
        }
        else
        {
            cmsLog_error("img_util_open() failed");
            result = CMSRET_INVALID_IMAGE;
            keepReading = FALSE;
        }
    }

#ifdef SUPPORT_DEBUG_TOOLS
    uploadDiag.uploadBufLen = uploadBufLen;
#endif

    cmsLog_notice("upload start with len = %d", uploadBufLen);

    // only kill processes when upload image (but not for upload configurations)
    if (upload_type == WEB_UPLOAD_IMAGE)
    {
       cmsImg_sendLoadStartingMsg(msgHandle, connIfName);
    }

    printf("httpd: allocating %d byte buffer to hold image segment.\n", uploadBufLen);
    if ((curPtr = (char *) cmsMem_alloc(uploadBufLen, 0)) == NULL)
    {
        cmsLog_error("Failed to allocate memory for the image. Size required %d", uploadBufLen);
        return CMSRET_RESOURCE_EXCEEDED;
    }
    else
    {
        cmsLog_debug("%d bytes allocated for image data at %p", uploadBufLen, curPtr);
        printf("httpd: memory allocated.\n");
    }

    uploadBufPtr = curPtr;
    *imageBufPtrPtr = curPtr;

    /* Accumulated image size. */
    imageSizeAcc = 0;

    /*
     * This is the loop that reads the image into our buffer.
     * Using fgets is very problematic when
     * the TCP buffer for this socket in the modem becomes empty before the
     * image has been completely transfered.
     * This may happen if the network is congested or the sending browser is slow.
     * What happens if the partial data does not end in newline?  Experiments show
     * weird behavior by fgets.  So use fgetc instead.
     */
    while (keepReading)
    {
       while (EOF != (intc = get_single_char(stream)))
       {
          char c = (char) intc;

          *curPtr = c;
          curPtr++;
          uploadSize++;
          imageSizeAcc++;
          validImageBlockSize = uploadSize;

          if (uploadSize >= uploadBufLen)
          {

             if (flashB != 0 && upload_type != WEB_PMD_UPLOAD_CALIBRATION)
             {
                 incWriteCheck = TRUE;
                 break;
             }
             else
             {
                 cmsLog_error("more bytes uploaded than allocated buf, "
                  "uploadSize=%d allocated=%d", uploadSize, uploadBufLen);
                 result = CMSRET_INVALID_IMAGE;
                 break;
             }
          }
       }

       /*
        * Now that we've sucked in the user data, we can break out of this loop if
        * there was an error.
        */
       if (result != CMSRET_SUCCESS)
       {
          keepReading = FALSE;
          break;
       }

       if (intc == EOF)
       {
          char *searchStart;

          cmsLog_debug("fgetc got EOF after %d bytes", uploadSize);

          /* this clears the EOF indication so we can keep reading if necessary. */
          clearerr(stream);

          /*
           * The data ends with boundary marker, then -- 0d 0a
           * Go back to the start of the boundary marker.
           */
          searchStart = uploadBufPtr + uploadSize - 4 - boundaryLen;

          /*
           * If we are not at the end of the transfer, searchStart will point to
           * binary datat.  Printing binary buffer will screw up serial console.
           *  cmsLog_debug("searching starting at =>%s<=", searchStart);
           */
          cmsLog_debug("searching for boundary =>%s<=", boundary);

          if (!strncmp(searchStart, boundary, boundaryLen))
          {
             cmsLog_debug("found end boundary");
             endBoundaryFound = TRUE;
             keepReading = FALSE;

             /* don't include the end boundary marker in the buffer that is returned
              * to the caller.  There are two characters before the end boundary marker
              * that also needs to be deleted.
              */
             searchStart -= 2;
             *searchStart = '\0';

             /* Adjust. */
             validImageBlockSize = uploadSize - 4 - boundaryLen - 2;
             imageSizeAcc = imageSizeAcc - 4 - boundaryLen - 2;
          }
          else
          {
             /*
              * got EOF on stream, but we cannot find end marker.  Must be
              * slow network or browser.  Sleep 100ms to let browser send more data.
              * sleep at most 300 times, which is equal to about 30 seconds.
              */
             cmsLog_debug("got EOF but could not find end marker, uploadSize=%d, "
               "sleepCount=%d", uploadSize, sleepCount);
             //     dumpHex((unsigned char *) curPtr - 128, 128);
             usleep(100 * USECS_IN_MSEC);
             sleepCount++;
             if (sleepCount > 10*30)
             {
                keepReading = FALSE;
             }
          }
       }

       if ((intc != EOF) && (incWriteCheck != TRUE))
       {
          cmsLog_error("impossible! intc is not EOF");
       }

       cmsLog_debug("image uploadSize=%d, imageLen=%d, validLen=%d",
         uploadSize, imageSizeAcc, validImageBlockSize);

       if (flashB != 0 && upload_type != WEB_PMD_UPLOAD_CALIBRATION)
       {
          if ((incWriteCheck == TRUE) || (endBoundaryFound == TRUE))
          {
             char **uploadBufPP = &uploadBufPtr;
             UINT32 uploadBufLenNew;
   
             uploadBufLenNew = adjustUploadBufSize(uploadBufLen,
               (imageSizeEst - imageSizeAcc), boundaryLen);
             result = imgIncrementalWriteProc(uploadBufPP, validImageBlockSize,
               uploadBufLenNew);
             if (result == CMSRET_SUCCESS)
             {
                if (uploadBufLenNew != 0)
                {
                   uploadBufPtr = *uploadBufPP;
                   uploadBufLen = uploadBufLenNew;
                }
                curPtr = uploadBufPtr;
                incWriteCheck = FALSE;
                uploadSize = 0;
                validImageBlockSize = 0;
             }
             else if (result == CMSRET_RESOURCE_EXCEEDED)
             {
                img_util_abort(imgutilHandle);
                return result;
             }
             else
             {
                incWriteCheck = FALSE;
                keepReading = FALSE;
             }
          }
       }
    }

    if (result == CMSRET_SUCCESS)
    {
        if (!endBoundaryFound)
        {
           cmsLog_error("end boundary was not found, uploadSize=%d", uploadSize);
           result = CMSRET_INVALID_IMAGE;
        }
        else
        {
           *imageLen = imageSizeAcc;
        }
    }

    if (flashB != 0 && upload_type != WEB_PMD_UPLOAD_CALIBRATION)
    {
       if (result == CMSRET_SUCCESS)
       {
          int rc = 0;

          rc = img_util_close(imgutilHandle, NULL, NULL);
          result = (rc == 0) ? CMSRET_SUCCESS: CMSRET_INVALID_IMAGE;
       }
       else
       {
          img_util_abort(imgutilHandle);
       }
    }

    if (upload_type == WEB_PMD_UPLOAD_CALIBRATION && result == CMSRET_SUCCESS)
    {
       result = CMSRET_PMD_CALIBRATION_FILE_SUCCESS;
    }
    cmsLog_debug("final image uploadSize=%d, imageLen=%d result=%d", uploadSize, *imageLen, result);

#ifdef SUPPORT_DEBUG_TOOLS
    uploadDiag.uploadSize = uploadSize;
    uploadDiag.sleepCountM = sleepCount;
#endif

    return result;
}

CmsRet glbUploadStatus;
CmsImageFormat glbImgFormat;

void upload_info_page(FILE * stream)
{
    char pathBuf[BUFLEN_1024];

    /*
     * this web page will tell the user whether we successfully uploaded
     * the image file.  The user will still have to wait a few minutes for
     * the writing of the flash and reboot.
     */
    cmsLog_debug("sending back upload status");
    makePathToWebPage(pathBuf, sizeof(pathBuf), "uploadinfo.html");
    do_ej(pathBuf, stream);
}

void save_pmd_upload(char *imagePtr, UINT32 imageLen)
{
    FILE *pmd_calibration_file;

    pmd_calibration_file = fopen(PMD_CALIBRATION_FILE_PATH, "w");
    if (pmd_calibration_file)
    {
        fwrite(imagePtr, imageLen, 1, pmd_calibration_file);
        fclose(pmd_calibration_file);
    }
    else
    {
        printf("Can't write to %s.\n%s\n", PMD_CALIBRATION_FILE_PATH, strerror(errno));
        cmsLog_debug("Can't write to %s. %s", PMD_CALIBRATION_FILE_PATH, strerror(errno));
    }
}

int do_upload_pre(FILE * stream, int content_len, int upload_type)
{
    char *imagePtr;
    UINT32 imageLen;  // this should match the image size on filesystem

    cmsLog_debug("content_len=%d upload_type=%d", content_len, upload_type);

#ifdef SUPPORT_DEBUG_TOOLS
    memset((void *) &uploadDiag, 0, sizeof(uploadDiag));
    cmsTms_get(&(uploadDiag.startTms));
#endif

#ifdef SUPPORT_TR69C_AUTONOMOUS_TRANSFER_COMPLETE
    memset(&imageTransferStats,0,sizeof(CmsImageTransferStats));
    imageTransferStats.startTime = time(NULL);
#endif

    glbImgFormat = CMS_IMAGE_FORMAT_INVALID;
    glbUploadStatus = upLoadImage(stream, content_len, upload_type, &imagePtr, &imageLen, 1);
    if (glbUploadStatus != CMSRET_SUCCESS && glbUploadStatus != CMSRET_PMD_CALIBRATION_FILE_SUCCESS)
    {
#ifdef SUPPORT_DEBUG_TOOLS
       /*
       * Print some diagnostic info so we know what went wrong.
       */
       printf("glbBoundary   =>%s<=\n", glbBoundary);
       printf("start boundary=>%s<=\n", uploadDiag.startBoundary);
       printf("line1         =%s\n", uploadDiag.line1);
       printf("line2         =%s\n", uploadDiag.line2);
       printf("Post Header content_len=%d\n", content_len);
       printf("uploadBufLen =%d\n", uploadDiag.uploadBufLen);
       printf("uploadSize   =%d\n", uploadDiag.uploadSize);
       printf("imageLen     =%d\n", imageLen);
       printf("sleepCountH  =%d\n", uploadDiag.sleepCountH);
       printf("sleepCountM  =%d\n", uploadDiag.sleepCountM);
       printf("glbUploadStatus  =%d\n", glbUploadStatus);
#endif
       if (upload_type == WEB_UPLOAD_IMAGE)
       {
          cmsImg_sendLoadDoneMsg(msgHandle);
       }
    }

    upload_info_page(stream);

    if (glbUploadStatus == CMSRET_SUCCESS)
    {
       glbImgFormat = CMS_IMAGE_FORMAT_FLASH;
       glbStsFlag = WEB_STS_UPLOAD;
       return WEB_STS_OK;
    }
    else if (glbUploadStatus == CMSRET_PMD_CALIBRATION_FILE_SUCCESS)
    {
       save_pmd_upload(imagePtr, imageLen);
       return WEB_STS_OK;      
    }
    else
    {
       return WEB_STS_ERROR;
    }
}

void do_upload_post(void)
{
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_notice("Calling cmsImg_write image now");

#ifdef SUPPORT_TR69C_AUTONOMOUS_TRANSFER_COMPLETE
   imageTransferStats.fileType = glbImgFormat;
   imageTransferStats.faultCode = CMSRET_SUCCESS;
   imageTransferStats.isDownload = TRUE;
#endif

   /*
    * Set image state to BOOT_SET_NEW_IMAGE so the bootloader will boot
    * the new image that is written with the cmsImg_writeImage call above.
    * However, if we are writing a Modular Software Primary Firmware Patch
    * image, just reboot with the current image (so don't set image state.)
    */
   if (glbImgFormat == CMS_IMAGE_FORMAT_BROADCOM ||
       glbImgFormat == CMS_IMAGE_FORMAT_FLASH)
   {
       ret = devCtl_setImageState(BOOT_SET_NEW_IMAGE);
   }
#ifdef SUPPORT_TR69C_AUTONOMOUS_TRANSFER_COMPLETE
   imageTransferStats.completeTime = time(NULL);
   cmsImg_storeImageTransferStats(&imageTransferStats);
#endif

   cmsImg_reboot();

   /*
    * Actually, the kernel (in board.c) calls reset after a successful flash
    * write, and cmsImg_writeImage will request a reboot on successful config
    * file write.  So we may not get here.
    */
#ifdef SUPPORT_TR69C_AUTONOMOUS_TRANSFER_COMPLETE
   imageTransferStats.faultCode = ret;
#endif
   if (ret == CMSRET_SUCCESS)
   {
      cmsLog_notice("flash image complete, ret=%d", ret);
   }
   else
   {
      cmsLog_notice("flash image failed, ret=%d", ret);
#ifdef SUPPORT_TR69C_AUTONOMOUS_TRANSFER_COMPLETE
      strcpy(imageTransferStats.faultStr,"Flash image failed.");
      /* send a message to TR69c to do autonmous transfer complete.  If flash were successful,
       * after reboot the autonomous transfer message would be sent. */
      cmsImg_sendAutonomousTransferCompleteMsg(msgHandle,&imageTransferStats);
#endif
   }

   return;
}

static UINT32 calcUploadBufAllocSize(UINT32 remainImgLen, UINT32 boundaryLen)
{
    imgutil_flash_info_t flash_info;
    UINT32 blockSize = DEFAULT_UPLOAD_BUF_SIZE;
    UINT32 bufSize;
    int rc = 0;

    rc = img_util_get_flash_info(imgutilHandle, &flash_info);
    if ((rc == 0) && (flash_info.flashType == FLASH_INFO_FLAG_NAND))
    {
        blockSize = flash_info.eraseSize;
    }

    if (blockSize < boundaryLen)
    {
        cmsLog_error("blockSize %d too small, boundaryLen %d",
          blockSize, boundaryLen);
        return DEFAULT_UPLOAD_BUF_SIZE;
    }

    /* To include the boundary content in a single block. */
    if ((remainImgLen >= blockSize) &&
      (remainImgLen <= (2 * blockSize)))
    {
        bufSize = 2 * blockSize;
    }
    else
    {
        bufSize = blockSize;
    }

    cmsLog_debug("blk%d, remain%d, bound%d",
      blockSize, remainImgLen, boundaryLen);

    return bufSize;
}

static UINT32 adjustUploadBufSize(UINT32 uploadBufLenOld, UINT32 remainImgLen,
  UINT32 boundaryLen)
{
    UINT32 uploadBufLen;
    UINT32 uploadBufLenNew;

    uploadBufLenNew = calcUploadBufAllocSize(remainImgLen, boundaryLen);
    uploadBufLen = (uploadBufLenNew > uploadBufLenOld) ? uploadBufLenNew : 0;
    if (uploadBufLen != 0)
    {
        cmsLog_debug("uploadBufLen adjusted to %d", uploadBufLen);
    }

    return uploadBufLen;
}

static CmsRet imgIncrementalWriteProc(char **uploadBufPP,
  UINT32 imgBlockSize, UINT32 uploadBufLenNew)
{
    int byteCount;
    char *uploadBufP;
    CmsRet ret = CMSRET_SUCCESS;

    /* Write. */
    byteCount = img_util_write(imgutilHandle, (UINT8*)*uploadBufPP, imgBlockSize);
    if ((byteCount < 0) || (byteCount != (int)imgBlockSize))
    {
        cmsLog_error("img_util_write() failed, towrite=%d, ret=%d",
          imgBlockSize, byteCount);
        ret = CMSRET_INVALID_IMAGE;
        return ret;
    }

    /* Keep boundary content in a single buffer. */
    if (uploadBufLenNew != 0)
    {
        CMSMEM_FREE_BUF_AND_NULL_PTR(*uploadBufPP);
        if ((uploadBufP = (char*)cmsMem_alloc(uploadBufLenNew, 0)) == NULL)
        {
            cmsLog_error("Failed to allocate memory, size=%d", uploadBufLenNew);
            return CMSRET_RESOURCE_EXCEEDED;
        }
        else
        {
            cmsLog_debug("%d bytes allocated for image at %p",
              uploadBufLenNew, uploadBufP);
        }
        *uploadBufPP = uploadBufP;
    }

    return ret;
}
