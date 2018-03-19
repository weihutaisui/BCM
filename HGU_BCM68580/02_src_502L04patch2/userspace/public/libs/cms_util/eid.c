/***********************************************************************
 *
 *  Copyright (c) 2006-2007  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2011:DUAL/GPL:standard

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

#include "cms.h"
#include "cms_util.h"
#include "oal.h"


/*
 * The entityInfoArray has been replaced by text files starting with
 * eid*.txt in the scripts directory.  Broadcom developers should
 * add new EID info in files beginning with eid_bcm_
 * External developers should add EID info in separate files beginning
 * with "eid", for example, eid_custom.txt
 * Please also see cms_eid.h for EID number range assignments.
 */

/* doubly linked list of eid info structs obtained by parsing eid_ files */
static DLIST_HEAD(eInfo_head);
static UINT32 num_eInfo_entries=0;
static UBOOL8 is_eInfo_initialized=FALSE;
static CmsRet init_eInfo(void);
static void insert_eInfo_in_order(CmsEntityInfo **eInfo);


static inline void check_eInfo_init(void)
{
   if (!is_eInfo_initialized)
   {
      if (CMSRET_SUCCESS != init_eInfo())
      {
         cmsLog_error("Failed to initialize CMS Entity Info!!");
      }
      is_eInfo_initialized=TRUE;
   }
}

UINT32 cmsEid_getNumberOfEntityInfoEntries(void)
{
   check_eInfo_init();
   return num_eInfo_entries;
}


const CmsEntityInfo *cmsEid_getFirstEntityInfo(void)
{
   check_eInfo_init();

   if (dlist_empty(&eInfo_head))
   {
      return NULL;
   }

   return ((CmsEntityInfo *) eInfo_head.next);
}


const CmsEntityInfo *cmsEid_getNextEntityInfo(const CmsEntityInfo *eInfo)
{
   check_eInfo_init();

   if (dlist_empty(&eInfo_head))
   {
      return NULL;
   }

   if (NULL == eInfo)
   {
      return cmsEid_getFirstEntityInfo();
   }

   if (eInfo->dlist.next == &eInfo_head)
   {
      return NULL;
   }

   return ((CmsEntityInfo *) eInfo->dlist.next);
}


const CmsEntityInfo *cmsEid_getEntityInfo(CmsEntityId eid)
{
   CmsEntityInfo *eInfo;

   check_eInfo_init();

   dlist_for_each_entry(eInfo, &eInfo_head, dlist)
   {
      if (eid == eInfo->eid)
      {
         return eInfo;
      }
   }

   cmsLog_debug("eid %d (0x%x) not found.", eid, eid);
   return NULL;
}


const CmsEntityInfo *cmsEid_getEntityInfoAutoCreate(CmsEntityId eid)
{
   const CmsEntityInfo *eInfo;

   check_eInfo_init();

   eInfo = cmsEid_getEntityInfo(eid);
   if (eInfo == NULL)
   {
      CmsEntityInfo *newInfo;

      /*
       * Could not find an existing eInfo, so just create a "blank" one.
       * This allows simple applications which do not need to specify any
       * eid info to connect to smd.
       */
      cmsLog_notice("Could not find eid %d, create a blank one", eid);
      newInfo = cmsMem_alloc(sizeof(CmsEntityInfo), ALLOC_ZEROIZE);
      if (newInfo == NULL)
      {
         cmsLog_error("allocation of CmsEntityInfo failed.");
      }
      else
      {
         newInfo->name = CMS_UNKNOWN_ENTITY_NAME;
         newInfo->eid = eid;
         insert_eInfo_in_order(&newInfo);
         eInfo = cmsEid_getEntityInfo(eid);
      }
   }

   return eInfo;
}


const CmsEntityInfo *cmsEid_getEntityInfoByAccessBit(UINT16 bit)
{
   CmsEntityInfo *eInfo;

   check_eInfo_init();

   if (bit == 0)
   {
      return NULL;
   }

   dlist_for_each_entry(eInfo, &eInfo_head, dlist)
   {
      if (bit == eInfo->accessBit)
      {
         return eInfo;
      }
   }

   cmsLog_error("Could not find eInfo with accessBit 0x%x", bit);
   return NULL;
}


