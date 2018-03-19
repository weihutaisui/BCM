#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>

#include <mocalib.h>


static char *chipId = NULL;    // -i option

static uint32_t nodeId;

GCAP_GEN static void showUsage()
{
    printf("Usage: GCAP.06 [-h]\n\
Report PHY bit rates between the GN & other nodes.\n\
\n\
Options:\n\
  -h   Display this help and exit\n");
}

GCAP_GEN static int getPhyRate(void * ctx, uint32_t node, uint32_t profile, uint32_t * phy_rate)
{
   struct moca_gen_node_ext_status gns_ext;
   struct moca_gen_node_ext_status_in gns_ext_in;
   int ret;

   gns_ext_in.index = node;
   gns_ext_in.profile_type = profile;
   ret = moca_get_gen_node_ext_status(ctx, &gns_ext_in, &gns_ext);
   if (ret != 0)
   {
      printf( "Error!  mocalib phyrate failure\n");
      return(-6);
   }

   *phy_rate = gns_ext.phy_rate;

   return(0);
}

GCAP_GEN static int printNodeRateInfo(void *ctx, int node, int nc_moca_version)
{
   struct moca_gen_node_status gns;
   int ret;
   uint32_t rxUcRate          = 0;
   uint32_t txUcRate          = 0;
   uint32_t rxBcRate          = 0;
   uint32_t rxUcNperRate      = 0;
   uint32_t rxUcVlperRate     = 0;
   uint32_t txUcNperRate      = 0;
   uint32_t txUcVlperRate     = 0;
   uint32_t rxBcNperRate      = 0;
   uint32_t rxBcVlperRate     = 0;
   uint32_t gcdTxRate         = 0;
   uint32_t gcdTxNperRate     = 0;
   uint32_t gcdTxVlperRate    = 0;
   uint32_t rxSecUcNperRate   = 0;
   uint32_t rxSecUcVlperRate  = 0;
   uint32_t txSecUcNperRate   = 0;
   uint32_t txSecUcVlperRate  = 0;

   chipId = NULL;
   
   ret = moca_get_gen_node_status(ctx, node, &gns);    
   if (ret != 0)
   {
      printf( "Error!  mocalib failure\n");
      return(-6);
   }
   
   if ( nodeId != node )
   {
       if ( ( gns.active_moca_version < MoCA_VERSION_2_0 ) || 
            ( nc_moca_version < MoCA_VERSION_2_0 ) )
       {
           ret = getPhyRate(ctx, node, MOCA_EXT_STATUS_PROFILE_RX_UCAST,    &rxUcRate);
           if (ret != 0) return (ret);
           ret = getPhyRate(ctx, node, MOCA_EXT_STATUS_PROFILE_TX_UCAST,    &txUcRate);
           if (ret != 0) return (ret);
           ret = getPhyRate(ctx, node, MOCA_EXT_STATUS_PROFILE_RX_BCAST,    &rxBcRate);
           if (ret != 0) return (ret);
           ret = getPhyRate(ctx, nodeId, MOCA_EXT_STATUS_PROFILE_TX_BCAST,  &gcdTxRate);
           if (ret != 0) return (ret);
       } 
       else
       {
           ret = getPhyRate(ctx, node, MOCA_EXT_STATUS_PROFILE_RX_UC_NPER,  &rxUcNperRate);
           if (ret != 0) return (ret);
           ret = getPhyRate(ctx, node, MOCA_EXT_STATUS_PROFILE_RX_UC_VLPER, &rxUcVlperRate);
           if (ret != 0) return (ret);
           ret = getPhyRate(ctx, node, MOCA_EXT_STATUS_PROFILE_TX_UC_NPER,  &txUcNperRate);
           if (ret != 0) return (ret);
           ret = getPhyRate(ctx, node, MOCA_EXT_STATUS_PROFILE_TX_UC_VLPER, &txUcVlperRate);
           if (ret != 0) return (ret);
           ret = getPhyRate(ctx, node, MOCA_EXT_STATUS_PROFILE_RX_BC_NPER,  &rxBcNperRate);
           if (ret != 0) return (ret);
           ret = getPhyRate(ctx, node, MOCA_EXT_STATUS_PROFILE_RX_BC_VLPER, &rxBcVlperRate);
           if (ret != 0) return (ret);
           ret = getPhyRate(ctx, nodeId, MOCA_EXT_STATUS_PROFILE_TX_BC_NPER,  &gcdTxNperRate ); 
           if (ret != 0) return (ret);
           ret = getPhyRate(ctx, nodeId, MOCA_EXT_STATUS_PROFILE_TX_BC_VLPER, &gcdTxVlperRate );
           if (ret != 0) return (ret);
           ret = getPhyRate(ctx, node, MOCA_EXT_STATUS_PROFILE_RX_UC_NPER_SEC_CH,  &rxSecUcNperRate);
           if (ret != 0) return (ret);
           ret = getPhyRate(ctx, node, MOCA_EXT_STATUS_PROFILE_RX_UC_VLPER_SEC_CH, &rxSecUcVlperRate);
           if (ret != 0) return (ret);
           ret = getPhyRate(ctx, node, MOCA_EXT_STATUS_PROFILE_TX_UC_NPER_SEC_CH,  &txSecUcNperRate);
           if (ret != 0) return (ret);
           ret = getPhyRate(ctx, node, MOCA_EXT_STATUS_PROFILE_TX_UC_VLPER_SEC_CH, &txSecUcVlperRate);
           if (ret != 0) return (ret);
       }
   }
   printf("%3d      %02X:%02X:%02X:%02X:%02X:%02X  %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d\n", node,
            gns.eui.addr[0], gns.eui.addr[1], gns.eui.addr[2], gns.eui.addr[3], gns.eui.addr[4], gns.eui.addr[5],
            txUcRate, rxUcRate, gcdTxRate, rxBcRate, 
            txUcNperRate, txUcVlperRate, rxUcNperRate, rxUcVlperRate, 
            txSecUcNperRate, txSecUcVlperRate, rxSecUcNperRate, rxSecUcVlperRate, 
            gcdTxNperRate, gcdTxVlperRate, rxBcNperRate, rxBcVlperRate );
   return(ret);
}

