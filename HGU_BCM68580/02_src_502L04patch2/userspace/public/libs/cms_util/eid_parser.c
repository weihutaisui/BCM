/***********************************************************************
 *
 *  Copyright (c) 2012  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2012:DUAL/GPL:standard

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


#include <stdio.h>

#include "cms.h"
#include "cms_util.h"
#include "oal.h"


#define EID_TOKEN_EID                 120
#define EID_TOKEN_ACCESSBIT           121
#define EID_TOKEN_NAME                122
#define EID_TOKEN_PATH                123
#define EID_TOKEN_RUNARGS             124
#define EID_TOKEN_FLAGS               125
#define EID_TOKEN_BACKLOG             126
#define EID_TOKEN_PORT                127
#define EID_TOKEN_MAXMEMUSAGE         128
#define EID_TOKEN_NORMALMEMUSAGE      129
#define EID_TOKEN_MINMEMUSAGE         130
#define EID_TOKEN_SCHEDPOLICY         131
#define EID_TOKEN_SCHEDPRIORITY       132
#define EID_TOKEN_CPUMASK             133
#define EID_TOKEN_CPUGROUPNAME        134
#define EID_TOKEN_ALTPATH             135


struct cms_token_map_entry eid_token_map_array[] = {
      {"eid",             EID_TOKEN_EID},
      {"accessBit",       EID_TOKEN_ACCESSBIT},
      {"name",            EID_TOKEN_NAME},
      {"path",            EID_TOKEN_PATH},
      {"altPath",         EID_TOKEN_ALTPATH},
      {"runArgs",         EID_TOKEN_RUNARGS},
      {"flags",           EID_TOKEN_FLAGS},
      {"backLog",         EID_TOKEN_BACKLOG},
      {"port",            EID_TOKEN_PORT},
      {"maxMemUsage",     EID_TOKEN_MAXMEMUSAGE},
      {"normalMemUsage",  EID_TOKEN_NORMALMEMUSAGE},
      {"minMemUsage",     EID_TOKEN_MINMEMUSAGE},
      {"schedPolicy",     EID_TOKEN_SCHEDPOLICY},
      {"schedPriority",   EID_TOKEN_SCHEDPRIORITY},
      {"cpuMask",         EID_TOKEN_CPUMASK},
      {"cpuGroupName",    EID_TOKEN_CPUGROUPNAME},
      {NULL, 0}
};



static unsigned int symbol_to_value(const char *valuebuf, unsigned int len);


CmsRet parse_eid_line(const char *line, UINT32 len, CmsEntityInfo *eInfo)
{
   struct cms_token_value_pair pair;

   memset(&pair, 0, sizeof(pair));

   if (!cmsTok_tokenizeLine(line, len, eid_token_map_array, &pair))
   {
      return CMSRET_INVALID_PARAM_NAME;
   }

   switch(pair.token)
   {
   case EID_TOKEN_EID:
      eInfo->eid = symbol_to_value(pair.valuebuf, sizeof(pair.valuebuf));
      // printf("eid = %d (%s)\n", eInfo->eid, pair.valuebuf);
      break;

   case EID_TOKEN_ACCESSBIT:
      eInfo->accessBit = symbol_to_value(pair.valuebuf, sizeof(pair.valuebuf));
      //printf("accessBit = 0x%x\n", eInfo->accessBit);
      break;

   case EID_TOKEN_NAME:
      eInfo->name = cmsMem_strdup(pair.valuebuf);
      //printf("name = %s\n", eInfo->name);
      break;

   case EID_TOKEN_PATH:
         eInfo->path = cmsMem_strdup(pair.valuebuf);
         break;

   case EID_TOKEN_ALTPATH:
         eInfo->altPath = cmsMem_strdup(pair.valuebuf);
         break;

   case EID_TOKEN_RUNARGS:
         eInfo->runArgs = cmsMem_strdup(pair.valuebuf);
         break;

   case EID_TOKEN_FLAGS:
         eInfo->flags = symbol_to_value(pair.valuebuf, sizeof(pair.valuebuf));
         // printf("flags = 0x%x\n", eInfo->flags);
         break;

   case EID_TOKEN_BACKLOG:
         eInfo->backLog = (UINT8) symbol_to_value(pair.valuebuf, sizeof(pair.valuebuf));
         break;

   case EID_TOKEN_PORT:
      eInfo->port = (UINT16) symbol_to_value(pair.valuebuf, sizeof(pair.valuebuf));
#ifdef DESKTOP_LINUX
      if (eInfo->port < 1024)
      {
         eInfo->port += CMS_DESKTOP_SERVER_PORT_OFFSET;
      }
#endif
      break;

   case EID_TOKEN_MAXMEMUSAGE:
      eInfo->maxMemUsage = symbol_to_value(pair.valuebuf, sizeof(pair.valuebuf));
      break;

   case EID_TOKEN_NORMALMEMUSAGE:
      eInfo->normalMemUsage = symbol_to_value(pair.valuebuf, sizeof(pair.valuebuf));
      break;

   case EID_TOKEN_MINMEMUSAGE:
      eInfo->minMemUsage = symbol_to_value(pair.valuebuf, sizeof(pair.valuebuf));
      break;

   case EID_TOKEN_SCHEDPOLICY:
      eInfo->schedPolicy = symbol_to_value(pair.valuebuf, sizeof(pair.valuebuf));
      break;

   case EID_TOKEN_SCHEDPRIORITY:
      eInfo->schedPriority = symbol_to_value(pair.valuebuf, sizeof(pair.valuebuf));
      break;

   case EID_TOKEN_CPUMASK:
      eInfo->cpuMask = symbol_to_value(pair.valuebuf, sizeof(pair.valuebuf));
      break;

   case EID_TOKEN_CPUGROUPNAME:
      eInfo->cpuGroupName = cmsMem_strdup(pair.valuebuf);
      break;

   default:
      cmsLog_error("Unrecognized token %d", pair.token);
      return CMSRET_INTERNAL_ERROR;
      break;
   }

   return CMSRET_SUCCESS;
}



/****************** eid symbol table functions ******************/

