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
static char *onoff = NULL;

GCAP_GEN static void showUsage()
{
    printf("Usage: DCAP.46 [<[ON]/OFF>] [ARPL Threshold] [-r] [-h]\n\
Set or report SAPMEN, SAPM and ARPL_THLD .\n\
\n\
Options:\n\
 ON/OFF To turn 'ON' OR 'OFF'\n\
        Subcarrier Added PHY Margin\n\
 ARPL Threshold\n\
        value in -dBm with the minus sign\n\
  -r    Reset SoC to make configuration changes effective\n\
  -h    Display this help and exit\n");
}

GCAP_GEN int DCAP_46_main(int argc, char **argv)
{
    int ret=0;
    int i;
    void *ctx;
    int argstart;
    int32_t arpl;
    uint32_t sapm_en;
    uint32_t bReport = 0;
    uint32_t bArpl = 0;
    struct moca_sapm_table_100 sapmtbl;

    chipId = NULL;
    onoff = NULL;
    reset = 0;

    // ----------- Parse parameters

    if ((argc < 2) || (argv[1] == NULL) || (argv[1][0] == '-'))
    {
        argstart = 1;
        bReport = 1;
    }
    else
    {
        argstart = 2;
        onoff = argv[1];  
        if ((strcmp(onoff, "ON") != 0) && (strcmp(onoff, "OFF") != 0))
        {
           printf("Error!  Invalid parameter - %s\n",onoff);
           return(-3);
        }
        if ((argstart < argc) && (argv[argstart][0] == '-'))
        {
           if ((argv[argstart][1] >= '0') && (argv[argstart][1] <= '9'))
           {
              arpl = atoi(argv[argstart]);
              bArpl = 1;                    
              argv[argstart][0] = 'a'; // so getopt doesn't parse it
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

    while((ret = getopt(argc, argv, "rhi:")) != -1)
    {
        switch(ret)
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
    // ----------- Initialize

    
    ctx = moca_open(chipId);

    if (!ctx)
    {
        printf( "Error!  Unable to connect to moca instance\n");
        return(-2);
    }

    if ( bReport == 1)
    {
       ret = moca_get_sapm_en(ctx, &sapm_en);
       if (ret != MOCA_API_SUCCESS)
       {
          moca_close(ctx);
          printf( "Error!  could not get SAPM_EN\n");
          return(-3);
       }

       printf("SAPM: %s\n", sapm_en?"ON":"OFF");

       ret = moca_get_arpl_th_100(ctx, &arpl);
       if (ret != MOCA_API_SUCCESS)
       {
          moca_close(ctx);
          printf( "Error!  could not get Arpl_Thr\n");
          return(-4);
       }
       printf("ARPL Threshold: %d\n", arpl); 

       ret = moca_get_sapm_table_100(ctx, &sapmtbl);
       if (ret != MOCA_API_SUCCESS)
       {
          moca_close(ctx);
          printf( "Error!  could not get SAPM table\n");
          return(-5);
       }
       printf("SAPM Table:");
       for (i=0;i<512;i++)
       {
          if (i%8 == 0)
             printf("\n");
          printf ("[%3d=%4d.%d]", i, sapmtbl.val[i]/2,(sapmtbl.val[i]&1)*5);
       }
       printf("\n");
    } 
    else
    {
       if (strcmp(onoff, "ON") == 0)
       {
          sapm_en = 1;
          for (i=0 ; i < 512; i++) 
          {
             if ((i >40) && (i < 49))
                sapmtbl.val[i] = 4 * 2;
             else if ((i >48) && (i < 57))
                sapmtbl.val[i] = 7 * 2;
             else if ((i >56) && (i < 61))
                sapmtbl.val[i] = 60 * 2;
             else
                sapmtbl.val[i] = 0;
          }  
       }
       else
       {
          sapm_en = 0;

          for (i=0 ; i < 512; i++) 
          {
             sapmtbl.val[i] = 0;
          }
       }

       ret = moca_set_sapm_en(ctx, sapm_en);
       if (ret != MOCA_API_SUCCESS)
       {
          moca_close(ctx);
          printf( "Error!  could not set SAPM_EN\n");
          return(-6);
       }
       if ( bArpl == 1)
       {
          ret = moca_set_arpl_th_100(ctx, arpl);
          if (ret != MOCA_API_SUCCESS)
          {
             moca_close(ctx);
             printf( "Error!  could not set Arpl_Thld\n");
             return(-7);
          }
       }

       ret = moca_set_sapm_table_100(ctx, &sapmtbl);
       if (ret != MOCA_API_SUCCESS)
       {
          moca_close(ctx);
          printf( "Error!  could not set SAPM table\n");
          return(-8);
       }

    }
    // ----------- Activate Settings   
    
    if (reset)
    {
       ret = moca_set_restart(ctx);
       if (ret != MOCA_API_SUCCESS)
       {
          moca_close(ctx);
          printf( "Error!  Reinitialize\n");
          return(-9);
       }
    }
    
    // ----------- Finish

    moca_close(ctx);

    return(0);
}


