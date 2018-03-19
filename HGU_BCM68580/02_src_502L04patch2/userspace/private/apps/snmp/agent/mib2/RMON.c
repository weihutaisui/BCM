#ifdef BUILD_SNMP_BRIDGE_MIB
/* General includes */
#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <fcntl.h>
#include <net/if.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//#include <linux/if_arp.h>


/* SNMP includes */
#include "asn1.h"
#include "snmp.h"
#include "agtMib.h"
#include "agtEngine.h"

#include "RMON.h"

#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"


pETHER_STATS_TABLE_LIST pEtherStatsList;

void add_etherStatsInfoToList(pETHER_STATS_ENTRY_INFO p)
{
   if (pEtherStatsList->head == NULL) 
   {
      pEtherStatsList->head = p;
      pEtherStatsList->tail = p;
   }
   else 
   {
      pEtherStatsList->tail->next = p;
      pEtherStatsList->tail = p;
   }
   pEtherStatsList->count++;
}

void free_etherStatsInfo(void) 
{
   pETHER_STATS_ENTRY_INFO ptr;

   while (pEtherStatsList->head) 
   {
      ptr = pEtherStatsList->head;
      pEtherStatsList->head = pEtherStatsList->head->next;
      free(ptr);
   }
   pEtherStatsList->count = 0;
}

int scan_etherStatsTable(void)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanEthIntfObject *ethObj = NULL;
   int index = 1;
   pETHER_STATS_ENTRY_INFO pEntry;

   free_etherStatsInfo();
   while (cmsObj_getNext(MDMOID_LAN_ETH_INTF, &iidStack, (void **) &ethObj) == CMSRET_SUCCESS)
   {
      if ((pEntry= (pETHER_STATS_ENTRY_INFO)malloc(sizeof(ETHER_STATS_ENTRY_INFO))) != NULL) 
      {
         memset(pEntry,0,sizeof(ETHER_STATS_ENTRY_INFO));

         pEntry->etherStatsIndex = index;
         pEntry->etherStatsCollisions = 0;
         add_etherStatsInfoToList(pEntry);
      }
      cmsObj_free((void **) &ethObj);
      index++;
   }

   return 1;
}

unsigned char *
var_etherStatsEntry(int *var_len, Oid *newoid, Oid *reqoid, int searchType, snmp_info_t *mesg, WRITE_METHOD *write_method)
{
   int column = newoid->name[(newoid->namelen - 1)];
   int result;
   pETHER_STATS_ENTRY_INFO pEntry;
   int index;

   index = newoid->namelen;
   newoid->namelen++;
   if( mesg->pdutype != SNMP_SET_REQ_PDU ) 
   {
      scan_etherStatsTable();
      pEntry = pEtherStatsList->head;

      while (pEntry) 
      {
         newoid->name[index] = pEntry->etherStatsIndex;
         /* Determine whether it is the requested OID	*/
         result = compare(reqoid, newoid);
         if (((searchType == EXACT) && (result == 0)) ||((searchType == NEXT) && (result < 0)))
         {
            break; /* found */
         }
         pEntry = pEntry->next;
      } 

      if (pEntry == NULL)
      {
         return NO_MIBINSTANCE;
      }

      *var_len = sizeof(long);

      switch (column) 
      {
         case I_etherStatsIndex:
            return (unsigned char *)&pEntry->etherStatsIndex;
         case I_etherStatsCollisions:
            return (unsigned char *)&pEntry->etherStatsCollisions;
         default:
            return NO_MIBINSTANCE;
      } /* switch */
   } /* != SNMP_SET_REQ_PDU */
   else 
   {
      newoid->name[index] = reqoid->name[index];
      /* return whatever as long as it's non-zero */
      return (unsigned char*)&long_return;
   }
}


static oid etherStatsEntry_oid[] = { O_etherStatsEntry };
static Object etherStatsEntry_variables[] = 
{
   { SNMP_INTEGER, (RONLY| COLUMN), (void*)var_etherStatsEntry,
       {1, { I_etherStatsIndex}}},
   { SNMP_COUNTER, (RONLY| COLUMN), (void*)var_etherStatsEntry,
       {1, { I_etherStatsCollisions }}},
   { (int)NULL }
};

static SubTree etherStatsEntry_tree =  { NULL, etherStatsEntry_variables,
                                        (sizeof(etherStatsEntry_oid)/sizeof(oid)), etherStatsEntry_oid};

/* This is the MIB registration function. This should be called */
/* within the init_rmon-function */
void register_subtrees_of_rmon()
{
    insert_group_in_mib(&etherStatsEntry_tree);
}

/* rmon initialisation (must also register the MIB module tree) */
void init_rmon()
{
    register_subtrees_of_rmon();
    
    if ((pEtherStatsList = (pETHER_STATS_TABLE_LIST)malloc(sizeof(ETHER_STATS_TABLE_LIST))) == NULL) 
    {
       cmsLog_error("init_rmon malloc memory failed for pEtherStatsList");
       exit(0);
    } 
    memset(pEtherStatsList,0,sizeof(ETHER_STATS_TABLE_LIST));

}
#endif
