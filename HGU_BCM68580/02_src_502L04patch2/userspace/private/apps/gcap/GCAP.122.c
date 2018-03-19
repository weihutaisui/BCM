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

static char *chipId = NULL;    // -i option
static int persistent = 0;     // -M option
static int reset = 0;          // -r option

GCAP_GEN static void showUsage()
{
 printf("Usage: GCAP.122 <VALUE> [-h] \n\
  Configure SEQ_NUMmr to this value.\n\
  If no value, current setting is displayed\n\
\n\
Options:\n\
  <VALUE>  Value ranges from 0x0 to 0xFFFF.\n\
  -h       Display this help and exit\n");
}

GCAP_GEN int GCAP_122_main(int argc, char **argv)
{
    int nRet = MOCA_API_SUCCESS;
    void *ctx;
    uint32_t nondefseqnum;
    char *end =NULL;

    chipId = NULL;
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
       nondefseqnum = strtoul(argv[1], &end, 0);
       if( *end != '\0' )
       {
            printf( "Error! Invalid parameter for <SEQ_NUMmr>\n");
            moca_close(ctx);
            return(-4);
       }
       nRet = moca_set_mr_seq_num(ctx, nondefseqnum);
       if (nRet != MOCA_API_SUCCESS)
       {
         moca_close(ctx);
         printf( "Error!  Internal\n");
         return(-5);
       }
   }
   else    
   {
       nRet = moca_get_mr_seq_num(ctx, &nondefseqnum);
       if (nRet != MOCA_API_SUCCESS)
       {
         moca_close(ctx);
         printf( "Error!  Internal\n");
         return(-6);
       }
       printf("SEQ_NUM_MR is 0x%x \n",  nondefseqnum);
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