const CmsEntityInfo *cmsEid_getEntityInfoByStringName(const char *name)
{
   CmsEntityInfo *eInfo;

   check_eInfo_init();

   if ((name == NULL) || (0 == cmsUtl_strlen(name)))
   {
      return NULL;
   }

   dlist_for_each_entry(eInfo, &eInfo_head, dlist)
   {
      if (!cmsUtl_strcmp(eInfo->name, name))
      {
         return eInfo;
      }
   }

   return NULL;
}


CmsRet cmsEid_getStringNamesFromBitMask(UINT16 bitMask, char **buf)
{
   UINT32 i, mask;
   UINT32 len=1; /* for empty mask, return a buffer with an empty string. */
   UINT32 idx=0;
   UINT32 numBits = sizeof(bitMask) * 8;
   const CmsEntityInfo *info;
   UBOOL8 firstName=TRUE;
   CmsRet ret = CMSRET_SUCCESS;

   check_eInfo_init();

   /* first loop calculates how much space needed for string names */
   for (mask=1, i=0; i < numBits; i++)
   {
      if (mask & bitMask)
      {
         info = cmsEid_getEntityInfoByAccessBit(bitMask & mask);
         if (info == NULL)
         {
            cmsLog_error("unrecognized bitmask 0x%x", (bitMask & mask));
            ret = CMSRET_SUCCESS_UNRECOGNIZED_DATA_IGNORED;
         }
         else
         {
            len += cmsUtl_strlen(info->name) + 1;
         }
      }

      mask = mask << 1;
   }

   if (((*buf) = cmsMem_alloc(len, ALLOC_ZEROIZE)) == NULL)
   {
      return CMSRET_RESOURCE_EXCEEDED;
   }

   /* this loop copies string names into buffer */
   for (mask=1, i=0; i < numBits; i++)
   {
      if (mask & bitMask)
      {
         info = cmsEid_getEntityInfoByAccessBit(bitMask & mask);
         if (info != NULL && info->name)
         {
            if (firstName)
            {
               idx = sprintf((*buf), "%s", info->name);
               firstName = FALSE;
            }
            else
            {
               idx += sprintf(&((*buf)[idx]), ",%s", info->name);
            }
         }
      }

      mask = mask << 1;
   }

   cmsAst_assert(idx < len);

   return ret;
}



CmsRet cmsEid_getBitMaskFromStringNames(const char *buf, UINT16 *bitMask)
{
   const char *start;
   const char *end;
   const CmsEntityInfo *info;
   CmsRet ret = CMSRET_SUCCESS;

   check_eInfo_init();

   if (bitMask == NULL)
   {
      return CMSRET_INVALID_ARGUMENTS;
   }

   *bitMask = 0;

   if ((buf == NULL) || (strlen(buf) == 0))
   {
      /* null or empty string means no bits are set */
      return CMSRET_SUCCESS;
   }

   start = buf;
   end = strchr(start, ',');
   while (end != NULL)
   {
      char name[BUFLEN_256]; /* this should be long enough to hold string names */

      cmsAst_assert((end - start + 1) < (SINT32) sizeof(name));
      snprintf(name, end - start + 1, "%s", start);
      info = cmsEid_getEntityInfoByStringName(name);
      if (info == NULL)
      {
         cmsLog_debug("ignoring name %s", name);
         ret = CMSRET_SUCCESS_UNRECOGNIZED_DATA_IGNORED;
      }
      else
      {
         (*bitMask) |= info->accessBit;
      }

      start = end+1;
      while ((*start == ' ') && (*start != 0))
      {
         start++;
      }

      if (*start != 0)
      {
         end = strchr(start, ',');
      }
      else
      {
         end = NULL;
      }
   }

   /* there is one more name at the end (with no trailing comma) */
   info = cmsEid_getEntityInfoByStringName(start);
   if (info == NULL)
   {
      cmsLog_debug("ignoring name %s", start);
      ret = CMSRET_SUCCESS_UNRECOGNIZED_DATA_IGNORED;
   }
   else
   {
      (*bitMask) |= info->accessBit;
   }



   return ret;
}