GCAP_GEN int GCAP_06_main(int argc, char **argv)
{
    int ret;
    void *ctx;
    int altnode;
    int altnodemask;
    struct moca_interface_status if_status;
    struct moca_network_status   net_status;
    struct moca_gen_node_status  gen_node_status;
    int nc_moca_version;

#if defined(STANDALONE)
        int i;
    
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

    // ----------- Parse parameters
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

    if (ret != 0)
    {
        printf( "Error!  mocalib interface failure\n");
        moca_close(ctx);
        return(-3);
    }

    if (if_status.link_status != 1)
    {
        printf( "Error! No Link\n");
        moca_close(ctx);
        return(-4);
    }

    ret = moca_get_network_status(ctx, &net_status);
    if (ret != 0)
    {
        printf( "Error!  mocalib net failure\n");
        moca_close(ctx);
        return(-4);
    }

    nodeId = net_status.node_id;

    ret = moca_get_gen_node_status(ctx, net_status.nc_node_id, &gen_node_status);
    if (ret != 0)
    {
        printf( "Error!  mocalib node status failure\n");
        moca_close(ctx);
        return(-5);
    }

    nc_moca_version = gen_node_status.active_moca_version;
    
    altnode = 0;
    altnodemask = net_status.connected_nodes;

    printf("Management GN Node ID= %d\n", nodeId);
    printf("PHY BitRate Information (in Mbps):\n");
    printf("-----------------------------------------------------------------------------------------------------------\n");
    printf("                                                                     2nd  2nd  2nd  2nd                    \n");
    printf("                                                NPER VPER NPER VPER NPER VPER NPER VPER NPER VPER NPER VPER\n");
    printf("                              Tx   Rx   Tx   Rx   Tx   Tx   Rx   Rx   Tx   Tx   Rx   Rx   Tx   Tx   Rx   Rx\n");
    printf("                              UC   UC  GCD  GCD   UC   UC   UC   UC   UC   UC   UC   UC  GCD  GCD  GCD  GCD\n");
    printf("Node ID  MAC Addr            50M  50M  50M  50M 100M 100M 100M 100M 100M 100M 100M 100M 100M 100M 100M 100M\n");
    printf("-----------------------------------------------------------------------------------------------------------\n");

    while (altnode < 16)
    {
        if ((altnodemask &(1<<altnode)) != 0)
        {
           ret = printNodeRateInfo(ctx, altnode, nc_moca_version);

           if (ret != 0)
              break;
        }

        altnode++;
    }

    // ----------- Finish

    moca_close(ctx);

    return(ret);
}
