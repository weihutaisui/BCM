#ifndef	_IF_MIB_
#define	_IF_MIB_

/* required include files (IMPORTS) */
#include	"ifTypeMib.h"
#include	"snmpv2Mib.h"
#include	"snmpv2Tc.h"
#include	"snmpv2Smi.h"

void ifMibFree(void);
void init_ifmib(void);
void register_subtrees_of_ifmib(void);

/* defined objects in this module */


/* MIB object ifMIB = mib_2, 31 */
#define	I_ifMIB	31
#define	O_ifMIB	1, 3, 6, 1, 2, 1, 31

/* MIB object ifMIBObjects = ifMIB, 1 */
#define	I_ifMIBObjects	1
#define	O_ifMIBObjects	1, 3, 6, 1, 2, 1, 31, 1

/* MIB object interfaces = mib_2, 2 */
#define	I_interfaces	2
#define	O_interfaces	1, 3, 6, 1, 2, 1, 2

/* MIB object ifNumber = interfaces, 1 */
#define	I_ifNumber	1
#define	O_ifNumber	1, 3, 6, 1, 2, 1, 2, 1

/* MIB object ifTableLastChange = ifMIBObjects, 5 */
#define	I_ifTableLastChange	5
#define	O_ifTableLastChange	1, 3, 6, 1, 2, 1, 31, 1, 5

/* MIB object ifTable = interfaces, 2 */
#define	I_ifTable	2
#define	O_ifTable	1, 3, 6, 1, 2, 1, 2, 2

/* MIB object ifEntry = ifTable, 1 */
#define	I_ifEntry	1
#define	O_ifEntry	1, 3, 6, 1, 2, 1, 2, 2, 1

/* MIB object ifIndex = ifEntry, 1 */
#define	I_ifIndex	1
#define	O_ifIndex	1, 3, 6, 1, 2, 1, 2, 2, 1, 1

/* MIB object ifDescr = ifEntry, 2 */
#define	I_ifDescr	2
#define	O_ifDescr	1, 3, 6, 1, 2, 1, 2, 2, 1, 2

/* MIB object ifType = ifEntry, 3 */
#define	I_ifType	3
#define	O_ifType	1, 3, 6, 1, 2, 1, 2, 2, 1, 3

/* MIB object ifMtu = ifEntry, 4 */
#define	I_ifMtu	4
#define	O_ifMtu	1, 3, 6, 1, 2, 1, 2, 2, 1, 4

/* MIB object ifSpeed = ifEntry, 5 */
#define	I_ifSpeed	5
#define	O_ifSpeed	1, 3, 6, 1, 2, 1, 2, 2, 1, 5

/* MIB object ifPhysAddress = ifEntry, 6 */
#define	I_ifPhysAddress	6
#define	O_ifPhysAddress	1, 3, 6, 1, 2, 1, 2, 2, 1, 6

/* MIB object ifAdminStatus = ifEntry, 7 */
#define	I_ifAdminStatus	7
#define	O_ifAdminStatus	1, 3, 6, 1, 2, 1, 2, 2, 1, 7

/* MIB object ifOperStatus = ifEntry, 8 */
#define	I_ifOperStatus	8
#define	O_ifOperStatus	1, 3, 6, 1, 2, 1, 2, 2, 1, 8

/* MIB object ifLastChange = ifEntry, 9 */
#define	I_ifLastChange	9
#define	O_ifLastChange	1, 3, 6, 1, 2, 1, 2, 2, 1, 9

/* MIB object ifInOctets = ifEntry, 10 */
#define	I_ifInOctets	10
#define	O_ifInOctets	1, 3, 6, 1, 2, 1, 2, 2, 1, 10

/* MIB object ifInUcastPkts = ifEntry, 11 */
#define	I_ifInUcastPkts	11
#define	O_ifInUcastPkts	1, 3, 6, 1, 2, 1, 2, 2, 1, 11

/* MIB object ifInNUcastPkts = ifEntry, 12 */
#define	I_ifInNUcastPkts	12
#define	O_ifInNUcastPkts	1, 3, 6, 1, 2, 1, 2, 2, 1, 12

/* MIB object ifInDiscards = ifEntry, 13 */
#define	I_ifInDiscards	13
#define	O_ifInDiscards	1, 3, 6, 1, 2, 1, 2, 2, 1, 13

