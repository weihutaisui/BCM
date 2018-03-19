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
#include "GCAP_Common.h"

#define POWER_CAPABILITY_M0     (1 << 0)
#define POWER_CAPABILITY_M1     (1 << 1)
#define POWER_CAPABILITY_M2     (1 << 2)
#define POWER_CAPABILITY_M3     (1 << 3)

static char *chipId = NULL;    // -i option

GCAP_GEN static void showUsage()
{
    printf("Usage: GCAP.115  [-h] \n\
 Report the power state capabilities of the node \n\
\n\
Options:\n\
  -h     Display this help and exit\n");
}

GCAP_GEN int GCAP_115_main(int argc, char **argv)
{
    int nRet = MOCA_API_SUCCESS;
    void *ctx;
    uint32_t pwrstate;
    uint32_t node_pwr_state;

    chipId = NULL;

    // ----------- Parse parameters
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
   ctx = moca_open(chipId);

    if (!ctx)
    {
        printf( "Error!  Unable to connect to moca instance\n");
        return(-2);
    }
    nRet = moca_get_power_state(ctx, &node_pwr_state);
    if (nRet != MOCA_API_SUCCESS)
    {
       moca_close(ctx);
       printf( "Error! Internal-2\n");
       return(-3);
    }

    nRet = moca_get_power_state_capabilities(ctx, &pwrstate);
    if (nRet != MOCA_API_SUCCESS)
    {
       moca_close(ctx);
       printf( "Error! Internal-3\n");
       return(-4);
    }

    if (node_pwr_state == 0 )
    {
       printf("Node can transition to power state ");  
       if ( pwrstate & POWER_CAPABILITY_M1 )
       {
          printf("M1 ");
       }      
       if ( pwrstate & POWER_CAPABILITY_M2 )
       {
          printf("M2 ");
       }     
       if ( pwrstate & POWER_CAPABILITY_M3 )
       {
          printf("M3 ");
       }
       printf("\n");
    }
    else
    {
       printf("Node can transition to power state ");
       if ( pwrstate & POWER_CAPABILITY_M0 )
       {
          printf("M0 \n");
       }
    }

    moca_close(ctx);
    return(0);
}
