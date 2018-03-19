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

GCAP_GEN static void showUsage()
{
    printf("Usage: GCAP.07 [-h]\n\
Report beacon information when a valid beacon is detected.\n\
\n\
Options:\n\
  -h   Display this help and exit\n");
}


GCAP_GEN int GCAP_07_main(int argc, char **argv)
{
    int nRet = 0;
    struct moca_interface_status if_status;
    void *ctx;

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
    // ----------- Initialize

    ctx = moca_open(chipId);

    if (!ctx)
    {
        printf( "Error!  Unable to connect to moca instance\n");
        return(-2);
    }

   

   nRet = moca_get_interface_status(ctx, &if_status);
    if (nRet != MOCA_API_SUCCESS) 
    {
        printf( "Error!  Internal_1\n");
        moca_close(ctx);
        return(-3);
    }

   printf("Beacon at %dMhz with primary channel at %dMhz is detected.\n", (int)if_status.rf_channel *25, (int) if_status.primary_channel *25);
   
    // ----------- Finish

    moca_close(ctx);
    return(0);
}