/* MIB object ifInErrors = ifEntry, 14 */
#define	I_ifInErrors	14
#define	O_ifInErrors	1, 3, 6, 1, 2, 1, 2, 2, 1, 14

/* MIB object ifInUnknownProtos = ifEntry, 15 */
#define	I_ifInUnknownProtos	15
#define	O_ifInUnknownProtos	1, 3, 6, 1, 2, 1, 2, 2, 1, 15

/* MIB object ifOutOctets = ifEntry, 16 */
#define	I_ifOutOctets	16
#define	O_ifOutOctets	1, 3, 6, 1, 2, 1, 2, 2, 1, 16

/* MIB object ifOutUcastPkts = ifEntry, 17 */
#define	I_ifOutUcastPkts	17
#define	O_ifOutUcastPkts	1, 3, 6, 1, 2, 1, 2, 2, 1, 17

/* MIB object ifOutNUcastPkts = ifEntry, 18 */
#define	I_ifOutNUcastPkts	18
#define	O_ifOutNUcastPkts	1, 3, 6, 1, 2, 1, 2, 2, 1, 18

/* MIB object ifOutDiscards = ifEntry, 19 */
#define	I_ifOutDiscards	19
#define	O_ifOutDiscards	1, 3, 6, 1, 2, 1, 2, 2, 1, 19

/* MIB object ifOutErrors = ifEntry, 20 */
#define	I_ifOutErrors	20
#define	O_ifOutErrors	1, 3, 6, 1, 2, 1, 2, 2, 1, 20

/* MIB object ifOutQLen = ifEntry, 21 */
#define	I_ifOutQLen	21
#define	O_ifOutQLen	1, 3, 6, 1, 2, 1, 2, 2, 1, 21

/* MIB object ifSpecific = ifEntry, 22 */
#define	I_ifSpecific	22
#define	O_ifSpecific	1, 3, 6, 1, 2, 1, 2, 2, 1, 22

/* MIB object ifXTable = ifMIBObjects, 1 */
#define	I_ifXTable	1
#define	O_ifXTable	1, 3, 6, 1, 2, 1, 31, 1, 1

/* MIB object ifXEntry = ifXTable, 1 */
#define	I_ifXEntry	1
#define	O_ifXEntry	1, 3, 6, 1, 2, 1, 31, 1, 1, 1

/* MIB object ifName = ifXEntry, 1 */
#define	I_ifName	1
#define	O_ifName	1, 3, 6, 1, 2, 1, 31, 1, 1, 1, 1

/* MIB object ifInMulticastPkts = ifXEntry, 2 */
#define	I_ifInMulticastPkts	2
#define	O_ifInMulticastPkts	1, 3, 6, 1, 2, 1, 31, 1, 1, 1, 2

/* MIB object ifInBroadcastPkts = ifXEntry, 3 */
#define	I_ifInBroadcastPkts	3
#define	O_ifInBroadcastPkts	1, 3, 6, 1, 2, 1, 31, 1, 1, 1, 3

/* MIB object ifOutMulticastPkts = ifXEntry, 4 */
#define	I_ifOutMulticastPkts	4
#define	O_ifOutMulticastPkts	1, 3, 6, 1, 2, 1, 31, 1, 1, 1, 4

/* MIB object ifOutBroadcastPkts = ifXEntry, 5 */
#define	I_ifOutBroadcastPkts	5
#define	O_ifOutBroadcastPkts	1, 3, 6, 1, 2, 1, 31, 1, 1, 1, 5

/* MIB object ifHCInOctets = ifXEntry, 6 */
#define	I_ifHCInOctets	6
#define	O_ifHCInOctets	1, 3, 6, 1, 2, 1, 31, 1, 1, 1, 6

/* MIB object ifHCInUcastPkts = ifXEntry, 7 */
#define	I_ifHCInUcastPkts	7
#define	O_ifHCInUcastPkts	1, 3, 6, 1, 2, 1, 31, 1, 1, 1, 7

/* MIB object ifHCInMulticastPkts = ifXEntry, 8 */
#define	I_ifHCInMulticastPkts	8
#define	O_ifHCInMulticastPkts	1, 3, 6, 1, 2, 1, 31, 1, 1, 1, 8

/* MIB object ifHCInBroadcastPkts = ifXEntry, 9 */
#define	I_ifHCInBroadcastPkts	9
#define	O_ifHCInBroadcastPkts	1, 3, 6, 1, 2, 1, 31, 1, 1, 1, 9

