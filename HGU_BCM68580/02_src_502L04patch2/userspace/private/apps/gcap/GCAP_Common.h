#define POWER_M0     0
#define POWER_M1     1
#define POWER_M2     2
#define POWER_M3     3

int moca_parse_mac_addr ( char * macAddrString, macaddr_t * pMacAddr );

int moca_mac_to_node_id (void * ctx, macaddr_t * mac, uint32_t * node_id);

void moca_gcap_init();

void printPowerInfo(uint32_t i, uint32_t tx_power, uint32_t pwrstate);
