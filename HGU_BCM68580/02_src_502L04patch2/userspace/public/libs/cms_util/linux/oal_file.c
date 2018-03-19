/***********************************************************************
 *
<:copyright-BRCM:2007:DUAL/GPL:standard

   Copyright (c) 2007 Broadcom 
   All Rights Reserved

Unless you and Broadcom execute a separate written software license
agreement governing use of this software, this software is licensed
to you under the terms of the GNU General Public License version 2
(the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
with the following added to such license:

   As a special exception, the copyright holders of this software give
   you permission to link this software with independent modules, and
   to copy and distribute the resulting executable under terms of your
   choice, provided that you also meet, for each linked independent
   module, the terms and conditions of the license of that module.
   An independent module is a module which is not derived from this
   software.  The special exception does not apply to any modifications
   of the software.

Not withstanding the above, under no circumstances may you combine
this software in any way with any other Broadcom software provided
under a license other than the GPL, without Broadcom's express prior
written consent.

:>
 *
 ************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>

#include "cms.h"
#include "cms_util.h"
#include "../oal.h"

UBOOL8 oalFil_isFilePresent(const char *filename)
{
   struct stat statbuf;
   SINT32 rc;

   rc = stat(filename, &statbuf);

   if (rc == 0)
   {
      return TRUE;
   }
   else
   {
      return FALSE;
   }
}


SINT32 oalFil_getSize(const char *filename)
{
   struct stat statbuf;
   SINT32 rc;

   rc = stat(filename, &statbuf);

   if (rc == 0)
   {
      return ((SINT32) statbuf.st_size);
   }
   else
   {
      return -1;
   }
}


CmsRet oalFil_copyToBuffer(const char *filename, UINT8 *buf, UINT32 *bufSize)
{
   SINT32 actualFileSize;
   SINT32 fd, rc;

   if (-1 == (actualFileSize = oalFil_getSize(filename)))
   {
      cmsLog_error("could not get filesize for %s", filename);
      return CMSRET_INTERNAL_ERROR;
   }

   if (*bufSize < (UINT32) actualFileSize)
   {
      cmsLog_error("user supplied buffer is %d, filename actual size is %d", *bufSize, actualFileSize);
      return CMSRET_RESOURCE_EXCEEDED;
   }

   *bufSize = 0;
       
   fd = open(filename, 0);
   if (fd < 0)
   {
      cmsLog_error("could not open file %s, errno=%d", filename, errno);
      return CMSRET_INTERNAL_ERROR;
   }

   rc = read(fd, buf, actualFileSize);
   if (rc != actualFileSize)
   {
      cmsLog_error("read error, got %d, expected %d", rc, actualFileSize);
      close(fd);
      return CMSRET_INTERNAL_ERROR;
   }

   close(fd);

   /* let user know how many bytes was actually copied */
   *bufSize = (UINT32) actualFileSize;
   return CMSRET_SUCCESS;
}


CmsRet oalFil_writeToProc(const char *procFilename __attribute__((unused)), const char *s __attribute__((unused)))
{
   CmsRet ret=CMSRET_SUCCESS;

#ifdef DESKTOP_LINUX

   cmsLog_debug("writing %s to %s", s, procFilename);

#else

   /* on the modem */
   SINT32 fd, rc;

   if ((fd = open(procFilename, O_RDWR)) < 0)
   {
      cmsLog_error("could not open %s", procFilename);
      return CMSRET_INTERNAL_ERROR;
   }

   rc = write(fd, s, strlen(s));

   if (rc < (SINT32) strlen(s))
   {
      cmsLog_error("write %s to %s failed, rc=%d", s, procFilename, rc);
      ret = CMSRET_INTERNAL_ERROR;
   }

   close(fd);

#endif  /* DESKTOP_LINUX */

   return ret;
}