/* MIB object ifHCOutOctets = ifXEntry, 10 */
#define	I_ifHCOutOctets	10
#define	O_ifHCOutOctets	1, 3, 6, 1, 2, 1, 31, 1, 1, 1, 10

/* MIB object ifHCOutUcastPkts = ifXEntry, 11 */
#define	I_ifHCOutUcastPkts	11
#define	O_ifHCOutUcastPkts	1, 3, 6, 1, 2, 1, 31, 1, 1, 1, 11

/* MIB object ifHCOutMulticastPkts = ifXEntry, 12 */
#define	I_ifHCOutMulticastPkts	12
#define	O_ifHCOutMulticastPkts	1, 3, 6, 1, 2, 1, 31, 1, 1, 1, 12

/* MIB object ifHCOutBroadcastPkts = ifXEntry, 13 */
#define	I_ifHCOutBroadcastPkts	13
#define	O_ifHCOutBroadcastPkts	1, 3, 6, 1, 2, 1, 31, 1, 1, 1, 13

/* MIB object ifLinkUpDownTrapEnable = ifXEntry, 14 */
#define	I_ifLinkUpDownTrapEnable	14
#define	O_ifLinkUpDownTrapEnable	1, 3, 6, 1, 2, 1, 31, 1, 1, 1, 14

/* MIB object ifHighSpeed = ifXEntry, 15 */
#define	I_ifHighSpeed	15
#define	O_ifHighSpeed	1, 3, 6, 1, 2, 1, 31, 1, 1, 1, 15

/* MIB object ifPromiscuousMode = ifXEntry, 16 */
#define	I_ifPromiscuousMode	16
#define	O_ifPromiscuousMode	1, 3, 6, 1, 2, 1, 31, 1, 1, 1, 16

/* MIB object ifConnectorPresent = ifXEntry, 17 */
#define	I_ifConnectorPresent	17
#define	O_ifConnectorPresent	1, 3, 6, 1, 2, 1, 31, 1, 1, 1, 17

/* MIB object ifAlias = ifXEntry, 18 */
#define	I_ifAlias	18
#define	O_ifAlias	1, 3, 6, 1, 2, 1, 31, 1, 1, 1, 18

/* MIB object ifCounterDiscontinuityTime = ifXEntry, 19 */
#define	I_ifCounterDiscontinuityTime	19
#define	O_ifCounterDiscontinuityTime	1, 3, 6, 1, 2, 1, 31, 1, 1, 1, 19

/* MIB object ifStackTable = ifMIBObjects, 2 */
#define	I_ifStackTable	2
#define	O_ifStackTable	1, 3, 6, 1, 2, 1, 31, 1, 2

/* MIB object ifStackEntry = ifStackTable, 1 */
#define	I_ifStackEntry	1
#define	O_ifStackEntry	1, 3, 6, 1, 2, 1, 31, 1, 2, 1

/* MIB object ifStackHigherLayer = ifStackEntry, 1 */
#define	I_ifStackHigherLayer	1
#define	O_ifStackHigherLayer	1, 3, 6, 1, 2, 1, 31, 1, 2, 1, 1

/* MIB object ifStackLowerLayer = ifStackEntry, 2 */
#define	I_ifStackLowerLayer	2
#define	O_ifStackLowerLayer	1, 3, 6, 1, 2, 1, 31, 1, 2, 1, 2

/* MIB object ifStackStatus = ifStackEntry, 3 */
#define	I_ifStackStatus	3
#define	O_ifStackStatus	1, 3, 6, 1, 2, 1, 31, 1, 2, 1, 3

/* MIB object ifStackLastChange = ifMIBObjects, 6 */
#define	I_ifStackLastChange	6
#define	O_ifStackLastChange	1, 3, 6, 1, 2, 1, 31, 1, 6

/* MIB object ifRcvAddressTable = ifMIBObjects, 4 */
#define	I_ifRcvAddressTable	4
#define	O_ifRcvAddressTable	1, 3, 6, 1, 2, 1, 31, 1, 4

/* MIB object ifRcvAddressEntry = ifRcvAddressTable, 1 */
#define	I_ifRcvAddressEntry	1
#define	O_ifRcvAddressEntry	1, 3, 6, 1, 2, 1, 31, 1, 4, 1

