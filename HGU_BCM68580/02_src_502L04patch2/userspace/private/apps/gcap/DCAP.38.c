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

static char *chipId = NULL;    // -i option
static int persistent = 0;     // -M option
static int reset = 0;          // -r option
static char *onoff = NULL;

GCAP_GEN static void showUsage()
{
    printf("Usage: DCAP.38 <[ON]/OFF> [-M] [-r] [-h]\n\
Set DUT to operate in CW mode \n\
Note that resetting SoC is required for configuration to take affect.\n\
\n\
Options:\n\
 ON/OFF Turn ON or OFF the continuous power TX mode (default OFF)\n\
  -M    Make configuration changes permanent\n\
  -r    Reset SoC to make configuration changes effective\n\
  -h    Display this help and exit\n");
}

GCAP_GEN int DCAP_38_main(int argc, char **argv)
{
    int ret;
    void *ctx;
    uint32_t const_tx_mode;
    struct moca_const_tx_params const_tx_params;
    uint32_t bandwidth;
    uint32_t lof;
    int32_t pco;

    chipId = NULL;
    onoff = NULL;
    persistent=0;
    reset=0;
 
    // ----------- Parse parameters

   if ((argc < 2) || (argv[1] == NULL) || (argv[1][0] == '-'))
    {
        onoff = "ON";
    }
    else
    {
        onoff = argv[1];
    }
    if ((strcmp(onoff, "ON") != 0) && (strcmp(onoff, "OFF") != 0))
    {
        printf("Error!  Invalid parameter - %s\n",onoff);
        return(-3);
    }
#if defined(STANDALONE)
   int i;

   for (i=1; i < argc; i++)
   {
       if (strcmp(argv[i], "-i") == 0)
       {
          i++;
          chipId = argv[i];
       }
       else if (strcmp(argv[i], "-M") == 0)
       {
          persistent = 1; 
       }
       else if (strcmp(argv[i], "-r") == 0)
       {
          reset = 1; 
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

    while((ret = getopt(argc, argv, "Mrhi:")) != -1) 
    {
        switch(ret) 
        {
        case 'i':
            chipId = optarg;
            break;
        case 'M':
            persistent = 1;
            break;
        case 'r':
            reset = 1;
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
    ret = moca_get_const_tx_params(ctx, &const_tx_params);
    ret |= moca_get_lof(ctx, &lof);
    ret |= moca_get_primary_ch_offset(ctx, &pco);

    if (strcmp(onoff, "ON") == 0)
    {
       if ((pco == 0) && ((lof == 1150) || (lof == 500) || (lof == 675))) // bottom frequencies of each band
          bandwidth = MOCA_BANDWIDTH_50MHZ;
       else
          bandwidth = MOCA_BANDWIDTH_100MHZ;
       const_tx_mode = 1;
       const_tx_params.const_tx_submode = 2;
    }
    else
    {
       bandwidth = MOCA_BANDWIDTH_50MHZ;
       const_tx_mode = 0;
       const_tx_params.const_tx_submode = 0;
    }

    ret |= moca_set_continuous_power_tx_mode(ctx, const_tx_mode);
    ret |= moca_set_const_tx_params(ctx, &const_tx_params);  
    ret |= moca_set_secondary_ch_offset(ctx, 0);

    if (ret != MOCA_API_SUCCESS)
    {
         moca_close(ctx);
         printf( "Error!  Internal error 1\n");
         return(-3);
    }    

    ret = moca_set_bandwidth(ctx, bandwidth);

    if (ret != MOCA_API_SUCCESS)
    {
       printf( "Error! Internal failure 2\n");
       moca_close(ctx);
       return(-7);
    }
 
    // ----------- Finish
 

    if (reset)
    {
        ret = moca_set_restart(ctx);
    }
    
    if (ret != MOCA_API_SUCCESS)
    {
        moca_close(ctx);
        printf( "Error!  Internal 2\n");
        return(-4);
    }

    if (persistent)
    {
       ret = moca_set_persistent(ctx);
       if (ret != MOCA_API_SUCCESS)
       {
          moca_close(ctx);
          printf( "Error!  Unable to save persistent parameters\n");
          return(-5);
       }
    }
    moca_close(ctx);

    return(0);
}

