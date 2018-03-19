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
static char *mask = NULL;

GCAP_GEN static void showUsage()
{
    printf("Usage: DCAP.125 <mask> [-i iface] [-h]\n\
Set the retransmission priorities mask.\n\
\n\
Bit Positions for Priority Level Bit Mask = Background (bit 3), High (bit 2), \n\
Medium (bit 1), Low (bit 0)\n\
\n\
Bit value= disabled (0) or enabled (1)\n\
\n\
\n\
Options:\n\
 <mask> Retransmission bit mask: [0 - 15]. Setting the retransmission bit mask to \n\
        0 effectively disables retransmission mode.\n\
  -h   Display this help and exit\n");
}

GCAP_GEN int DCAP_125_main(int argc, char **argv)
{
    int ret = 0;
    int rtrPrioMask = 0;
    struct moca_rtr_config rtr_config;
    void *ctx;
    char *end;
    
    chipId = NULL;
    mask = NULL;

    // ----------- Parse parameters
    if (argc < 2)
    {
        fprintf(stderr, "Error!  Missing parameter - mask\n");
        return(-2);
    }

    mask = argv[1];
    
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
    //     
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
    rtrPrioMask = strtoul(mask, &end, 0);

    if ((end != NULL) && (*end != '\0'))
    {
       fprintf(stderr, "Error!  Invalid parameter\n");
       return(-2);
    }

    if (rtrPrioMask > 0xF)
    {
       fprintf(stderr, "Error! Invalid retransmission bit mask. Valid values are 0-15.\n");
       return(-3);
    }

    // ----------- Initialize

    ctx = moca_open(chipId);

    if (!ctx)
    {
        fprintf(stderr, "Error!  Unable to connect to moca instance\n");
        return(-4);
    }

    rtr_config.low  = ((rtrPrioMask & (1 << 0)) == 0) ? MOCA_RTR_CONFIG_LOW_MIN  : MOCA_RTR_CONFIG_LOW_MAX;
    rtr_config.med  = ((rtrPrioMask & (1 << 1)) == 0) ? MOCA_RTR_CONFIG_MED_MIN  : MOCA_RTR_CONFIG_MED_MAX;
    rtr_config.high = ((rtrPrioMask & (1 << 2)) == 0) ? MOCA_RTR_CONFIG_HIGH_MIN : MOCA_RTR_CONFIG_HIGH_MAX;
    rtr_config.bg   = ((rtrPrioMask & (1 << 3)) == 0) ? MOCA_RTR_CONFIG_BG_MIN   : MOCA_RTR_CONFIG_BG_MAX;

    ret = moca_set_rtr_config(ctx, &rtr_config);
    if (ret != MOCA_API_SUCCESS) 
    {
        fprintf(stderr, "Error!  Internal_1\n");
        moca_close(ctx);
        return(-5);
    }


    // ----------- Finish

    moca_close(ctx);
    return(0);
}