/* MIB object ifRcvAddressAddress = ifRcvAddressEntry, 1 */
#define	I_ifRcvAddressAddress	1
#define	O_ifRcvAddressAddress	1, 3, 6, 1, 2, 1, 31, 1, 4, 1, 1

/* MIB object ifRcvAddressStatus = ifRcvAddressEntry, 2 */
#define	I_ifRcvAddressStatus	2
#define	O_ifRcvAddressStatus	1, 3, 6, 1, 2, 1, 31, 1, 4, 1, 2

/* MIB object ifRcvAddressType = ifRcvAddressEntry, 3 */
#define	I_ifRcvAddressType	3
#define	O_ifRcvAddressType	1, 3, 6, 1, 2, 1, 31, 1, 4, 1, 3

/* MIB object linkDown = snmpTraps, 3 */
#define	I_linkDown	3
#define	O_linkDown	1, 3, 6, 1, 6, 3, 1, 1, 5, 3

/* MIB object linkUp = snmpTraps, 4 */
#define	I_linkUp	4
#define	O_linkUp	1, 3, 6, 1, 6, 3, 1, 1, 5, 4

/* MIB object ifConformance = ifMIB, 2 */
#define	I_ifConformance	2
#define	O_ifConformance	1, 3, 6, 1, 2, 1, 31, 2

/* MIB object ifGroups = ifConformance, 1 */
#define	I_ifGroups	1
#define	O_ifGroups	1, 3, 6, 1, 2, 1, 31, 2, 1

/* MIB object ifCompliances = ifConformance, 2 */
#define	I_ifCompliances	2
#define	O_ifCompliances	1, 3, 6, 1, 2, 1, 31, 2, 2

/* MIB object ifCompliance3 = ifCompliances, 3 */
#define	I_ifCompliance3	3
#define	O_ifCompliance3	1, 3, 6, 1, 2, 1, 31, 2, 2, 3

/* MIB object ifGeneralInformationGroup = ifGroups, 10 */
#define	I_ifGeneralInformationGroup	10
#define	O_ifGeneralInformationGroup	1, 3, 6, 1, 2, 1, 31, 2, 1, 10

/* MIB object ifFixedLengthGroup = ifGroups, 2 */
#define	I_ifFixedLengthGroup	2
#define	O_ifFixedLengthGroup	1, 3, 6, 1, 2, 1, 31, 2, 1, 2

/* MIB object ifHCFixedLengthGroup = ifGroups, 3 */
#define	I_ifHCFixedLengthGroup	3
#define	O_ifHCFixedLengthGroup	1, 3, 6, 1, 2, 1, 31, 2, 1, 3

/* MIB object ifPacketGroup = ifGroups, 4 */
#define	I_ifPacketGroup	4
#define	O_ifPacketGroup	1, 3, 6, 1, 2, 1, 31, 2, 1, 4

/* MIB object ifHCPacketGroup = ifGroups, 5 */
#define	I_ifHCPacketGroup	5
#define	O_ifHCPacketGroup	1, 3, 6, 1, 2, 1, 31, 2, 1, 5

/* MIB object ifVHCPacketGroup = ifGroups, 6 */
#define	I_ifVHCPacketGroup	6
#define	O_ifVHCPacketGroup	1, 3, 6, 1, 2, 1, 31, 2, 1, 6

/* MIB object ifRcvAddressGroup = ifGroups, 7 */
#define	I_ifRcvAddressGroup	7
#define	O_ifRcvAddressGroup	1, 3, 6, 1, 2, 1, 31, 2, 1, 7

/* MIB object ifStackGroup2 = ifGroups, 11 */
#define	I_ifStackGroup2	11
#define	O_ifStackGroup2	1, 3, 6, 1, 2, 1, 31, 2, 1, 11

/* MIB object ifCounterDiscontinuityGroup = ifGroups, 13 */
#define	I_ifCounterDiscontinuityGroup	13
#define	O_ifCounterDiscontinuityGroup	1, 3, 6, 1, 2, 1, 31, 2, 1, 13

/* MIB object linkUpDownNotificationsGroup = ifGroups, 14 */
#define	I_linkUpDownNotificationsGroup	14
#define	O_linkUpDownNotificationsGroup	1, 3, 6, 1, 2, 1, 31, 2, 1, 14

