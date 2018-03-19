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
    printf("Usage: GCAP.109 <ON/OFF> [-r] [-M] [-h] [-i] \n\
 Disable or Enable TURBO_MODE of the node \n\
 Reset is required for changes to take effect \n\
\n\
Options:\n\
<ON/OFF>     Turn ON or OFF turbo mode.\n\
  -M    Make configuration changes permanent\n\
  -r     Reset SoC to make configuration changes effective\n\
  -h     Display this help and exit\n");
}

GCAP_GEN int GCAP_109_main(int argc, char **argv)
{
    int nRet = MOCA_API_SUCCESS;
    void *ctx;
    uint32_t  turbo;
    int i;

    chipId = NULL;
    on_off = NULL;
    persistent = 0;
    reset = 0;

    // ----------- Parse parameters
    if ( argc < 2 )
    {
            printf( "Error! Missing arguments\n");
            return(-1);
    }
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

    for(i=1;i<argc;i++)
    {
        if (argv[i][0] == '-')
        {
            // Ignore
        }
        else 
        {
           on_off = argv[i];
           if( !strcmp( on_off, "ON" ) )
           {
             turbo = 1;  
           }
           else if( !strcmp( on_off, "OFF" ) )
           {
             turbo = 0;   
           }
           else
           {
              printf( "Error! Invalid parameter %s\n", on_off);
              moca_close(ctx);
              return(-4);
           }
           nRet = moca_set_turbo_en(ctx, turbo);
           if (nRet != MOCA_API_SUCCESS)
           {
              moca_close(ctx);
              printf( "Error!  Internal\n");
              return(-5);
           }        
        }
    }

   if (reset)
   {
      nRet = moca_set_restart(ctx);
      if (nRet != MOCA_API_SUCCESS)
      {
         moca_close(ctx);
         printf( "Error!  Internal-4\n");
         return(-7);
      }
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
