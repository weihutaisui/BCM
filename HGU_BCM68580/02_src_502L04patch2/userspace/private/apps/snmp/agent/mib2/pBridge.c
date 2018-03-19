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

#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"

#include "pBridge.h"

extern char outputBuffer[];

int g_dot1dStpPriority = 0;

#define DOT1D_PORT_DEFAULTUSER_PRIORITY_MIN_VALUE    0
#define DOT1D_PORT_DEFAULTUSER_PRIORITY_MAX_VALUE    7

pDOT1D_PORT_PRIORITY_TABLE_LIST pDot1dPortPriorityList;

void add_dot1dPortPriorityInfoToList(pDOT1D_PORT_PRIORITY_ENTRY_INFO p)
{
   if (pDot1dPortPriorityList->head == NULL) 
   {
      pDot1dPortPriorityList->head = p;
      pDot1dPortPriorityList->tail = p;
   }
   else 
   {
      pDot1dPortPriorityList->tail->next = p;
      pDot1dPortPriorityList->tail = p;
   }
   pDot1dPortPriorityList->count++;
}

void free_dot1dPortPriorityInfo(void) 
{
   pDOT1D_PORT_PRIORITY_ENTRY_INFO ptr;

   while (pDot1dPortPriorityList->head) 
   {
      ptr = pDot1dPortPriorityList->head;
      pDot1dPortPriorityList->head = pDot1dPortPriorityList->head->next;
      free(ptr);
   }
   pDot1dPortPriorityList->count = 0;
}

int scan_dot1dPortPriorityTable(void)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanEthIntfObject *ethObj = NULL;

   pDOT1D_PORT_PRIORITY_ENTRY_INFO pEntry;

   free_dot1dPortPriorityInfo();

   while (cmsObj_getNext(MDMOID_LAN_ETH_INTF, &iidStack, (void **) &ethObj) == CMSRET_SUCCESS)
   {
      if ((pEntry= (pDOT1D_PORT_PRIORITY_ENTRY_INFO)malloc(sizeof(DOT1D_PORT_PRIORITY_ENTRY_INFO))) != NULL) 
      {
         memset(pEntry,0,sizeof(DOT1D_PORT_PRIORITY_ENTRY_INFO));

         pEntry->index = iidStack.instance[iidStack.currentDepth-1];
         pEntry->dot1dPortDefaultUserPriority = ethObj->X_BROADCOM_COM_EthernetPriorityMark;
         add_dot1dPortPriorityInfoToList(pEntry);
      }
      cmsObj_free((void **) &ethObj);
   }

   return 1;
}