/* MIB object ifTestTable = ifMIBObjects, 3 */
#define	I_ifTestTable	3
#define	O_ifTestTable	1, 3, 6, 1, 2, 1, 31, 1, 3

/* MIB object ifTestEntry = ifTestTable, 1 */
#define	I_ifTestEntry	1
#define	O_ifTestEntry	1, 3, 6, 1, 2, 1, 31, 1, 3, 1

/* MIB object ifTestId = ifTestEntry, 1 */
#define	I_ifTestId	1
#define	O_ifTestId	1, 3, 6, 1, 2, 1, 31, 1, 3, 1, 1

/* MIB object ifTestStatus = ifTestEntry, 2 */
#define	I_ifTestStatus	2
#define	O_ifTestStatus	1, 3, 6, 1, 2, 1, 31, 1, 3, 1, 2

/* MIB object ifTestType = ifTestEntry, 3 */
#define	I_ifTestType	3
#define	O_ifTestType	1, 3, 6, 1, 2, 1, 31, 1, 3, 1, 3

/* MIB object ifTestResult = ifTestEntry, 4 */
#define	I_ifTestResult	4
#define	O_ifTestResult	1, 3, 6, 1, 2, 1, 31, 1, 3, 1, 4

/* MIB object ifTestCode = ifTestEntry, 5 */
#define	I_ifTestCode	5
#define	O_ifTestCode	1, 3, 6, 1, 2, 1, 31, 1, 3, 1, 5

/* MIB object ifTestOwner = ifTestEntry, 6 */
#define	I_ifTestOwner	6
#define	O_ifTestOwner	1, 3, 6, 1, 2, 1, 31, 1, 3, 1, 6

/* MIB object ifGeneralGroup = ifGroups, 1 */
#define	I_ifGeneralGroup	1
#define	O_ifGeneralGroup	1, 3, 6, 1, 2, 1, 31, 2, 1, 1

/* MIB object ifTestGroup = ifGroups, 8 */
#define	I_ifTestGroup	8
#define	O_ifTestGroup	1, 3, 6, 1, 2, 1, 31, 2, 1, 8

/* MIB object ifStackGroup = ifGroups, 9 */
#define	I_ifStackGroup	9
#define	O_ifStackGroup	1, 3, 6, 1, 2, 1, 31, 2, 1, 9

/* MIB object ifOldObjectsGroup = ifGroups, 12 */
#define	I_ifOldObjectsGroup	12
#define	O_ifOldObjectsGroup	1, 3, 6, 1, 2, 1, 31, 2, 1, 12

/* MIB object ifCompliance = ifCompliances, 1 */
#define	I_ifCompliance	1
#define	O_ifCompliance	1, 3, 6, 1, 2, 1, 31, 2, 2, 1

/* MIB object ifCompliance2 = ifCompliances, 2 */
#define	I_ifCompliance2	2
#define	O_ifCompliance2	1, 3, 6, 1, 2, 1, 31, 2, 2, 2

/* Put here additional MIB specific include definitions */
#define IF_LINE_LENGTH     256
#define IF_IANAIFTYPE_OTHER             1
#define IF_IANAIFTYPE_ETHERNETCSMACD    6
#define IF_IANAIFTYPE_TOKENRING         9
#define IF_IANAIFTYPE_FDDI              15
#define IF_IANAIFTYPE_PPP               23
#define IF_IANAIFTYPE_SOFTWARELOOPBACK  24
#define IF_IANAIFTYPE_SLIP              28
#define IF_IANAIFTYPE_ARCNETPLUS        35
#define IF_IANAIFTYPE_ATM               37
#define IF_IANAIFTYPE_LOCALTALK         42
#define IF_IANAIFTYPE_HIPPI             47
#define IF_IANAIFTYPE_AAL5              49
#define IF_IANAIFTYPE_IEEE80211         71
#define IF_IANAIFTYPE_ADSL              94
#define IF_IANAIFTYPE_TUNNEL            131
#define IF_IANAIFTYPE_USB               160
#define IF_IANAIFTYPE_BRIDGE            209

