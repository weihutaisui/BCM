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

GCAP_GEN static void showUsage()
{
    printf("Usage: GCAP.13 [-h]\n\
Report Node ID, MAC address and total number of nodes in the network.\n\
\n\
Options:\n\
  -h   Display this help and exit\n");
}

GCAP_GEN static int printNodeInfo(void *ctx, int node)
{
   struct moca_gen_node_status gns;
   int ret;
   
   ret = moca_get_gen_node_status(ctx, node, &gns);    

   if (ret != 0)
   {
      printf( "Error!  mocalib failure\n");
      return(-6);
   }

    printf("Node ID: %2d Node MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n", node,
        gns.eui.addr[0], gns.eui.addr[1], gns.eui.addr[2], gns.eui.addr[3], gns.eui.addr[4], gns.eui.addr[5] );

    return(0);
}

GCAP_GEN int GCAP_13_main(int argc, char **argv)
{
    int ret;
    void *ctx;
    
    int altnode;
    int altnodemask;
    int ncnode;
    int bncnode;

    struct moca_interface_status if_status;
    struct moca_network_status   net_status;

    chipId = NULL;

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
        printf( "Error!  mocalib failure\n");
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
        printf( "Error!  mocalib failure\n");
        moca_close(ctx);
        return(-4);
    }

    ncnode = net_status.nc_node_id;
    bncnode = net_status.backup_nc_id;

    printf("Management GN Node ID= %d\n", net_status.node_id);
    printf("       NC ");    
    ret = printNodeInfo(ctx, ncnode);
    if (ret != 0)
    {
        moca_close(ctx);
        return(ret);
    }
    printf("Backup NC ");
    ret = printNodeInfo(ctx, bncnode);
    if (ret != 0)
    {
        moca_close(ctx);
        return(ret);
    }

    altnode = 0;
    altnodemask = net_status.connected_nodes;

    while (altnode < 16)
    {
        if (((altnodemask &(1<<altnode)) != 0) &&
            (altnode != ncnode) && (altnode != bncnode))
        {
           printf("          "); 
           ret = printNodeInfo(ctx, altnode);
           if (ret != 0)
               break;
        }

        altnode++;
    }

    printf("\nTotal number of nodes: %d\n", moca_count_bits(net_status.connected_nodes));
        
    // ----------- Finish

    moca_close(ctx);

    return(0);
}

