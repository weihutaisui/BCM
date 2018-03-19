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

static char *on_off = NULL; 
static char *chipId = NULL;    // -i option


GCAP_GEN static void showUsage()
{
    printf("Usage: GCAP.34 <ON/OFF>  [-h] \n\
Set the maximum aggregation level on this node. \n\
\n\
Options:\n\
<ON/OFF>     Turn ON or OFF aggregation limit.\n\
  -h     Display this help and exit\n");
}

GCAP_GEN int GCAP_34_main(int argc, char **argv)
{
    int nRet = MOCA_API_SUCCESS;
    void *ctx;
    uint32_t  minAggrWaitTime;

    chipId = NULL;
    on_off = NULL;

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
     if (argc < 2)
     {
         printf( "Error!  Missing parameter <ON/OFF>\n");
         return(-2);
     }
     else
     {
         on_off = argv[1];
     }
    // ----------- Initialize

   ctx = moca_open(chipId);

    if (!ctx)
    {
        printf( "Error!  Unable to connect to moca instance\n");
        return(-3);
    }

   if( !strcmp( on_off, "ON" ) )
   {
         /* When the GCAP.34 is ON the MIN_AGGR_WAITING_TIME = MAX_AGGR_PACKETS*1/(RATE/8/PACKET_SIZE).
          * At RATE=1 Mbps, thePACKET_SIZE=64 Bytes (GCAP.34 definition)
          *    MIN_AGGR_WAITING_TIME = MAX_AGGR_PACKETS * 512 usec
          * At RATE=64 Mbps, the PACKET_SIZE=800 bytes (GCAP.34 definition)
          *    MIN_AGGR_WAITING_TIME = MAX_AGGR_PACKETS * 100 usec 
          * At RATE = 76Kpps, the PACKET_SIZE=576 bytes, agg level 20 (GCAP.34 2.0 definition) 
          *   MIN_AGGR_WAITING_TIME = MAX_AGGR_PACKETS * 60.63 = 1213 (round it up)
          */
          minAggrWaitTime = 1213;  
    }
    else if( !strcmp( on_off, "OFF" ) )
    {
       minAggrWaitTime = 0;
    }
    else
    {
      printf( "Error! Invalid parameter %s\n", on_off);
      moca_close(ctx);
      return(-4);
    }
    nRet = moca_set_min_aggr_waiting_time(ctx, minAggrWaitTime);
    if (nRet != MOCA_API_SUCCESS)
    {
        moca_close(ctx);
        printf( "Error!  Internal\n");
        return(-5);
    }
    moca_close(ctx);
    return(0);
}
