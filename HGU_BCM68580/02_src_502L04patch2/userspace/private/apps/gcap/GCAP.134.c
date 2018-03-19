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
    printf("Usage: GCAP.134 <enable/disable> [-M] [-r] [-h] \n\
 Report/Set beaconing of the node \n\
 Note that resetting SoC is required for configuration to take effect.\n\
\n\
Options:\n\
<enable/disable>    enable or disable\n\
  -r     Reset SoC to make configuration changes effective\n\
  -M     Make configuration changes permanent\n\
  -h     Display this help and exit\n");
}

GCAP_GEN int GCAP_134_main(int argc, char **argv)
{
    int nRet = MOCA_API_SUCCESS;
    void *ctx;
    uint32_t  nc_mode;
    int arg_index = 1; // The enable/disable argument is usually argv[1], but the getopt routine might move the args around

    chipId = NULL;
    on_off = NULL;

    persistent = 0;
    reset = 0;

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

    while((nRet = getopt(argc, argv, "Mrhi:")) != -1) 
    {
        switch(nRet) 
        {
        case 'i':
            chipId = optarg;
            arg_index++;
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
        arg_index++;
     }
#endif
   ctx = moca_open(chipId);

    if (!ctx)
    {
        printf( "Error!  Unable to connect to moca instance\n");
        return(-3);
    }

   if ((argc > 1) && (argv[arg_index] != NULL))
   {
      on_off = argv[arg_index];
      if( !strcmp( on_off, "enable" ) )
      {
          nc_mode = 0;  
      }
      else if( !strcmp( on_off, "disable" ) )
      {
          nc_mode = 2;   
      }
      else
      {
         printf( "Error! Invalid parameter %s\n", on_off);
         moca_close(ctx);
         return(-4);
      }
      nRet = moca_set_nc_mode(ctx, nc_mode);
      if (nRet != MOCA_API_SUCCESS)
      {
         moca_close(ctx);
         printf( "Error!  Internal\n");
         return(-5);
      }
   }
   else  
   {
        nRet = moca_get_nc_mode(ctx, &nc_mode);
        if (nRet != MOCA_API_SUCCESS)
        {
            moca_close(ctx);
            printf( "Error!  Internal\n");
            return(-6);
      }
      printf("Beaconing is %s (%d) during network search\n", nc_mode==2?"disabled":"enabled", nc_mode);
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
