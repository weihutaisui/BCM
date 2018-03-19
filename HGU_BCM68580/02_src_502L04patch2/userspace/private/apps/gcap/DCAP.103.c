#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>

#include <mocalib.h>


static int reset = 0;          // -r option
static char *chipId = NULL;    // -i option
static int persistent = 0;     // -M option
static char *beaconpwr = NULL;


GCAP_GEN static void showUsage()
{
    printf("Usage: DCAP.103 <PWR> [-M] [-r] [-h]\n\
Set or report beacon power of the node\n\
Note that resetting SoC is required for configuration to take affect.\n\
\n\
Options:\n\
 <PWR> Amount of power reduction for Beacons in dB (rounded down to a multiple of 3)\n\
  -M   Make configuration changes permanent\n\
  -r    Reset SoC to make configuration changes effective\n\
  -h    Display this help and exit\n");
}

GCAP_GEN int DCAP_103_main(int argc, char **argv)
{
    int ret = 0;
    void *ctx;
    uint32_t getpwr =0;
    uint32_t pwr = 0;

    chipId = NULL;
    beaconpwr = NULL;
    persistent=0;
    reset=0;

#if defined(STANDALONE)
    int i;
#endif

    // ----------- Parse parameters
    if (argc < 2)
    {
      getpwr = 1;
    } 
    else 
    {

       beaconpwr = argv[1];

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
       while((ret = getopt(argc, argv, "Mrhi:")) != -1) 
       {
           switch(ret) 
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
    }

    // ----------- Initialize


   ctx = moca_open(chipId);

   if (!ctx)
   {
        printf( "Error!  Unable to connect to moca instance\n");
        return(-2);
   }

        
   if (getpwr)
   {
      ret = moca_get_beacon_pwr_reduction(ctx, &pwr);
      if (ret != MOCA_API_SUCCESS)
      {
         moca_close(ctx);
         printf( "Error!  Internal-1\n");
         return(-3);
      }
      printf("Beacon power reduction: %d dB\n", pwr*3);
      moca_close(ctx);
      return(0);
   }

   pwr = atol(beaconpwr);
   pwr /= 3;
  
   if ( pwr > MOCA_BEACON_PWR_REDUCTION_MAX )
   {
               
      printf( "Error! Invalid value %d, beacon power must be between 0 and %d\n", pwr, MOCA_BEACON_PWR_REDUCTION_MAX*3);
      moca_close(ctx);
      return(-4);
   }
   
   ret = moca_set_beacon_pwr_reduction(ctx, pwr);
   if (ret != MOCA_API_SUCCESS)
   {
      moca_close(ctx);
      printf( "Error!  Internal\n");
      return(-2);
   }

   if (reset)
   {
      ret = moca_set_restart(ctx);
      if (ret != MOCA_API_SUCCESS)
      {
         moca_close(ctx);
         printf( "Error!  Reinitialize\n");
         return(-3);
      }
   }

    if (persistent)
    {
       ret = moca_set_persistent(ctx);
       if (ret != MOCA_API_SUCCESS)
       {
          moca_close(ctx);
          printf( "Error!  Unable to save persistent parameters\n");
          return(-8);
       }
    }

    moca_close(ctx);


    return(0);
}


