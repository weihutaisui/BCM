#ifndef _QBRIDGE_H_
#define _QBRIDGE_H_

#include "cms.h"

/* defined objects in this module */

/* MIB object dot1qTpFdbEntry = dot1qTpFdbTable, 1 */
#define I_dot1qTpFdbEntry   1
#define O_dot1qTpFdbEntry   1, 3, 6, 1, 2, 1, 17, 7, 1, 2, 2, 1

/* MIB object dot1qTpFdbAddress = dot1qTpFdbEntry, 1 */
#define I_dot1qTpFdbAddress   1
#define O_dot1qTpFdbAddress   1, 3, 6, 1, 2, 1, 17, 7, 1, 2, 2, 1, 1

/* MIB object dot1qTpFdbPort = dot1qTpFdbEntry, 13 */
#define I_dot1qTpFdbPort   2
#define O_dot1qTpFdbPort   1, 3, 6, 1, 2, 1, 17, 7, 1, 2, 2, 1, 2

/* MIB object dot1qTpFdbStatus = dot1qTpFdbEntry, 21 */
#define I_dot1qTpFdbStatus   3
#define O_dot1qTpFdbStatus   1, 3, 6, 1, 2, 1, 17, 7, 1, 2, 2, 1, 3

/* MIB object dot1qVlanStaticEntry = dot1qVlanStaticTable, 1 */
#define I_dot1qVlanStaticEntry   1
#define O_dot1qVlanStaticEntry   1, 3, 6, 1, 2, 1, 17, 7, 1, 4, 3, 1


/* MIB object dot1qVlanStaticName = dot1qVlanStaticEntry, 1 */
#define I_dot1qVlanStaticName   1
#define O_dot1qVlanStaticName   1, 3, 6, 1, 2, 1, 17, 7, 1, 4, 3, 1, 1

/* MIB object dot1qVlanStaticEgressPorts = dot1qVlanStaticEntry, 2 */
#define I_dot1qVlanStaticEgressPorts   2
#define O_dot1qVlanStaticEgressPorts   1, 3, 6, 1, 2, 1, 17, 7, 1, 4, 3, 1, 2

/* MIB object dot1qVlanForbiddenEgressPorts = dot1qVlanStaticEntry, 3 */
#define I_dot1qVlanForbiddenEgressPorts   3
#define O_dot1qVlanForbiddenEgressPorts   1, 3, 6, 1, 2, 1, 17, 7, 1, 4, 3, 1, 3

/* MIB object dot1qVlanStaticUntaggedPorts = dot1qVlanStaticEntry, 4 */
#define I_dot1qVlanStaticUntaggedPorts   4
#define O_dot1qVlanStaticUntaggedPorts   1, 3, 6, 1, 2, 1, 17, 7, 1, 4, 3, 1, 4

/* MIB object dot1qVlanStaticRowStatus = dot1qVlanStaticEntry, 5 */
#define I_dot1qVlanStaticRowStatus   5
#define O_dot1qVlanStaticRowStatus   1, 3, 6, 1, 2, 1, 17, 7, 1, 4, 3, 1, 5

/* MIB object dot1qPortVlanEntry = dot1qPortVlanTable, 1 */
#define I_dot1qPortVlanEntry   1
#define O_dot1qPortVlanEntry   1, 3, 6, 1, 2, 1, 17, 7, 1, 4, 5, 1

/* MIB object dot1qPvid = dot1qPortVlanEntry, 1 */
#define I_dot1qPvid   1
#define O_dot1qPvid   1, 3, 6, 1, 2, 1, 17, 7, 1, 4, 5, 1, 1

/* MIB object dot1qVlanCurrentEntry = dot1qVlanCurrentTable, 1 */
#define I_dot1qVlanCurrentEntry   1
#define O_dot1qVlanCurrentEntry   1, 3, 6, 1, 2, 1, 17, 7, 1, 4, 2, 1

/* MIB object dot1qVlanTimeMark = dot1qVlanCurrentEntry, 1 */
#define I_dot1qVlanTimeMark   1
#define O_dot1qVlanTimeMark   1, 3, 6, 1, 2, 1, 17, 7, 1, 4, 2, 1, 1

/* MIB object dot1qVlanIndex = dot1qVlanCurrentEntry, 2 */
#define I_dot1qVlanIndex   2
#define O_dot1qVlanIndex   1, 3, 6, 1, 2, 1, 17, 7, 1, 4, 2, 1, 2

/* MIB object dot1qVlanFdbId = dot1qVlanCurrentEntry, 3 */
#define I_dot1qVlanFdbId   3
#define O_dot1qVlanFdbId   1, 3, 6, 1, 2, 1, 17, 7, 1, 4, 2, 1, 3

/* MIB object dot1qVlanCurrentEgressPorts = dot1qVlanCurrentEntry, 4 */
#define I_dot1qVlanCurrentEgressPorts   4
#define O_dot1qVlanCurrentEgressPorts   1, 3, 6, 1, 2, 1, 17, 7, 1, 4, 2, 1, 4

/* MIB object dot1qVlanCurrentUntaggedPorts = dot1qVlanCurrentEntry, 5 */
#define I_dot1qVlanCurrentUntaggedPorts   5
#define O_dot1qVlanCurrentUntaggedPorts   1, 3, 6, 1, 2, 1, 17, 7, 1, 4, 2, 1, 5

