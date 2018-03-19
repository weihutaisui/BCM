#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>

#include <mocalib.h>
#include <mocalib-cli.h>

static char *chipId = NULL;    // -i option

GCAP_GEN static void showUsage()
{
    printf("Usage: GCAP.39 [nodeId] [-h]\n\
Report PHY bit rates between the GN & other nodes.\n\
\n\
Options:\n\
  [nodeId]  Node Id\n\
  -h   Display this help and exit\n");
}

struct profile_index_desc
{
   uint32_t index;
   uint32_t num_carriers;
   char *   desc;
};

struct profile_index_desc profiles[] =
{
   { MOCA_EXT_STATUS_PROFILE_TX_UCAST,     MOCA_MAX_SUB_CARRIERS_1_1,   "TX Unicast 50 MHz"},
   { MOCA_EXT_STATUS_PROFILE_TX_BCAST,     MOCA_MAX_SUB_CARRIERS_1_1,   "TX Broadcast 50 MHz"},
   { MOCA_EXT_STATUS_PROFILE_TX_UC_VLPER,  MOCA_MAX_SUB_CARRIERS,       "TX Unicast VLPER 100 MHz"},
   { MOCA_EXT_STATUS_PROFILE_TX_UC_NPER,   MOCA_MAX_SUB_CARRIERS,       "TX Unicast NPER 100 MHz"},
   { MOCA_EXT_STATUS_PROFILE_TX_BC_VLPER,  MOCA_MAX_SUB_CARRIERS,       "TX Broadcast VLPER 100 MHz"},
   { MOCA_EXT_STATUS_PROFILE_TX_BC_NPER,   MOCA_MAX_SUB_CARRIERS,       "TX Broadcast NPER 100 MHz"},
   { MOCA_EXT_STATUS_PROFILE_RX_UCAST,     MOCA_MAX_SUB_CARRIERS_1_1,   "RX Unicast 50 MHz"},
   { MOCA_EXT_STATUS_PROFILE_RX_BCAST,     MOCA_MAX_SUB_CARRIERS_1_1,   "RX Broadcast 50 MHz"},
   { MOCA_EXT_STATUS_PROFILE_RX_UC_VLPER,  MOCA_MAX_SUB_CARRIERS,       "RX Unicast VLPER 100 MHz"},
   { MOCA_EXT_STATUS_PROFILE_RX_UC_NPER,   MOCA_MAX_SUB_CARRIERS,       "RX Unicast NPER 100 MHz"},
   { MOCA_EXT_STATUS_PROFILE_RX_BC_VLPER,  MOCA_MAX_SUB_CARRIERS,       "RX Broadcast VLPER 100 MHz"},
   { MOCA_EXT_STATUS_PROFILE_RX_BC_NPER,   MOCA_MAX_SUB_CARRIERS,       "RX Broadcast NPER 100 MHz"},
   { MOCA_EXT_STATUS_PROFILE_RX_UC_NPER_SEC_CH,   MOCA_MAX_SUB_CARRIERS,       "RX Unicast NPER Sec-Ch 100 MHz"},
   { MOCA_EXT_STATUS_PROFILE_RX_UC_VLPER_SEC_CH,  MOCA_MAX_SUB_CARRIERS,       "RX Unicast VLPER Sec-Ch 100 MHz"},
   { MOCA_EXT_STATUS_PROFILE_TX_UC_NPER_SEC_CH,   MOCA_MAX_SUB_CARRIERS,       "TX Unicast NPER Sec-Ch 100 MHz"},
   { MOCA_EXT_STATUS_PROFILE_TX_UC_VLPER_SEC_CH,  MOCA_MAX_SUB_CARRIERS,       "TX Unicast VLPER Sec-Ch 100 MHz"},
};
#define NUM_PROFILES sizeof(profiles)/sizeof(struct profile_index_desc)

