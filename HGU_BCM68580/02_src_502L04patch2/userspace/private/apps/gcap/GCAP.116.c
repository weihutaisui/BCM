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
static char *newstate = NULL;
static char *forcestate = NULL;


GCAP_GEN static void showUsage()
{
    printf("Usage: GCAP.116  [-s state] [-f] [-h]\n\
 Request the node to transition to a different power state\n\
\n\
Options:\n\
 -s   0 = M0 (ACTIVE), 1 = M1 (IDLE), 2 = M2 (STANDBY),\n\
        3 =M3 (SLEEP)\n\
 -f   force to wake up node \n\
 -h   Display this help and exit\n");
}

GCAP_GEN int GCAP_116_main(int argc, char **argv)
{
    int ret = 0;
    uint32_t staterq = 0;
    uint32_t rsp_code = 0;
    uint32_t forcewakeup =0;
    void *ctx;
    int i = 0;

    chipId = NULL;
    newstate = NULL;
    forcestate = NULL;

    // ----------- Parse parameters
    if (argc < 2)
    {
        printf( "Error!  Missing parameter - State\n");
        return(-2);
    }

#if defined(STANDALONE)
   for (i=1; i < argc; i++)
   {
        if (strcmp(argv[i], "-i") == 0)
        {
            i++;
            chipId = argv[i];
        }
        else if (strcmp(argv[i], "-s") == 0)
        {
           i++;
           newstate = argv[i];
        }
        else if (strcmp(argv[i], "-f") == 0)
        {
           i++;
           forcestate = argv[i];
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
    
    while((ret = getopt(argc, argv, "his:f:")) != -1) 
    {
        switch(ret)
        {
        case 'i':
            chipId = optarg;
            break;
        case 's':
            newstate = optarg;
            break;
        case 'f':
            forcestate = optarg;
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
    if ( forcestate != NULL )
    {
       forcewakeup =  atoi(forcestate); 
       if ( (forcewakeup != 0) && (forcewakeup != 1) )
       {
           printf("Error!  Illegal value %d for -f option. Must be 0, 1\n", forcewakeup);
           return(-2);
       }
     
    }

    if (newstate != NULL )
    {
        staterq = atoi(newstate);
        if (staterq > 3)
        {
           printf("Error!  Illegal value %d for -s option. Must be 0, 1, 2, or 3\n", staterq);
           return(-3);
       }
   }
   else 
   {
         printf( "Error!  Missing parameter <-s state>\n");
         return(-4);
    }
    // ----------- Initialize

    ctx=moca_open(chipId);

    if (!ctx)
    {
        printf( "Error!  Unable to connect to moca instance\n");
        return(-5);
    }

    if ( forcestate != NULL )
    {
       ret = moca_set_filter_m2_data_wakeUp(ctx, forcewakeup);
       if (ret != MOCA_API_SUCCESS)
       {
         moca_close(ctx);
         printf( "Error!  Internal\n");
         return(-6);
       }
    } 

    struct moca_wom_pattern_set wom_pattern;
    if (staterq == POWER_M2)
    { 
       memset(&wom_pattern, 0, sizeof(wom_pattern));

       for (i = 0; i < 16; i++)
       {
          if (i < 6)
          {
            wom_pattern.bytes[i] = 0xFF;
            wom_pattern.mask[i] = 0x00;
          }
          else
          {
            wom_pattern.mask[i] = MOCA_WOM_PATTERN_MASK_DEF;
          }
       }

       wom_pattern.index = 0;
       
       ret = moca_set_wom_pattern(ctx, &wom_pattern);
       ret = moca_set_wom_mode(ctx, 1);
    }
    else if ((staterq == POWER_M1) || (staterq == POWER_M0))
    {
       ret = moca_set_wom_mode(ctx, 0);
    }
        
    moca_do_ps_cmd(ctx, staterq, &rsp_code);
    if (rsp_code == 0)
    {
        printf("Success: Power state changed to %d response is ACK\n", staterq);
    }
    else
    {
        printf("Power state response is NACK \n");
    }

    moca_close(ctx);

    return(ret);
}