void cmsEid_dumpEntityInfo(const CmsEntityInfo *eInfo)
{
   if (NULL == eInfo)
   {
      cmsLog_error("Cannot dump NULL eInfo!");
      return;
   }

   printf("====== eid %d (0x%x) ======\n", eInfo->eid, eInfo->eid);
   printf("name        = %s\n", eInfo->name);
   if (eInfo->path)
      printf("path        = %s\n", eInfo->path);
   if (eInfo->runArgs)
      printf("runArgs     = %s\n", eInfo->runArgs);
   if (eInfo->accessBit)
      printf("accessBit   = 0x%x\n", eInfo->accessBit);
   printf("flags       = 0x%x\n", eInfo->flags);
   if (eInfo->port || (eInfo->flags & (EIF_SERVER|EIF_SERVER_TCP)))
      printf("port        = %d\n", eInfo->port);
   if (eInfo->backLog || (eInfo->flags & EIF_SERVER_TCP))
      printf("backLog     = %d\n", eInfo->backLog);
   if (eInfo->schedPolicy || (eInfo->flags & EIF_SET_SCHED))
      printf("schedPolicy    = %d\n", eInfo->schedPolicy);
   if (eInfo->schedPriority || (eInfo->flags & EIF_SET_SCHED))
      printf("schedPriority  = %d\n", eInfo->schedPriority);
   if (eInfo->cpuMask || (eInfo->flags & EIF_SET_CPUMASK))
      printf("cpuMask     = 0x%x\n", eInfo->cpuMask);
   if (eInfo->cpuGroupName || (eInfo->flags & EIF_SET_CGROUPS))
      printf("cpuGroupName= %s\n", eInfo->cpuGroupName);
   printf("\n");
}


SINT32 cmsEid_getPid(void)
{
   return (oal_getPid());
}


/******** internal functions */


/* in eid_parser.c */
extern CmsRet init_eid_symbol_table(void);
extern void free_eid_symbol_table(void);
extern void parse_eid_line(const char *line, int len, CmsEntityInfo *eInfo);

static UBOOL8 is_eInfo_valid(const CmsEntityInfo *eInfo)
{
   if (eInfo->eid == 0)
   {
      cmsLog_error("eInfo must have a non-zero EID");
      return FALSE;
   }

   if (eInfo->name == NULL)
   {
      cmsLog_error("eInfo %d must have a name", eInfo->eid);
      return FALSE;
   }

   if ((eInfo->flags & EIF_SET_CPUMASK) && (eInfo->cpuMask == 0))
   {
      cmsLog_error("eInfo %d has EIF_SET_CPUMASK, but cpuMask is 0",
                    eInfo->eid);
      return FALSE;
   }

   if ((eInfo->flags & EIF_SET_CGROUPS) && (eInfo->cpuGroupName == NULL))
   {
      cmsLog_error("eInfo %d has EIF_SET_CGROUPS, but cpuGroupName is NULL",
                    eInfo->eid);
      return FALSE;
   }

   return TRUE;
}

/** return TRUE if the two eInfo structs are different. */
static UBOOL8 is_eInfo_different(const CmsEntityInfo *orig,
                                 const CmsEntityInfo *curr)
{
   if ((orig->eid != curr->eid) ||
       (orig->accessBit != curr->accessBit) ||
       (cmsUtl_strcmp(orig->name, curr->name)) ||
       (cmsUtl_strcmp(orig->path, curr->path)) ||
       (cmsUtl_strcmp(orig->runArgs, curr->runArgs)) ||
       (orig->flags != curr->flags) ||
       (orig->backLog != curr->backLog) ||
       (orig->port != curr->port) ||
       (orig->maxMemUsage != curr->maxMemUsage) ||
       (orig->normalMemUsage != curr->normalMemUsage) ||
       (orig->minMemUsage != curr->minMemUsage) ||
       (orig->schedPolicy != curr->schedPolicy) ||
       (orig->schedPriority != curr->schedPriority) ||
       (orig->cpuMask != curr->cpuMask) ||
       (cmsUtl_strcmp(orig->cpuGroupName, curr->cpuGroupName)))
   {
      return TRUE;
   }

   return FALSE;
}


