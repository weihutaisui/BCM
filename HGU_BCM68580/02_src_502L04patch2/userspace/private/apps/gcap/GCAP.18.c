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
    printf("Usage: GCAP.18 [-h]\n\
Report the Privacy Status and the password on the node's MoCA interface.\n\
\n\
Options:\n\
  -h      Display this help and exit\n");
}

GCAP_GEN int GCAP_18_main(int argc, char **argv)
{
    void *ctx;
    int ret =0;
    struct moca_password password;
    uint32_t privacy;

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
       return(0); 
    }
 }

#else

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
        return(-7);
    }

    // ----------- Get Settings 

   ret = moca_get_privacy_en(ctx, &privacy);

    if (ret != MOCA_API_SUCCESS)
    {
        moca_close(ctx);
        printf( "Error!  Internal 1\n");
        return(-8);
    }

    printf("MoCA Privacy %s\n", (privacy == 1) ? "enabled" : "disabled" );

    memset(&password, 0, sizeof(password));
    ret = moca_get_password(ctx, &password);
    if (ret != MOCA_API_SUCCESS)
    {
        moca_close(ctx);
        printf( "Error!  Internal 2\n");
        return(-9);
    }

   printf("password: %s \n", password.password);
 
    // ----------- Finish

    moca_close(ctx);

    return(0);
}

