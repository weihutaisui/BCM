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

static char *on_off = NULL; 
static char *chipId = NULL;    // -i option
static int persistent = 0;     // -M option
static int reset = 0;          // -r option

GCAP_GEN static void showUsage()
{
    printf("Usage: GCAP.123 [mode] [-M] [-r] [-h]\n\
 Report/Set the LOF_UPDATE of the node \n\
 Note that resetting SoC is required for configuration to take effect.\n\
\n\
Options:\n\
 [mode] Set mode to enabled or disabled\n\
  -M   Make configuration changes permanent\n\
  -r   Reset SoC to make configuration changes effective\n\
  -h   Display this help and exit\n");

}

GCAP_GEN int GCAP_123_main(int argc, char **argv)
{
    int nRet = MOCA_API_SUCCESS;
    void *ctx;
    uint32_t  lofup;
    int i;
    uint32_t lofSet = 0;

    chipId = NULL;
    on_off = NULL;

    persistent = 0;
    reset = 0;

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
    // ----------- Parse parameters

    opterr = 0;

    while((nRet = getopt(argc, argv, "Mirh::")) != -1) 
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
        else if (!lofSet)
        {
           on_off = argv[i];
           lofSet = 1;
           if( !strcmp( on_off, "enabled" ) )
           {
              lofup = 1;  
           }
           else if( !strcmp( on_off, "disabled" ) )
           {
              lofup = 0;   
           }
           else
           {
              printf( "Error! Invalid parameter %s\n", on_off);
              moca_close(ctx);
              return(-4);
           }
           nRet = moca_set_lof_update(ctx, lofup);
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
        nRet = moca_get_lof_update(ctx, &lofup);
        if (nRet != MOCA_API_SUCCESS)
        {
            moca_close(ctx);
            printf( "Error!  Internal\n");
            return(-6);
      }
      printf("LOF_UPDATE mode is %s (%d) \n", lofup==0?"disabled":"enabled", lofup);
   }

   if (persistent)
   {
       nRet = moca_set_persistent(ctx);
       if (nRet != MOCA_API_SUCCESS)
       {
           moca_close(ctx);
           printf( "Error!	Unable to save persistent parameters\n");
           return(-7);
       }
   }

   if (reset)
   {
      nRet = moca_set_restart(ctx);
      if (nRet != MOCA_API_SUCCESS)
      {
         moca_close(ctx);
         printf( "Error!  Internal-4\n");
         return(-8);
      }
   }

   moca_close(ctx);
   return(0);
}
