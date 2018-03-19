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
    printf("Usage: GCAP.126 [-h]\n\
Report the current retransmission mode & retransmission priority mask.\n\
\n\
Options:\n\
  -h   Display this help and exit\n");
}

GCAP_GEN int GCAP_126_main(int argc, char **argv)
{
    int ret = 0;
    int rtr_priority = 0;
    
    struct moca_rtr_config rtr_config;
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
    
    while((ret = getopt(argc, argv, "hi:")) != -1) 
    {
        switch(ret)
        {
        case 'i':
            chipId = optarg;
            break;
        case '?':
            printf( "Error!  Invalid parameter - %c\n", optopt);
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

    // ----------- Get retransmission configuration
    ret = moca_get_rtr_config(ctx, &rtr_config);
    if (ret != MOCA_API_SUCCESS) 
    {
        printf( "Error!  Internal_1\n");
        moca_close(ctx);
        return(-3);
    }

    if (rtr_config.low  != 0) rtr_priority |= (1 << 0);
    if (rtr_config.med  != 0) rtr_priority |= (1 << 1);
    if (rtr_config.high != 0) rtr_priority |= (1 << 2);
    if (rtr_config.bg   != 0) rtr_priority |= (1 << 3);

    printf ("Retransmission Mode     : %s\n", (rtr_priority == 0) ? "OFF" : "ON");
    printf ("Retransmission Priority : 0x%x\n", rtr_priority);

    // ----------- Finish

    moca_close(ctx);
    return(0);
}



