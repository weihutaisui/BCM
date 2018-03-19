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

static int persistent = 0;     // -M option
static int reset = 0;          // -r option
static char *primOffset = NULL;    // -p option
static char *secOffset = NULL;    // -s option
static char *chipId = NULL;    // -i option
static char *lof = NULL;

GCAP_GEN static void showUsage()
{
    printf("Usage: DCAP.02 <OF> [-p <00/01/10>][-s <00/01/10>][-M] [-r] [-h]\n\
Set Node to operate at a single frequency or network search.\n\
Note that resetting SoC is required for configuration to take effect.\n\
If multiple configuration parameters are being changed, -r option can be\n\
used in the last command.\n\
\n\
Options:\n\
 <OF>  Frequency of Operation in MHz. Valid values are a MoCA channel frequency\n\
       or -1 to go to Network Search.\n\
  -p   Offset of primary channel, 00 same, 01 above, 10 below.\n\
  -s   Offset of secondary channel relative to primary channel for bonded mode.\n\
  -M   Make configuration changes permanent\n\
  -r   Reset SoC to make configuration changes effective\n\
  -h   Display this help and exit\n");
}

GCAP_GEN int DCAP_02_main(int argc, char **argv)
{
    int ret = MOCA_API_SUCCESS;
    void *ctx;
    uint32_t freq = 0;
    uint32_t rf_band;
    int32_t prim_offset = 0;
    int32_t sec_offset = 0;
    uint32_t const_tx_mode;
    uint32_t bandwidth;
    struct moca_const_tx_params const_tx_params;

    chipId = NULL;
    primOffset = NULL;
    secOffset = NULL;
    lof = NULL;
    persistent=0;
    reset=0;

    // ----------- Parse parameters


    if (argc < 2)
    {
        printf( "Error!  Missing parameter - OF\n");
        return(-2);
    }

    lof = argv[1];

#if defined(STANDALONE)
   int i;
   for (i=1; i < argc; i++)
   {
      if (strcmp(argv[i], "-i") == 0)
      {
         i++;
         chipId = argv[i];
      }
      else if (strcmp(argv[i], "-p") == 0)
      {
         i++;
         primOffset = argv[i];
      }
      else if (strcmp(argv[i], "-s") == 0)
      {
         i++;
         secOffset = argv[i];
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
      else if (strcmp(argv[i], "1") == 0)
      {
          // Do nothing, -1 is a valid value
      }
      else if (strcmp(argv[i], "-h") == 0)
      {
         showUsage();
         return(0); 
      }
   } 
#else
    opterr = 0;

    while((ret = getopt(argc, argv, "Mrh1i:p:s: ")) != -1) 
    {
        switch(ret) 
        {
        case 'i':
            chipId = optarg;
            break;
        case 'p':
            primOffset = optarg;
            break;
        case 's':
            secOffset = optarg;
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
        case '1':
            // Do nothing, -1 is a valid value
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
        
    // ----------- Save Settings 
    
    /* TBD: Get Persistent data first? */

    if (primOffset != NULL)
    {
        if (!strcmp(primOffset, "10"))
        {
           prim_offset = -25;
        }
        else if (!strcmp(primOffset, "00"))
        {
           prim_offset = 0;
        }
        else if (!strcmp(primOffset, "01"))
        {
           prim_offset = 25;
        }
        else
        {
           printf( "Error!  Illegal value for -p option. Must be one of '10', '00', or '01'.\n");
           moca_close(ctx);
           return(-3);
        }

        moca_set_primary_ch_offset(ctx, prim_offset);
    }

    if (secOffset != NULL)
    {
        if (!strcmp(secOffset, "10"))
        {
           sec_offset = -125;
        }
        else if (!strcmp(secOffset, "00"))
        {
           sec_offset = 0;
        }
        else if (!strcmp(secOffset, "01"))
        {
           sec_offset = 125;
        }
        else
        {
           printf( "Error!  Illegal value for -s option. Must be one of '10', '00', or '01'.\n");
           moca_close(ctx);
           return(-3);
        }

        moca_set_secondary_ch_offset(ctx, sec_offset);
    }
    if (atoi(lof) == -1)
    {
        ret = moca_set_single_channel_operation(ctx, MOCA_SINGLE_CH_OP_NETWORK_SEARCH);
        if (ret != MOCA_API_SUCCESS)
        {
            moca_close(ctx);
            printf( "Error!  SetInitParms\n");
            return(-4);
        }
    }
    else
    {
        /* TBD: Handle rf band change */
        freq = atol(lof);

        ret = moca_set_lof(ctx, freq);
        if (ret != MOCA_API_SUCCESS)
        {
            moca_close(ctx);
            printf( "Error!  Internal 1\n");
            return(-5);
        }

        ret = moca_get_rf_band(ctx, &rf_band);
        if (ret != MOCA_API_SUCCESS)
        {
            moca_close(ctx);
            printf( "Error!  Internal 2\n");
            return(-6);
        }

        ret = moca_set_single_channel_operation(ctx, MOCA_SINGLE_CH_OP_SINGLE);
        if (ret != MOCA_API_SUCCESS)
        {
            moca_close(ctx);
            printf( "Error!  Internal 4\n");
            return(-8);
        }

    }    


    ret = moca_get_const_tx_params(ctx, &const_tx_params);
    ret |= moca_get_continuous_power_tx_mode(ctx, &const_tx_mode);
   
    if (ret != MOCA_API_SUCCESS)
    {
       moca_close(ctx);
       printf( "Error!  Internal 4\n");
       return(-8);
    }

    // hack for CW mode when lof is the bottom frequency of the band:
    if ((const_tx_params.const_tx_submode == 2) && //CW mode
        (const_tx_mode == 1) &&
        (prim_offset == 0) &&
        ((freq == 1150) || (freq == 500) || (freq == 675))) // bottom frequencies of each band
    {
       bandwidth = MOCA_BANDWIDTH_50MHZ;
       ret = moca_set_bandwidth(ctx, bandwidth);
       if (ret != MOCA_API_SUCCESS)
       {
          moca_close(ctx);
          printf( "Error!  Internal 5\n");
          return(-9);
       }
    }


    // ----------- Activate Settings   

    if (reset)
    {
        ret = moca_set_restart(ctx);
    }
    
    if (ret != MOCA_API_SUCCESS)
    {
        moca_close(ctx);
        printf( "Error!  Invalid parameter - OF\n");
        return(-9);
    }

    if (persistent)
    {
       ret = moca_set_persistent(ctx);
       if (ret != MOCA_API_SUCCESS)
       {
          moca_close(ctx);
          printf( "Error!  Unable to save persistent parameters\n");
          return(-10);
       }
    }


    // ----------- Finish

    moca_close(ctx);

    return(0);
}