static DLIST_HEAD(eid_symbol_head);
#define MAX_EID_SYMBOL_LEN   128

struct eid_symbol_entry {
   DlistNode dlist;
   char symbol[MAX_EID_SYMBOL_LEN];
   UINT32 value;
};

static struct eid_symbol_entry *parse_eid_symbol_line(const char *line, UINT32 len);

CmsRet init_eid_symbol_table()
{
   FILE *fp;
   struct eid_symbol_entry *sym_entry;
   char line[MAX_EID_LINE_LENGTH]={0};
   char path[CMS_MAX_FULLPATH_LENGTH]={0};
   UINT32 lineLen=sizeof(line);
   CmsRet rval;

   rval = cmsUtl_getRunTimePath(CMS_EID_DIR, path, sizeof(path));
   if (rval != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not form root path to %s", CMS_EID_DIR);
      return rval;
   }

   rval = cmsUtl_strncat(path, sizeof(path), "/symbol_table.txt");
   if (rval != CMSRET_SUCCESS)
   {
      return rval;
   }

   cmsLog_debug("opening %s", path);

   /* open file */
   if (NULL == (fp = fopen(path, "r")))
   {
      cmsLog_error("Could not open %s", path);
      return CMSRET_OBJECT_NOT_FOUND;
   }

   while (CMSRET_SUCCESS == rval &&
          cmsTok_getNextDataLine(fp, line, &lineLen))
   {
      sym_entry = parse_eid_symbol_line(line, lineLen);
      if (sym_entry)
      {
         dlist_append((DlistNode *) sym_entry, &eid_symbol_head);
      }
      else
      {
         rval = CMSRET_INTERNAL_ERROR;
      }

      /* reset lineLen to the full length of the line before next call */
      lineLen = sizeof(line);
   }

   fclose(fp);
   return rval;
}

