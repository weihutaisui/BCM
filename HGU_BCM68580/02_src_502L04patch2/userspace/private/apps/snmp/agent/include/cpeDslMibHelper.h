#ifndef	_CPE_DSL_MIB_HELPER_H_
#define	_CPE_DSL_MIB_HELPER_H_
#ifdef LGD_TO_DO
#include "ifcdefs.h"
#endif
#include "cpeDslMib.h"

#define CPE_AGENT_ERROR     -1
#define CPE_AGENT_OK        0

typedef struct cpePvcEntry {
  int portId;
  int vpi;
  int vci;
  int pvcId;       /* conId of Wan Info */
  int ifIndex;     /* ifIndex */
  int upperEncap;  /* protocol of Wan Info */
  int adminStatus; /* service of Wan Flag Info */
  int operStatus;  /* wan info status */
  int bridgeMode;  /* derived from protocol; adminEnable or adminDisable */
  int macLearnMode; /* derived from protocol; adminEnable or adminDisable */
  int igmpMode;     
  int dhcpcMode;
  int natMode;
#ifdef CT_SNMP_MIB_VERSION_2
  int bindingMode;
  UINT32 ethernetPortMap;
  UINT32 serviceTypeIndex;
#endif
  int conId;        
  int pvcIfIndex;

  struct cpePvcEntry *next;
} CPE_PVC_ENTRY, *pCPE_PVC_ENTRY;

typedef struct wanInfoList {
  pCPE_PVC_ENTRY head;
  pCPE_PVC_ENTRY tail;
  int count;
} WAN_INFO_LIST, *pWAN_INFO_LIST;

typedef struct cpePvcParam {
  int vpi;
  int vci;
  int column;
  int value;
} CPE_PVC_PARAM, *pCPE_PVC_PARAM;

#define CPE_PVC_ENCAP_PPPOA         1
#define CPE_PVC_ENCAP_PPPOE         2
#define CPE_PVC_ENCAP_ROUTER2684    3
#define CPE_PVC_ENCAP_BRIDGE2684    4
#define CPE_PVC_ENCAP_BRIDGE2684IP  5
#define CPE_PVC_ENCAP_ILMI          7

#define CPE_PPP_ADMIN_UP            1
#define CPE_PPP_ADMIN_DOWN          2
#define CPE_PPP_ADMIN_TESTING       3
#define CPE_PPP_ADMIN_CANCEL        4
#define CPE_PPP_TEST_RESULT_SUCCESS 1
#define CPE_PPP_TEST_RESULT_TESTING 2
#define CPE_PPP_TEST_RESULT_AUTHFAIL 3
#define CPE_PPP_TEST_RESULT_FAILURE  4
#define CPE_PPP_TIMEOUT_MAX_SEC      259200   /* WEBUI, 4320 minutes * 60 seconds */
#define CPE_PPP_DEFAULT_MSS          1432     /* ethernet (1500) - ppp 68 */
#define CPE_PPP_PVC_IFINDEX_SET      1
#define CPE_PPP_PVC_OTHER_SET        2

#define CPE_PPP_NAME_LEN             32
#define CPE_PPP_PASSWORD_LEN         32

#define CPE_RESTORE_TO_NULL          1
#define CPE_RESTORE_TO_DEFAULT       2

#define CPE_IP_TABLE_LOWER_INDEX_SET  1
#define CPE_IP_TABLE_IP_SET                      2
#define CPE_IP_TABLE_MASK_SET                 4
#define CPE_IP_TABLE_GW_SET                     8
#define CPE_IP_TABLE_GW_ONOFF_SET        0x10
#define CPE_IP_TABLE_ROW_STATUS_SET    0x20

#ifdef CT_SNMP_MIB_VERSION_2
#define CPE_ETHERNETPORT_IFINDEX_SET 			1
#define CPE_ETHERNETPORT_SERIALNUMBER_SET 	2
#define CPE_ETHERNETPORT_BINDINGMODE_SET   	4
#define CPE_ETHERNETPORT_IGMPONOFF_SET		8
#define CPE_ETHERNETPORT_I8021DPARAMETER_SET 0x10

#define CPE_SERVICETYPE_INDEX_SET				1
#define CPE_PVCSERVICETYPE_SET					2
#define CPE_PVC_PCR_SET							4
#define CPE_PVC_MCR_SET						8
#define CPE_PVC_SCR_SET							0x10
#define CPE_SERVICETYPE_ROWSTATUS_SET		0x20

#define CPE_SERVICETYPE_UBR		1
#define CPE_SERVICETYPE_RT_VBR		2
#define CPE_SERVICETYPE_NRT_VBR	3
#define CPE_SERVICETYPE_CBR		4
#endif
#define CPE_PPP_PVC_INDEX_SET            1
#define CPE_PPP_SERVICE_NAME_SET         2
#define CPE_PPP_TIMEOUT_SET              4
#define CPE_PPP_STATUS_SET               8
#define CPE_PPP_ADMIN_SET                0x10