int config_dot1dPortPriorityTable(pDOT1D_PORT_PRIORITY_ENTRY_INFO pEntry)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanEthIntfObject *ethObj = NULL;
   QMgmtClassificationObject *cObj = NULL;

   CmsRet ret;

   cmsLog_error("<LGDDBG pEntry->index[%d]", pEntry->index);
             
   while (cmsObj_getNext(MDMOID_LAN_ETH_INTF, &iidStack, (void **) &ethObj) == CMSRET_SUCCESS)
   {
      cmsLog_error("<LGDDBG iidStack.instance[%d]", (iidStack.instance[iidStack.currentDepth-1]));

      if((iidStack.instance[iidStack.currentDepth-1])==pEntry->index)
      {
         ethObj->X_BROADCOM_COM_EthernetPriorityMark = pEntry->dot1dPortDefaultUserPriority;

         if ((ret = cmsObj_set((void *)ethObj, &iidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsObj_set returns error. ret=%d", ret);
            cmsObj_free((void **) &ethObj);
            return -1;
         }         
      }   
      cmsObj_free((void **) &ethObj);   
   }

   //Update QBridge Queue Classification Table

   while ((ret = cmsObj_getNext(MDMOID_Q_MGMT_CLASSIFICATION, &iidStack, (void **)&cObj)) == CMSRET_SUCCESS)
   {
      if (cObj->X_BROADCOM_COM_VLANIDTag>0)
      {
         char tmpBuf[64];
         memset(tmpBuf, 0, sizeof(tmpBuf));
         sprintf(tmpBuf, "InternetGatewayDevice.LANDevice.1.LANEthernetInterfaceConfig.%d.", pEntry->index);

         if(strstr(cObj->classInterface,tmpBuf))
         {
            cObj->ethernetPriorityMark = pEntry->dot1dPortDefaultUserPriority;

            if ((ret = cmsObj_set((void *)cObj, &iidStack)) != CMSRET_SUCCESS)
            {
               cmsLog_error("cmsObj_set returns error. ret=%d", ret);
               cmsObj_free((void **) &ethObj);
               return -1;
            }              
         }
      }
      cmsObj_free((void **)&cObj);

   }
   cmsMgm_saveConfigToFlash();
   return 0;
}

int write_dot1dPortPriorityEntry(int action,unsigned char *var_val, unsigned char varval_type, 
                                 int var_val_len, unsigned char *statP, Oid *name, int name_len)
{
   int index, column, value;
   int namelen;
   int intValue;
   pDOT1D_PORT_PRIORITY_ENTRY_INFO pEntry= NULL;
   int i, ret;
   static int set=0;
   static DOT1D_PORT_PRIORITY_ENTRY_INFO input;

   namelen = (int)name->namelen;

   index = (int)name->name[namelen-1]; 

   column =  (int)name->name[namelen-2];

   cmsLog_error("<LGDDBG index[%d]", index);
   
   if(set==0) 
   {  
      memset(&input,0,sizeof(DOT1D_PORT_PRIORITY_ENTRY_INFO));
   }   

   switch (action) 
   {
      case RESERVE1:
         switch (column) 
         {
            case I_dot1dPortDefaultUserPriority:
               value = (int)*var_val;
               if((value <DOT1D_PORT_DEFAULTUSER_PRIORITY_MIN_VALUE)
                   ||(value>DOT1D_PORT_DEFAULTUSER_PRIORITY_MAX_VALUE))
               {
                  return SNMP_ERROR_WRONGVALUE;
               }
               set = 1;
               break;
            default:
               return SNMP_ERROR_NOTWRITABLE;
          } /* switch column */
          break;

      case RESERVE2:
          break;

      case COMMIT:
         switch (column) 
         {
            case I_dot1dPortDefaultUserPriority:
               value = (int)*var_val;
               input.dot1dPortDefaultUserPriority = value;
               break;
            default:
               break;
          } /* switch */

          if (set==1) 
          {                         
             scan_dot1dPortPriorityTable();       

             pEntry = pDot1dPortPriorityList->head;
             
             while (pEntry) 
             {
                if(pEntry->index == index)
                { 
                   break;
                }
                else
                {
                   pEntry = pEntry->next;
                }
             }

             if (pEntry == NULL) 
             {
                return SNMP_ERROR_COMMITFAILED;
             }

             pEntry->dot1dPortDefaultUserPriority = input.dot1dPortDefaultUserPriority;

             ret = config_dot1dPortPriorityTable(pEntry);

             if (ret == -1) 
             {
                return SNMP_ERROR_COMMITFAILED;
             }
          }
          break;

      case FREE:
          set = 0;
          memset(&input,0,sizeof(DOT1D_PORT_PRIORITY_ENTRY_INFO));
          break;
   } /* switch */
   return SNMP_ERROR_NOERROR;
}

unsigned char *
var_dot1dPortPriorityEntry(int *var_len, Oid *newoid, Oid *reqoid, int searchType, snmp_info_t *mesg, WRITE_METHOD *write_method)
{
   int column = newoid->name[(newoid->namelen - 1)];
   int result;
   pDOT1D_PORT_PRIORITY_ENTRY_INFO pEntry;
   int index;

#ifdef BUILD_SNMP_SET
   *write_method = (WRITE_METHOD)write_dot1dPortPriorityEntry;
#else
   *write_method = 0;
#endif

   index = newoid->namelen;
   newoid->namelen++;
   if( mesg->pdutype != SNMP_SET_REQ_PDU ) 
   {
      scan_dot1dPortPriorityTable();
      pEntry = pDot1dPortPriorityList->head;
      
      while (pEntry) 
      {         
         newoid->name[index] = pEntry->index;
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
         case I_dot1dPortDefaultUserPriority:
            return (unsigned char *)&pEntry->dot1dPortDefaultUserPriority;
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

#ifdef BUILD_SNMP_SET
int write_dot1dStpPriority(int action,
                      unsigned char *var_val, unsigned char varval_type, int var_val_len,
                      unsigned char *statP, Oid *name, int name_len)
{
   switch (action) 
   {
      case RESERVE1:
         if (varval_type != SNMP_INTEGER)
            return SNMP_ERROR_WRONGTYPE;
         break;
      case RESERVE2:
         break;
      case COMMIT:
         g_dot1dStpPriority = (int)*var_val;
         break;
      case FREE:
         break;
    }
    return SNMP_ERROR_NOERROR;
}
#endif /* BUILD_SNMP_SET */

unsigned char *
var_dot1dStpPriority(int *var_len, Access_rec *access_id,
                     WRITE_METHOD *write_method)
{

#ifdef BUILD_SNMP_SET
   *write_method = (WRITE_METHOD)write_dot1dStpPriority;
#else
   *write_method = 0;
#endif

   *var_len = sizeof(long);
   *(long*)outputBuffer =  g_dot1dStpPriority;

   return (unsigned char *)(outputBuffer);
}

/******************** dot1dPortPriorityEntry ********************************/
static oid dot1dPortPriorityEntry_oid[] = { O_dot1dPortPriorityEntry };
static Object dot1dPortPriorityEntry_variables[] = 
{
   { SNMP_INTEGER, (RWRITE| COLUMN), (void*)var_dot1dPortPriorityEntry ,
       {1, { I_dot1dPortDefaultUserPriority}}},
   { (int)NULL }
};
static SubTree dot1dPortPriorityEntry_tree =  { NULL, dot1dPortPriorityEntry_variables,
                                        (sizeof(dot1dPortPriorityEntry_oid)/sizeof(oid)), dot1dPortPriorityEntry_oid};


/******************** dot1dStpPriority ********************************/
static oid dot1dStp_oid[] = { O_dot1dStp };
static Object dot1dStp_variables[] = 
{
   { SNMP_INTEGER, (RWRITE| SCALAR), (void*)var_dot1dStpPriority ,
       {2, { I_dot1dStpPriority, 0}}},
   { (int)NULL }
};
static SubTree dot1dStp_tree =  { NULL, dot1dStp_variables,
                                        (sizeof(dot1dStp_oid)/sizeof(oid)), dot1dStp_oid};



/* This is the MIB registration function. This should be called */
/* within the init_rmon-function */
void register_subtrees_of_pBridge()
{
   insert_group_in_mib(&dot1dPortPriorityEntry_tree);
   insert_group_in_mib(&dot1dStp_tree);
}

/* rmon initialisation (must also register the MIB module tree) */
void init_pBridge()
{
    register_subtrees_of_pBridge();
    
    if ((pDot1dPortPriorityList = (pDOT1D_PORT_PRIORITY_TABLE_LIST)malloc(sizeof(DOT1D_PORT_PRIORITY_TABLE_LIST))) == NULL) 
    {
       cmsLog_error("init_pBridge malloc memory failed for pDot1dPortPriorityList");
       exit(0);
    } 
    memset(pDot1dPortPriorityList,0,sizeof(DOT1D_PORT_PRIORITY_TABLE_LIST));

}
#endif