CmsRet oalFil_writeBufferToFile(const char *filename, const UINT8 *buf, UINT32 bufLen)
{
   CmsRet ret=CMSRET_SUCCESS;
   SINT32 fd, rc;

   if ((fd = open(filename, O_RDWR|O_CREAT|O_TRUNC, S_IRWXU)) < 0)
   {
      cmsLog_error("could not open %s", filename);
      return CMSRET_INTERNAL_ERROR;
   }

   rc = write(fd, buf, bufLen);

   if (rc < (SINT32) bufLen)
   {
      cmsLog_error("write to %s failed, rc=%d", filename, rc);
      ret = CMSRET_INTERNAL_ERROR;
   }

   close(fd);

   return ret;
}


UBOOL8 oalFil_isDirPresent(const char *dirname)
{
   struct stat statbuf;
   SINT32 rc;

   rc = stat(dirname, &statbuf);

   if (rc == 0 && S_ISDIR(statbuf.st_mode))
   {
      return TRUE;
   }
   else
   {
      return FALSE;
   }
}


CmsRet oalFil_removeDir(const char *dirname)
{
   DIR *d;
   struct dirent *dent;

   cmsLog_debug("dirname=%s", dirname);
   /*
    * Remove all non-directories in this dir.
    * Recurse into any sub-dirs and remove them.
    */
   d = opendir(dirname);
   if (NULL == d)
   {
      /* dir must not exist, no need to remove */
      return CMSRET_SUCCESS;
   }

   while (NULL != (dent = readdir(d)))
   {
      char path[BUFLEN_1024]={0};

      if (!strcmp(dent->d_name, ".") || !strcmp(dent->d_name, ".."))
         continue;

      snprintf(path, sizeof(path)-1, "%s/%s", dirname, dent->d_name);
      if (DT_DIR == dent->d_type)
      {
         oalFil_removeDir(path);
      }
      else
      {
         unlink(path);
      }
   }

   closedir(d);

   if (0 != rmdir(dirname))
   {
      cmsLog_error("rmdir of %s failed!", dirname);
   }

   return CMSRET_SUCCESS;
}


CmsRet oalFil_makeDir(const char *dirname)
{

   if (0 != mkdir(dirname, S_IRWXU))
   {
      if (errno != EEXIST)
      {
         cmsLog_error("mkdir on %s failed", dirname);
         return CMSRET_INTERNAL_ERROR;
      }
      else
      {
         /* something by that name already exists, hopefully, it is the
          * directory we wanted to create.
          */
         cmsLog_debug("%s exists, return SUCCESS anyways", dirname);
         return CMSRET_SUCCESS;
      }
   }

   return CMSRET_SUCCESS;
}

static void insert_inOrder(DlistNode *dirHead, DlistNode *node)
{
   DlistNode *tmp = dirHead->next;

   while (tmp != dirHead)
   {
      if (strcmp( (((CmsFileListEntry *) node)->filename),
                  (((CmsFileListEntry *) tmp)->filename)) < 0)
      {
         dlist_prepend(node, tmp);
         return;
      }
      tmp = tmp->next;
   }

   dlist_prepend(node, dirHead);
}

CmsRet oalFil_getOrderedFileList(const char *dirname, DlistNode *dirHead)
{
   CmsFileListEntry *fent;
   DIR *d;
   struct dirent *dent;

   if (NULL == (d = opendir(dirname)))
   {
      cmsLog_error("could not open %s", dirname);
      return CMSRET_OBJECT_NOT_FOUND;
   }

   while (NULL != (dent = readdir(d)))
   {
      if (strcmp(dent->d_name, ".") && strcmp(dent->d_name, ".."))
      {
         fent = cmsMem_alloc(sizeof(CmsFileListEntry), ALLOC_ZEROIZE);
         if (NULL == fent)
         {
            cmsLog_error("Could not allocate memory for fileEntry");
            cmsFil_freeFileList(dirHead);
            closedir(d);
            return CMSRET_RESOURCE_EXCEEDED;
         }
         snprintf(fent->filename, sizeof(fent->filename)-1, "%s", dent->d_name);
         insert_inOrder(dirHead, (DlistNode *) fent);
      }
   }

   closedir(d);

   return CMSRET_SUCCESS;
}

