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
#include "GCAP_Common.h"

static char *chipId = NULL;    // -i option


GCAP_GEN static void showUsage()
{
    printf("Usage: GCAP.130 [-h]\n\
Display selected values from OFDMA Subchannel Definition Table\n\
\n\
Options:\n\
  -h     Display this help and exit\n");
}

GCAP_GEN static void moca_print_ofdma_definition(struct moca_ofdma_definition_table * in)
{
   uint32_t i;

   printf("== ofdma_definition ");
   printf(" ====================== \n");

   for (i = 0; i < in->ofdmaDefTabNum; i++) {
      printf("SubChannel: %d \n", i);
      printf("================================== \n");
      printf("OFDMA_SUBCHANNEL_DEFINITION_ID: 0x%x\n", in->subchannelDefId[i]);
      printf("NODE_BITMASK:                   0x%x\n", in->node_bitmask[i]);
      printf("subchannel_NBAS   :             0x%x\n", in->subchannel_NBAS[i]);      
      if (i % 8 == 7) printf("\n                    ");
   }
   printf(" ======================================= \n\n");
}

GCAP_GEN int GCAP_130_main(int argc, char **argv)
{
    int ret = 0;
    void *ctx;
    struct moca_ofdma_definition_table ofdma_table;

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
    // ----------- Initialize
#endif
    ctx=moca_open(chipId);

    if (!ctx)
    {
        printf( "Error!  Unable to connect to moca instance\n");
        return(-2);
    }
    memset(&ofdma_table, 0, sizeof(ofdma_table));
    ret = moca_get_ofdma_definition_table(ctx, &ofdma_table);
    
    if (ret != MOCA_API_SUCCESS)
    {
        printf( "Error! Internal failure-1\n");
        moca_close(ctx);
        return(0);
    }  
    if (ofdma_table.ofdmaDefTabNum > 0)
    {
       moca_print_ofdma_definition(&ofdma_table);
    }
    else
    {
       printf("Table is empty \n");
    }

    moca_close(ctx);

    return(0);
}

