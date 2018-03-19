#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>

#include <mocalib.h>

static char *chipId = NULL;    // -i option
static int persistent = 0;     // -M option
static int reset = 0;          // -r option

GCAP_GEN static void showUsage()
{
    printf("Usage: GCAP.09 [-m <taboo_mask>] [-o <taboo_offset>][-h] [-M] [-r] \n\
Report or Set Taboo frequency mask.\n\
\n\
Options:\n\
  -h             Display this help and exit\n\
  -M             Make configuration changes permanent\n\
  -r             Reset SoC to make configuration changes effective\n\
  -m             Taboo mask for this node\n\
  -o             Taboo offset for this node\n\
  \n");
}


GCAP_GEN int GCAP_09_main(int argc, char **argv)
{
    int ret = MOCA_API_SUCCESS;
    void *ctx;
    uint32_t startSet = 0;
    uint32_t maskSet = 0;    
    uint32_t startValue = 0;
    uint32_t maskValue = 0;    
    struct moca_taboo_channels taboo;
    struct moca_network_status ns;
    int i = 1;
    char *end;

    chipId = NULL;
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
      else if (strcmp(argv[i], "-r") == 0)
      {
         reset = 1;
      }
      else if (strcmp(argv[i], "-M") == 0)
      {
         persistent = 1;
      }
      else if (strcmp(argv[i], "-m") == 0)
      {
              i++;
              if (i < argc)
              { 
                 if ((argv[i][0] == '0') && (argv[i][1] == 'x'))
                 {
                    maskValue = strtoul(argv[i], &end, 0);          
                    if (*end != '\0')
                    {
                       printf( "Error!  Invalid parameter\n");
                       return(-1);
                    }
                    maskSet = 1;
                 }
                 else
                 {
                    printf( "Error!  Invalid value for taboo_mask, should start with 0x\n");
                    return(-2);
                 }
              }      
      }
      else if (strcmp(argv[i], "-o") == 0)
      {
          i++;
          startValue = strtoul(argv[i], &end, 0);
          if (*end != '\0')
          {
             printf( "Error!  Invalid parameter\n");
             return(-2);
          }
          startSet = 1;
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

   
   while((ret = getopt(argc, argv, "hMri:m:o: ")) != -1)
   {
         switch(ret)
         {
         case 'i':
             chipId = optarg;
             i +=2;
             break;
         case 'M':
             persistent = 1;
              i++;
              break;            
         case 'r':
             reset = 1;
              i++;
              break;  
         case 'm':
              i++;
              if (i < argc)
              { 
                 if ((argv[i][0] == '0') && (argv[i][1] == 'x'))
                 {
                    maskValue = strtoul(argv[i], &end, 0);          
                    if (*end != '\0')
                    {
                       printf( "Error!  Invalid parameter\n");
                       return(-1);
                    }
                    maskSet = 1;
                 }
                 else
                 {
                    printf( "Error!  Invalid value for taboo_mask, should start with 0x\n");
                    return(-2);
                 }
              }
              break;
         case 'o':
             startValue = strtoul(optarg, &end, 0);
             if (*end != '\0')
             {
                 printf( "Error!  Invalid parameter\n");
                 return(-2);
             }
             startSet = 1;
             i++;
             break;
         case '?':
             printf( "Error!  Invalid option - %c\n", optopt);
             return(-3);
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

    if ( (maskSet != 0) || (startSet != 0))
    {
        ret = moca_get_taboo_channels(ctx, &taboo);
        if (ret != MOCA_API_SUCCESS)
        {
            moca_close(ctx);
            printf( "Error!  GetTaboo\n");
            return(-6);
        }

        if (maskSet != 0)
           taboo.taboo_fixed_mask_start = startValue;
        if (startSet != 0)
           taboo.taboo_fixed_channel_mask = maskValue;
        
        ret = moca_set_taboo_channels(ctx, &taboo);
        
        if (ret != MOCA_API_SUCCESS)
        {
            moca_close(ctx);
            printf( "Error!  SetTaboo\n");
            return(-6);
        }

        if (persistent)
        {
            ret = moca_set_persistent(ctx);
            if (ret != MOCA_API_SUCCESS)
            {
               moca_close(ctx);
               printf( "Error!  Unable to save persistent parameters\n");
               return(-7);
            }
        }

        if (reset)
        {
            ret = moca_set_restart(ctx);

            if (ret != MOCA_API_SUCCESS)
            {
                moca_close(ctx);
                printf( "Error!  Reinitialize\n");
                return(-8);
            }
        }
    }
    else
    {
        // ----------- Get info

       ret = moca_get_network_status(ctx, &ns);

        if (ret != MOCA_API_SUCCESS)
        {            
            printf( "Error!  Internal failure\n");
            moca_close(ctx);
            return(-9);
        }

        printf("TABOO_CHANNEL_MASK  = 0x%06x\n", ns.network_taboo_mask);
        printf("TABOO_MASK_START    = %d\n", ns.network_taboo_start);    
    }
    
    // ----------- Finish
    moca_close(ctx);

    return(0);
}



