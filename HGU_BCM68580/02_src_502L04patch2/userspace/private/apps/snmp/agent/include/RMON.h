#ifndef _RMON_H_
#define _RMON_H_

/* required include files (IMPORTS) */

void rmonFree(void);
void init_rmon(void);
void register_subtrees_of_rmon(void);

/* defined objects in this module */


/* MIB object etherStatsEntry = etherStatsTable, 1 */
#define I_etherStatsEntry   1
#define O_etherStatsEntry   1, 3, 6, 1, 2, 1, 16, 1, 1, 1

/* MIB object etherStatsIndex = etherStatsEntry, 1 */
#define I_etherStatsIndex   1
#define O_etherStatsIndex   1, 3, 6, 1, 2, 1, 16, 1, 1, 1, 1

/* MIB object etherStatsCollisions = etherStatsEntry, 13 */
#define I_etherStatsCollisions   13
#define O_etherStatsCollisions   1, 3, 6, 1, 2, 1, 16, 1, 1, 1, 13

/* MIB object etherStatsCollisions = etherStatsEntry, 21 */
#define I_etherStatsStatus   21
#define O_etherStatsStatus   1, 3, 6, 1, 2, 1, 16, 1, 1, 1, 21

typedef struct etherStatsEntryInfo 
{
  int etherStatsIndex;
  int etherStatsCollisions;
  struct etherStatsEntryInfo *next;
} ETHER_STATS_ENTRY_INFO, *pETHER_STATS_ENTRY_INFO;

typedef struct etherStatsTableList 
{
  int count;
  pETHER_STATS_ENTRY_INFO head;
  pETHER_STATS_ENTRY_INFO tail;
} ETHER_STATS_TABLE_LIST, *pETHER_STATS_TABLE_LIST;


#endif /* _RMON_H_ */