#define CPE_PVC_ENCAP_SET                1
#define CPE_PVC_ADMIN_SET                2
#define CPE_PVC_BRIDGE_MODE_SET          4
#define CPE_PVC_MAC_LEARN_SET            8
#define CPE_PVC_IGMP_MODE_SET            0x10
#define CPE_PVC_DHCP_CLIENT_SET          0x20
#define CPE_PVC_NAT_SET                  0x40
#ifdef CT_SNMP_MIB_VERSION_2
#define CPE_PVC_BINDING_MODE_SET 0x80
#define CPE_PVC_ETHERNET_PORTMAP_SET 0x100
#define CPE_PVC_SERVICE_TYPEINDEX_SET 0x200
#endif

#ifdef LGD_TO_DO
void cpeGetConfigId(unsigned char *configId, int new);
int cpeSetConfigId(unsigned char *configId);
int cpeGetAdslLineModType(void);
int cpeConvertAdslModLineType(int snmpVar);
int cpeGetAdslTrellisMode();
int cpeSetAdslTrellisMode(int mode);
int cpeSetAdslModulationMode(int mode);
int cpeSetAdslBitSwapMode(int mode);
int cpeGetAdslBitSwapMode();
void cpeGetProdId(char *prodId);
void cpeGetSerialNum(char *serialNum);
void cpeReboot();
void cpeSave();
void cpeRestoreToNull();
void cpeRestoreToDefault();
int cpeSaveTrap(int trapMode);
void cpeScanWanInfo(void);
void cpeScanPppInfo(void);
void cpeFreeWanInfo(void);
void cpeFreePppInfo(void);
void cpeFreeIpInfo(void);
int cpeScanIpTable(void);

#ifdef CT_SNMP_MIB_VERSION_2
int cpeScanEthernetPortTable(void);
void cpeAddEthernetPortInfoToList(pCPE_ETHERNETPORT_TABLE_INFO p);
void cpeFreeEthernetPortInfo(void);
int cpeScanServiceTypeTable(void);
void cpeFreeServiceTypeInfo(void);
int cpeConfigServiceTypeTable(pCPE_SERVICETYPE_TABLE_INFO pServiceType);
#endif
extern int getAdslLineTrellisMode(void);
int cpeAddWanInfo(int vpi, int vci);
int cpeConfigWanInfo();
void cpePppInitDefault(int index, int link, pCPE_PPP_INFO pEntry);
int cpeConfigPppInfo(unsigned long param);
int cpeConfigIpTable(pCPE_IP_TABLE_INFO pIp);
int cpeSetDhcpMode(int mode);
int cpeGetDhcpMode(void);
int cpeGetDnsAddr(int type, struct in_addr *ip);
int cpeSetDnsAddr(int type, struct in_addr *ip);
void cpePollPingResult(int junk);
void cpePollFtpResult(int junk);
void cpePollFtpDlResult(int junk);
int cpeStartPing(void);
int cpeStopPing(void);
void cpeSendPingResult(void);
void cpeFtpStop(void);
void cpeFtpUpgrade(pCPE_FTP_INFO pFtpData);
void cpeFtpTest(pCPE_FTP_INFO pFtpData);
void cpeSendFtpResult(void);
int cpeStopPppMonitor(WAN_CON_ID *wanId);
int cpeStartPppMonitor(WAN_CON_ID *wanId);
void cpeSendPppResult(int result);
void cpeGetSnmpRoCommunity(char *com);
void cpeSaveConfigIdToFlash(unsigned char *configId);
int cpeGetStoredSnmpParam(pCPE_SYS_INFO pSysinfo);
extern void resetRunningConfig(int mode);
extern int BcmPpp_isRunning( PWAN_CON_ID pWanId);
extern void BcmPppoe_createDefault( PWAN_CON_ID pWanId);
extern void BcmPppoa_createDefault(PWAN_CON_ID pWanId);
extern void BcmIpoa_createDefault(PWAN_CON_ID pWanId);

void cpeProto_createDefaultAndSet(pCPE_PVC_ENTRY param, int copy);

#ifdef CT_SNMP_MIB_VERSION_2

int TdAddEntry(CPE_SERVICETYPE_TABLE_INFO* tableRow);
int nmsPortMapAdd(char *grpName, char *grpedIntf);
int nmsGetPvcBindMode(int vpi, int vci);
void  nmsGetPvcGroupName(int vpi, int vci, char *groupName);
int nmsPortMapRemove(char *lst);
int cpeConfigEthernetPortTable(pCPE_ETHERNETPORT_TABLE_INFO pEthernetPort) ;
int nmsGetEthernetPortIgmpOnOff(int ethIfIndex);
int nmsGetEthernetPortI8021dPara(int ethIfIndex);
int nmsGetEthernetPortBindMode(int ethIfIndex);
int cpeQosTagModeConfig(int value);
int cpeSetDhcpRelayAddr(struct in_addr *ip);
int nmsGetPvcEthernetPortMap(int vpi, int vci);
int nmsGetPvcEthernetPortMapNoBindMode(int vpi, int vci);
int nmsSetPvcEthernetPortMapNoBindMode(int vpi, int vci,int value);

#endif
#else
void cpeGetSnmpRoCommunity(char *com);
void cpePollFtpResult(int junk);
void cpePollFtpDlResult(int junk);
void cpeFtpStop(void);
void cpeFtpUpgrade(pCPE_FTP_INFO pFtpData);
void cpeFtpTest(pCPE_FTP_INFO pFtpData);
void cpeSendFtpResult(void);
#endif
#endif /* _CPE_DSL_MIB_HELPER_H_ */
