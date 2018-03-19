#ifndef _PBRIDGE_H_
#define _PBRIDGE_H_

#include "cms.h"

/* defined objects in this module */

/* MIB object dot1dPortPriorityEntry = dot1dPortPriorityTable, 1 */
#define I_dot1dPortPriorityEntry   1
#define O_dot1dPortPriorityEntry   1, 3, 6, 1, 2, 1, 17, 6, 1, 2, 1, 1

/* MIB object dot1dPortDefaultUserPriority = dot1dPortPriorityEntry, 1 */
#define I_dot1dPortDefaultUserPriority   1
#define O_dot1dPortDefaultUserPriority   1, 3, 6, 1, 2, 1, 17, 6, 1, 2, 1, 1, 1

/* MIB object dot1dStp = dot1dBridge, 2 */
#define I_dot1dStp   2
#define O_dot1dStp   1, 3, 6, 1, 2, 1, 17, 2

/* MIB object dot1dStpPriority = dot1dStp, 2 */
#define I_dot1dStpPriority   2
#define O_dot1dStpPriority   1, 3, 6, 1, 2, 1, 17, 2, 2

typedef struct dot1dPortPriorityEntryInfo 
{
  int index;
  int dot1dPortDefaultUserPriority;
  struct dot1dPortPriorityEntryInfo *next;
} DOT1D_PORT_PRIORITY_ENTRY_INFO, *pDOT1D_PORT_PRIORITY_ENTRY_INFO;

typedef struct dot1dPortPriorityTableList 
{
  int count;
  pDOT1D_PORT_PRIORITY_ENTRY_INFO head;
  pDOT1D_PORT_PRIORITY_ENTRY_INFO tail;
} DOT1D_PORT_PRIORITY_TABLE_LIST, *pDOT1D_PORT_PRIORITY_TABLE_LIST;

/* required include files (IMPORTS) */

void init_pBridge(void);
void register_subtrees_of_pBridge(void);

#endif /* _QBRIDGE_H_ */
