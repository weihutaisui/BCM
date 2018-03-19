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


GCAP_GEN static void showUsage()
{
    printf("Usage: DCAP.45 <[ON]/OFF> [-r] [-h]\n\
Set Entry Node capability of the node.\n\
Default values will be used if parameters are not supplied.\n\
Note that resetting SoC is required for configuration to take affect.\n\
\n\
Options:\n\
 ON/OFF Turn ON or OFF Entry Node capability (default ON)\n\
  -r    Reset SoC to make configuration changes effective\n\
  -h    Display this help and exit\n");
}

GCAP_GEN int DCAP_45_main(int argc, char **argv)
{
    int ret = 0;
    void *ctx;
    uint32_t encapable =0;

    chipId = NULL;
    onoff = NULL;
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

    while((ret = getopt(argc, argv, "rhi:")) != -1) 
    {
        switch(ret) 
        {
        case 'i':
            chipId = optarg;
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
        return(-2);
    }


    if (strcmp(onoff, "ON") == 0)
    {
        encapable = 1;
    }
    else
    {
        encapable = 0;
    }

    ret = moca_set_en_capable(ctx, encapable);
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