/** take data from the new struct and put it in the original entry.
 * Free the new struct.  This round-about procedure is needed in case
 * smd has a reference to the existing structure.
 */
static void overwrite_eInfo(CmsEntityInfo *orig, CmsEntityInfo *curr)
{
   char *tmp;

   if (cmsUtl_strcmp(orig->name, curr->name))
   {
      cmsLog_error("Cannot change the name field on eid %d (%s -> %s)",
            orig->eid, orig->name, curr->name);
      cmsMem_free(curr);
      return;
   }

   orig->accessBit = curr->accessBit;
   orig->flags = curr->flags;
   orig->backLog = curr->backLog;
   orig->port = curr->port;
   orig->maxMemUsage = curr->maxMemUsage;
   orig->normalMemUsage = curr->normalMemUsage;
   orig->minMemUsage = curr->minMemUsage;
   orig->schedPolicy = curr->schedPolicy;
   orig->schedPriority = curr->schedPriority;
   orig->cpuMask = curr->cpuMask;

   if (cmsUtl_strcmp(orig->path, curr->path))
   {
      tmp = orig->path;
      orig->path = curr->path;
      cmsMem_free(tmp);  // cmsMem_free can handle null buf
      curr->path = NULL;
   }

   if (cmsUtl_strcmp(orig->runArgs, curr->runArgs))
   {
      tmp = orig->runArgs;
      orig->runArgs = curr->runArgs;
      cmsMem_free(tmp);  // cmsMem_free can handle null buf
      curr->runArgs = NULL;
   }

   if (cmsUtl_strcmp(orig->cpuGroupName, curr->cpuGroupName))
   {
      tmp = orig->cpuGroupName;
      orig->cpuGroupName = curr->cpuGroupName;
      cmsMem_free(tmp);  // cmsMem_free can handle null buf
      curr->cpuGroupName = NULL;
   }

   printf("Customized eInfo entry detected for\n");
   cmsEid_dumpEntityInfo(orig);

   return;
}

static void free_eInfo(CmsEntityInfo **eInfo)
{
   CMSMEM_FREE_BUF_AND_NULL_PTR((*eInfo)->name);
   CMSMEM_FREE_BUF_AND_NULL_PTR((*eInfo)->path);
   CMSMEM_FREE_BUF_AND_NULL_PTR((*eInfo)->runArgs);
   CMSMEM_FREE_BUF_AND_NULL_PTR((*eInfo)->cpuGroupName);

   CMSMEM_FREE_BUF_AND_NULL_PTR((*eInfo));
}

static void insert_eInfo_in_order(CmsEntityInfo **eInfo)
{
   DlistNode *tmp=eInfo_head.next;

   while (tmp != &eInfo_head)
   {
      if ((*eInfo)->eid == ((CmsEntityInfo *) tmp)->eid)
      {
         if (is_eInfo_different((CmsEntityInfo *) tmp, *eInfo))
         {
            overwrite_eInfo((CmsEntityInfo *) tmp, *eInfo);
         }
         free_eInfo(eInfo);
         return;
      }

      if ((*eInfo)->eid < ((CmsEntityInfo *) tmp)->eid)
      {
          dlist_prepend(((DlistNode *) *eInfo), tmp);
          num_eInfo_entries++;
          *eInfo = NULL;
          return;
      }
      tmp = tmp->next;
   }

   dlist_prepend(((DlistNode *) *eInfo), &eInfo_head);
   num_eInfo_entries++;
   *eInfo = NULL;
   return;
}


/** parse a single eid_*.txt file
 *
 */
