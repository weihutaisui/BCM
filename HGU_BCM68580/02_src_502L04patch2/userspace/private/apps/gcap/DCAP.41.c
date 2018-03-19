#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>

#include <mocalib.h>


static int reset = 0;          // -r option
static char *chipId = NULL;    // -i option
static char *onoff = NULL;
static char *chnl = NULL;


GCAP_GEN static void showUsage()
{
    printf("Usage: DCAP.41 <[ON]/OFF> [-r] [-h]\n\
Set node to operate in non-transmit mode.\n\
Default values will be used if parameters are not supplied.\n\
Note that resetting SoC is required for configuration to take affect.\n\
\n\
Options:\n\
 ON/OFF Turn ON or OFF non-transmit mode(default OFF)\n\
  -r    Reset SoC to make configuration changes effective\n\
  -v    channel,  1 for Beacon 50 Mhz, 2 for Primary 100 Mhz.\n\
  -h    Display this help and exit\n");
}

GCAP_GEN int DCAP_41_main(int argc, char **argv)
{
    int ret = 0;
    void *ctx;
    uint32_t txmode =0;
    uint32_t bandwidth;
    int rxchnl=2;

    chipId = NULL;
    onoff = NULL;
    chnl = NULL; 
    bandwidth = MOCA_BANDWIDTH_100MHZ;
    reset=0;

    // ----------- Parse parameters


    if ((argc < 2) || (argv[1] == NULL) || (argv[1][0] == '-'))
    {
        onoff = "OFF";
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
      else if (strcmp(argv[i], "-r") == 0)
      {
         reset = 1; 
      }

      else if (strcmp(argv[i], "-v") == 0)
      {
          i++;
          chnl = argv[i];
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
    while((ret = getopt(argc, argv, "rhv:i:")) != -1) 
    {
        switch(ret) 
        {
        case 'i':
            chipId = optarg;
            break;
        case 'r':
            reset = 1;
            break;
        case 'v':
            chnl = optarg;
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
    if (chnl != NULL )
    {
       rxchnl = atoi(chnl);

       if ((rxchnl != 1) && (rxchnl != 2))
       {
          printf("Error!  Illegal value for -v option. Must be one of '1' or '2'\n");
          return(-2);
       }

       if (rxchnl == 1)
          bandwidth = MOCA_BANDWIDTH_50MHZ;
    }

    ctx = moca_open(chipId);

    if (!ctx)
    {
        printf( "Error!  Unable to connect to moca instance\n");
        return(-2);
    }


    if (strcmp(onoff, "ON") == 0)
    {
        txmode = 2;
    }
    else
    {
        bandwidth = MOCA_BANDWIDTH_100MHZ;
        txmode = 0;
    }

    ret = moca_set_bandwidth(ctx, bandwidth);
    if (ret != MOCA_API_SUCCESS)
    {
       moca_close(ctx);
       printf( "Error!  Internal\n");
       return(-4);
    }


    ret = moca_set_continuous_power_tx_mode(ctx, txmode);
    if (ret != MOCA_API_SUCCESS)
    {
       moca_close(ctx);
       printf( "Error!  Internal\n");
       return(-2);
    }

    if (reset)
    {
         ret = moca_set_restart(ctx);
         if (ret != MOCA_API_SUCCESS)
         {
            moca_close(ctx);
            printf( "Error!  Reinitialize\n");
            return(-3);
         }
     }

    moca_close(ctx);


    return(0);
}


