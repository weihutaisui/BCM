#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>

#include <mocalib.h>
#include "GCAP_Common.h"

static char *chipId = NULL;    // -i option

GCAP_GEN static void showUsage()
{
    printf("Usage: GCAP.24 [-r] [-h]\n\
Report or reset packet aggregation and packet fragmentation statistics.\n\
\n\
Options:\n\
  -r   Reset aggregation statistics\n\
  -h   Display this help and exit\n");
}



GCAP_GEN int GCAP_24_main(int argc, char **argv)
{
    int ret = MOCA_API_SUCCESS;
    struct moca_interface_status status;
    struct moca_gen_stats gs;
    void *ctx;
    uint32_t reset_stats = 0;
    int count;

    chipId = NULL;

    // ----------- Parse parameters
#if defined(STANDALONE)
   int i;

   for (i=1; i < argc; i++)
   {
        if (strcmp(argv[i], "-i") == 0)
        {
           chipId = argv[i+1];
           i++;
        }
        else if (strcmp(argv[i], "-r") == 0)
        {
           reset_stats = 1;
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
        case 'r':
            reset_stats = 1;
            break;
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

    // ----------- Get info
    ret = moca_get_interface_status(ctx, &status);

    if (ret != MOCA_API_SUCCESS)
    {
        printf( "Error!  Internal failure-1\n");
        moca_close(ctx);
        return(-3);
    }

    if (status.link_status != MOCA_LINK_UP)
    {
        printf( "Error! No Link\n");
        moca_close(ctx);
        return(-4);
    }  

    if ( reset_stats == 1)
    {
      ret = moca_set_reset_stats(ctx);
       if (ret != MOCA_API_SUCCESS)
       {
           printf( "Error!  Internal failure-2\n");
           moca_close(ctx);
           return(-5);
       }
    } 

    // ----------- Output Data   
    ret = moca_get_gen_stats(ctx, reset_stats, &gs);
    
    if (ret != MOCA_API_SUCCESS)
    {
        printf( "Error!  Internal failure-3\n");
        moca_close(ctx);
        return(-6);
    }

    printf ("Total TX Pkts        : %8u   ", 
      gs.ecl_tx_total_pkts  ) ;
    printf ("Total RX Pkts        : %8u   \n\n", 
      gs.ecl_rx_total_pkts) ;
    printf ("Aggregation Stats:\n\n");
    printf ("Highest RX Aggr Level: %8u   ", gs.aggr_pkt_stats_rx_max );
    printf ("Count                : %8u\n", gs.aggr_pkt_stats_rx_count);
    printf ("\n");
    for (count = 0; count < 20; count++) {
       if (count == 0)
       {
          printf ("TX Pkts ( non - aggr): %8u\n", gs.aggr_pkt_stats_tx[count]) ;
       }
       else
       {
          printf ("TX Pkts (%2u-pkt aggr): %8u\n", 
                   (count+1), gs.aggr_pkt_stats_tx[count]);
       }
    }
    printf ("\n");
    printf("Total TX fragmented MPDUs   : %8u   \n",   gs.mac_frag_mpdu_tx);
    printf("Total RX fragmented MPDUs   : %8u   \n",   gs.mac_frag_mpdu_rx);
    // ----------- Finish

    moca_close(ctx);

    return(0);
}

