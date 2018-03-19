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
    printf("Usage: DCAP.102 [-h]\n\
Report the PER mode of this node, NPER (nominal) or VLPER (very low).\n\
\n\
Options:\n\
  -h   Display this help and exit\n");
}

GCAP_GEN int DCAP_102_main(int argc, char **argv)
{
    int ret;
    void *ctx;
    uint32_t per_mode;

    chipId = NULL;
    
    // ----------- Parse parameters
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
       }
   }
#else    
    opterr = 0;
    while((ret = getopt(argc, argv, "hi:")) != -1) 
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

    // ----------- Get info
    ret = moca_get_per_mode(ctx, &per_mode);

    if (ret != 0)
    {
        fprintf(stderr, "Error!  mocalib failure\n");
        moca_close(ctx);
        return(-3);
    }

    printf("%s\n", (per_mode == 0 ? "NPER" : (per_mode == 1 ? "VLPER" : "Unknown")));
        
    // ----------- Finish

    moca_close(ctx);

    return(0);
}

