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

static int persistent = 0;     // -M option
static int reset = 0;          // -r option
static char *chipId = NULL;    // -i option

GCAP_GEN static void showUsage()
{
    printf("Usage: GCAP.103 [-s <Beacon Backoff index>] [-M] [-r] [-h] \n\
Set Golden Node to operate with beacon backoff enabled.\n\
If no options are specified this command will display the Beacon \n\
Backoff value set in the local node. Setting to 0 will disable.\n\
Note that resetting SoC is required for configuration to take effect.\n\
\n\
Options:\n\
  -s     Set the Beacon Backoff value set in the local node\n\
  -M     Make configuration changes permanent\n\
  -r     Reset SoC to make configuration changes effective\n\
  -h     Display this help and exit\n\
\n\
Beacon Backoff Index Values:\n\
  0 (-0 db)\n\
  1 (-3 dB)\n\
  2 (-6 dB)\n\
  3 (-9 dB)\n\
  4 (-12 dB)\n\
  5 (-15 dB)\n");
}

GCAP_GEN int GCAP_103_main(int argc, char **argv)
{
    int nRet = MOCA_API_SUCCESS;
    void *ctx;
    uint32_t beaconPwrReduction = 0;
    uint32_t beaconPwrReductionEn;
    uint32_t  rfbnd;
    int i;

    persistent = 0;
    reset = 0;

    chipId = NULL;
    
    // ----------- Parse parameters

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
       else if (strcmp(argv[i], "-s") == 0)
       {
          i++;
          beaconPwrReduction = atoi(argv[i]);
          if (beaconPwrReduction > 5)
          {
            printf( "Error!  Invalid backoff index value.\n");
            return(-1);
          }
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

    // ----------- Initialize

   ctx = moca_open(chipId);

    if (!ctx)
    {
        printf( "Error!  Unable to connect to moca instance\n");
        return(-3);
    }

    /* If there are no parameters, just print out the current setting */
    if (argc == 1)
    {
        nRet = moca_get_beacon_pwr_reduction_en(ctx, &beaconPwrReductionEn);
        if (nRet != MOCA_API_SUCCESS)
        {
           printf( "Error!  Internal failure - 1\n");
           moca_close(ctx);
           return(-4);
        }
        nRet = moca_get_beacon_pwr_reduction(ctx, &beaconPwrReduction);
        if (beaconPwrReductionEn)
        {
            printf( "Beacon Backoff set to %d (-%d dB).\n",
                beaconPwrReduction, (beaconPwrReduction * 3));
        }
        else
        {
            printf( "Beacon Backoff disabled.\n");
        }
    }
    else
    {
       nRet =   moca_get_rf_band(ctx, &rfbnd);
       if (  (rfbnd ==  MOCA_RF_BAND_E  || rfbnd == MOCA_RF_BAND_F) &&
             (beaconPwrReduction != 0) )
       {
            printf( "Beacon Backoff should be disabled for rf_band %d.\n", rfbnd);
           moca_close(ctx);
           return(-5);          
       }
       beaconPwrReductionEn = (beaconPwrReduction == 0) ? 0 : 1;
       nRet = moca_set_beacon_pwr_reduction_en(ctx, beaconPwrReductionEn);
       if (nRet != MOCA_API_SUCCESS)
       {
           printf( "Error!  Internal failure - 2\n");
           moca_close(ctx);
           return(-6);
       }
       nRet = moca_set_beacon_pwr_reduction(ctx, beaconPwrReduction);
       if (nRet != MOCA_API_SUCCESS)
       {
           printf( "Error!  Internal failure - 3\n");
           moca_close(ctx);
           return(-7);
       }

       if (reset)
       {
           nRet = moca_set_restart(ctx);
       }
    
       if (nRet != MOCA_API_SUCCESS)
       {
          moca_close(ctx);
          printf( "Error!  Internal-4\n");
          return(-8);
       }

       if (persistent)
       {
         nRet = moca_set_persistent(ctx);
         if (nRet != MOCA_API_SUCCESS)
         {
             moca_close(ctx);
             printf( "Error!  Unable to save persistent parameters\n");
             return(-9);
          }
       }
    }
    moca_close(ctx);
    return(0);
}
