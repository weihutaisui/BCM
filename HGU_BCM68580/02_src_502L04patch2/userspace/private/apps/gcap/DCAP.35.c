#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>

#ifdef STANDALONE
#include "moca.h"
#endif

#include <mocalib.h>

static char *chipId = NULL;    // -i option
static char *bndSet = NULL;
static int persistent = 0;     // -M option
static int reset = 0;          // -r option

GCAP_GEN static void showUsage()
{
    printf("Usage: DCAP.35 [ED|DL|DH|E|F|H]  [-r]  [-M]  [-h]\n\
Report or switch network search among D-low, D-High, Extended D, E or F band. \n\
\n\
Options:\n\
   ED   Switch network search to Extended Band D.\n\
   DL    Switch network search to Search Sub-Band D-Low. \n\
   DH    Switch network search to Search Sub-Band D-High. \n\
   E    Switch network search to Search Band E.\n\
   F    Switch network search to Search Band F.\n\
   H    Switch network search to Search Band H.\n\
  -M   Make configuration changes permanent\n\
  -r   Reset SoC to make configuration changes effective\n\
  -h   Display this help and exit\n");

}

GCAP_GEN static void printBnd(uint32_t bnd)
{
   switch(bnd)
   {
      case (0):
            printf("the band is MOCA_RF_BAND_D_LOW \n");  
            break;
      case(1):
            printf("the band is MOCA_RF_BAND_D_HIGH \n");  
            break;
      case(2):
            printf("the band is MOCA_RF_BAND_EX_D \n");  
            break;
      case(3):
            printf("the band is MOCA_RF_BAND_E \n");  
            break;
      case(4):
            printf("the band is MOCA_RF_BAND_F \n");  
            break;
      case(6):
            printf("the band is MOCA_RF_BAND_H \n");  
            break;
      default:
            return; 
   }
}

GCAP_GEN int DCAP_35_main(int argc, char **argv)
{
    int ret = 0;
    void *ctx;
    int getbnd =0;
    int setbnd = 0;
    uint32_t setband = 0;
    uint32_t getband;
    uint32_t lofbnd = 0;

    persistent = 0;
    reset = 0;

    chipId = NULL;
    bndSet = NULL;

    // ----------- Parse parameters

    if ((argc < 2) || (argv[1] == NULL) || (argv[1][0] == '-'))
    {
        getbnd = 1;
    } 
    else 
    {
        bndSet = argv[1];
        if (bndSet != NULL )
        {
            if  (!strcmp(bndSet, "ED"))
            {
                setband = MOCA_RF_BAND_EX_D;  
                lofbnd = MOCA_LOF_BAND_EX_D_DEF;
                setbnd =1;
            }
            else if (!strcmp(bndSet, "DL"))
            {
                setband = MOCA_RF_BAND_D_LOW;  
                lofbnd = MOCA_LOF_BAND_D_LOW_DEF;
                setbnd =1;
            }
            else if  (!strcmp(bndSet, "DH") )
            {
                setband = MOCA_RF_BAND_D_HIGH; 
                lofbnd = MOCA_LOF_BAND_D_HIGH_DEF;
                setbnd =1;
            }
            else if  (!strcmp(bndSet, "E") )
            {
                setband = MOCA_RF_BAND_E; 
                lofbnd = MOCA_LOF_BAND_E_DEF;
                setbnd =1;
            }
            else if  (!strcmp(bndSet, "F") )
            {
                setband = MOCA_RF_BAND_F; 
                lofbnd = MOCA_LOF_BAND_F_DEF;
                setbnd =1;
            }
            else if  (!strcmp(bndSet, "H") )
            {
                setband = MOCA_RF_BAND_H; 
                lofbnd = MOCA_LOF_BAND_H_DEF;
                setbnd =1;
            }
            else
            {
                printf( "Error! Invalid band value\n");
                return(-3);
            }
        }
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
    ctx = moca_open(chipId);

    if (!ctx)
    {
        printf( "Error!  Unable to connect to moca instance\n");
        return(-2);
    }

    if ( setbnd  )
    {
        printf("Setting rf band to %s \n",bndSet);
        ret = moca_set_rf_band(ctx, setband);

        ret |= moca_set_lof(ctx, lofbnd);
        if (ret != MOCA_API_SUCCESS)
        {
            moca_close(ctx);
            printf( "Error!  Internal error 1\n");
            return(-3);
        }  
#ifdef STANDALONE
        {
           char argv[2][2]; 

           if (lofbnd < 1100)
             argv[1][0] = 'L';
           else
             argv[1][0] = 'H';
           argv[1][1] = '\0';

           argv[0][0] = '\0';
           MOCA_CLI_Rfswitch(2, (char **)argv);
        }
#endif
   }

   if ( getbnd )
   {
       ret =   moca_get_rf_band(ctx, &getband);
       if (ret != MOCA_API_SUCCESS)
       {
          moca_close(ctx);
          printf( "Error!  Internal error 1\n");
       }
       printBnd(getband);
   }
 
    // ----------- Finish
    if (reset)
    {
        ret = moca_set_restart(ctx);
    }
    
    if (ret != MOCA_API_SUCCESS)
    {
        moca_close(ctx);
        printf( "Error!  Internal 2\n");
        return(-4);
    }

    if (persistent)
    {
       ret = moca_set_persistent(ctx);
       if (ret != MOCA_API_SUCCESS)
       {
          moca_close(ctx);
          printf( "Error!  Unable to save persistent parameters\n");
          return(-5);
       }
    }
    moca_close(ctx);

    return(0);
}

