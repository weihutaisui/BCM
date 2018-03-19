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
static int persistent = 0;     // -M option
static int reset = 0;          // -r option
static char *tlp = NULL;

GCAP_GEN static void showUsage()
{
    printf("Usage: GCAP.107 [tlpmode=<1,2>] [-M] [-r] [-h] \n\
Set or report the TLP mode of the node \n\
\n\
Options:\n\
<1/2>   Set tlp mode to 1(TLP_MIN1 and TLP_MAX1) or.\n\
                        2(TLP_MIN2 and TLP_MAX2)\n\
-r      Reset SoC to make configuration changes effective\n\
-M      Make configuration changes permanent\n\
-h      Display this help and exit\n");
}

GCAP_GEN int GCAP_107_main(int argc, char **argv)
{
    int nRet = MOCA_API_SUCCESS;
    void *ctx;
    int i;
    uint32_t tlpSet = 0;
    uint32_t  tlpmode;

    chipId = NULL;
    tlp = NULL;
    persistent = 0;
    reset = 0;

   // ----------- Parse parameters

#if defined(STANDALONE)
   for (i=1; i < argc; i++)
   {
       if (strcmp(argv[i], "-i") == 0)
       {
           i++;
           chipId = argv[i];
       }
       else if (strcmp(argv[i], "-M") == 0)
       {
          persistent = 1;
       }
       else if (strcmp(argv[i], "-r") == 0)
       {
          reset = 1;
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

    while((nRet = getopt(argc, argv, "hrMi:")) != -1) 
    {
        switch(nRet) 
        {
        case 'i':
            chipId = optarg;
            break;
        case 'M':
            persistent = 1;
            break;
        case 'r':
            reset = 1;
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
        return(-3);
    }

   if ((argc > 1) && (argv[1] != NULL))
   {
      for(i=1;i<argc;i++)
      {
        if (argv[i][0] == '-')
        {
            // Ignore
        }
        else if (!tlpSet)
        {
           tlp = argv[i];
           tlpSet = 1;
           if ((strcmp(tlp, "tlpmode=1") != 0) && (strcmp(tlp, "tlpmode=2") != 0))
           {
              printf("Error! tlpmode must be 1 or 2\n");
              moca_close(ctx);
              return(-2);
           }
           if ( strcmp(tlp, "tlpmode=1") == 0 ) 
           {
              tlpmode = 1;
           }
           if ( strcmp(tlp, "tlpmode=2") == 0 ) 
           {
              tlpmode = 2;
           }   
           nRet = moca_set_tlp_mode(ctx, tlpmode);
           if (nRet != MOCA_API_SUCCESS)
           {
             moca_close(ctx);
             printf( "Error!  Internal\n");
             return(-5);
           }
        }
      }
   }
   else  
   {
      nRet = moca_get_tlp_mode(ctx, &tlpmode);
      if (nRet != MOCA_API_SUCCESS)
      {
         moca_close(ctx);
         printf( "Error!  Internal\n");
         return(-4);
      }
      printf("TLP Mode is %d \n",tlpmode);
   }

   if (reset)
   {
      nRet = moca_set_restart(ctx);
   }
    
   if (nRet != MOCA_API_SUCCESS)
   {
       moca_close(ctx);
       printf( "Error!  Internal-4\n");
       return(-7);
   }

   if (persistent)
   {
      nRet = moca_set_persistent(ctx);
      if (nRet != MOCA_API_SUCCESS)
      {
          moca_close(ctx);
          printf( "Error!  Unable to save persistent parameters\n");
          return(-8);
      }
   }

   moca_close(ctx);
   return(0);
}
