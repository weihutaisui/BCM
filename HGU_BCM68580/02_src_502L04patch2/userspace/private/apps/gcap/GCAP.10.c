#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>

#include <mocalib.h>

static char *chipId = NULL;    // -i option

GCAP_GEN static void showUsage()
{
    printf("Usage: GCAP.10 [-h] \n\
Options:\n\
  -h   Display this help and exit\n\
Function: Report beacon, primary and secondary channels of operation for network\n");
}

GCAP_GEN int GCAP_10_main(int argc, char **argv)
{
    int ret;
    void *ctx;
    struct moca_interface_status status;

    chipId = NULL;

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
    // ----------- Parse parameters
    opterr = 0;
    
    while((ret = getopt(argc, argv, "hi:")) != -1) 
    {
        switch(ret)
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
    // ----------- Initialize

    ctx=moca_open(chipId);

    if (!ctx)
    {
        printf( "Error!  Unable to connect to moca instance\n");
        return(-2);
    }

    // ----------- Obtain status 

    ret = moca_get_interface_status(ctx, &status);

    if (ret != MOCA_API_SUCCESS)
    {
        printf( "Error!  Internal failure\n");
        moca_close(ctx);
        return(-3);
    }
    
    if (status.link_status != MOCA_LINK_UP)
    {
        printf( "Error! No Link\n");
        moca_close(ctx);
        return(-4);
    }
    
    // ----------- Output Data   

    printf("Beacon Channel   : %4d.0 MHz\n", status.rf_channel * 25);
    printf("Primary Channel  : %4d.0 MHz\n", status.primary_channel * 25);
    printf("Secondary Channel: %4d.0 MHz\n", status.secondary_channel * 25);
    
    // ----------- Finish

    moca_close(ctx);

    return(0);
}