static CmsRet parse_eid_file(const char *filename)
{
   CmsEntityInfo *eInfo=NULL;
   char linebuf[MAX_EID_LINE_LENGTH]={0};
   UINT32 linebufLen=sizeof(linebuf);
   FILE *fp;
   CmsRet rval=CMSRET_SUCCESS;

   cmsLog_debug("filename=%s", filename);

   if (NULL == (fp = fopen(filename, "r")))
   {
      cmsLog_error("Could not open %s", filename);
      return CMSRET_OBJECT_NOT_FOUND;
   }

   while (CMSRET_SUCCESS == rval &&
          cmsTok_getNextDataLine(fp, linebuf, &linebufLen))
   {
      if (!strncmp(linebuf, "BEGIN", 5))
      {
         if (NULL == eInfo)
         {
            eInfo = (CmsEntityInfo *) cmsMem_alloc(sizeof(CmsEntityInfo), ALLOC_ZEROIZE);
            if (NULL == eInfo)
            {
               cmsLog_error("Could not allocate CmsEntityInfo");
               rval = CMSRET_RESOURCE_EXCEEDED;
            }
         }
         else
         {
            cmsLog_error("invalid formatting in %s, expecting END, but got BEGIN?",
                         filename);
            free_eInfo(&eInfo);
            rval = CMSRET_INVALID_ARGUMENTS;
         }
      }
      else if (!strncmp(linebuf, "END", 3))
      {
         if (NULL != eInfo)
         {
            if (is_eInfo_valid(eInfo))
            {
               insert_eInfo_in_order(&eInfo);
            }
            else
            {
               free_eInfo(&eInfo);
            }
         }
         else
         {
            cmsLog_error("invalid formatting in %s, unexpected END?",
                         filename);
            rval = CMSRET_INVALID_ARGUMENTS;
         }
      }
      else
      {
         /* we entered the body of the BEGIN/END block.  Parse it */
         if (NULL != eInfo)
         {
            parse_eid_line(linebuf, linebufLen, eInfo);
         }
         else
         {
            cmsLog_error("invalid formatting in %s, missing BEGIN?",
                         filename);
            rval = CMSRET_INVALID_ARGUMENTS;
         }
      }

      /* reset linebufLen to the full length of the buf before next call */
      linebufLen = sizeof(linebuf);
   }

   if (eInfo != NULL)
   {
      cmsLog_error("invalid formatting in %s, missing END?", filename);
      free_eInfo(&eInfo);
      rval = CMSRET_INVALID_ARGUMENTS;
   }

   fclose(fp);

   return rval;
}


CmsRet init_eInfo()
{
   DLIST_HEAD(dirHead);
   char eidPath[CMS_MAX_FULLPATH_LENGTH]={0};
   CmsRet rval;

   if (CMSRET_SUCCESS != (rval = init_eid_symbol_table()))
   {
      return rval;
   }

   rval = cmsUtl_getRunTimePath(CMS_EID_DIR, eidPath, sizeof(eidPath));
   if (rval != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not form root path to %s", CMS_EID_DIR);
      return rval;
   }


   if (CMSRET_SUCCESS == (rval = cmsFil_getOrderedFileList(eidPath, &dirHead)))
   {
      DlistNode *tmp = dirHead.next;
      while (rval == CMSRET_SUCCESS && tmp != &dirHead)
      {
         CmsFileListEntry *fent = (CmsFileListEntry *) tmp;
         if (fent->filename[0] == 'e' &&
             fent->filename[1] == 'i' &&
             fent->filename[2] == 'd')
         {
            char fullpathname[CMS_MAX_FULLPATH_LENGTH]={0};
            SINT32 rc;
            rc = snprintf(fullpathname, sizeof(fullpathname), "%s/%s",
                          eidPath, fent->filename);
            if (rc >= (SINT32) sizeof(fullpathname))
            {
               cmsLog_error("CMS_MAX_FULLPATH_LENGTH (%d) exceeded on %s",
                            CMS_MAX_FULLPATH_LENGTH, fullpathname);
            }
            rval = parse_eid_file(fullpathname);
         }

         tmp = tmp->next;
      }

      cmsFil_freeFileList(&dirHead);
   }

   free_eid_symbol_table();

   return rval;
}

