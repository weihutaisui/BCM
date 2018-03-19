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


static int reset = 0;          // -r option
static char *chipId = NULL;    // -i option
static char *onoff = NULL;
static int persistent = 0;     // -M option

GCAP_GEN static void showUsage()
{
    printf("Usage: DCAP.42 [<[ON]/OFF>] [GARPL_Threshold] [RLAPM_value] [-r] [-M] [-h]\n\
Set or report RLAPMEN and RLAPM.\n\
\n\
Options:\n\
 ON/OFF To turn 'ON' OR 'OFF'\n\
        Subcarrier Added PHY Margin\n\
 GARPL_Threshold \n\
        value in -dBm with the minus sign\n\
 RLAPM_value \n\
        Margin Adjustments per RX power \n\
  -r    Reset SoC to make configuration changes effective\n\
  -M    Make configuration changes permanent\n\
  -h    Display this help and exit\n");
}

GCAP_GEN int DCAP_42_main(int argc, char **argv)
{
    int ret=0;
    int i;
    void *ctx;
    int32_t garpl;

    chipId = NULL;
    onoff = NULL;
    persistent=0;
    reset=0;

#if defined(STANDALONE)
    int32_t rlapmValue = 0;
#else
    float rlapmValue = 0;
#endif
    uint32_t rlapm_en = 0;
    uint32_t enSet = 0;
    uint32_t garplSet = 0;
    uint32_t rlapmSet = 0;
    uint32_t rlapmid =0;
    struct moca_rlapm_table_100 rlapmtbl;

    // ----------- Parse parameters

    if ((argc > 1) && (argv[1] != NULL))
    {
      for(i=1;i<argc;i++)
      {
         if ( (argv[i][0] == '-' ) && ( (argv[i][1] == 'h') ||
                                         (argv[i][1] == 'r') ||
                                         (argv[i][1] == 'M')) )
         {
            // Ignore
         }
         else if (!enSet)
         {
            onoff = argv[i]; 
            enSet = 1;
            if ((strcmp(onoff, "ON") != 0) && (strcmp(onoff, "OFF") != 0))
            {
               printf("Error!  Invalid parameter - %s\n",onoff);
               return(-2);
            }
         }
         else if (!garplSet)
         {
            garplSet = 1;
            if ((i < argc) && (argv[i][0] == '-'))
            {
               if ((argv[i][1] >= '0') && (argv[i][1] <= '9'))
               {
                  garpl = atoi(argv[i]);  
                  rlapmid = abs(garpl);
                  if (rlapmid > 65 )
                  {
                     printf( "Error! Invalid GARPL %d, should be value between 0 and -65\n", garpl);
                     return(-3);  
                  }
                  argv[i][0] = 'a'; // so getopt doesn't parse it
               }
            } 
         }
         else if (!rlapmSet)
         {
            rlapmSet = 1;
            if (i < argc)
            {
                if ((argv[i][0] >= '0') && (argv[i][0] <= '9'))
                {
#if defined(STANDALONE)
                  rlapmValue = atoi(argv[i]);
#else
                  rlapmValue = atof(argv[i]);
#endif
                  if (rlapmValue >=  60 )
                  {
#if defined(STANDALONE)
                     printf( "Error! Invalid RLAPM value %d, should be value between 0 and 59\n", rlapmValue);
#else
                     printf( "Error! Invalid RLAPM value %f, should be value between 0 and 59\n", rlapmValue);
#endif
                     return(-4);  
                  }  
                }
                else
                {
                     printf( "Error! Invalid RLAPM value should be value between 0 and 60\n");
                     return(-5);  
                }  
            }   
         }   
    }
   }

#if defined(STANDALONE)

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
    while((ret = getopt(argc, argv, "Mrhi:")) != -1)
    {
        switch(ret)
        {
        case 'i':
            chipId = optarg;
            break;
        case 'r':
            reset = 1;
            break;
        case 'M':
            persistent = 1;
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
    // ----------- Initialize

    
    ctx = moca_open(chipId);

    if (!ctx)
    {
        printf( "Error!  Unable to connect to moca instance\n");
        return(-6);
    }

   if ((argc > 1) && (argv[1] != NULL))
   {
     if (enSet)
     {
        if (strcmp(onoff, "ON") == 0)
        {
           rlapm_en = 1;
        }
        else
        {
           rlapm_en = 0;
        }
        ret = moca_set_rlapm_en(ctx, rlapm_en);
        if (ret != MOCA_API_SUCCESS)
        {
           moca_close(ctx);
           printf( "Error!  could not set rlapmen\n");
           return(-7);
        } 
     }
     if ( (garplSet ) && (rlapmSet) )
     {
         if ( rlapm_en == 1 )
         {
             if (rlapmid == 0 )
             {
                 for (i = 0; i <66; i++)
                 {
                     rlapmtbl.rlapmtable[i] = (rlapmValue * 2);
                 }
             }
             else if (rlapmid == 65 )
             {
                for (i = 0; i < 65; i++)
                {
                     rlapmtbl.rlapmtable[i] = MOCA_RLAPM_TABLE_100_RLAPMTABLE_DEF;
                } 
                rlapmtbl.rlapmtable[65] = (int)(rlapmValue * 2);
             }
             else
             {  
                for (i = rlapmid; i < 66; i++)
                {
                     rlapmtbl.rlapmtable[i] = (int)(rlapmValue * 2);
                } 
                for (i = 0; i < rlapmid; i++)
                {
                     rlapmtbl.rlapmtable[i] = MOCA_RLAPM_TABLE_100_RLAPMTABLE_DEF;
                } 
             }
             ret = moca_set_rlapm_table_100(ctx, &rlapmtbl);
             if (ret != MOCA_API_SUCCESS)
             {
                moca_close(ctx);
                printf( "Error!  could not set rlapm table\n");
                return(-8);
             }
         }
     }
   }
   else 
   {
       ret = moca_get_rlapm_en(ctx, &rlapm_en);
       if (ret != MOCA_API_SUCCESS)
       {
          moca_close(ctx);
          printf( "Error!  could not get RLAPM_EN\n");
          return(-9);
       }

       printf("RLAPM: %s\n", rlapm_en?"ON":"OFF");


       ret = moca_get_rlapm_table_100(ctx, &rlapmtbl);
       if (ret != MOCA_API_SUCCESS)
       {
          moca_close(ctx);
          printf( "Error!  could not get rlapm table\n");
          return(-10);
       }
       printf("RLAPM Table:");
       for (i=0;i<66;i++)
       {
          if (i%8 == 0)
             printf("\n");
          printf ("[%3d=%4d.%d]", i, rlapmtbl.rlapmtable[i]/2, (rlapmtbl.rlapmtable[i]&1)*5);
       }
       printf("\n");
    } 
    
    // ----------- Activate Settings   
       if (persistent)
    {
       ret = moca_set_persistent(ctx);
       if (ret != MOCA_API_SUCCESS)
       {
          moca_close(ctx);
          printf( "Error!  Unable to save persistent parameters\n");
          return(-11);
       }
    }

    if (reset)
    {
       ret = moca_set_restart(ctx);
       if (ret != MOCA_API_SUCCESS)
       {
          moca_close(ctx);
          printf( "Error!  Reinitialize\n");
          return(-12);
       }
    }
    
    // ----------- Finish

    moca_close(ctx);

    return(0);
}