/* MIB object dot1qVlanStatus = dot1qVlanCurrentEntry, 6 */
#define I_dot1qVlanStatus   6
#define O_dot1qVlanStatus   1, 3, 6, 1, 2, 1, 17, 7, 1, 4, 2, 1, 6

/* MIB object dot1qVlanCreationTime = dot1qVlanCurrentEntry, 7 */
#define I_dot1qVlanCreationTime   7
#define O_dot1qVlanCreationTime   1, 3, 6, 1, 2, 1, 17, 7, 1, 4, 2, 1, 7



typedef struct dot1qTpFdbEntryInfo 
{
  int index;
  char dot1qTpFdbAddress[MAC_ADDR_LEN];
  int dot1qTpFdbPort;
  int dot1qTpFdbStatus;
  struct dot1qTpFdbEntryInfo *next;
} DOT1Q_TP_FDB_ENTRY_INFO, *pDOT1Q_TP_FDB_ENTRY_INFO;

typedef struct dot1qTpFdbTableList 
{
  int count;
  pDOT1Q_TP_FDB_ENTRY_INFO head;
  pDOT1Q_TP_FDB_ENTRY_INFO tail;
} DOT1Q_TP_FDB_TABLE_LIST, *pDOT1Q_TP_FDB_TABLE_LIST;

typedef struct dot1qVlanCurrentEntryInfo 
{
  int index;
  int dot1qVlanTimeMark;
  unsigned int dot1qVlanIndex;
  int dot1qVlanFdbId;
  char *dot1qVlanCurrentEgressPorts;
  char *dot1qVlanCurrentUntaggedPorts;
  int dot1qVlanStatu;
  int dot1qVlanCreationTime;
  struct dot1qVlanCurrentEntryInfo *next;
} DOT1Q_VLAN_CURRENT_ENTRY_INFO, *pDOT1Q_VLAN_CURRENT_ENTRY_INFO;

typedef struct dot1qVlanCurrentTableList 
{
  int count;
  pDOT1Q_VLAN_CURRENT_ENTRY_INFO head;
  pDOT1Q_VLAN_CURRENT_ENTRY_INFO tail;
} DOT1Q_VLAN_CURRENT_TABLE_LIST, *pDOT1Q_VLAN_CURRENT_TABLE_LIST;


typedef struct dot1qVlanStaticEntryInfo 
{
  int vlanId;
  char *dot1qVlanStaticName;
  char *dot1qVlanStaticEgressPorts;
  char *dot1qVlanForbiddenEgressPorts;
  char *dot1qVlanStaticUntaggedPorts;
  int dot1qVlanStaticRowStatus;
  struct dot1qVlanStaticEntryInfo *next;
} DOT1Q_VLAN_STATIC_ENTRY_INFO, *pDOT1Q_VLAN_STATIC_ENTRY_INFO;

typedef struct dot1qVlanStaticTableList 
{
  int count;
  pDOT1Q_VLAN_STATIC_ENTRY_INFO head;
  pDOT1Q_VLAN_STATIC_ENTRY_INFO tail;
} DOT1Q_VLAN_STATIC_TABLE_LIST, *pDOT1Q_VLAN_STATIC_TABLE_LIST;

typedef struct dot1qPortVlanEntryInfo 
{
  int index;
  unsigned int dot1qPvid;
  struct dot1qPortVlanEntryInfo *next;
} DOT1Q_PORT_VLAN_ENTRY_INFO, *pDOT1Q_PORT_VLAN_ENTRY_INFO;

typedef struct dot1qPortVlanTableList 
{
  int count;
  pDOT1Q_PORT_VLAN_ENTRY_INFO head;
  pDOT1Q_PORT_VLAN_ENTRY_INFO tail;
} DOT1Q_PORT_VLAN_TABLE_LIST, *pDOT1Q_PORT_VLAN_TABLE_LIST;

/* required include files (IMPORTS) */

#define DOT1QTPFDBSTATUS_VALUE_OTHER              1
#define DOT1QTPFDBSTATUS_VALUE_INVALID            2
#define DOT1QTPFDBSTATUS_VALUE_LEARNED            3
#define DOT1QTPFDBSTATUS_VALUE_SELF               4
#define DOT1QTPFDBSTATUS_VALUE_MGMT               5

#define DOT1Q_VLAN_STATIC_NAME_SET                1
#define DOT1Q_VLAN_STATIC_EGRESSPORT_SET          2
#define DOT1Q_VLAN_FORBIDDEN_EGRESSPORT_SET       4
#define DOT1Q_VLAN_STATIC_UNTAGGEDPORTS_SET       8
#define DOT1Q_VLAN_STATIC_ROWSTATUS_SET           0x10

#define ROW_STATUS_ACTIVE                         1
#define ROW_STATUS_NOT_IN_SERVICE                 2
#define ROW_STATUS_NOT_READY                      3
#define ROW_STATUS_CREATE_AND_GO                  4
#define ROW_STATUS_CREATE_AND_WAIT                5
#define ROW_STATUS_DESTROY                        6

#define DOT1Q_PVID_SET                            1

#define ETH_PORTLIST_PORT_1_VALUE                 0x80
#define ETH_PORTLIST_PORT_2_VALUE                 0x40
#define ETH_PORTLIST_PORT_3_VALUE                 0x20
#define ETH_PORTLIST_PORT_4_VALUE                 0x10

void init_qBridge(void);
void register_subtrees_of_qBridge(void);

#endif /* _QBRIDGE_H_ */