void free_eid_symbol_table()
{
   DlistNode *node = eid_symbol_head.next;

   while (node != &eid_symbol_head)
   {
      dlist_unlink(node);
      node = eid_symbol_head.next;
   }
}


/** parse a line containing data in the form of <symbol> <value>, e.g.
 *
 *    EID_xxx 99
 * zzzz         0x1000
 *
 * whitespace anywhere is ignored.
 * Allocate and fill in an eid_symbol_entry and return it.
 */
struct eid_symbol_entry *parse_eid_symbol_line(const char *line, UINT32 len)
{
   UINT32 i=0;
   struct eid_symbol_entry *sym_entry;

   sym_entry = cmsMem_alloc(sizeof(struct eid_symbol_entry), ALLOC_ZEROIZE);
   if (NULL == sym_entry)
   {
      return NULL;
   }

   /* skip leading whitespace */
   while (i < len && isspace(line[i]))
         i++;

   /* grab symbol */
   if (i < len)
   {
      UINT32 j=0;
      while (!isspace(line[i]) && i < len && j < sizeof(sym_entry->symbol)-1)
      {
         sym_entry->symbol[j] = line[i];
         j++;
         i++;
      }
   }

   /* skip middle whitespace */
   while (i < len && isspace(line[i]))
      i++;

   /* grab value string */
   if (i < len)
   {
      UINT32 j=0;
      char valuebuf[128]={0};
      while (!isspace(line[i]) && i < len && j < sizeof(valuebuf)-1)
      {
         valuebuf[j] = line[i];
         j++;
         i++;
      }
      sym_entry->value = strtoul(valuebuf, NULL, 0);
   }

   return sym_entry;
}

static unsigned int symbol_to_value(const char *valuebuf, UINT32 len)
{
   UINT32 val=0;
   char *endptr=NULL;
   UINT32 i;
   UBOOL8 ordflags=FALSE;
   struct eid_symbol_entry *sym_entry;

   /* first try a straightforward strtoul, if that works, we're done */
   val = strtoul(valuebuf, &endptr, 0);
   if (endptr != valuebuf)
   {
      return val;
   }

   /* check for or'd flags */
   for (i=0; i < len; i++)
   {
      if (valuebuf[i] == '|')
      {
         ordflags=TRUE;
         break;
      }
   }

   /* if not ordflags, then its a simple symbol to value conversion */
   if (!ordflags)
   {
      dlist_for_each_entry(sym_entry, &eid_symbol_head, dlist)
      {
         if (!strcmp(sym_entry->symbol, valuebuf))
         {
            return sym_entry->value;
         }
      }
      cmsLog_error("Could not find symbol %s in eid_symbol_table", valuebuf);
      return 0;
   }

   /* break down the or'd elements one by one */
   i=0;
   while (i < len)
   {
      char tmpbuf[128];
      unsigned int j;

      /* skip any leading whitespace or | signs */
      while ((i < len) &&
            (isspace(valuebuf[i]) || valuebuf[i] == '|'))
      {
         i++;
      }

      if (valuebuf[i] == 0)
      {
         break;
      }

      /* grab the individual symbol */
      memset(tmpbuf, 0, sizeof(tmpbuf));
      j=0;
      while ((i < len) && (j < sizeof(tmpbuf)) &&
             (!isspace(valuebuf[i]) && valuebuf[i] != '|') && valuebuf[i] != 0)
      {
         tmpbuf[j] = valuebuf[i];
         j++;
         i++;
      }

      if (j > 0)
      {
         // printf("processing or'd symbol ==>%s<== (%d)\n", tmpbuf, j);
         val |= symbol_to_value(tmpbuf, j);
         //printf("   val=0x%x\n", val);
      }
   }

   return val;
}


