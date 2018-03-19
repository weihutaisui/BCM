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
static int reset = 0;          // -r option

GCAP_GEN static void showUsage()
{
    printf("Usage: GCAP.111 <ON/OFF> [-r] [-h] \n\
Set POLICING of the node \n\
\n\
Options:\n\
<ON/OFF>     Turn ON or OFF POLICING.\n\
  -r     Reset SoC to make configuration changes effective\n\
  -h     Display this help and exit\n");
}

GCAP_GEN int GCAP_111_main(int argc, char **argv)
{
    int nRet = MOCA_API_SUCCESS;
    void *ctx;
    uint32_t  polen;

    chipId = NULL;
    on_off = NULL;
    reset=0;

    // ----------- Parse parameters

   if ((argc > 1) && (argv[1] != NULL))
   {
      on_off = argv[1];
      if( !strcmp( on_off, "ON" ) )
      {
          polen = 1;  
      }
      else if( !strcmp( on_off, "OFF" ) )
      {
          polen = 0;   
      }
      else
      {
          printf( "Error! Invalid parameter %s\n", on_off);
          showUsage();
          return(-2);
      }
   }
   else
   {
      printf( "Error!  Missing parameter ON/OFF\n");
      return(-1);
   }
   
#if defined(STANDALONE)
   uint32_t i;

   for (i=1; i < argc; i++)
   {
       if (strcmp(argv[i], "-i") == 0)
       {
           i++;
           chipId = argv[i];
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

    while((nRet = getopt(argc, argv, "hri::")) != -1) 
    {
        switch(nRet) 
        {
        case 'i':
            chipId = optarg;
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
    
   nRet = moca_set_policing_en(ctx, polen);
   if (nRet != MOCA_API_SUCCESS)
   {
       moca_close(ctx);
       printf( "Error!  Internal\n");
       return(-5);
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
   moca_close(ctx);
   return(0);
}
