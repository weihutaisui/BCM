#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>

#include <mocalib.h>
#include <mocaint.h>

static char *chipId = NULL;    // -i option

GCAP_GEN static void showUsage()
{
    printf("Usage: GCAP.05 [-h]\n\
Report backup NC node ID and MAC address.\n\
\n\
Options:\n\
  -h   Display this help and exit\n");
}

GCAP_GEN static int printNodeInfo(void *ctx, uint32_t node)
{
     int nRet = 0;
     struct moca_gen_node_status gsn;

    nRet = moca_get_gen_node_status(ctx, node,  &gsn);
    printf ("Node ID: %2d Node MAC Address:  %02x:%02x:%02x:%02x:%02x:%02x\n",
              (int) node, MOCA_DISPLAY_MAC(gsn.eui));

    if (nRet != MOCA_API_SUCCESS)
    {
        printf( "Error! Internal_3\n");
        return(-6);
    }

    return(0);
}

GCAP_GEN int GCAP_05_main(int argc, char **argv)
{
    int nRet = 0;
    
    struct moca_interface_status is;
    struct moca_network_status nws;
    uint32_t bncnode;
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
    
    while((nRet = getopt(argc, argv, "hi:")) != -1) 
    {
        switch(nRet)
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
    nRet = moca_get_interface_status(ctx, &is);
    if (nRet != MOCA_API_SUCCESS) 
    {
        printf( "Error!  Internal_1\n");
        moca_close(ctx);
        return(-3);
    }

    nRet = moca_get_network_status(ctx, &nws);
    if (nRet != MOCA_API_SUCCESS) 
    {
        printf( "Error!  Internal_2\n");
        moca_close(ctx);
        return(-3);
    }
 
    if (is.link_status != 1)
    {
        printf( "Error! No Link\n");
        moca_close(ctx);
        return(-4);
    }
    bncnode =   nws.backup_nc_id;  
    if (bncnode >= MOCA_MAX_NODES)
    {
         printf ("Backup NC                : N/A \n") ;
    }
    else
    {
        printf("Backup NC ");    
        nRet = printNodeInfo(ctx, bncnode);    
    }

    // ----------- Finish

    moca_close(ctx);
    return(nRet);
}



