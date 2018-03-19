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
    printf("Usage: GCAP.04 [-h]\n\
Report NC node ID and MAC address.\n\
\n\
Options:\n\
  -h   Display this help and exit\n");
}

GCAP_GEN static int printNodeInfo(void *ctx, int node)
{
    struct moca_gen_node_status nodestatus;
    int ret;
    
    ret = moca_get_gen_node_status(ctx, node, &nodestatus);

    if (ret != MOCA_API_SUCCESS)
    {
        printf( "Error!  Internal failure 2\n");
        return(-6);
    }

    printf("Node ID: %2d Node MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n", node,
        MOCA_DISPLAY_MAC(nodestatus.eui));

    return(0);
}

GCAP_GEN int GCAP_04_main(int argc, char **argv)
{
    int ret;
    
    struct moca_interface_status ifstatus;
    struct moca_network_status nws;
    void *ctx;

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

    // ----------- Get info

    ret = moca_get_interface_status(ctx, &ifstatus);

    if (ret != MOCA_API_SUCCESS)
    {
        printf( "Error!  Interface failure\n");
        moca_close(ctx);
        return(-3);
    }

    if (ifstatus.link_status != MOCA_LINK_UP)
    {
        printf( "Error! No Link\n");
        moca_close(ctx);
        return(-4);
    }
        
    ret = moca_get_network_status(ctx, &nws);    
    
    if (ret == 0)
    {
        printf("NC ");    
        ret = printNodeInfo(ctx, (int)nws.nc_node_id);    
    }
    // ----------- Finish

    moca_close(ctx);

    return(ret);
}


