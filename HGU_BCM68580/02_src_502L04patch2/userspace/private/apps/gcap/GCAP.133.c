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
static char *nthMsdu = NULL;

GCAP_GEN static void showUsage()
{
    printf("Usage: GCAP.133 nthMsdu [-h]\n\
Flag 1 NACK for an MSDU in every Nth MSDU received.\n\
\n\
Options:\n\
  nthMsdu   Set Nth Msdu.  Only allowable values are:\n\
            1e-3, 1e-4, 1e-5, 1e-6, 1e-7 and 1e-8 and\n\
            0 to turn off the deliberate flag of NACK\n\
            in BlockAck for an MSDU.\n\
  -h   Display this help and exit\n");
}

GCAP_GEN int GCAP_133_main(int argc, char **argv)
{
    int ret = 0;
    void *ctx;
    uint32_t nackRateExp = 0;
    uint32_t nackRate = 0;
    
    chipId = NULL;
    nthMsdu = NULL;

    // ----------- Parse parameters
    if (argc < 2)
    {
        printf( "Error!  Missing parameter - nthMsdu\n");
        return(-2);
    }

    nthMsdu = argv[1];

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
    if (nthMsdu[0] == '0')
    {
       nackRateExp = 0;
    }
    else if ((nthMsdu[0] == '1') &&
             (nthMsdu[1] == 'e') &&
             (nthMsdu[2] == '-') ) 
    {
       nackRateExp = nthMsdu[3] - '0';
    }
    else
    {
       printf( "Error! Invalid NthMsdu number.  Valid numbers are: 1e-3, 1e-4, 1e-5, 1e-6, 1e-7 and 1e-8 and 0\n");
       return(-2);
    }

    if ((nackRateExp == 1) ||
         (nackRateExp == 2) ||
         (nackRateExp > 8))
    {
       printf( "Error! Invalid NthMsdu number.  Valid numbers are: 1e-3, 1e-4, 1e-5, 1e-6, 1e-7 and 1e-8 and 0\n");
       return(-3);
    }

    // ----------- Initialize

    ctx = moca_open(chipId);

    if (!ctx)
    {
        printf( "Error!  Unable to connect to moca instance\n");
        return(-4);
    }

    if (nackRateExp != 0)
    {
        nackRate = 1;
        while (nackRateExp--)
        {
           nackRate *= 10;
        }
    }

    ret = __moca_set_block_nack_rate(ctx, nackRate);
    if (ret != MOCA_API_SUCCESS) 
    {
        printf( "Error!  Internal_1\n");
        moca_close(ctx);
        return(-5);
    }


    // ----------- Finish

    moca_close(ctx);
    return(0);
}



