#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>

#include <mocalib.h>
#include "GCAP_Common.h"

static char *chipId = NULL;    // -i option

GCAP_GEN static void showUsage()
{
    printf("Usage: GCAP.26 [-h]\n\
   Report the current network (beacon) MOCA_VERSION.\n\
\n\
Options:\n\
  -h   Display this help and exit\n");
}




GCAP_GEN int GCAP_26_main(int argc, char **argv)
{
    int ret = MOCA_API_SUCCESS;
    struct moca_network_status nws;
    struct moca_interface_status status;
    void *ctx;

    chipId = NULL;

    // ----------- Parse parameters
#if defined(STANDALONE)
   int i;

   for (i=1; i < argc; i++)
   {
       if (strcmp(argv[i], "-i") == 0)
       {
          chipId = argv[i+1];
          i++;
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
    ret = moca_get_interface_status(ctx, &status);

    if (ret != MOCA_API_SUCCESS)
    {
        printf( "Error!  Internal failure\n");
        moca_close(ctx);
        return(-3);
    }

    if (status.link_status != MOCA_LINK_UP)
    {
        printf( "Error! No Link\n");
        moca_close(ctx);
        return(-4);
    }  
 

    // ----------- Output Data   
     ret = moca_get_network_status(ctx, &nws);
    
    if (ret != MOCA_API_SUCCESS)
    {
        printf( "Error!  Internal failure\n");
        moca_close(ctx);
        return(-5);
    }

    printf ("Network (Beacon) MOCA_VERSION      : 0x%x \n", nws.network_moca_version)  ;
    
    // ----------- Finish

    moca_close(ctx);

    return(0);
}