GCAP_GEN int GCAP_39_main(int argc, char **argv)
{
    int ret = MOCA_API_SUCCESS;
    void *ctx;
    struct moca_interface_status if_status;
    struct moca_network_status nw_status;
    struct moca_gen_node_ext_status_in node_status_in; 
    struct moca_gen_node_ext_status node_status_out;
    struct moca_gen_node_status nodestatus;
    int start_node = 0;
    int end_node = MOCA_MAX_NODES - 1;
    int i;

    chipId = NULL;

    // ----------- Parse parameters

    if (argc == 2)
    {
        start_node = end_node = atoi(argv[1]);
        if (start_node >= MOCA_MAX_NODES)
        {
           printf( "Error!  Invalid Node ID\n");
           return(-1);
        }
    }
    
#if defined(STANDALONE) 
   for (i=1; i < argc; i++)
   {
      if (strcmp(argv[i], "-i") == 0)
      {
         i++;
         chipId = argv[i];
      }
      else if (strcmp(argv[i], "?") == 0)
      {
         printf( "Error!  Invalid option - %s\n", argv[i]);
         return(-1);
      }
      else if (strcmp(argv[i], "-h") == 0)
      {
         showUsage();
         return(0); 
      }
   }
#else 
    opterr = 0;

    while((ret = getopt(argc, argv, "hi:")) != -1) 
    {
        switch(ret)
        {
        case 'i':
            chipId = optarg;
            break;
        case '?':
            printf( "Error!  Invalid option - %c\n", optopt);
            return(-1);
            break;
        case 'h':
        default:
            showUsage();
            return(0); 
        }
    }
#endif
    // ----------- Initialize

    ctx = moca_open(chipId);

    if (!ctx)
    {
        printf( "Error!  Unable to connect to moca instance\n");
        return(-2);
    }

    // ----------- Get info

    ret = moca_get_interface_status(ctx, &if_status);

    if (ret != MOCA_API_SUCCESS)
    {
        printf( "Error!  Internal failure\n");
        moca_close(ctx);
        return(-3);
    }

    if (if_status.link_status != MOCA_LINK_UP)
    {
        printf( "Error! No Link\n");
        moca_close(ctx);
        return(-4);
    }  

    ret = moca_get_network_status(ctx, &nw_status);
    if (ret != MOCA_API_SUCCESS)
    {
        printf( "Error!  Failure getting network status\n");
        moca_close(ctx);
        return(-4);
    }

    printf("Management GN Node ID= %d\n", nw_status.node_id);

    for (; start_node <= end_node; start_node++)
    {
        if ((nw_status.connected_nodes & (1 << start_node)) != 0)
        {        
            ret = moca_get_gen_node_status(ctx, start_node, &nodestatus);
            for (i = 0; i < NUM_PROFILES; i++)
            {
               node_status_in.index = start_node;
               node_status_in.profile_type = profiles[i].index;
               ret = moca_get_gen_node_ext_status(ctx, &node_status_in, &node_status_out);
               if ((ret == MOCA_API_SUCCESS) && (node_status_out.nbas != 0))
               {
                  printf("%s\n", profiles[i].desc);
                  printf("NodeID         : %3d\n", start_node);
                  printf("MAC Address    : %02X:%02X:%02X:%02X:%02X:%02X\n", MOCA_DISPLAY_MAC(nodestatus.eui));
                  printf("PHY Rate       :  %d Mbps\n", node_status_out.phy_rate);
                  printf("NBAS           :  %d \n", node_status_out.nbas);
                  printf("CP             :  %d \n", node_status_out.cp);
                  printf("TxPowerAdjust  :  %d dB\n", node_status_out.tx_backoff); 
                  printf("Subcarriers\n");
                  mocacli_print_bit_loading(&node_status_out.bit_loading[0], NULL, profiles[i].num_carriers);
                  printf("\n");
               }
            }
            printf("----------------------------------------------\n");    
        }
    }

    // ----------- Finish

    moca_close(ctx);

    return(0);
}
