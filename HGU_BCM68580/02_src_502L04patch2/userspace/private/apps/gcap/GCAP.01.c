#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>

#include <mocalib.h>
#include <mocaint.h>
#include "GCAP_Common.h"


static char *chipId = NULL;    // -i option

static int interval = 0;
static int duration = 0;
static int endless = 0;


GCAP_GEN static void showUsage()
{
    printf("Usage: GCAP.01 [-t <polling interval>] [-d <polling duration>] [-h]\n\
Report time-stamps whenever key exchanges happen between nodes.\n\
\n\
Options:\n\
  -t   Set polling interval in number of seconds (default 1 second)\n\
  -d   Set polling duration in number of minutes (default forever until\n\
       Ctrl-C to break)\n\
  -e   Allow polling to continue if link goes down (default polling\n\
       stops when MoCA link goes down)\n\
  -h   Display this help and exit\n");
}

GCAP_GEN static void callback(void * userarg, struct moca_key_changed * out)
{
#ifndef STANDALONE
   time_t t;
   struct tm *timex;


   t=time(NULL);
   timex=localtime(&t);

   printf("%02d:%02d:%02d ", timex->tm_hour, timex->tm_min, timex->tm_sec); 
#endif

   switch(out->key_type) 
   {
      case MOCA_KEY_CHANGED_TEK:
         printf("TEK Changed to %s\n", out->even_odd?"ODD":"EVEN");
         break;
      case MOCA_KEY_CHANGED_PMK:
         printf("PMK Changed to %s\n", out->even_odd?"ODD":"EVEN");
         break;
      case MOCA_KEY_CHANGED_APMK:
         printf("APMK Changed to %s\n", (out->even_odd & MOCA_KEY_CHANGED_APMK_EVEN_ODD_BIT)?"ODD":"EVEN");
         break;
      case MOCA_KEY_CHANGED_ATEK: 
         printf("ATEK Changed to %s\n", (out->even_odd & MOCA_KEY_CHANGED_ATEK_EVEN_ODD_BIT)?"ODD":"EVEN"); 
         break;
      default:
         printf("Unknown key changed event %d\n", out->key_type);
         break;
   }
}


GCAP_GEN static void link_cb(void * userarg, uint32_t status)
{
    if (status == 0)
    {
        printf("Error! No Link\n");
        moca_cancel_event_loop(userarg);
    }
}


GCAP_GEN int GCAP_01_main(int argc, char **argv)
{
    int ret;
    
    uint32_t privacyEn;
    struct moca_interface_status status;
    struct moca_network_status nws;
    void *ctx;

    chipId = NULL;

#if defined(STANDALONE)
   int i;
   for (i=1; i < argc; i++)
   {
       if (strcmp(argv[i], "-x") == 0)
       {
          i++;
          chipId = argv[i];
       }
       else if (strcmp(argv[i], "-e") == 0)
       {
          endless = 1;
       }
       else if (strcmp(argv[i], "-t") == 0)
       {
          interval = atoi(argv[i+1]);
          i++; 
       }
       else if (strcmp(argv[i], "-d") == 0)
       {
          duration = atoi(argv[i+1]);
          i++;
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
    moca_gcap_init();

    // ----------- Parse parameters
    
    opterr = 0;
    
    while((ret = getopt(argc, argv, "hex:d:t:")) != -1) 
    {
        switch(ret)
        {
        case 'e':
            endless = 1;
            break;
        case 'x':
            chipId = optarg;
            break;            
        case 't':
            interval = atoi(optarg);
            break;
        case 'd':
            duration = atoi(optarg);
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
 // ----------- Get info
    ret = moca_get_interface_status(ctx, &status);

    if (ret != MOCA_API_SUCCESS)
    {
        printf( "Error! Internal failure\n");
        moca_close(ctx);
        return(-3);
    }

    if (status.link_status != MOCA_LINK_UP)
    {
        printf( "Error! No Link\n");
        moca_close(ctx);
        return(-4);
    }  

    ret = moca_get_network_status(ctx, &nws);
    if (ret != MOCA_API_SUCCESS)
    {
        printf( "Error! Internal failure\n");
        moca_close(ctx);
        return(-3);
    }

    ret = moca_get_privacy_en(ctx, &privacyEn);

    if (privacyEn == MOCA_PRIVACY_EN_DEF)
    {
        printf( "Error! Privacy is not enabled!\n");
        moca_close(ctx);
        return(-5); 
    }
    
    printf("Waiting for Privacy key exchange\n");

    moca_register_key_changed_cb(ctx, callback, ctx);

    if (endless == 0)
       moca_register_link_up_state_cb(ctx, link_cb, ctx);

    if (duration)
        MoCAOS_Alarm(duration*60);

    ret = moca_event_loop(ctx);

    moca_close(ctx);

    return(0);
}


