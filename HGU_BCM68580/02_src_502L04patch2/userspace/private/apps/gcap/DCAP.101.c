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

static char *chipId = NULL;    // -i option
static char *per_mode_str = NULL;
static int persistent = 0;     // -M option

GCAP_GEN static void showUsage()
{
    printf("Usage: DCAP.101 <Per mode> [-M] [-h]\n\
Set the PER mode of the node to either NPER or VLPER.\n\
\n\
Options:\n\
 <Per mode>  'NPER' or 'VLPER'.\n\
  -M   Make configuration changes permanent\n\
  -h   Display this help and exit\n");
}

GCAP_GEN int DCAP_101_main(int argc, char **argv)
{
    int ret = MOCA_API_ERROR;
    void *ctx;
    uint32_t per_mode = MOCA_PER_MODE_NPER;

    chipId = NULL;
    per_mode_str = NULL;
    persistent=0;

    // ----------- Parse parameters

    if (argc < 2)
    {
        fprintf(stderr, "Error!  Missing parameter - Per mode\n");
        return(-2);
    }

    per_mode_str = argv[1];
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
       else if (strcmp(argv[i], "?") == 0)
       {
          printf( "Error!  Invalid option - %s\n", argv[i]);
          return(-1);
       }
       else if (strcmp(argv[i], "-h") == 0)
       {
          showUsage();
       }
   }
#else
    opterr = 0;
    while((ret = getopt(argc, argv, "Mhi:")) != -1) 
    {
        switch(ret) 
        {
        case 'i':
            chipId = optarg;
            break;
        case '?':
            fprintf(stderr, "Error!  Invalid option - %c\n", optopt);
            return(-1);
            break;
        case 'M':
            persistent = 1;
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
        fprintf(stderr, "Error!  Unable to connect to moca instance\n");
        return(-2);
    }
        
    // ----------- Save Settings 
    
    if (per_mode_str != NULL)
    {
        if (!strcmp(per_mode_str, "NPER"))
        {
           per_mode = MOCA_PER_MODE_NPER;
        }
        else if (!strcmp(per_mode_str, "VLPER"))
        {
           per_mode = MOCA_PER_MODE_VLPER;
        }
        else
        {
           fprintf(stderr, "Error!  Illegal value for <Per mode>. Must be one of 'NPER' or 'VLPER'.\n");
           moca_close(ctx);
           return(-3);
        }

        ret = moca_set_per_mode(ctx, per_mode);
    }
    else
    {
       moca_close(ctx);
       fprintf(stderr, "Error!  Invalid parameter - Per mode\n");
       return(-4);
    }

    // ----------- Activate Settings   
    
    if (ret != MOCA_API_SUCCESS)
    {
        moca_close(ctx);
        fprintf(stderr, "Error!  Invalid parameter - Per mode\n");
        return(-5);
    }

    if (persistent)
    {
       ret = moca_set_persistent(ctx);
       if (ret != MOCA_API_SUCCESS)
       {
          moca_close(ctx);
          fprintf(stderr, "Error!  Unable to save persistent parameters\n");
          return(-10);
       }
    }

    // ----------- Finish

    moca_close(ctx);

    return(0);
}
