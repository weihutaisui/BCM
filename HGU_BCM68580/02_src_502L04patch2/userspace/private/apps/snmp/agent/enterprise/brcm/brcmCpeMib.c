#ifdef BRCM_CPE_MIB

/* General includes */
#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* SNMP includes */
#include "asn1.h"
#include "snmp.h"
#include "agtMib.h"
#include "agtEngine.h"
#include "brcmCpeMib.h"
#include "brcmCpeMibHelper.h"

#ifdef BUILD_SNMP_DEBUG
#include "snmpDebug.h"
extern SNMP_DEBUG_FLAG snmpDebug;
#endif


pBRCM_CPE_INFO pCpeInfo;

void brcmCpeMibFree(void)
{
  if (pCpeInfo)
    free(pCpeInfo);
}

void brcmInitCpeInfo(pBRCM_CPE_INFO pCpeInfo) {
  pCpeInfo->wirelessTransceiver = brcmCpeGetWirelessTransceiver();
}

/* CPE_DSL_MIB initialisation (must also register the MIB module tree) */
void init_brcm_cpe_mib(void)
{
  register_subtrees_of_brcm_cpe_mib();

  pCpeInfo = (pBRCM_CPE_INFO)malloc(sizeof(BRCM_CPE_INFO));
  if (pCpeInfo == NULL)
    return;
  memset(pCpeInfo,0,sizeof(BRCM_CPE_INFO));
  brcmInitCpeInfo(pCpeInfo);
}

/* 
 * for synchronizing configuration by network manager.
 */
int write_wirelessTransceiver(int action, unsigned char *var_val, unsigned char varval_type, int var_val_len,
			      unsigned char *statP, oid *name, int name_len)
{
  int input;

  switch (action) 
    {
    case RESERVE1:
      if (varval_type != SNMP_INTEGER)
        return SNMP_ERROR_WRONGTYPE;
      if ( ((int)*var_val != BRCM_CPE_WIRELESS_TRANSCEIVER_ON) &&
	   ((int)*var_val != BRCM_CPE_WIRELESS_TRANSCEIVER_OFF) )
	return SNMP_ERROR_WRONGVALUE;
      break;
    case RESERVE2:
      break;
    case COMMIT:
      input = (int)*var_val;
#ifdef BUILD_SNMP_DEBUG
      printf("write_wirelessTransceiver(): input value %x\n",input);
#endif
      brcmCpeSetWirelessTransceiver(input);
    case FREE:
      break;
    }
    return SNMP_ERROR_NOERROR;
}

/* Wireless transceiver mode.   When off, wireless traffic is received nor trasnmitted. */
unsigned char *
var_wirelessTransceiver(int *var_len, snmp_info_t *mesg, WRITE_METHOD *write_method)
                       
{
  *write_method = (WRITE_METHOD)write_wirelessTransceiver;

  long_return = brcmCpeGetWirelessTransceiver();
  *var_len = sizeof(long);
  return (unsigned char *)&long_return;
}

static oid wireless_oid[] = { O_wireless };
static Object wireless_variables[] = {
    { SNMP_INTEGER, (RWRITE| SCALAR), (void*)var_wirelessTransceiver,
                 {2, { I_wirelessTransceiver, 0 }}},
    { (int)NULL }
    };
static SubTree wireless_tree =  { NULL, wireless_variables,
	        (sizeof(wireless_oid)/sizeof(oid)), wireless_oid};

/* This is the MIB registration function. This should be called */
/* within the init_BRCM_DSL_CPE-function */
void register_subtrees_of_brcm_cpe_mib(void)
{
    insert_group_in_mib(&wireless_tree);
}

#endif /* BRCM_CPE_MIB */