CmsRet oalFil_renameFile(const char *oldName, const char *newName)
{
   
   if (rename(oldName, newName) == -1)
   {
      cmsLog_debug("rename from %s to %s fails",oldName,newName);
   }
   return CMSRET_SUCCESS;
}

CmsRet oalFil_getNumFilesInDir(const char *dirname, UINT32 *num)
{
   DIR *d;
   struct dirent *dent;
   UINT32 count = 0;
   CmsRet ret = CMSRET_SUCCESS;

   if (NULL == (d = opendir(dirname)))
   {
      cmsLog_error("could not open %s", dirname);
      ret =  CMSRET_OBJECT_NOT_FOUND;
   }
   else
   {
      while (NULL != (dent = readdir(d)))
      {
         if (strcmp(dent->d_name, ".") && strcmp(dent->d_name, ".."))
         {
            count++;
         }
      }
      closedir(d);
   }
   *num = count;
   return (ret);
}


CmsRet oal_getIntPrefixFromFileName(char *fileName, UINT32 *pNum)
{
   CmsRet ret = CMSRET_SUCCESS;
   int prefix; 
   char dontcareStr[BUFLEN_16];
   int dontcare;
   int matched;

   /* file name is in this format: intPrefix.xx.xx */
   matched = sscanf(fileName,"%d.%s.%d",&prefix,dontcareStr,&dontcare);
   if (matched != 0)
   {
      *pNum = prefix;
   }
   else
   {
      return (CMSRET_INTERNAL_ERROR);
   }
   return ret;
}

static void insert_inNumericalOrder(DlistNode *dirHead, DlistNode *node)
{
   UINT32 newFilePrefix;
   UINT32 filePrefix;
   DlistNode *tmp = dirHead->next;
   CmsRet ret;
   
   ret = oal_getIntPrefixFromFileName(((CmsFileListEntry *) node)->filename, &newFilePrefix);
   if (ret != CMSRET_SUCCESS)
   {
      return;
   }
   while (tmp != dirHead)
   {
      if (oal_getIntPrefixFromFileName(((CmsFileListEntry *) tmp)->filename,&filePrefix) == CMSRET_SUCCESS)
      {
         if (newFilePrefix < filePrefix)
         {
            dlist_prepend(node, tmp);
            return;
         }
      }
      tmp = tmp->next;
   }
   dlist_prepend(node, dirHead);
}

CmsRet oalFil_getNumericalOrderedFileList(const char *dirname, DlistNode *dirHead)
{
   CmsFileListEntry *fent;
   DIR *d;
   struct dirent *dent;

   if (NULL == (d = opendir(dirname)))
   {
      cmsLog_error("could not open %s", dirname);
      return CMSRET_OBJECT_NOT_FOUND;
   }

   while (NULL != (dent = readdir(d)))
   {
      if (strcmp(dent->d_name, ".") && strcmp(dent->d_name, ".."))
      {
         fent = cmsMem_alloc(sizeof(CmsFileListEntry), ALLOC_ZEROIZE);
         if (NULL == fent)
         {
            cmsLog_error("Could not allocate memory for fileEntry");
            cmsFil_freeFileList(dirHead);
            closedir(d);
            return CMSRET_RESOURCE_EXCEEDED;
         }
         snprintf(fent->filename, sizeof(fent->filename)-1, "%s", dent->d_name);
         insert_inNumericalOrder(dirHead, (DlistNode *) fent);
      }
   }
   closedir(d);

   return CMSRET_SUCCESS;
}

CmsRet oalFil_readFirstlineFromFile(char *fileName, char *line, UINT32 lineSize)
{
   FILE *fp = NULL;

   if ((fp = fopen(fileName, "r")) == NULL)
   {
      cmsLog_error("failed to open file %s", fileName);
      return CMSRET_INVALID_ARGUMENTS;	  
   }      

   memset(line, 0, lineSize);
   
   if( fgets(line, lineSize, fp) )
   {
      /*Terminate CR*/   
      line[strlen(line)-1] = '\0';
   }
   
   fclose(fp);
   
   return CMSRET_SUCCESS;
}
