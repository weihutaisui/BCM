#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>

#include <mocalib.h>

static int persistent = 0;     // -M option
static int reset = 0;          // -r option
static char *chipId = NULL;    // -i option
static char *lof = NULL;

static int isValidFrequency( uint32_t rfband, uint32_t freq )
{

  if (freq == 0)
      return 1;
   
   if (rfband == MOCA_RF_BAND_D_LOW)
   {
      switch ( freq ) {
         /* LAN */
         case 1125 :
         case 1150 :
         case 1175 :
         case 1200 :
         case 1225 :
           return 1 ;
      
         default :
           return 0 ;
      }
   }
   if (rfband == MOCA_RF_BAND_D_HIGH)
   {
      switch ( freq ) {
         /* LAN */
         case 1350 :
         case 1375 :
         case 1400 :
         case 1425 :
         case 1450 :
         case 1475 :
         case 1500 :
         case 1525 :
         case 1550 :
         case 1575 :
         case 1600 :
         case 1625 :
           return 1 ;
      
         default :
           return 0 ;
      }
   }
   if (rfband == MOCA_RF_BAND_EX_D)
   {
      switch ( freq ) {
         /* LAN */
         case 1125 :
         case 1150 :
         case 1175 :
         case 1200 :
         case 1225 :
         case 1250 :
         case 1275 :
         case 1300 :
         case 1325 :
         case 1350 :
         case 1375 :
         case 1400 :
         case 1425 :
         case 1450 :
         case 1475 :
         case 1500 :
         case 1525 :
         case 1550 :
         case 1575 :
         case 1600 :
         case 1625 :
           return 1 ;
      
         default :
           return 0 ;
      }
   }
   else if (rfband == MOCA_RF_BAND_E)
   {
      switch ( freq ) {
         /* MidRF */
         case 500 :
         case 525 :
         case 550 :
         case 575 :
         case 600 :
           return 1 ;
         default :
           return 0 ;
      }
   }
   else if (rfband == MOCA_RF_BAND_F)
   {
      switch ( freq ) {
         case 675:
         case 700:
         case 725:
         case 750:
         case 775:
         case 800:
         case 825:
         case 850:
            return 1 ;
         default : 
            return 0;
      }
   }
   else if (rfband == MOCA_RF_BAND_H)
   {
      switch ( freq ) {
         case 975:
         case 1000:
         case 1025:
            return 1 ;
         default :
            return 0;
      }
   }
   else // unknown band
   {
      return 0;
   }
   
}


GCAP_GEN static void showUsage()
{
    printf("Usage: GCAP.08 <LOF> [-M] [-r] [-h]\n\
Set or report LOF of the node. No input parameters are  \n\
required to report LOF\n\n\
\n\
Options:\n\
 <LOF> Last Frequency of Operation in MHZ.\n\
  -M   Make configuration changes permanent\n\
  -r   Reset SoC to make configuration changes effective\n\
  -h   Display this help and exit\n");
}

GCAP_GEN int GCAP_08_main(int argc, char **argv)
{
    int ret = 0;
    void *ctx;
    int getlof = 0;
    uint32_t freq = 0;
    uint32_t rfband =0;

    chipId = NULL;
    lof = NULL;
    persistent = 0;
    reset = 0;

    // ----------- Parse parameters


    if (argc < 2)
    {
      getlof = 1;
    } else {

       lof = argv[1];
    }

#if defined(STANDALONE)
   int i;

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
     else if (strcmp(argv[i], "-M") == 0)
     {
        persistent = 1;
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
    while((ret = getopt(argc, argv, "Mrh1i:")) != -1) 
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
    
    // ----------- Initialize
#endif
    ctx=moca_open(chipId);
    if (!ctx)
    {
        printf( "Error!  Unable to connect to moca instance\n");
        return(-2);
    }
        
    if (getlof)
    {
        ret = moca_get_lof(ctx, &freq);
        if (ret != MOCA_API_SUCCESS)
        {
            moca_close(ctx);
            printf( "Error!  Internal 1\n");
            return(-3);
        }
        printf("LOF channel: %4d.0 MHz\n", (int)freq);
        moca_close(ctx);
        return(0);
    }
    
    freq = atol(lof);
        
    ret = moca_get_rf_band(ctx, &rfband);
    if (ret != MOCA_API_SUCCESS)
    {
       moca_close(ctx);
       printf( "Error!  Internal 2\n");
       return(-4);
    }   
        
    if (!isValidFrequency(rfband, freq))
    {
       moca_close(ctx);
       printf( "Error!  Invalid parameter  LOF for the rf_band %d \n", (int)rfband);
       return(-5);
    }   
        
    ret = moca_set_lof(ctx, freq);
    if (ret != MOCA_API_SUCCESS)
    {
        moca_close(ctx);
        printf( "Error!  Internal 3\n");
        return(-6);
    }
   
 // ----------- Activate Settings   

    if (reset)
    {
        ret = moca_set_restart(ctx);
    }
    
    if (ret != MOCA_API_SUCCESS)
    {
        moca_close(ctx);
        printf( "Error!  Internal 4\n");
        return(-7);
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


    // ----------- Finish

    moca_close(ctx);


    return(0);
}
