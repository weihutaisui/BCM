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
    printf("Usage: GCAP.14 [-h]\n\
Report frequency offset between Golden Node & DUT.\n\
\n\
Options:\n\
  -h   Display this help and exit\n");
}



GCAP_GEN int GCAP_14_main(int argc, char **argv)
{
    int ret =0;
    void *ctx;

    struct moca_interface_status if_status;
    struct moca_network_status   net_status;
    struct moca_gen_node_status gns;
    int altnode;
    int altnodemask;

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

   ctx=moca_open(chipId);

    if (!ctx)
    {
        printf( "Error!  Unable to connect to moca instance\n");
        return(-2);
    }

    // ----------- Obtain status 

    ret = moca_get_interface_status(ctx, &if_status);

    if (ret != 0)
    {
        printf( "Error!  internal error 1\n");
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
    
    altnode = 0;
    altnodemask = net_status.connected_nodes;

    while (altnode < 16)
    {
        if (((altnodemask &(1<<altnode)) != 0) && (altnode !=net_status.node_id))
            break;
        
        altnode++;
    }

    if (altnode > 15)
    {
        printf("Error!  Could not find peer node\n");
        moca_close(ctx);
        return(-5);
    }

    memset(&gns, 0, sizeof(gns));
    ret = moca_get_gen_node_status(ctx, altnode, &gns); 
 
    if (ret != MOCA_API_SUCCESS)
    {
        printf( "Error!  Internal failure\n");
        moca_close(ctx);
        return(-6);
    }
    
    // ----------- Output Data   

    printf("Frequency Offset of node %d: %dHz\n", altnode, gns.freq_offset);
    
    // ----------- Finish

    moca_close(ctx);

    return(0);
}