/* some status definition */
#define IF_ADMIN_STATUS_UP              1
#define IF_ADMIN_STATUS_DOWN            2
#define IF_ADMIN_STATUS_TESTING         3
#define IF_OPER_STATUS_UP               1
#define IF_OPER_STATUS_DOWN             2
#define IF_OPER_STATUS_TESTING          3
#define IF_OPER_STATUS_UNKNOWN          4
#define IF_OPER_STATUS_DORMANT          5
#define IF_OPER_STATUS_NOTPRESENT       6
#define IF_OPER_STATUS_LOWER_LAYER_DOWN 7
/* some private device names */
#define IF_BRCM_BR_DEV_NAME             "br"
#define IF_BRCM_LO_DEV_NAME             "lo"
#define IF_BRCM_ETH_DEV_NAME            "eth"
#define IF_BRCM_USB_DEV_NAME            "usb"
#define IF_BRCM_WL_DEV_NAME             "wl"
#define IF_BRCM_DSL_DEV_NAME            "dsl0"
#define IF_BRCM_ATM_DEV_NAME            "atm0"
#define IF_BRCM_ATM_CPCS_DEV_NAME       "cpcs0"
#define IF_BRCM_PPP_DEV_NAME            "ppp"
#define IF_BRCM_NAS_DEV_NAME            "nas"

#define IF_INDEX_BASE                 1
#define IF_INDEX_PVC_BASE             150000
#define IF_INDEX_PVC_END              159999
#define IF_INDEX_PPP_BASE             210000
#define IF_INDEX_PPP_END              219999
#define IF_INDEX_ETH_BASE             10000
#define IF_INDEX_ETH_END              19999
#define IF_INDEX_USB_BASE             20000
#define IF_INDEX_USB_END              29999
#define IF_INDEX_IEEE_802DOT1_BASE    30000
#define IF_INDEX_IEEE_802DOT1_END     39999
#define IF_INDEX_ADSL_BASE            40000
#define IF_INDEX_ADSL_END             49999
#define IF_INDEX_MODEM_BASE           50000
#define IF_INDEX_RS232_BASE           60000
#define IF_INDEX_BRIDGE_BASE          110000
#define IF_INDEX_BRIDGE_END           119999
#define IF_INDEX_ADSL_INTERLEAVE_BASE 120000
#define IF_INDEX_ADSL_FAST_BASE       130000
#define IF_INDEX_ATM_BASE             140000
#define IF_INDEX_ATM_END              149999

typedef struct intfInfo 
{
  char *if_name;       /* name, e.g. ``en'' or ``lo'' */
  int if_index;
  char *if_unit;       /* sub-unit for lower level driver */
  int  if_mtu;        /* maximum transmission unit */
  short if_flags;      /* up/down, broadcast, etc. */
  int if_metric;     /* routing metric (external only) */
  char if_hwaddr[6];  /* ethernet address */
  int if_type;       /* interface type: 1=generic,
                      * 28=slip, ether=6, loopback=24 */
  u_long if_speed;      /* interface speed: in bits/sec */
  struct sockaddr if_addr;       /* interface's address */
  struct sockaddr ifu_broadaddr; /* broadcast address */
  struct sockaddr ia_subnetmask; /* interface's mask */

  struct ifqueue {
    int             ifq_len;
    int             ifq_drops;
  } if_snd;              /* output queue */
  u_long if_ibytes;     /* octets received on interface */
  u_long if_ipackets;   /* packets received on interface */
  u_long if_ierrors;    /* input errors on interface */
  u_long if_iqdrops;    /* input queue overruns */
  u_long if_ififo;
  u_long if_iframe;
  u_long if_icompressed;
  u_long if_multicast;
  u_long if_obytes;     /* octets sent on interface */
  u_long if_opackets;   /* packets sent on interface */
  u_long if_oerrors;    /* output errors on interface */
  u_long if_odrop; 
  u_long if_ofifo; 
  u_long if_collisions; /* collisions on csma interfaces */
  u_long if_carrier;
  u_long if_ocompressed;
  u_long if_lastchange; /* TimeTicks (1/100ths of a second since last change) */
  struct intfInfo *next;
} INTF_INFO, *pINTF_INFO;

typedef struct intfInfoList 
{
  int ifCount;
  pINTF_INFO pIf;
  pINTF_INFO pIfTail;
} INTF_INFO_LIST, *pINTF_INFO_LIST;

int ifGetIfIndexByName(char *name);
int ifGetNameByIfIndex(int index, char *name);
int ifScanInterfaces(void);
int if_type_from_name(char *name);
#endif	/* _IF_MIB_ */
