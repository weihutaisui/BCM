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

static int reset = 0;          // -r option
static char *chipId = NULL;    // -i option
static int persistent = 0;     // -M option


GCAP_GEN static void showUsage()
{
    printf("Usage: GCAP.43 TxPower [-r] [-M] [-h]\n\
Set Golden Node Transmit Power Backoff to xx dB.\n\
Note that resetting SoC is required for configuration to take effect.\n\
\n\
Options:\n\
  TxPower    Transmit Power Backoff in dB: 0 to 10 \n\
  -r    Reset SoC to make configuration changes effective\n\
  -M    Make configuration changes permanent\n\
  -h    Display this help and exit\n");
}

GCAP_GEN int GCAP_43_main(int argc, char **argv)
{
    int nRet = MOCA_API_SUCCESS;
    void *ctx;
    int max_tx_power;
    int backoffIndex;

    chipId = NULL;
    persistent = 0;
    reset = 0;

    // ----------- Parse parameters

    if ((argc < 2) || (argv[1] == NULL))
    {
          fprintf(stderr, "Error! missing parameters \n");
          return(-2);
    }
    backoffIndex = atoi(argv[1]);
    if ((backoffIndex < 0) || (backoffIndex > 10))
    {
        fprintf(stderr, "Error!  Invalid backoff index value range from 0-10.\n");
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
    while((nRet = getopt(argc, argv, "Mhri:")) != -1) 
    {
        switch(nRet) 
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
    // ----------- Initialize

   ctx = moca_open(chipId);

    if (!ctx)
    {
        printf( "Error!  Unable to connect to moca instance\n");
        return(-3);
    }



 
    max_tx_power = MOCA_MAX_TX_POWER_DEF - (backoffIndex * 3);  

    nRet = moca_set_max_tx_power(ctx, max_tx_power);

    if (nRet != 0)
    {
       printf( "Error!internal failure-3\n");
       moca_close(ctx);
       return(-6);
    }

    if (persistent)
    {
        nRet = moca_set_persistent(ctx);
        if (nRet != MOCA_API_SUCCESS)
        {
           moca_close(ctx);
           printf( "Error!  Unable to save persistent parameters\n");
           return(-7);
        }
    }

    if (reset)
    {
        nRet = moca_set_restart(ctx);
    
        if (nRet != MOCA_API_SUCCESS)
        {
            moca_close(ctx);
            printf( "Error!  Internal-4\n");
            return(-8);
        }
    }

    moca_close(ctx);
    return(0);
}
