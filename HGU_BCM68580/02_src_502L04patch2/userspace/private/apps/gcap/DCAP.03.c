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
static char *onoff = NULL;
static char *chnl = NULL;   //-v option

GCAP_GEN static void showUsage()
{
    printf("Usage: DCAP.03 <[ON]/OFF> <-v 1/2/3/4> [-r] [-h]\n\
Set Node to operate in continuous power TX mode.\n\
Default values will be used if parameters are not supplied.\n\
Note that resetting SoC is required for configuration to take effect.\n\
If multiple configuration parameters are being changed, -r option can be\n\
used in the last command.\n\
\n\
Options:\n\
 ON/OFF Turn ON or OFF the continuous power TX mode \n\
        OFF - permanently, '-M' can be omitted.\n\
  -v    channel mode\n\
          1 for Beacon 50 MHz\n\
          2 for Primary 100 MHz\n\
          3 for Secondary 100 MHz\n\
          4 for Primary and Secondary (2 x 100 MHz)\n\
  -r    Reset SoC to make configuration changes effective\n\
  -h    Display this help and exit\n");
}

GCAP_GEN int DCAP_03_main(int argc, char **argv)
{
#if defined(STANDALONE)
    int i;
#endif
    int ret = 0;
    void *ctx;    
    uint32_t    txchnl =1;
    uint32_t bandwidth = 0;
    uint32_t ctx_mode = 1;
    int32_t sec_ch_offs = 0;
    struct moca_const_tx_params const_tx_params;

    chipId = NULL;
    onoff = NULL;
    chnl = NULL;
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
        return(-2);
    }
#if defined(STANDALONE)
    for (i=1; i < argc; i++)
    {
       if (strcmp(argv[i], "-v") == 0)
       {
          i++;
          chnl = argv[i];
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

    while((ret = getopt(argc, argv, "hri:v:")) != -1) 
    {
        switch(ret) 
        {
        case 'i':
            chipId = optarg;
            break;
        case 'v':
            chnl = optarg;
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
    if (chnl != NULL )
    {
       txchnl = atoi(chnl);

       if ((txchnl < 1) || (txchnl > 4))
       {
          printf("Error!  Illegal value for -v option. Must be one of '1', '2', '3' or '4'\n");
          return(-2);
       }
    }

    // ----------- Initialize

    ctx = moca_open(chipId);

    if (!ctx)
    {
        printf( "Error!  Unable to connect to moca instance\n");
        return(-3);
    }

    ret = moca_get_secondary_ch_offset(ctx, &sec_ch_offs);
    if (ret != MOCA_API_SUCCESS)
    {
       printf( "Error! Internal failure 2.0\n");
       moca_close(ctx);
       return(-4);
    }
    
    if (chnl != NULL )
    {
       switch (txchnl)
       {
           case 1:
              bandwidth = 0;
              ctx_mode = 1;
              break;
   
           case 2:
              bandwidth = 1;
              ctx_mode = 1;
              break;
              
           case 3:
              bandwidth = 1;
              ctx_mode = 5;
              if (sec_ch_offs == 0) 
                 sec_ch_offs = 1;
              
              ret = moca_set_bonding(ctx, 1);
              if (ret != MOCA_API_SUCCESS)
              {
                 printf( "Error! Internal failure 2.3\n");
                 moca_close(ctx);
                 return(-6);
              }
              break;
   
           case 4:
              bandwidth = 1;
              ctx_mode = 6;
              if (sec_ch_offs == 0) 
                 sec_ch_offs = 1;

              ret = moca_set_bonding(ctx, 1);
              if (ret != MOCA_API_SUCCESS)
              {
                 printf( "Error! Internal failure 2.4\n");
                 moca_close(ctx);
                 return(-7);
              }
              break;
        }
       
        ret = moca_set_bandwidth(ctx, bandwidth);
        if (ret != MOCA_API_SUCCESS)
        {
           printf( "Error! Internal failure 3\n");
           moca_close(ctx);
           return(-8);
        }
    }

    // ----------- Get info
    ret = moca_get_const_tx_params(ctx, &const_tx_params);

    if (strcmp(onoff, "ON") == 0)
    {
       const_tx_params.const_tx_submode = 1;
       ret = moca_set_continuous_power_tx_mode(ctx, ctx_mode);
       ret |= moca_set_const_tx_params(ctx, &const_tx_params); 
       ret |= moca_set_secondary_ch_offset(ctx, sec_ch_offs);
    }
    else
    {
        const_tx_params.const_tx_submode = 0;
        ret = moca_set_continuous_power_tx_mode(ctx, 0);
        ret |= moca_set_const_tx_params(ctx, &const_tx_params); 
    }

    if (ret != MOCA_API_SUCCESS)
    {
        printf( "Error! Internal failure-1\n");
        moca_close(ctx);
        return(-9);
    }

    
  /*TBD  set the Tx mode channel  */
 
    if (reset)
    {
        ret = moca_set_restart(ctx);

        if (ret != MOCA_API_SUCCESS)
        {
           moca_close(ctx);
           printf( "Error!  Invalid parameter \n");
           return(-10);
        }
    }

    moca_close(ctx);

    return(0);
}


