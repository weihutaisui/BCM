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
    printf("Usage: GCAP.114  [-h] \n\
 Report the current power state of the node \n\
\n\
Options:\n\
  -h     Display this help and exit\n");
}

GCAP_GEN int GCAP_114_main(int argc, char **argv)
{
    int nRet = MOCA_API_SUCCESS;
    void *ctx;
    uint32_t  pwrstate;

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
   ctx = moca_open(chipId);

    if (!ctx)
    {
        printf( "Error!  Unable to connect to moca instance\n");
        return(-2);
    }


      nRet = moca_get_power_state(ctx, &pwrstate);
      if (nRet != MOCA_API_SUCCESS)
      {
         moca_close(ctx);
         printf( "Error!  Internal\n");
         return(-3);
      }
      printf("Power state of this node is : 0x%x\n", pwrstate);

      switch(pwrstate)
      {
         case (POWER_M0):
           printf("M0 ");
           break;
         case (POWER_M1):
          printf("M1 ");
          break;      
         case (POWER_M2):
          printf("M2 ");
          break;
         case (POWER_M3):
          printf("M3 ");
          break;
        default:
         printf("Unknown ");
         break;
      }     
      printf("\n");
   moca_close(ctx);
   return(0);
}
